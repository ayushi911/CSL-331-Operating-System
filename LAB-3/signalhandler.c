//Template to explain signal handler registration for various signals.

/*
*  General Comment: Please note some signals cannot be overridden by the user
* due to the permissions issue at the OS level and also the PC
* A common problem you might face if using cygwin environment
*/

/*new signal handler implementation*/
void newSignalHandler(int signo, void* some_other_stuff){

switch(signo)

	SIGINT:
	/*do something*/
	break;
	
	SIGKILL:
	/*do something*/
	break;	

	SIGBUS:
	/*do something*/
	break;
..
etc
..

}
void childStatus(signo, void *)
{
....Child ---? PID, What command it was? What is the exit status?
}

/*main program*/

int main(int argc, char *argv[]) {

/*declare a sigaction struct*/
struct sigaction sVal;

/* 
* initialize sVal to newSignalHandler
*/

sVal.sa_sigaction=newSignalHandler;

/* 
* Registration step
* For each signal  number use "sigaction" 
* library call to register the signal handler
*
* Older linux programs used the library call "signal" 
* to achieve the same, E.g.,     signal(SIGINT, HandleSignal);
*
* But using sigaction is more preferred than signal due to
* various reasons
*/

/*registering handler for SIGINT*/
sigaction (SIGINT, &sVal, NULL);

/*registering handler for SIGBUS*/
sigaction (SIGBUS, &sVal, NULL);
...

/*rest of main program (your shell program) follows here*/
...

}
