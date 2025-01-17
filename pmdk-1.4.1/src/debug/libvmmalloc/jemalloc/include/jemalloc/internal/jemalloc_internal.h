#ifndef JEMALLOC_INTERNAL_H
#define	JEMALLOC_INTERNAL_H

#include "jemalloc_internal_defs.h"
#include "jemalloc/internal/jemalloc_internal_decls.h"

#ifdef JEMALLOC_UTRACE
#include <sys/ktrace.h>
#endif

#define	JEMALLOC_NO_DEMANGLE
#ifdef JEMALLOC_JET
#  define JEMALLOC_N(n) jet_##n
#  include "jemalloc/internal/public_namespace.h"
#  define JEMALLOC_NO_RENAME
#  include "jemalloc/jemalloc.h"
#  undef JEMALLOC_NO_RENAME
#else
#  define JEMALLOC_N(n) je_vmem_je_##n
#  include "jemalloc/jemalloc.h"
#endif
#include "jemalloc/internal/private_namespace.h"

static const bool config_debug =
#ifdef JEMALLOC_DEBUG
    true
#else
    false
#endif
    ;
static const bool have_dss =
#ifdef JEMALLOC_DSS
    true
#else
    false
#endif
    ;
static const bool config_fill =
#ifdef JEMALLOC_FILL
    true
#else
    false
#endif
    ;
static const bool config_lazy_lock =
#ifdef JEMALLOC_LAZY_LOCK
    true
#else
    false
#endif
    ;
static const bool config_prof =
#ifdef JEMALLOC_PROF
    true
#else
    false
#endif
    ;
static const bool config_prof_libgcc =
#ifdef JEMALLOC_PROF_LIBGCC
    true
#else
    false
#endif
    ;
static const bool config_prof_libunwind =
#ifdef JEMALLOC_PROF_LIBUNWIND
    true
#else
    false
#endif
    ;
static const bool config_munmap =
#ifdef JEMALLOC_MUNMAP
    true
#else
    false
#endif
    ;
static const bool config_stats =
#ifdef JEMALLOC_STATS
    true
#else
    false
#endif
    ;
static const bool config_tcache =
#ifdef JEMALLOC_TCACHE
    true
#else
    false
#endif
    ;
static const bool config_tls =
#ifdef JEMALLOC_TLS
    true
#else
    false
#endif
    ;
static const bool config_utrace =
#ifdef JEMALLOC_UTRACE
    true
#else
    false
#endif
    ;
static const bool config_valgrind =
#ifdef JEMALLOC_VALGRIND
    true
#else
    false
#endif
    ;
static const bool config_xmalloc =
#ifdef JEMALLOC_XMALLOC
    true
#else
    false
#endif
    ;
static const bool config_ivsalloc =
#ifdef JEMALLOC_IVSALLOC
    true
#else
    false
#endif
    ;

#ifdef JEMALLOC_ATOMIC9
#include <machine/atomic.h>
#endif

#if (defined(JEMALLOC_OSATOMIC) || defined(JEMALLOC_OSSPIN))
#include <libkern/OSAtomic.h>
#endif

#ifdef JEMALLOC_ZONE
#include <mach/mach_error.h>
#include <mach/mach_init.h>
#include <mach/vm_map.h>
#include <malloc/malloc.h>
#endif

#define	RB_COMPACT
#include "jemalloc/internal/rb.h"
#include "jemalloc/internal/qr.h"
#include "jemalloc/internal/ql.h"

/*
 * jemalloc can conceptually be broken into components (arena, tcache, etc.),
 * but there are circular dependencies that cannot be broken without
 * substantial performance degradation.  In order to reduce the effect on
 * visual code flow, read the header files in multiple passes, with one of the
 * following cpp variables defined during each pass:
 *
 *   JEMALLOC_H_TYPES   : Preprocessor-defined constants and psuedo-opaque data
 *                        types.
 *   JEMALLOC_H_STRUCTS : Data structures.
 *   JEMALLOC_H_EXTERNS : Extern data declarations and function prototypes.
 *   JEMALLOC_H_INLINES : Inline functions.
 */
/******************************************************************************/
#define	JEMALLOC_H_TYPES

#include "jemalloc/internal/jemalloc_internal_macros.h"

#define	MALLOCX_LG_ALIGN_MASK	((int)0x3f)

/* Smallest size class to support. */
#define	LG_TINY_MIN		3
#define	TINY_MIN		(1U << LG_TINY_MIN)

/*
 * Minimum alignment of allocations is 2^LG_QUANTUM bytes (ignoring tiny size
 * classes).
 */
#ifndef LG_QUANTUM
#  if (defined(__i386__) || defined(_M_IX86))
#    define LG_QUANTUM		4
#  endif
#  ifdef __ia64__
#    define LG_QUANTUM		4
#  endif
#  ifdef __alpha__
#    define LG_QUANTUM		4
#  endif
#  ifdef __sparc64__
#    define LG_QUANTUM		4
#  endif
#  if (defined(__amd64__) || defined(__x86_64__) || defined(_M_X64))
#    define LG_QUANTUM		4
#  endif
#  ifdef __arm__
#    define LG_QUANTUM		3
#  endif
#  ifdef __aarch64__
#    define LG_QUANTUM		4
#  endif
#  ifdef __hppa__
#    define LG_QUANTUM		4
#  endif
#  ifdef __mips__
#    define LG_QUANTUM		3
#  endif
#  ifdef __powerpc__
#    define LG_QUANTUM		4
#  endif
#  ifdef __s390__
#    define LG_QUANTUM		4
#  endif
#  ifdef __SH4__
#    define LG_QUANTUM		4
#  endif
#  ifdef __tile__
#    define LG_QUANTUM		4
#  endif
#  ifdef __le32__
#    define LG_QUANTUM		4
#  endif
#  ifndef LG_QUANTUM
#    error "No LG_QUANTUM definition for architecture; specify via CPPFLAGS"
#  endif
#endif

#define	QUANTUM			((size_t)(1U << LG_QUANTUM))
#define	QUANTUM_MASK		(QUANTUM - 1)

/* Return the smallest quantum multiple that is >= a. */
#define	QUANTUM_CEILING(a)						\
	(((a) + QUANTUM_MASK) & ~QUANTUM_MASK)

#define	LONG			((size_t)(1U << LG_SIZEOF_LONG))
#define	LONG_MASK		(LONG - 1)

/* Return the smallest long multiple that is >= a. */
#define	LONG_CEILING(a)							\
	(((a) + LONG_MASK) & ~LONG_MASK)

#define	SIZEOF_PTR		(1U << LG_SIZEOF_PTR)
#define	PTR_MASK		(SIZEOF_PTR - 1)

/* Return the smallest (void *) multiple that is >= a. */
#define	PTR_CEILING(a)							\
	(((a) + PTR_MASK) & ~PTR_MASK)

/*
 * Maximum size of L1 cache line.  This is used to avoid cache line aliasing.
 * In addition, this controls the spacing of cacheline-spaced size classes.
 *
 * CACHELINE cannot be based on LG_CACHELINE because __declspec(align()) can
 * only handle raw constants.
 */
#define	LG_CACHELINE		6
#define	CACHELINE		64
#define	CACHELINE_MASK		(CACHELINE - 1)

/* Return the smallest cacheline multiple that is >= s. */
#define	CACHELINE_CEILING(s)						\
	(((s) + CACHELINE_MASK) & ~CACHELINE_MASK)

/* Page size.  STATIC_PAGE_SHIFT is determined by the configure script. */
#ifdef PAGE_MASK
#  undef PAGE_MASK
#endif
#define	LG_PAGE		STATIC_PAGE_SHIFT
#define	PAGE		((size_t)(1U << STATIC_PAGE_SHIFT))
#define	PAGE_MASK	((size_t)(PAGE - 1))

/* Return the smallest pagesize multiple that is >= s. */
#define	PAGE_CEILING(s)							\
	(((s) + PAGE_MASK) & ~PAGE_MASK)

/* Return the nearest aligned address at or below a. */
#define	ALIGNMENT_ADDR2BASE(a, alignment)				\
	((void *)((uintptr_t)(a) & (-(alignment))))

/* Return the offset between a and the nearest aligned address at or below a. */
#define	ALIGNMENT_ADDR2OFFSET(a, alignment)				\
	((size_t)((uintptr_t)(a) & (alignment - 1)))

/* Return the smallest alignment multiple that is >= s. */
#define	ALIGNMENT_CEILING(s, alignment)					\
	(((s) + (alignment - 1)) & (-(alignment)))

/* Declare a variable length array */
#if __STDC_VERSION__ < 199901L
#  ifdef _MSC_VER
#    include <malloc.h>
#ifndef alloca
#    define alloca _alloca
#endif
#  else
#    ifdef JEMALLOC_HAS_ALLOCA_H
#      include <alloca.h>
#    else
#      include <stdlib.h>
#    endif
#  endif
#  define VARIABLE_ARRAY(type, name, count) \
	type *name = alloca(sizeof(type) * (count))
#else
#  define VARIABLE_ARRAY(type, name, count) type name[(count)]
#endif

#include "jemalloc/internal/valgrind.h"
#include "jemalloc/internal/util.h"
#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/prng.h"
#include "jemalloc/internal/ckh.h"
#include "jemalloc/internal/size_classes.h"
#include "jemalloc/internal/stats.h"
#include "jemalloc/internal/ctl.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/tsd.h"
#include "jemalloc/internal/mb.h"
#include "jemalloc/internal/extent.h"
#include "jemalloc/internal/arena.h"
#include "jemalloc/internal/bitmap.h"
#include "jemalloc/internal/base.h"
#include "jemalloc/internal/chunk.h"
#include "jemalloc/internal/huge.h"
#include "jemalloc/internal/rtree.h"
#include "jemalloc/internal/tcache.h"
#include "jemalloc/internal/hash.h"
#include "jemalloc/internal/quarantine.h"
#include "jemalloc/internal/prof.h"
#include "jemalloc/internal/pool.h"
#include "jemalloc/internal/vector.h"

#undef JEMALLOC_H_TYPES
/******************************************************************************/
#define	JEMALLOC_H_STRUCTS

#include "jemalloc/internal/valgrind.h"
#include "jemalloc/internal/util.h"
#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/prng.h"
#include "jemalloc/internal/ckh.h"
#include "jemalloc/internal/size_classes.h"
#include "jemalloc/internal/stats.h"
#include "jemalloc/internal/ctl.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/tsd.h"
#include "jemalloc/internal/mb.h"
#include "jemalloc/internal/bitmap.h"
#include "jemalloc/internal/extent.h"
#include "jemalloc/internal/arena.h"
#include "jemalloc/internal/base.h"
#include "jemalloc/internal/chunk.h"
#include "jemalloc/internal/huge.h"
#include "jemalloc/internal/rtree.h"
#include "jemalloc/internal/tcache.h"
#include "jemalloc/internal/hash.h"
#include "jemalloc/internal/quarantine.h"
#include "jemalloc/internal/prof.h"
#include "jemalloc/internal/pool.h"
#include "jemalloc/internal/vector.h"

typedef struct {
	uint64_t	allocated;
	uint64_t	deallocated;
} thread_allocated_t;
/*
 * The JEMALLOC_ARG_CONCAT() wrapper is necessary to pass {0, 0} via a cpp macro
 * argument.
 */
#define	THREAD_ALLOCATED_INITIALIZER	JEMALLOC_ARG_CONCAT({0, 0})

#undef JEMALLOC_H_STRUCTS
/******************************************************************************/
#define	JEMALLOC_H_EXTERNS

extern bool	opt_abort;
extern bool	opt_junk;
extern size_t	opt_quarantine;
extern bool	opt_redzone;
extern bool	opt_utrace;
extern bool	opt_xmalloc;
extern bool	opt_zero;
extern size_t	opt_narenas;

extern bool	in_valgrind;

/* Number of CPUs. */
extern unsigned	ncpus;

extern unsigned	npools;
extern unsigned	npools_cnt;

extern pool_t	base_pool;
extern pool_t	**pools;

extern malloc_mutex_t	pools_lock;
extern void	*(*base_malloc_fn)(size_t);
extern void	(*base_free_fn)(void *);
extern bool	pools_shared_data_create(void);

arena_t	*arenas_extend(pool_t *pool, unsigned ind);
bool	arenas_tsd_extend(tsd_pool_t *tsd, unsigned len);
void	arenas_cleanup(void *arg);
arena_t	*choose_arena_hard(pool_t *pool);
void	jemalloc_prefork(void);
void	jemalloc_postfork_parent(void);
void	jemalloc_postfork_child(void);

#include "jemalloc/internal/valgrind.h"
#include "jemalloc/internal/util.h"
#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/prng.h"
#include "jemalloc/internal/ckh.h"
#include "jemalloc/internal/size_classes.h"
#include "jemalloc/internal/stats.h"
#include "jemalloc/internal/ctl.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/tsd.h"
#include "jemalloc/internal/mb.h"
#include "jemalloc/internal/bitmap.h"
#include "jemalloc/internal/extent.h"
#include "jemalloc/internal/arena.h"
#include "jemalloc/internal/base.h"
#include "jemalloc/internal/chunk.h"
#include "jemalloc/internal/huge.h"
#include "jemalloc/internal/rtree.h"
#include "jemalloc/internal/tcache.h"
#include "jemalloc/internal/hash.h"
#include "jemalloc/internal/quarantine.h"
#include "jemalloc/internal/prof.h"
#include "jemalloc/internal/pool.h"
#include "jemalloc/internal/vector.h"

#undef JEMALLOC_H_EXTERNS
/******************************************************************************/
#define	JEMALLOC_H_INLINES

#include "jemalloc/internal/pool.h"
#include "jemalloc/internal/valgrind.h"
#include "jemalloc/internal/util.h"
#include "jemalloc/internal/atomic.h"
#include "jemalloc/internal/prng.h"
#include "jemalloc/internal/ckh.h"
#include "jemalloc/internal/size_classes.h"
#include "jemalloc/internal/stats.h"
#include "jemalloc/internal/ctl.h"
#include "jemalloc/internal/mutex.h"
#include "jemalloc/internal/tsd.h"
#include "jemalloc/internal/mb.h"
#include "jemalloc/internal/extent.h"
#include "jemalloc/internal/base.h"
#include "jemalloc/internal/chunk.h"
#include "jemalloc/internal/huge.h"

/*
 * Include arena.h the first time in order to provide inline functions for this
 * header's inlines.
 */
#define	JEMALLOC_ARENA_INLINE_A
#include "jemalloc/internal/arena.h"
#undef JEMALLOC_ARENA_INLINE_A

#ifndef JEMALLOC_ENABLE_INLINE
malloc_tsd_protos(JEMALLOC_ATTR(unused), arenas, tsd_pool_t)

size_t	s2u(size_t size);
size_t	sa2u(size_t size, size_t alignment);
unsigned	narenas_total_get(pool_t *pool);
arena_t	*choose_arena(arena_t *arena);
#endif

#if (defined(JEMALLOC_ENABLE_INLINE) || defined(JEMALLOC_C_))
/*
 * Map of pthread_self() --> arenas[???], used for selecting an arena to use
 * for allocations.
 */
malloc_tsd_externs(arenas, tsd_pool_t)
malloc_tsd_funcs(JEMALLOC_ALWAYS_INLINE, arenas, tsd_pool_t, {0},
    arenas_cleanup)


/*
 * Check if the arena is dummy.
 */
JEMALLOC_ALWAYS_INLINE bool
is_arena_dummy(arena_t *arena) {
	return (arena->ind == ARENA_DUMMY_IND);
}

/*
 * Compute usable size that would result from allocating an object with the
 * specified size.
 */
JEMALLOC_ALWAYS_INLINE size_t
s2u(size_t size)
{

	if (size <= SMALL_MAXCLASS)
		return (small_s2u(size));
	if (size <= arena_maxclass)
		return (PAGE_CEILING(size));
	return (CHUNK_CEILING(size));
}

/*
 * Compute usable size that would result from allocating an object with the
 * specified size and alignment.
 */
JEMALLOC_ALWAYS_INLINE size_t
sa2u(size_t size, size_t alignment)
{
	size_t usize;

	assert(alignment != 0 && ((alignment - 1) & alignment) == 0);

	/*
	 * Round size up to the nearest multiple of alignment.
	 *
	 * This done, we can take advantage of the fact that for each small
	 * size class, every object is aligned at the smallest power of two
	 * that is non-zero in the base two representation of the size.  For
	 * example:
	 *
	 *   Size |   Base 2 | Minimum alignment
	 *   -----+----------+------------------
	 *     96 |  1100000 |  32
	 *    144 | 10100000 |  32
	 *    192 | 11000000 |  64
	 */
	usize = ALIGNMENT_CEILING(size, alignment);
	/*
	 * (usize < size) protects against the combination of maximal
	 * alignment and size greater than maximal alignment.
	 */
	if (usize < size) {
		/* size_t overflow. */
		return (0);
	}

	if (usize <= arena_maxclass && alignment <= PAGE) {
		if (usize <= SMALL_MAXCLASS)
			return (small_s2u(usize));
		return (PAGE_CEILING(usize));
	} else {
		size_t run_size;

		/*
		 * We can't achieve subpage alignment, so round up alignment
		 * permanently; it makes later calculations simpler.
		 */
		alignment = PAGE_CEILING(alignment);
		usize = PAGE_CEILING(size);
		/*
		 * (usize < size) protects against very large sizes within
		 * PAGE of SIZE_T_MAX.
		 *
		 * (usize + alignment < usize) protects against the
		 * combination of maximal alignment and usize large enough
		 * to cause overflow.  This is similar to the first overflow
		 * check above, but it needs to be repeated due to the new
		 * usize value, which may now be *equal* to maximal
		 * alignment, whereas before we only detected overflow if the
		 * original size was *greater* than maximal alignment.
		 */
		if (usize < size || usize + alignment < usize) {
			/* size_t overflow. */
			return (0);
		}

		/*
		 * Calculate the size of the over-size run that arena_palloc()
		 * would need to allocate in order to guarantee the alignment.
		 * If the run wouldn't fit within a chunk, round up to a huge
		 * allocation size.
		 */
		run_size = usize + alignment - PAGE;
		if (run_size <= arena_maxclass)
			return (PAGE_CEILING(usize));
		return (CHUNK_CEILING(usize));
	}
}

JEMALLOC_INLINE unsigned
narenas_total_get(pool_t *pool)
{
	unsigned narenas;
	malloc_rwlock_rdlock(&pool->arenas_lock);
	narenas = pool->narenas_total;
	malloc_rwlock_unlock(&pool->arenas_lock);

	return (narenas);
}

/*
 * Choose an arena based on a per-thread value.
 * Arena pointer must be either a valid arena pointer or a dummy arena with
 * pool field filled.
 */
JEMALLOC_INLINE arena_t *
choose_arena(arena_t *arena)
{
	arena_t *ret;
	tsd_pool_t *tsd;
	pool_t *pool;

	if (!is_arena_dummy(arena))
		return (arena);

	pool = arena->pool;
	tsd = arenas_tsd_get();

	/* expand arenas array if necessary */
	if ((tsd->npools <= pool->pool_id) &&
	    arenas_tsd_extend(tsd, pool->pool_id)) {
		return (NULL);
	}

	if ( (tsd->seqno[pool->pool_id] != pool->seqno) ||
		(ret = tsd->arenas[pool->pool_id]) == NULL) {
		ret = choose_arena_hard(pool);
		assert(ret != NULL);
	}

	return (ret);
}
#endif

#include "jemalloc/internal/bitmap.h"
#include "jemalloc/internal/rtree.h"
/*
 * Include arena.h the second and third times in order to resolve circular
 * dependencies with tcache.h.
 */
#define	JEMALLOC_ARENA_INLINE_B
#include "jemalloc/internal/arena.h"
#undef JEMALLOC_ARENA_INLINE_B
#include "jemalloc/internal/tcache.h"
#define	JEMALLOC_ARENA_INLINE_C
#include "jemalloc/internal/arena.h"
#undef JEMALLOC_ARENA_INLINE_C
#include "jemalloc/internal/hash.h"
#include "jemalloc/internal/quarantine.h"

#ifndef JEMALLOC_ENABLE_INLINE
void	*imalloct(size_t size, bool try_tcache, arena_t *arena);
void	*imalloc(size_t size);
void	*pool_imalloc(pool_t *pool, size_t size);
void	*icalloct(size_t size, bool try_tcache, arena_t *arena);
void	*icalloc(size_t size);
void	*pool_icalloc(pool_t *pool, size_t size);
void	*ipalloct(size_t usize, size_t alignment, bool zero, bool try_tcache,
    arena_t *arena);
void	*ipalloc(size_t usize, size_t alignment, bool zero);
void	*pool_ipalloc(pool_t *pool, size_t usize, size_t alignment, bool zero);
size_t	isalloc(const void *ptr, bool demote);
size_t	pool_isalloc(pool_t *pool, const void *ptr, bool demote);
size_t	ivsalloc(const void *ptr, bool demote);
size_t	u2rz(size_t usize);
size_t	p2rz(const void *ptr);
void	idalloct(void *ptr, bool try_tcache);
void	pool_idalloct(pool_t *pool, void *ptr, bool try_tcache);
void	idalloc(void *ptr);
void	iqalloct(void *ptr, bool try_tcache);
void	pool_iqalloct(pool_t *pool, void *ptr, bool try_tcache);
void	iqalloc(void *ptr);
void	*iralloct_realign(void *ptr, size_t oldsize, size_t size, size_t extra,
    size_t alignment, bool zero, bool try_tcache_alloc, bool try_tcache_dalloc,
    arena_t *arena);
void	*iralloct(void *ptr, size_t size, size_t extra, size_t alignment,
    bool zero, bool try_tcache_alloc, bool try_tcache_dalloc, arena_t *arena);
void	*iralloc(void *ptr, size_t size, size_t extra, size_t alignment,
    bool zero);
void	*pool_iralloc(pool_t *pool, void *ptr, size_t size, size_t extra,
    size_t alignment, bool zero);
bool	ixalloc(void *ptr, size_t size, size_t extra, size_t alignment,
    bool zero);
int		msc_clz(unsigned int val);
malloc_tsd_protos(JEMALLOC_ATTR(unused), thread_allocated, thread_allocated_t)
#endif

#if (defined(JEMALLOC_ENABLE_INLINE) || defined(JEMALLOC_C_))

# ifdef _MSC_VER
JEMALLOC_ALWAYS_INLINE int
msc_clz(unsigned int val)
{
	unsigned int res = 0;

# if LG_SIZEOF_INT == 2
	if (_BitScanReverse(&res, val)) {
		return 31 - res;
	}
	else {
		return 32;
	}
# elif LG_SIZEOF_INT == 3
	if (_BitScanReverse64(&res, val)) {
		return 63 - res;
	}
	else {
		return 64;
	}
# else
#	error "Unsupported clz function for that size of int"
# endif
}
#endif

JEMALLOC_ALWAYS_INLINE void *
imalloct(size_t size, bool try_tcache, arena_t *arena)
{
	assert(size != 0);

	if (size <= arena_maxclass)
		return (arena_malloc(arena, size, false, try_tcache));
	else
		return (huge_malloc(arena, size, false));
}

JEMALLOC_ALWAYS_INLINE void *
imalloc(size_t size)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, &base_pool);
	return (imalloct(size, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE void *
pool_imalloc(pool_t *pool, size_t size)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, pool);
	return (imalloct(size, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE void *
icalloct(size_t size, bool try_tcache, arena_t *arena)
{
	if (size <= arena_maxclass)
		return (arena_malloc(arena, size, true, try_tcache));
	else
		return (huge_malloc(arena, size, true));
}

JEMALLOC_ALWAYS_INLINE void *
icalloc(size_t size)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, &base_pool);
	return (icalloct(size, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE void *
pool_icalloc(pool_t *pool, size_t size)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, pool);
	return (icalloct(size, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE void *
ipalloct(size_t usize, size_t alignment, bool zero, bool try_tcache,
    arena_t *arena)
{
	void *ret;

	assert(usize != 0);
	assert(usize == sa2u(usize, alignment));

	if (usize <= arena_maxclass && alignment <= PAGE)
		ret = arena_malloc(arena, usize, zero, try_tcache);
	else {
		if (usize <= arena_maxclass) {
			ret = arena_palloc(choose_arena(arena), usize,
			    alignment, zero);
		} else if (alignment <= chunksize)
			ret = huge_malloc(arena, usize, zero);
		else
			ret = huge_palloc(arena, usize, alignment, zero);
	}

	assert(ALIGNMENT_ADDR2BASE(ret, alignment) == ret);
	return (ret);
}

JEMALLOC_ALWAYS_INLINE void *
ipalloc(size_t usize, size_t alignment, bool zero)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, &base_pool);
	return (ipalloct(usize, alignment, zero, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE void *
pool_ipalloc(pool_t *pool, size_t usize, size_t alignment, bool zero)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, pool);
	return (ipalloct(usize, alignment, zero, true, &dummy));
}

/*
 * Typical usage:
 *   void *ptr = [...]
 *   size_t sz = isalloc(ptr, config_prof);
 */
JEMALLOC_ALWAYS_INLINE size_t
isalloc(const void *ptr, bool demote)
{
	size_t ret;
	arena_chunk_t *chunk;

	assert(ptr != NULL);
	/* Demotion only makes sense if config_prof is true. */
	assert(config_prof || demote == false);

	chunk = (arena_chunk_t *)CHUNK_ADDR2BASE(ptr);
	if (chunk != ptr)
		ret = arena_salloc(ptr, demote);
	else
		ret = huge_salloc(ptr);

	return (ret);
}

/*
 * Typical usage:
 *   void *ptr = [...]
 *   size_t sz = isalloc(ptr, config_prof);
 */
JEMALLOC_ALWAYS_INLINE size_t
pool_isalloc(pool_t *pool, const void *ptr, bool demote)
{
	size_t ret;
	arena_chunk_t *chunk;

	assert(ptr != NULL);
	/* Demotion only makes sense if config_prof is true. */
	assert(config_prof || demote == false);

	chunk = (arena_chunk_t *)CHUNK_ADDR2BASE(ptr);
	if (chunk != ptr)
		ret = arena_salloc(ptr, demote);
	else
		ret = huge_pool_salloc(pool, ptr);

	return (ret);
}

JEMALLOC_ALWAYS_INLINE size_t
ivsalloc(const void *ptr, bool demote)
{
	size_t i;

	malloc_mutex_lock(&pools_lock);

	unsigned n = npools;
	for (i = 0; i < n; ++i) {
		pool_t *pool = pools[i];
		if (pool == NULL)
			continue;

		/* Return 0 if ptr is not within a chunk managed by jemalloc. */
		if (rtree_get(pool->chunks_rtree,
				 (uintptr_t)CHUNK_ADDR2BASE(ptr)) != 0)
		break;
	}

	malloc_mutex_unlock(&pools_lock);

	if (i == n)
		return 0;

	return (isalloc(ptr, demote));
}

JEMALLOC_INLINE size_t
u2rz(size_t usize)
{
	size_t ret;

	if (usize <= SMALL_MAXCLASS) {
		size_t binind = small_size2bin(usize);
		assert(binind < NBINS);
		ret = arena_bin_info[binind].redzone_size;
	} else
		ret = 0;

	return (ret);
}

JEMALLOC_INLINE size_t
p2rz(const void *ptr)
{
	size_t usize = isalloc(ptr, false);

	return (u2rz(usize));
}

JEMALLOC_ALWAYS_INLINE void
idalloct(void *ptr, bool try_tcache)
{
	arena_chunk_t *chunk;

	assert(ptr != NULL);

	chunk = (arena_chunk_t *)CHUNK_ADDR2BASE(ptr);
	if (chunk != ptr)
		arena_dalloc(chunk, ptr, try_tcache);
	else
		huge_dalloc(&base_pool, ptr);
}

JEMALLOC_ALWAYS_INLINE void
pool_idalloct(pool_t *pool, void *ptr, bool try_tcache)
{
	arena_chunk_t *chunk;

	assert(ptr != NULL);

	chunk = (arena_chunk_t *)CHUNK_ADDR2BASE(ptr);
	if (chunk != ptr)
		arena_dalloc(chunk, ptr, try_tcache);
	else
		huge_dalloc(pool, ptr);
}

JEMALLOC_ALWAYS_INLINE void
idalloc(void *ptr)
{

	idalloct(ptr, true);
}

JEMALLOC_ALWAYS_INLINE void
iqalloct(void *ptr, bool try_tcache)
{

	if (config_fill && opt_quarantine)
		quarantine(ptr);
	else
		idalloct(ptr, try_tcache);
}

JEMALLOC_ALWAYS_INLINE void
pool_iqalloct(pool_t *pool, void *ptr, bool try_tcache)
{

	if (config_fill && opt_quarantine)
		quarantine(ptr);
	else
		pool_idalloct(pool, ptr, try_tcache);
}

JEMALLOC_ALWAYS_INLINE void
iqalloc(void *ptr)
{

	iqalloct(ptr, true);
}

JEMALLOC_ALWAYS_INLINE void *
iralloct_realign(void *ptr, size_t oldsize, size_t size, size_t extra,
    size_t alignment, bool zero, bool try_tcache_alloc, bool try_tcache_dalloc,
    arena_t *arena)
{
	void *p;
	size_t usize, copysize;

	usize = sa2u(size + extra, alignment);
	if (usize == 0)
		return (NULL);
	p = ipalloct(usize, alignment, zero, try_tcache_alloc, arena);
	if (p == NULL) {
		if (extra == 0)
			return (NULL);
		/* Try again, without extra this time. */
		usize = sa2u(size, alignment);
		if (usize == 0)
			return (NULL);
		p = ipalloct(usize, alignment, zero, try_tcache_alloc, arena);
		if (p == NULL)
			return (NULL);
	}
	/*
	 * Copy at most size bytes (not size+extra), since the caller has no
	 * expectation that the extra bytes will be reliably preserved.
	 */
	copysize = (size < oldsize) ? size : oldsize;
	memcpy(p, ptr, copysize);
	pool_iqalloct(arena->pool, ptr, try_tcache_dalloc);
	return (p);
}

JEMALLOC_ALWAYS_INLINE void *
iralloct(void *ptr, size_t size, size_t extra, size_t alignment, bool zero,
    bool try_tcache_alloc, bool try_tcache_dalloc, arena_t *arena)
{
	size_t oldsize;

	assert(ptr != NULL);
	assert(size != 0);

	oldsize = isalloc(ptr, config_prof);

	if (alignment != 0 && ((uintptr_t)ptr & ((uintptr_t)alignment-1))
	    != 0) {
		/*
		 * Existing object alignment is inadequate; allocate new space
		 * and copy.
		 */
		return (iralloct_realign(ptr, oldsize, size, extra, alignment,
		    zero, try_tcache_alloc, try_tcache_dalloc, arena));
	}

	if (size + extra <= arena_maxclass) {
		void *ret;

		ret = arena_ralloc(arena, ptr, oldsize, size, extra,
		    alignment, zero, try_tcache_alloc,
		    try_tcache_dalloc);

		if ((ret != NULL) || (size + extra > oldsize))
			return (ret);

		if (oldsize > chunksize) {
			size_t old_usize JEMALLOC_CC_SILENCE_INIT(0);
			UNUSED size_t old_rzsize JEMALLOC_CC_SILENCE_INIT(0);
			if (config_valgrind && in_valgrind) {
				old_usize = isalloc(ptr, config_prof);
				old_rzsize = config_prof ?
					p2rz(ptr) : u2rz(old_usize);
			}
			ret = huge_ralloc(arena, ptr, oldsize, chunksize, 0,
			    alignment, zero, try_tcache_dalloc);

			JEMALLOC_VALGRIND_REALLOC(true, ret, s2u(chunksize),
				true, ptr, old_usize, old_rzsize, true, false);

			if (ret != NULL) {
				/* Now, it should succeed... */
				return arena_ralloc(arena, ret, chunksize, size,
				    extra, alignment, zero, try_tcache_alloc,
				    try_tcache_dalloc);
			}
		}

		return NULL;
	} else {
		return (huge_ralloc(arena, ptr, oldsize, size, extra,
		    alignment, zero, try_tcache_dalloc));
	}
}

JEMALLOC_ALWAYS_INLINE void *
iralloc(void *ptr, size_t size, size_t extra, size_t alignment, bool zero)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, &base_pool);
	return (iralloct(ptr, size, extra, alignment, zero, true, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE void *
pool_iralloc(pool_t *pool, void *ptr, size_t size, size_t extra,
    size_t alignment, bool zero)
{
	arena_t dummy;
	DUMMY_ARENA_INITIALIZE(dummy, pool);
	return (iralloct(ptr, size, extra, alignment, zero, true, true, &dummy));
}

JEMALLOC_ALWAYS_INLINE bool
ixalloc(void *ptr, size_t size, size_t extra, size_t alignment, bool zero)
{
	size_t oldsize;

	assert(ptr != NULL);
	assert(size != 0);

	oldsize = isalloc(ptr, config_prof);
	if (alignment != 0 && ((uintptr_t)ptr & ((uintptr_t)alignment-1))
	    != 0) {
		/* Existing object alignment is inadequate. */
		return (true);
	}

	if (size <= arena_maxclass)
		return (arena_ralloc_no_move(ptr, oldsize, size, extra, zero));
	else
		return (huge_ralloc_no_move(&base_pool, ptr, oldsize, size, extra, zero));
}

malloc_tsd_externs(thread_allocated, thread_allocated_t)
malloc_tsd_funcs(JEMALLOC_ALWAYS_INLINE, thread_allocated, thread_allocated_t,
    THREAD_ALLOCATED_INITIALIZER, malloc_tsd_no_cleanup)
#endif

#include "jemalloc/internal/prof.h"

#undef JEMALLOC_H_INLINES

#ifdef _WIN32
	#define __builtin_clz(x) msc_clz(x)
#endif
/******************************************************************************/
#endif /* JEMALLOC_INTERNAL_H */
