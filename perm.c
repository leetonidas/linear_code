#include <stdio.h>
#include <stdint.h>

uint8_t perm[25] = {0};
__attribute__((used))
static void permute(uint8_t *arr) {
//	puts("permute");
	if (perm[0] == 0) {
		for (size_t i = 0; i < 25; ++i)
			perm[i] = i + 1;
	}

	uint8_t pvt = arr[0];
	for (size_t i = 0; i < 25; ++i) {
		uint8_t p = perm[i];
		uint8_t v = (p <= i) ? perm[p - 1] : arr[p];
		perm[i] = arr[i + 1];
		arr[i + 1] = v;
	}

	for (size_t i = 0; i < 25; ++i) {
		if (perm[i] < pvt)
			++perm[i];
	}
/*
	for (size_t i = 0; i < 26; ++i) {
		printf("%2hhu ", arr[i]);
	}
	puts("");

	for (size_t i = 0; i < 25; ++i) {
		printf("%2hhu ", perm[i]);
	}
	puts("");
*/
}
