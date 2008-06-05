typedef void ( *pg_signal_function)( int );
void pg_signal_initialize(void);
int sigblock(int mask);
int siggetmask(void);
int sigsetmask(int mask);
#define sigmask(sig) (1 << (sig - 1))
pg_signal_function signal(int signum, pg_signal_function handler);

int kill(int pid, int sig);
int raise(int sig);

extern volatile int __pg_signal_count;
void __pg_poll_signals(void);
#define PG_POLL_SIGNALS() if (__pg_signal_count > 0) { __pg_poll_signals(); }

#undef SIGHUP
#define	SIGHUP	1	/* hangup */

#undef	SIGINT	
#define	SIGINT	2	/* interrupt */

#undef	SIGQUIT	
#define	SIGQUIT	3	/* quit */

#undef	SIGILL	
#define	SIGILL	4	/* illegal instruction (not reset when caught) */

#undef	SIGTRAP	
#define	SIGTRAP	5	/* trace trap (not reset when caught) */

#undef	SIGABRT	
#define	SIGABRT	6	/* abort(void) */

#undef	SIGIOT	
#define	SIGIOT	SIGABRT	/* compatibility */

#undef	SIGEMT	
#define	SIGEMT	7	/* EMT instruction */

#undef	SIGFPE	
#define	SIGFPE	8	/* floating point exception */

#undef	SIGKILL	
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */

#undef	SIGBUS	
#define	SIGBUS	10	/* bus error */

#undef	SIGSEGV	
#define	SIGSEGV	11	/* segmentation violation */

#undef	SIGSYS	
#define	SIGSYS	12	/* non-existent system call invoked */

#undef	SIGSYS	
#define	SIGPIPE	13	/* write on a pipe with no one to read it */

#undef	SIGALRM	
#define	SIGALRM	14	/* alarm clock */

#undef	SIGTERM	
#define	SIGTERM	15	/* software termination signal from kill */

#undef	SIGURG	
#define	SIGURG	16	/* urgent condition on IO channel */

#undef	SIGSTOP	
#define	SIGSTOP	17	/* sendable stop signal not from tty */

#undef	SIGTSTP	
#define	SIGTSTP	18	/* stop signal from tty */

#undef	SIGCONT	
#define	SIGCONT	19	/* continue a stopped process */

#undef	SIGCHLD	
#define	SIGCHLD	20	/* to parent on child stop or exit */

#undef	SIGTTIN	
#define	SIGTTIN	21	/* to readers pgrp upon background tty read */

#undef	SIGTTOU	
#define	SIGTTOU	22	/* like TTIN for output if (tp->t_local&LTOSTOP) */

#undef	SIGIO	
#define	SIGIO	23	/* input/output possible signal */

#undef	SIGXCPU	
#define	SIGXCPU	24	/* exceeded CPU time limit */

#undef	SIGXFSZ	
#define	SIGXFSZ	25	/* exceeded file size limit */

#undef	SIGVTALR
#define	SIGVTALRM 26	/* virtual time alarm */

#undef	SIGPROF	
#define	SIGPROF	27	/* profiling time alarm */

#undef SIGWINCH 
#define SIGWINCH 28	/* window size changes */

#undef SIGINFO	
#define SIGINFO	29	/* information request */

#undef SIGUSR1 
#define SIGUSR1 30	/* user defined signal 1 */

#undef SIGUSR2 
#define SIGUSR2 31	/* user defined signal 2 */


#define SIG_DFL ((pg_signal_function)0)
#define SIG_ERR ((pg_signal_function)-1)
#define SIG_IGN ((pg_signal_function)1)
