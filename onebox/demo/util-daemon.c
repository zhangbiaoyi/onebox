#include "onebox-common.h"
#include "util-daemon.h"

static volatile sig_atomic_t sigflag = 0;

/**
 * \brief Signal handler used to take the parent process out of stand-by
 */
static void SignalHandlerSigusr1 (int signo) {
    sigflag = 1;
}

static void TellWaitingParent (pid_t pid) 
{
    kill(pid, SIGUSR1);
}

static void WaitForChild (pid_t pid) 
{
    int status;
    //SCLogDebug("Daemon: Parent waiting for child to be ready...");
    /* Wait until child signals is ready */
    while (sigflag == 0) {
        if (waitpid(pid, &status, WNOHANG)) {
            /* Check if the child is still there, otherwise the parent should exit */
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                //SCLogError(SC_ERR_DAEMON, "Child died unexpectedly");
                exit(EXIT_FAILURE);
            }
        }
        /* sigsuspend(); */
        sleep(1);
    }
}

/**
 * \brief Close stdin, stdout, stderr.Redirect logging info to syslog
 *
 */
static void SetupLogging (void) {
    /* Redirect stdin, stdout, stderr to /dev/null  */
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0)
        return;
    if (dup2(fd, 0) < 0)
        return;
    if (dup2(fd, 1) < 0)
        return;
    if (dup2(fd, 2) < 0)
        return;
    close(fd);
}

void Daemonize (void)
{
    pid_t pid, sid;

    /* Register the signal handler */
    signal(SIGUSR1, SignalHandlerSigusr1);

    /* Creates a new process */
    pid = fork();

    if (pid < 0) {
        /* Fork error */
        //SCLogError(SC_ERR_DAEMON, "Error forking the process");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        /* Child continues here */
        //char *daemondir;

        umask(027);
        sid = setsid();
        if (sid < 0) {
            //SCLogError(SC_ERR_DAEMON, "Error creating new session");
            exit(EXIT_FAILURE);
           }

	   if (chdir("/") < 0) {
		//SCLogError(SC_ERR_DAEMON, "Error changing to working directory '/'");
            }

        SetupLogging();

        /* Child is ready, tell its parent */
        TellWaitingParent(getppid());

        /* Daemon is up and running */
        //SCLogDebug("Daemon is running");
        return;
    }

    /* Parent continues here, waiting for child to be ready */
    //SCLogDebug("Parent is waiting for child to be ready");
    WaitForChild(pid);

    /* Parent exits */
    //SCLogDebug("Child is ready, parent exiting");
    exit(EXIT_SUCCESS);
}
