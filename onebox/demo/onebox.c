#include "onebox-common.h"
#include "onebox.h"
#include "util-error.h"
#include "util-time.h"
#include "util-daemon.h"
#include "util-cpu.h"
#include "util-conf-node.h"
#include "util-config.h"
#include "util-atomic.h"
#include "util-threads.h"
#include "test-config.h"
#include "util-pool.h"
#include "cli/util-cli.h"
#include "cli/cli.h"

/********************global vars*****************/
char *conf_filename = NULL;
volatile uint8_t onebox_ctl_flags = 0;

/********************functions*****************/
void GlobalInits(void)
{

}

static void OBInstanceInit(OBInstance *onebox)
{
	memset(onebox->pcap_dev, 0, sizeof(onebox->pcap_dev));
	onebox->pid_filename = NULL;
	onebox->daemon = 0;
	onebox->unittest = 0;
}

void EngineStop(void)
{
    onebox_ctl_flags |= ONEBOX_STOP;
}

void EngineKill(void) 
{
    onebox_ctl_flags |= ONEBOX_KILL;
}

void EngineDone(void) 
{
    onebox_ctl_flags |= ONEBOX_DONE;
}

static void PrintVersion(void)
{
	printf("This is %s version %s\n", PROG_NAME, PROG_VER);
}

static void PrintUsgae(const char *progname)
{
	printf("%s %s\n", PROG_NAME, PROG_VER);
	printf("USAGE: %s [OPTIONS]\n\n", progname);
	printf("\t-c <path>                            : path to configuration file\n");
	printf("\t-T                                   : unittest\n");
	printf("\t-D                                   : run in daemonizae\n");
	printf("\t-i <dev or ip>                       : run in pcap live mode\n");
	printf("\t-r <path>                            : run in pcap file/offline mode\n");
	printf("\t-h                                   : help\n");
	printf("\t--dump-config                        : dump config\n");
	printf("\t--pidfile                            : pid file path\n");
}

static void ParseCommandLine(int argc, char** argv, OBInstance *onebox)
{
	int opt;
	int dump_config = 0;

	struct option long_opts[] = {
        {"dump-config", 0, &dump_config, 1},
        {"pcap", optional_argument, 0, 0},
        {"pidfile", required_argument, 0, 0},
        {NULL, 0, NULL, 0}
	};

    /* getopt_long stores the option index here. */
    int option_index = 0;
    char short_opts[] = "Dc:i:r:Th";

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1)
    {
        switch (opt) {
		//long options
            case 0:
		    if (strcmp((long_opts[option_index]).name , "pidfile") == 0){
			if (optarg != NULL) {
				onebox->pid_filename = optarg;
			}
		    }
                break;

		//short options
		case 'D':
		    onebox->daemon = 1;
		    break;

		case 'c':
			conf_filename = optarg;
		    break;

		case 'i':
		    printf("interface is %s\r\n", optarg);
		    break;

		case 'r':
		    printf("pcapfile is %s\r\n", optarg);
		    break;

		case 'T':
		    onebox->unittest = 1;
		    break;

		case 'h':
		    PrintUsgae(argv[0]);
		    break;

            default:
                break;
        }
    }

    printf("dump-config is %d\r\n", dump_config);
}

void RegisterAllModules(void)
{

}

OBError LoadConfig(char *conf_filename) 
{
    if (conf_filename == NULL) return(OB_OK);

    if (ConfLoadFile(conf_filename) != 0) {
        /* Error already displayed. */
        return(OB_ERR_CONF_LOAD);
    }

    return(OB_OK);
}

int main(int argc, char **argv)
{
	OBInstance onebox ;

	/********print some status*********/
	PrintVersion();
	UtilCpuPrintSummary();

	/*********instance init ***********/
	OBInstanceInit(&onebox);

	/*********config init ***********/
	ConfInit();

	/**********parse command line*****/
	ParseCommandLine(argc, argv, &onebox);

	/***********unit test ************/
	//if(onebox.unittest == 1) return RunUnittests(0, onebox.regex_arg);

	/*********** global vars init******/
	GlobalInits();
	//TimeInit();

	/**********load config file *****/
	if (conf_filename == NULL) conf_filename = DEFAULT_CONF_FILE;

	if (LoadConfig(conf_filename) != OB_OK) {
		exit(EXIT_FAILURE);
	}

	ReadConfigTest();
	OBAtomicTest();
	UtilThreadTest();
	CliTest();

	/**********daemonize ***********/
	if(onebox.daemon == 1) Daemonize();

	return 0;
}
