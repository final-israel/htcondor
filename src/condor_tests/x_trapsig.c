#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile int signalcaught = 0;

void catchsig(int sig);

struct sigaction sa;
struct sigaction old;

void catchsig( int sig )
{
		/*
		printf("Generate lots of output.................\n");
        printf("%d\n",sig);
		fflush(stdout);
		*/
		/*fsync(stdout);*/
        printf("%d\n",sig);
		fflush(stdout);
		signalcaught = sig;
		exit(sig);
}

int main(int argc, char **argv)
{
	int res = 0;

	/* do not assign sa_handler and sa_sigaction at the same time since on some
		architectures those two are a union */
	sa.sa_handler = catchsig;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NODEFER;

	/*signal( 1, (sighandler_t)&catchsig);*/
	res = sigaction( 1, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 1\n");
	}
	res = sigaction( 2, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 2\n");
	}
	res = sigaction( 3, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 3\n");
	}
	res = sigaction( 4, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 4\n");
	}
	res = sigaction( 5, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 5\n");
	}
	res = sigaction( 6, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 6\n");
	}
	res = sigaction( 7, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 7\n");
	}
	res = sigaction( 8, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 8\n");
	}
	/*
	**res = sigaction( 9, &sa, &old);
	**if(res != 0 )
	**
		**printf("failed to replace handler intr 9\n");
	**
	*/
	res = sigaction( 10, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 10\n");
	}
	res = sigaction( 11, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 11\n");
	}
	res = sigaction( 12, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 12\n");
	}
	res = sigaction( 13, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 13\n");
	}
	res = sigaction( 14, &sa, &old);
	if(res != 0 )
	{
		printf("failed to replace handler intr 14\n");
	}
	while(1);

	return 0;
}
