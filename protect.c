#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <sys/mman.h>

#include <elf.h>

#include "linkmap.h"

uint32_t _dl_new_hash (const char *str) {
	const uint8_t *s = (const uint8_t *) str;
	uint32_t h = 5381;
	uint32_t c0 = 0, c1 = 0;
	while (1) {
		c0 = s[0];
		if (c0 == 0)
			return h;

		c1 = s[1];
		if (c1 == 0) {
			c0 += h;
			h = h * 32 + c0;
			return h;
		}

		c1 += c0;
		h *= 33 * 33;
		c1 += c0 * 32;
		h += c1;
		s += 2;
	}
}

static struct link_map *getMapByHash(struct link_map *li, uint32_t hash) {
	for (;li;li = li->l_next) {
		char *name = li->l_name;
		for (char *cur = name; *cur; ++cur) {
			if (*cur == '/')
				name = cur + 1;
		}
		if (_dl_new_hash(name) == hash)
			return li;
	}
	return NULL;
}

static void *resolveInSlow(struct link_map *li, uint32_t hash) {
	Elf64_Sym *symtab = (Elf64_Sym*) li->l_info[DT_SYMTAB]->d_un.d_ptr;
	const char *strtab = (const char *) li->l_info[DT_STRTAB]->d_un.d_ptr;

	Elf64_Sym *sym = NULL;
	for (sym = symtab; _dl_new_hash(strtab + sym->st_name) != hash; ++sym) {}

	return (void *) (li->l_addr + sym->st_value);
}

void *resolveIn(struct link_map *li, uint32_t hash) {
	uint32_t idx;

	if (li->l_nbuckets == 0)
		return resolveInSlow(li, hash);

	Elf64_Sym *syms = (Elf64_Sym*) ((Elf64_Dyn *)li->l_info[DT_SYMTAB])->d_un.d_ptr;
	Elf64_Addr *bitmask = li->l_gnu_bitmask;
	if (bitmask != NULL) {
		Elf64_Addr bitmask_word = bitmask[(hash / 64) & li->l_gnu_bitmask_idxbits];

		uint32_t hashbit1 = hash & 63;
		uint32_t hashbit2 = (hash >> li->l_gnu_shift) & 63;

		if ((bitmask_word >> hashbit1) & (bitmask_word >> hashbit2) & 1) {
			Elf32_Word bucket = li->l_gnu_buckets[hash % li->l_nbuckets];
			if (bucket != 0) {
				const Elf32_Word *hasharr = &li->l_gnu_chain_zero[bucket];
				do {
					if (((*hasharr ^ hash) >> 1) == 0) {
						idx = hasharr - li->l_gnu_chain_zero;
						Elf64_Sym *sym = &syms[idx];
						return (void *) (li->l_addr + sym->st_value);
					}
				} while ((*hasharr++ & 1u) == 0);
			}
		}
	}
	return NULL;
}

static struct link_map *getMapContaining(struct link_map *maps, void *addr) {
	struct link_map *cur;
	for (cur = maps; cur; cur = cur->l_next) {
		uint8_t **walk = (uint8_t **)&cur->l_gnu_bitmask;
		while (*walk != (uint8_t*) cur->l_addr) {
			++walk;
		}

		if (walk[0] <= (uint8_t*) addr && (uint8_t*) addr < walk[1]) {
			return cur;
		}
	}

	return NULL;
}


#include <unistd.h>

static int overWriteSym(uintptr_t base, Elf64_Rela *begin, Elf64_Rela *end, const Elf64_Sym *syms, const char *strtab, uint32_t hash, uintptr_t tptr) {
	for (Elf64_Rela *r = begin; r < end; ++r) {
		const Elf64_Sym *sym = &syms[ELF64_R_SYM(r->r_info)];
		if (ELF64_ST_TYPE(sym->st_info) == STT_NOTYPE)
			continue;
		const char *name = strtab + sym->st_name;
		uint32_t ehash = _dl_new_hash(name);
		if (ehash != hash)
			continue;
		uintptr_t *rptr = (uintptr_t*) (base + r->r_offset);
		//printf("found target symbol: %s (currently: %#lx)\n", name, val);
		
		// for norelro
		//*rptr = tptr - base;
		// for RELRO, BIND_NOW
		*rptr = tptr;
		
		return 1;
	}
	return 0;
}

//void _ITM_consumeTMCloneTable(uint8_t *buf, size_t sz);
//void __obf_nop(void);

static void *ldpwn(void) {
	//__asm__ ("int $3\n\r");
	uintptr_t addr = (uintptr_t) __builtin_return_address(0);
	
	struct link_map *li = find_linkmap(addr);

	if (!li)
		return NULL;

	struct link_map *chall = NULL;
	for (struct link_map *tmp = li; tmp; tmp = tmp->l_next) {
		if (tmp->l_name[0] == 0)
			chall = tmp;
	}

//	struct link_map *libc = getMapByHash(li, 0x5fd7d493); //_dl_new_hash("libc.so.6"));
//	struct link_map *ld = getMapByHash(a, 0xf0cd994c); // ld-linux-x86-64.so.2;

	//printf("libc: %p\nld: %p\nchall: %p\n", libc, ld, chall);

	if (/*!libc || !ld ||*/ !chall) {
		return NULL;
	}

/*
	int (*dyn_printf)(const char *restrict, ...) = (int(*)(const char *restrict, ...)) resolveIn(libc, 0x156b2bb8);
	int (*dyn_mprotect)(void *, size_t, int) = (int(*)(void *, size_t, int)) resolveIn(libc, 0xd33b90b3);

	if (!dyn_printf || !dyn_mprotect) {
		return NULL;
	}

	dyn_printf("found printf: %p\n", printf);
*/


	Elf64_Rela *rel = (Elf64_Rela*) chall->l_info[DT_RELA]->d_un.d_ptr;
	Elf64_Rela *end = (Elf64_Rela*) (chall->l_info[DT_RELA]->d_un.d_ptr + chall->l_info[DT_RELASZ]->d_un.d_val);
	Elf64_Sym *symtab = (Elf64_Sym*) chall->l_info[DT_SYMTAB]->d_un.d_ptr;
	const char *strtab = (const char *) chall->l_info[DT_STRTAB]->d_un.d_ptr;

/*
	// print relocation entries
	for (Elf64_Rela *r = rel; r < end; ++r) {
		Elf64_Sym *sym = &symtab[ELF64_R_SYM(r->r_info)];
		uint64_t val = *(uint64_t*) (li->l_addr + r->r_offset);
		const char *name = "<no symbol>";
		if (ELF64_ST_TYPE(sym->st_info) != STT_NOTYPE)
			name = strtab + sym->st_name;
		if (r->r_addend)
			dyn_printf("%p: (%p) %s + %ld\n", r->r_offset, val, name, r->r_addend);
		else
			dyn_printf("%p: (%p) %s\n", r->r_offset, val, name);
	}
*/



	// find pointers to binary, end of text, ...
	uint8_t **walk = (uint8_t **)&chall->l_gnu_bitmask;
	while (*walk != (uint8_t*) chall->l_addr) {
//		dyn_printf("%p\n", *walk);
		++walk;
	}

//	dyn_printf("\timage text area: %p - %p\n", walk[0], walk[2]);
	size_t len = walk[2] - walk[0];


	uint64_t (*crc64_we)(uint8_t *data, size_t len) = (uint64_t (*)(uint8_t *data, size_t len)) (chall->l_addr + 0x6420);

	uint64_t crc = crc64_we(walk[0], (len + 0xf) & ~0xfull);
//	dyn_printf("%lx\n", crc);
	if (crc)
		return NULL;

	uint8_t *pptr = (uint8_t *) (chall->l_addr + 0x92e0);
	for (size_t i = 0; i < 25; ++i)
		pptr[i] = 25 - i;

	// _dl_new_hash("memfrob") = 0xbdd2d44d
	overWriteSym(chall->l_addr, rel, end, symtab, strtab, 0xbdd2d44d, (uintptr_t) chall->l_addr + 0x3410);

	return NULL;
}

void ldpwnifunc(void) __attribute__((ifunc("ldpwn")));
