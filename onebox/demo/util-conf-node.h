#ifndef __UTIL_CONF_NODE_H__
#define __UTIL_CONF_NODE_H__

#include "ds-queue.h"

/**
 * Structure of a configuration parameter.
 */
typedef struct ConfNode_ {
    char *name;
    char *val;

    int is_seq;

    /**< Flag that sets this nodes value as final. */
    int final;

    struct ConfNode_ *parent;
    TAILQ_HEAD(, ConfNode_) head;
    TAILQ_ENTRY(ConfNode_) next;
} ConfNode;

void ConfInit(void);
void ConfDeInit(void);
ConfNode *ConfGetRootNode(void);
int ConfGet(char *name, char **vptr);
int ConfGetInt(char *name, intmax_t *val);
int ConfGetBool(char *name, int *val);
int ConfGetDouble(char *name, double *val);
int ConfGetFloat(char *name, float *val);
int ConfSet(char *name, char *val);
int ConfSetFinal(char *name, char *val);
void ConfDump(void);
void ConfNodeDump(ConfNode *node, const char *prefix);
ConfNode *ConfNodeNew(void);
void ConfNodeFree(ConfNode *);
ConfNode *ConfGetNode(char *key);
void ConfCreateContextBackup(void);
void ConfRestoreContextBackup(void);
ConfNode *ConfNodeLookupChild(ConfNode *node, const char *key);
const char *ConfNodeLookupChildValue(ConfNode *node, const char *key);
void ConfNodeRemove(ConfNode *);
void ConfRegisterTests();
int ConfNodeChildValueIsTrue(ConfNode *node, const char *key);
int ConfValIsTrue(const char *val);
int ConfValIsFalse(const char *val);
void ConfNodePrune(ConfNode *node);

ConfNode *ConfNodeLookupKeyValue(ConfNode *base, const char *key, const char *value);
int ConfGetChildValue(ConfNode *base, char *name, char **vptr);
int ConfGetChildValueInt(ConfNode *base, char *name, intmax_t *val);
int ConfGetChildValueBool(ConfNode *base, char *name, int *val);
int ConfGetChildValueWithDefault(ConfNode *base, ConfNode *dflt, char *name, char **vptr);
int ConfGetChildValueIntWithDefault(ConfNode *base, ConfNode *dflt, char *name, intmax_t *val);
int ConfGetChildValueBoolWithDefault(ConfNode *base, ConfNode *dflt, char *name, int *val);
char *ConfLoadCompleteIncludePath(char *);

#endif
