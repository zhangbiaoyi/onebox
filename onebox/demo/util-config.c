#include "onebox-common.h"
#include "util-config.h"
#include "util-error.h"
#include "util-debug.h"
#include "util-mem.h"
#include "util-path.h"
#include "util-conf-node.h"
#include <yaml.h>

/************ define *************/
#define YAML_VERSION_MAJOR 1
#define YAML_VERSION_MINOR 1

#define DEFAULT_NAME_LEN 16
#define MANGLE_ERRORS_MAX 10

/************* vars  *************/
static int mangle_errors = 0;
static char *conf_dirname = NULL;

/* Configuration processing states. */
enum conf_state {
    CONF_KEY = 0,
    CONF_VAL,
    CONF_INCLUDE,
};

/************* funcs declare *************/
static int ConfYamlParse(yaml_parser_t *parser, ConfNode *parent, int inseq);

/************* funcs *************/
/**
 * \brief Mangle unsupported characters.
 *
 * \param string A pointer to an null terminated string.
 *
 * \retval none
 */
static void Mangle(char *string)
{
    char *c;

    while ((c = strchr(string, '_')))
        *c = '-';

    return;
}

/**
 * \brief Include a file in the configuration.
 *
 * \param parent The configuration node the included configuration will be
 *          placed at.
 * \param filename The filename to include.
 *
 * \retval 0 on success, -1 on failure.
 */
static int ConfYamlHandleInclude(ConfNode *parent, const char *filename)
{
	yaml_parser_t parser;
	char include_filename[PATH_MAX];
	FILE *file;

	if (yaml_parser_initialize(&parser) != 1) 
	{
		OBLogError(OB_ERR_CONF_YAML_ERROR, "Failed to initialize YAML parser");
		return -1;
	}

	if (PathIsAbsolute(filename)) 
	{
		strlcpy(include_filename, filename, sizeof(include_filename));
	}
	else 
	{
		snprintf(include_filename, sizeof(include_filename), "%s/%s", conf_dirname, filename);
	}

	file = fopen(include_filename, "r");
	if (file == NULL) 
	{
		OBLogError(OB_ERR_FOPEN, "Failed to open configuration include file %s: %s",
					include_filename, strerror(errno));
		return -1;
	}

	yaml_parser_set_input_file(&parser, file);

	if (ConfYamlParse(&parser, parent, 0) != 0) 
	{
		OBLogError(OB_ERR_CONF_YAML_ERROR, "Failed to include configuration file %s", filename);
		return -1;
	}

	yaml_parser_delete(&parser);
	fclose(file);

	return 0;
}

/**
 * \brief Parse a YAML layer.
 *
 * \param parser A pointer to an active yaml_parser_t.
 * \param parent The parent configuration node.
 *
 * \retval 0 on success, -1 on failure.
 */
static int ConfYamlParse(yaml_parser_t *parser, ConfNode *parent, int inseq)
{
	ConfNode *node = parent;
	yaml_event_t event;
	int done = 0;
	int state = 0;
	int seq_idx = 0;

	while (!done)
	{
		if (!yaml_parser_parse(parser, &event)) 
		{
			OBLogError(OB_ERR_CONF_YAML_ERROR,"Failed to parse configuration file at line %lu: %s\n",
                			(uintmax_t)parser->problem_mark.line, parser->problem);
			return -1;
		}

		if (event.type == YAML_DOCUMENT_START_EVENT) 
		{
			OBLogDebug("event.type=YAML_DOCUMENT_START_EVENT; state=%d", state);
			/* Verify YAML version - its more likely to be a valid
			* configuration file if the version is correct. */
			yaml_version_directive_t *ver = event.data.document_start.version_directive;
			if (ver == NULL) {
				fprintf(stderr, "ERROR: Invalid configuration file.\n\n");
				fprintf(stderr, "The configuration file must begin with the following two lines:\n\n");
				fprintf(stderr, "%%YAML 1.1\n---\n\n");
				goto fail;
			}
			int major = event.data.document_start.version_directive->major;
			int minor = event.data.document_start.version_directive->minor;
			if (!(major == YAML_VERSION_MAJOR && minor == YAML_VERSION_MINOR)) {
				fprintf(stderr, "ERROR: Invalid YAML version.  Must be 1.1\n");
				goto fail;
			}
		}
		else if (event.type == YAML_SCALAR_EVENT) 
		{
			char *value = (char *)event.data.scalar.value;
			char *tag = (char *)event.data.scalar.tag;
			OBLogDebug("event.type=YAML_SCALAR_EVENT; state=%d; value=%s; "
					"tag=%s; inseq=%d", state, value, tag, inseq);
			if (inseq) 
			{
				char sequence_node_name[DEFAULT_NAME_LEN];
				snprintf(sequence_node_name, DEFAULT_NAME_LEN, "%d", seq_idx++);
				ConfNode *seq_node = ConfNodeLookupChild(parent,sequence_node_name);
				if (seq_node != NULL) 
				{
					/* The sequence node has already been set, probably
					* from the command line.  Remove it so it gets
					* re-added in the expected order for iteration.
					*/
					TAILQ_REMOVE(&parent->head, seq_node, next);
				}
				else 
				{
					seq_node = ConfNodeNew();
					if (unlikely(seq_node == NULL)) {
						return -1;
					}
					seq_node->name = OBStrdup(sequence_node_name);
					if (unlikely(seq_node->name == NULL)) {
						OBFree(seq_node);
						return -1;
					}
					seq_node->val = OBStrdup(value);
					if (unlikely(seq_node->val == NULL)) {
						OBFree(seq_node->name);
						return -1;
					}
				}
				TAILQ_INSERT_TAIL(&parent->head, seq_node, next);	
			}
			else 
			{
				if (state == CONF_INCLUDE) 
				{
					OBLogInfo("Including configuration file %s.", value);
					if (ConfYamlHandleInclude(parent, value) != 0) {
						goto fail;
					}
					state = CONF_KEY;
				}
				else if (state == CONF_KEY) 
				{
					/* Top level include statements. */
					if ((strcmp(value, "include") == 0) && (parent == ConfGetRootNode())) 
					{
						state = CONF_INCLUDE;
						goto next;
					}

					if (parent->is_seq) 
					{
						if (parent->val == NULL) 
						{
							parent->val = OBStrdup(value);
							if (parent->val && strchr(parent->val, '_'))
								Mangle(parent->val);
						}
					}
					ConfNode *existing = ConfNodeLookupChild(parent, value);
					if (existing != NULL) 
					{
						if (!existing->final) 
						{
							OBLogInfo("Configuration node '%s' redefined.",existing->name);
							ConfNodePrune(existing);
						}
						node = existing;
					}
					else 
					{
						node = ConfNodeNew();
						node->name = OBStrdup(value);
						if (node->name && strchr(node->name, '_')) 
						{
							if (!(parent->name &&
								((strcmp(parent->name, "address-groups") == 0) ||
								(strcmp(parent->name, "port-groups") == 0)))) 
							{
								Mangle(node->name);
								if (mangle_errors < MANGLE_ERRORS_MAX) 
								{
									OBLogWarning(SC_WARN_DEPRECATED,
										"%s is deprecated. Please use %s on line %"PRIuMAX".",
										value, node->name, (uintmax_t)parser->mark.line+1);

									mangle_errors++;
									if (mangle_errors >= MANGLE_ERRORS_MAX)
										OBLogWarning(OB_WARN_DEPRECATED, "not showing more parameter name warnings.");
								}
							}
						}
						TAILQ_INSERT_TAIL(&parent->head, node, next);
					}
					state = CONF_VAL;
				}
				else 
				{
					if ((tag != NULL) && (strcmp(tag, "!include") == 0)) 
					{
						OBLogInfo("Including configuration file %s at parent node %s.", value, node->name);
						if (ConfYamlHandleInclude(node, value) != 0)
							goto fail;
					}
					else if (!node->final) 
					{
						if (node->val != NULL)
							OBFree(node->val);
						node->val = OBStrdup(value);
					}
					state = CONF_KEY;
				}
			}
		}
		else if (event.type == YAML_SEQUENCE_START_EVENT) 
		{
			OBLogDebug("event.type=YAML_SEQUENCE_START_EVENT; state=%d", state);
			if (ConfYamlParse(parser, node, 1) != 0)
				goto fail;
			state = CONF_KEY;
		}
		else if (event.type == YAML_SEQUENCE_END_EVENT) 
		{
			OBLogDebug("event.type=YAML_SEQUENCE_END_EVENT; state=%d", state);
			return 0;
		}
		else if (event.type == YAML_MAPPING_START_EVENT) 
		{
			OBLogDebug("event.type=YAML_MAPPING_START_EVENT; state=%d", state);
			if (inseq) 
			{
				char sequence_node_name[DEFAULT_NAME_LEN];
				snprintf(sequence_node_name, DEFAULT_NAME_LEN, "%d", seq_idx++);
				ConfNode *seq_node = ConfNodeLookupChild(node, sequence_node_name);
				if (seq_node != NULL) 
				{
					/* The sequence node has already been set, probably
					 * from the command line.  Remove it so it gets
					 * re-added in the expected order for iteration.
					 */
					TAILQ_REMOVE(&node->head, seq_node, next);
				}
				else 
				{
					seq_node = ConfNodeNew();
					if (unlikely(seq_node == NULL)) 
					{
						return -1;
					}
					seq_node->name = OBStrdup(sequence_node_name);
					if (unlikely(seq_node->name == NULL)) 
					{
						OBFree(seq_node);
						return -1;
					}
				}
				seq_node->is_seq = 1;
				TAILQ_INSERT_TAIL(&node->head, seq_node, next);
				if (ConfYamlParse(parser, seq_node, 0) != 0)
					goto fail;
			}
			else 
			{
				if (ConfYamlParse(parser, node, inseq) != 0)
					goto fail;
			}
			state = CONF_KEY;
		}
		else if (event.type == YAML_MAPPING_END_EVENT) {
			OBLogDebug("event.type=YAML_MAPPING_END_EVENT; state=%d", state);
			done = 1;
		}
		else if (event.type == YAML_STREAM_END_EVENT) {
			OBLogDebug("event.type=YAML_STREAM_END_EVENT; state=%d", state);
			done = 1;
		}

next:
        	yaml_event_delete(&event);
        	continue;

fail:
        	yaml_event_delete(&event);
        	return -1;
	}
	return 0;
}

int ConfLoadFile(char *filename)
{
	int ret = 0; 
	FILE *infile = NULL;
	yaml_parser_t parser;
	struct stat stat_buf;

	//...
	ConfNode *root = ConfGetRootNode();

	//.....
	if (yaml_parser_initialize(&parser) != 1) {
		OBLogError(OB_ERR_FATAL, "failed to initialize yaml parser.");
		return -1;
	}

	//...
	if (stat(filename, &stat_buf) == 0) 
	{
		if (stat_buf.st_mode & S_IFDIR) {
			OBLogError(OB_ERR_FATAL, "yaml argument is not a file but a directory: %s. "
                    		"Please specify the yaml file in your -c option.", filename);
			yaml_parser_delete(&parser);
            	return -1;
        	}
    	}

	//...
	infile = fopen(filename, "r");
	if (infile == NULL) {
		OBLogError(OB_ERR_FATAL, "failed to open file: %s: %s", filename, strerror(errno));
		yaml_parser_delete(&parser);
		return -1;
	}

	//...
	yaml_parser_set_input_file(&parser, infile);
	ret = ConfYamlParse(&parser, root, 0);
	yaml_parser_delete(&parser);
	fclose(infile);

	return ret;
}

