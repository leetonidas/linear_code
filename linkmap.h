#ifndef REV_LINKMAP_H
#define REV_LINKMAP_H

#include <stdint.h>
#include <elf.h>
#include <stdbool.h>

struct link_map;

struct r_scope_elem {
	/* Array of maps for the scope.  */
	struct link_map **r_list;
	/* Number of entries in the scope.  */
	unsigned int r_nlist;
};

struct r_search_path_struct {
	//struct r_search_path_elem **dirs;
	void **dirs;
	int malloced;
};

struct link_map {
	uintptr_t l_addr;
	char *l_name;
	void *l_dyn;
	struct link_map *l_prev;
	struct link_map *l_next;
	struct link_map *l_real;
	uint64_t l_ns;
	void *l_libname;
	Elf64_Dyn *l_info[DT_NUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM + DT_ADDRNUM];
	const Elf64_Phdr *l_phdr;	/* Pointer to program header table in core.  */
	Elf64_Addr l_entry;		/* Entry point location.  */
	Elf64_Half l_phnum;		/* Number of program header entries.  */
	Elf64_Half l_ldnum;		/* Number of dynamic segment entries.  */

	/* Array of DT_NEEDED dependencies and their dependencies, in
	   dependency order for symbol lookup (with and without
	   duplicates).  There is no entry before the dependencies have
	   been loaded.  */
	struct r_scope_elem l_searchlist;

	/* We need a special searchlist to process objects marked with
	   DT_SYMBOLIC.  */
	struct r_scope_elem l_symbolic_searchlist;

	/* Dependent object that first caused this object to be loaded.  */
	struct link_map *l_loader;

	/* Array with version names.  */
	void *l_versions;
	unsigned int l_nversions;

	/* Symbol hash table.  */
	//Elf_Symndx l_nbuckets;
	uint32_t l_nbuckets;
	Elf32_Word l_gnu_bitmask_idxbits;
	Elf32_Word l_gnu_shift;
	Elf64_Addr *l_gnu_bitmask;
	union {
		const Elf32_Word *l_gnu_buckets;
		const uint32_t *l_chain;
	};
	union {
		const Elf32_Word *l_gnu_chain_zero;
		const uint32_t *l_buckets;
	};

    unsigned int l_direct_opencount; /* Reference count for dlopen/dlclose.  */
    enum			/* Where this object came from.  */
      {
	lt_executable,		/* The main executable program.  */
	lt_library,		/* Library needed by main executable.  */
	lt_loaded		/* Extra run-time loaded shared object.  */
      } l_type:2;
    unsigned int l_dt_relr_ref:1; /* Nonzero if GLIBC_ABI_DT_RELR is
				     referenced.  */
    unsigned int l_relocated:1;	/* Nonzero if object's relocations done.  */
    unsigned int l_init_called:1; /* Nonzero if DT_INIT function called.  */
    unsigned int l_global:1;	/* Nonzero if object in _dl_global_scope.  */
    unsigned int l_reserved:2;	/* Reserved for internal use.  */
    unsigned int l_main_map:1;  /* Nonzero for the map of the main program.  */
    unsigned int l_visited:1;   /* Used internally for map dependency
				   graph traversal.  */
    unsigned int l_map_used:1;  /* These two bits are used during traversal */
    unsigned int l_map_done:1;  /* of maps in _dl_close_worker. */
    unsigned int l_phdr_allocated:1; /* Nonzero if the data structure pointed
					to by `l_phdr' is allocated.  */
    unsigned int l_soname_added:1; /* Nonzero if the SONAME is for sure in
				      the l_libname list.  */
    unsigned int l_faked:1;	/* Nonzero if this is a faked descriptor
				   without associated file.  */
    unsigned int l_need_tls_init:1; /* Nonzero if GL(dl_init_static_tls)
				       should be called on this link map
				       when relocation finishes.  */
    unsigned int l_auditing:1;	/* Nonzero if the DSO is used in auditing.  */
    unsigned int l_audit_any_plt:1; /* Nonzero if at least one audit module
				       is interested in the PLT interception.*/
    unsigned int l_removed:1;	/* Nozero if the object cannot be used anymore
				   since it is removed.  */
    unsigned int l_contiguous:1; /* Nonzero if inter-segment holes are
				    mprotected or if no holes are present at
				    all.  */
    unsigned int l_free_initfini:1; /* Nonzero if l_initfini can be
				       freed, ie. not allocated with
				       the dummy malloc in ld.so.  */
    unsigned int l_ld_readonly:1; /* Nonzero if dynamic section is readonly.  */
    unsigned int l_find_object_processed:1; /* Zero if _dl_find_object_update
					       needs to process this
					       lt_library map.  */

    /* NODELETE status of the map.  Only valid for maps of type
       lt_loaded.  Lazy binding sets l_nodelete_active directly,
       potentially from signal handlers.  Initial loading of an
       DF_1_NODELETE object set l_nodelete_pending.  Relocation may
       set l_nodelete_pending as well.  l_nodelete_pending maps are
       promoted to l_nodelete_active status in the final stages of
       dlopen, prior to calling ELF constructors.  dlclose only
       refuses to unload l_nodelete_active maps, the pending status is
       ignored.  */
    bool l_nodelete_active;
    bool l_nodelete_pending;

//#include <link_map.h>
// X86 link_map
    enum {
		lc_property_unknown = 0,		/* Unknown property status.  */
		lc_property_none = 1 << 0,		/* No property.  */
		lc_property_valid = 1 << 1		/* Has valid property.  */
  	} l_property:2;

	/* GNU_PROPERTY_X86_FEATURE_1_AND of this object.  */
	unsigned int l_x86_feature_1_and;

	/* GNU_PROPERTY_X86_ISA_1_NEEDED of this object.  */
	unsigned int l_x86_isa_1_needed;

// generic link_map
    uint32_t l_1_needed;
// link_map end

    /* Collected information about own RPATH directories.  */
    struct r_search_path_struct l_rpath_dirs;

    /* Collected results of relocation while profiling.  */
    void *l_reloc_result;

    /* Pointer to the version information if available.  */
    Elf64_Versym *l_versyms;

    /* String specifying the path where this object was found.  */
    const char *l_origin;

    /* Start and finish of memory map for this object.  l_map_start
       need not be the same as l_addr.  */
    Elf64_Addr l_map_start, l_map_end;
    /* End of the executable part of the mapping.  */
	Elf64_Addr l_text_end;
};

uint32_t _dl_new_hash (const char *str) __attribute__((pure,visibility("hidden")));

struct link_map *find_linkmap(uintptr_t starting_point) __attribute__((visibility("hidden")));

#endif // REV_LINKMAP_H
