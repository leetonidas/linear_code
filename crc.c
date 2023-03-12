#include <x86intrin.h>
#include <stdint.h>

typedef long long int v64x2 __attribute__((vector_size(16)));
/*
sage foo:

gf.<x> = GF(2)[]
r_we = gf([*map(int,bin(0x42F0E1EBA9EA3693)[:1:-1])]) + x^64
crc64f.<y> = GF(2)["x"].quotient(r_we)

# y^192 % r_we
hex(int("".join(map(str, list(y^192)))[::-1],2))

# y^128 % r_we
hex(int("".join(map(str, list(y^128)))[::-1],2))

# x^128 // r_we
hex(int("".join(map(str, list((x^128 // r_we))))[::-1],2))
*/

// see https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/fast-crc-computation-generic-polynomials-pclmulqdq-paper.pdf
__attribute__((used,visibility("hidden")))
uint64_t crc64_we(uint8_t *data, size_t len) {
	v64x2 *fast = (v64x2 *) data;
	// p = 0x42F0E1EBA9EA3693
	// y^128, y^192
	v64x2 poly = {0x5f5c3c7eb52fab6, 0x4eb938a7d257740e};
	//printf("poly: %#llx %#llx\n", poly[1], poly[0]);
	v64x2 old = {0, -1};
	//v64x2 old = {0, 0};
	v64x2 shfl = {0x08090a0b0c0d0e0full, 0x0001020304050607ull};
	//v64x2 shfl = {0x0706050403020100ull, 0x0f0e0d0c0b0a0908ull};

	// fold 128 at a time
	size_t fast_rounds = len >> 4;
	for (unsigned i = 0; i + 1 < fast_rounds; ++i) {
		old ^= _mm_shuffle_epi8(fast[i], shfl);
		//printf("old: 0x%llx%016llx\n", old[1], old[0]);
		v64x2 r1 = _mm_clmulepi64_si128(poly, old, 0x11);
		//printf("r1:  0x%llx%016llx\n", r1[1], r1[0]);
		v64x2 r2 = _mm_clmulepi64_si128(poly, old, 0x00);
		//printf("r2:  0x%llx%016llx\n", r2[1], r2[0]);
		old = r1 ^ r2;
	}

	// fold upper 64, such that 64 0 bits are appended
	old ^= _mm_shuffle_epi8(fast[fast_rounds - 1], shfl);
	//printf("old: 0x%llx%016llx\n", old[1], old[0]);

	v64x2 p0 = _mm_clmulepi64_si128(poly, old, 0x10);
	p0[1] ^= old[0];
	//printf("p0:  0x%llx%016llx\n", p0[1], p0[0]);

	// barrett reduce
	poly[0] = 0x42F0E1EBA9EA3693;
	poly[1] = 0x578d29d06cc4f872;
	//printf("poly:0x%llx%016llx\n", poly[1], poly[0]);
	v64x2 p1 = _mm_clmulepi64_si128(poly, p0, 0x11);
	p1[1] ^= p0[1];
	//printf("p1:  0x%llx%016llx\n", p1[1], p1[0]);
	

	v64x2 p2 = _mm_clmulepi64_si128(poly, p1, 0x10);
	p2[1] ^= p1[1];
	//printf("p2:  0x%llx%016llx\n", p2[1], p2[0]);

	v64x2 res = p0 ^ p2;
	//printf("res: 0x%llx%016llx\n", res[1], res[0]);
	return ((uint64_t) res[0]) ^ 0xffffffffffffffffull;
	// TODO handle len not multiple of 128
}
