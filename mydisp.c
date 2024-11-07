#define _DEFAULT_SOURCE          // needed for syscall
#define _POSIX_C_SOURCE 199309L  // needed for sigaction and siginfo_t
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <ucontext.h>
#include <unistd.h>

#include "syscall.h"

#ifndef PR_SET_SYSCALL_USER_DISPATCH
#pragma message "PR_SET_SYSCALL_USER_DISPATCH was not defined in the system headers, this feature might not be supported by the host's kernel"
/* Dispatch syscalls to a userspace handler */
#define PR_SET_SYSCALL_USER_DISPATCH	59
# define PR_SYS_DISPATCH_OFF		0
# define PR_SYS_DISPATCH_ON		1
/* The control values for the user space selector when dispatch is enabled */
# define SYSCALL_DISPATCH_FILTER_ALLOW	0
# define SYSCALL_DISPATCH_FILTER_BLOCK	1
#endif

#define REG_RDI 8
#define REG_RSI 9
#define REG_RDX 12
#define REG_RAX 13

//extern void __restore_rt(void);
int main(int argc, char **argv);

#define SYSCODE_START 0 //&__restore_rt
#define SYSCODE_SIZE  16 //(size_t)0x00080af0

static volatile char syscall_filter = SYSCALL_DISPATCH_FILTER_ALLOW;

#define UNUSED(n) (void)(n)

static void setup_filter(void)
{
	void *handle = dlopen("libc.so.6", RTLD_LAZY | RTLD_NOLOAD);
	if (handle == NULL) {
		fprintf(stderr, "dlopen: %s\n", dlerror());
		exit(1);
	}
	//printf("libc at %p\n", handle);
	void *sym = dlsym(handle, "killpg");
	if (sym == NULL) {
		fprintf(stderr, "dlsym: %s\n", dlerror());
		exit(1);
	}
	//printf("killpg at %p\n", sym);
	// crazy hack to only allow signal handling code to execute syscalls
	int result = prctl(PR_SET_SYSCALL_USER_DISPATCH,
			   PR_SYS_DISPATCH_ON,
			   sym, 256, //SYSCODE_START, SYSCODE_SIZE,
			   &syscall_filter);
	if (result != 0) {
		if (errno == EINVAL)
			fprintf(stderr, "Your kernel does not support PR_SET_SYSCALL_USER_DISPATCH.\n"
					"Please upgrade to Linux 5.11 or later to use this program.\n\n");
		else
			perror("prctl");
		exit(1);
	}
}

static void sigsys_handler(int signo, siginfo_t *info, void *context)
{
	UNUSED(signo);
	UNUSED(info);

	syscall_filter = SYSCALL_DISPATCH_FILTER_ALLOW;

	// this code is highly platform-specific (it will only work on x86_64)
	ucontext_t *ctx = (ucontext_t *)context;
#ifdef __x86_64__

    // capture all syscall and prints args 
	fprintf(stderr, "syscall: rax = %016llx, rdi = %016llx, rsi = %016llx, rdx = %016llx\n",
		ctx->uc_mcontext.gregs[REG_RAX],
		ctx->uc_mcontext.gregs[REG_RDI],
		ctx->uc_mcontext.gregs[REG_RSI],
		ctx->uc_mcontext.gregs[REG_RDX]);

	if (ctx->uc_mcontext.gregs[REG_RAX] == 39) { // if is getpid
		fprintf(stderr, "getting fake pid\n");
		ctx->uc_mcontext.gregs[REG_RAX] = 666; // return for x86_64 syscall is REG_RAX
	}
#else
	UNUSED(ctx);
#pragma message "Unsupported architecture. No information will be printed for syscalls."
#endif

	syscall_filter = SYSCALL_DISPATCH_FILTER_BLOCK;
}

int main(int argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);

	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &sigsys_handler;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGSYS, &sa, NULL);

	setup_filter();
	printf("Filter setup. Enabling, and jumping to custom code now...\n");
	syscall_filter = SYSCALL_DISPATCH_FILTER_BLOCK;

	int pid = getpid();

	syscall_filter = SYSCALL_DISPATCH_FILTER_ALLOW;
	printf("Done.\n");

    printf("fake pid is %d\n", pid);
    printf("real pid is %d\n", getpid());

	return 0;
}
