#define _GNU_SOURCE
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <stdint.h>
#include <sched.h>
#include <string.h>

__attribute__((noinline, optnone, visibility("hidden")))
void mfproxy(void *ptr, size_t l) {
	memfrob(ptr, l);
}

volatile long i = 0x10000000;
volatile long u = 0;

static int unfrob_guest(pid_t pid) {
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	uint8_t data[26];
	
	struct iovec local, remote;
	local.iov_base = data;
	local.iov_len = sizeof(data);
	remote.iov_base = (void *)regs.rdi;
	if (regs.rsi != sizeof(data)) return 1;
	remote.iov_len = sizeof(data);
	
	size_t nread = process_vm_readv(pid, &local, 1, &remote, 1, 0);
	if (nread != sizeof(data)) return 1;

	memfrob(data, sizeof(data));
	nread = process_vm_writev(pid, &local, 1, &remote, 1, 0);
	if (nread != sizeof(data)) return 1;

	// to make patching out the breakpoint sufficiently easy
	regs.rip += 4;
	ptrace(PTRACE_SETREGS, pid, NULL, &regs);

	return 0;
}

__attribute__((visibility("hidden")))
int install_observer(void) {
	//printf("installing!\n");
	char buf[4] = {0};

	//prctl(PR_SET_DUMPABLE, 0);
	prctl(PR_SET_NO_NEW_PRIVS, 1);

	int pfds[2];
	if (pipe(pfds) == -1) {
//		perror("pipes");
		return -1;
	}

	pid_t pid = fork();
	if (pid == 0) {
//		printf("child\n");
		// child
		pid_t wr;
		int status;

		pid = getppid();
		//printf("child - parent: %d\n", pid);
		read(pfds[0], buf, 3);
		if (ptrace(PTRACE_SEIZE, pid, NULL, PTRACE_O_TRACESECCOMP | PTRACE_O_TRACECLONE | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD)) {
//			perror("ptrace seize");
			_exit(1);
		}
//		printf("seized\n");
		if (ptrace(PTRACE_INTERRUPT, pid, NULL, NULL) != 0) {
//			perror("ptrace interrupt");
			_exit(1);
		}
//		printf("waiting\n");
		wr = waitpid(pid, &status, 0);
		// TODO assert

		ptrace(PTRACE_POKEDATA, pid, &u, 1);
		ptrace(PTRACE_POKEDATA, pid, &i, 1);

		if (ptrace(PTRACE_CONT, pid, 0, 0) != 0)
			_exit(1);
//		printf("finished setup\n");
		while ((wr = wait(&status))) {
			if (WIFSTOPPED(status)) {
				int sig = WSTOPSIG(status);
				if (status >> 16) {
					// event
					// just ignore
//					printf("%5d: stopped for event\n", wr);
					ptrace(PTRACE_CONT, wr, 0, 0);
				} else {
//					printf("%5d: stopped with signal %d\n", wr, sig);
					if (sig == SIGTRAP && unfrob_guest(wr) == 0) {
						ptrace(PTRACE_CONT, wr, 0, 0);
					} else
						ptrace(PTRACE_CONT, wr, 0, sig);
				}
			} else if (WIFEXITED(status)) {
//				printf("%5d: exited\n", wr);
				if (wr == pid)
					_exit(0);
			} else if (WIFSIGNALED(status)) {
//				printf("%5d: dead by signal\n", wr);
				if (wr == pid)
					_exit(1);
			} else {
//				printf("%5d: other stop\n", wr);
				ptrace(PTRACE_CONT, wr, 0, 0);
			}
		}
//		printf("bye from the tracer!\n");
		_exit(0);
	} else {
//		printf("parent - child: %d\n", pid);
		// parent
		prctl(PR_SET_PTRACER, pid);
		write(pfds[1], "ok", 3);
	}

	for (i; i; --i) {}
	return u;
}
