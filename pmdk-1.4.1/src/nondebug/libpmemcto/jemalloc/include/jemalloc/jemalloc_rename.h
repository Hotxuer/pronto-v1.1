/*
 * Name mangling for public symbols is controlled by --with-mangling and
 * --with-jemalloc-prefix.  With default settings the je_ prefix is stripped by
 * these macro definitions.
 */
#ifndef JEMALLOC_NO_RENAME
#  define je_pool_create je_cto_pool_create
#  define je_pool_delete je_cto_pool_delete
#  define je_pool_malloc je_cto_pool_malloc
#  define je_pool_calloc je_cto_pool_calloc
#  define je_pool_ralloc je_cto_pool_ralloc
#  define je_pool_aligned_alloc je_cto_pool_aligned_alloc
#  define je_pool_free je_cto_pool_free
#  define je_pool_malloc_usable_size je_cto_pool_malloc_usable_size
#  define je_pool_malloc_stats_print je_cto_pool_malloc_stats_print
#  define je_pool_extend je_cto_pool_extend
#  define je_pool_set_alloc_funcs je_cto_pool_set_alloc_funcs
#  define je_pool_check je_cto_pool_check
#  define je_malloc_conf je_cto_malloc_conf
#  define je_malloc_message je_cto_malloc_message
#  define je_malloc je_cto_malloc
#  define je_calloc je_cto_calloc
#  define je_posix_memalign je_cto_posix_memalign
#  define je_aligned_alloc je_cto_aligned_alloc
#  define je_realloc je_cto_realloc
#  define je_free je_cto_free
#  define je_mallocx je_cto_mallocx
#  define je_rallocx je_cto_rallocx
#  define je_xallocx je_cto_xallocx
#  define je_sallocx je_cto_sallocx
#  define je_dallocx je_cto_dallocx
#  define je_nallocx je_cto_nallocx
#  define je_mallctl je_cto_mallctl
#  define je_mallctlnametomib je_cto_mallctlnametomib
#  define je_mallctlbymib je_cto_mallctlbymib
#  define je_navsnprintf je_cto_navsnprintf
#  define je_malloc_stats_print je_cto_malloc_stats_print
#  define je_malloc_usable_size je_cto_malloc_usable_size
#  define je_memalign je_cto_memalign
#  define je_valloc je_cto_valloc
#endif
