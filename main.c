#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/user.h>

int install_observer(void);

/*
__attribute__((constructor))
static void guard() {
	puts(":(");
	exit(0);
}
*/

__asm__(".byte 11, 173, 192, 222, 222, 173, 190, 239");

// 25 * (25 + 1) / 2
static uint8_t graph[325] = {};

static int read_graph(char *inp) {
	int cn = 0;
	char *cc = inp;
	char last = 0;
	while (*cc) {
		if (*cc >= 'A' && *cc <= 'Z') {
//			printf("\t%c -- %c\n", cn + 'A', *cc);
			if (*cc <= last || (*cc - 'A') <= cn) return 1; // not alphabetically sorted
			int tmp = *cc - 'A';
			graph[(((26 - cn + 25) * cn) >> 1) + tmp - cn - 1] = 1;
		} else if(*cc >= '0' && *cc <= '9') {
			if (last <= '9') return 1; // two adjacent numbers
			cn += *cc - '/';
			if (cn > 25) return 1;
		} else if (*cc == '}' && cc[1] == 0)
			return cn != 25;
		else
			return 1;
		last = *cc++;
	}
	return 1;
}

__attribute__((const))
static int checkbfs(uint_fast8_t start, const uint8_t *chk) {
	uint_least32_t ma = (1 << (uint_least32_t)start), v = ma, mb;
	uint8_t ret = 0;
//	printf("starting at %c: ", start + 'A');
	while (ma) {
		mb = 0;
		for (uint_fast8_t i = 0; i < 26; ++i) {
			if ((ma & (1 << (uint_least32_t)i)) == 0)
				continue;
//			putchar(i + 'A');
			ret |= *chk++ ^ i;
			for (uint_fast8_t j = 0; j < i; ++j) {
				uint_least32_t tmp = 1 << (uint_least32_t)j;
				if (!graph[(((26 - j + 25) * j) >> 1) + i - j - 1] || (v & tmp)) continue;
				mb |= tmp;
				v |= tmp;
			}
			for (uint_fast8_t j = i + 1; j < 26; ++j) {
				uint_least32_t tmp = 1 << (uint_least32_t)j;
				if (!graph[(((26 - i + 25) * i) >> 1) + j - i - 1] || (v & tmp)) continue;
				mb |= tmp;
				v |= tmp;	
			}
		}
		ma = mb;
	}
//	putchar('\n');
	return ret | (~v & 0x3ffffff);
}

const static char sps[13] = "HXPREVFASTNOW";
static uint8_t bfss[13][26] =
	{ {45,61,60,46,63,62,57,33,35,41,38,32,34,40,56,59,39,44,47,43,50,37,42,58,36,51}
	, {61,51,37,32,35,60,33,34,44,46,47,42,38,56,36,50,45,58,39,62,41,43,40,63,57,59}
	, {37,44,57,62,33,41,47,43,46,51,42,59,50,45,35,56,58,34,61,36,60,32,63,38,40,39}
	, {59,47,51,60,38,43,50,37,36,34,56,57,42,63,61,32,46,44,40,39,58,62,33,35,45,41}
	, {46,37,43,44,36,61,41,47,45,63,51,60,50,38,58,62,39,59,42,33,34,56,35,57,32,40}
	, {63,33,60,59,62,42,61,46,57,51,36,37,32,56,43,40,39,34,50,45,58,35,47,41,44,38}
	, {47,58,35,33,38,40,46,60,57,36,63,41,42,61,39,50,51,62,43,45,32,34,44,56,59,37}
	, {42,41,59,56,51,40,46,57,63,47,62,44,58,35,37,61,43,60,38,32,36,50,34,45,33,39}
	, {56,46,44,42,35,57,41,63,62,37,39,60,38,61,36,50,34,32,43,33,40,58,45,51,59,47}
	, {57,33,63,51,41,32,61,35,36,37,56,62,50,38,47,34,46,45,60,42,40,43,44,58,39,59}
	, {39,62,34,63,58,32,44,33,36,45,40,59,38,47,60,46,35,57,61,50,51,37,56,42,43,41}
	, {36,46,60,39,61,32,40,50,44,45,38,37,43,56,47,62,42,59,63,41,51,58,34,35,33,57}
	, {60,56,44,62,37,63,39,46,35,51,45,41,36,38,61,47,50,34,32,59,58,33,40,43,42,57} };

void mfproxy(void *ptr, size_t l);

__attribute__((visibility("hidden")))
int main(int argc, char **argv) {
	char similey[3] = ":(";
	int cn = 0;
	int res = 0;

	if (!install_observer()) {
		puts(":(");
		return 1;
	}


	while (1) {
		if (argc != 2 || *(uint32_t*)argv[1] != 0x7b707868) {
			res = 1;
			break;
		}

		if (read_graph(argv[1] + 4)) {
			res = 1;
			break;
		}

		mfproxy(bfss[cn], 26);
		res |= checkbfs(sps[cn] - 'A', bfss[cn]);
		++cn;
		if (cn >= 13) break;
	};
	similey[1] += !res;
	puts(similey);
	return !!res;
}


__attribute__((used, visibility("hidden")))
void _ITM_consumeTMCloneTable(uint8_t *buf, size_t sz) {
//	puts("mymemfrob");
	for (size_t i = 0; i < sz; ++i)
		buf[i] ^= 42;
	//buf[sz - 1] ^= buf[sz - 2]; // A^B
	//buf[sz - 2] ^= buf[sz - 1]; // B^A^B = A
	//buf[sz - 1] ^= buf[sz - 2]; // A^B^A = B
	uint64_t *stack = (uint64_t*) __builtin_frame_address(0);
	//for (size_t i = 0; i < 40; ++i)
	//	printf("%lx\n", stack[offsetof(struct user_regs_struct, rip) / 8 + i]);
	//printf("%lx\n", stack[offsetof(struct user_regs_struct, rip) / 8 + 16]);
	stack[offsetof(struct user_regs_struct, rip) / 8 + 16] += 0x6520 - 0x11e9 - 4;
	stack[offsetof(struct user_regs_struct, rsp) / 8 + 16] += 0x18;
}

int ldpwnifunc(void);

__attribute__((used, visibility("hidden")))
void pwnhlp(void) {
	if (ldpwnifunc() != 0) {
		ldpwnifunc();
	}
}
