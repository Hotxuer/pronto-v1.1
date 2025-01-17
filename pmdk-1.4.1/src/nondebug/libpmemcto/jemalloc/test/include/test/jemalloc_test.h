#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#endif

/******************************************************************************/
/*
 * Define always-enabled assertion macros, so that test assertions execute even
 * if assertions are disabled in the library code.  These definitions must
 * exist prior to including "jemalloc/internal/util.h".
 */
#define	assert(e) do {							\
	if (!(e)) {							\
		malloc_printf(						\
		    "<jemalloc>: %s:%d: Failed assertion: \"%s\"\n",	\
		    __FILE__, __LINE__, #e);				\
		abort();						\
	}								\
} while (0)

#define	not_reached() do {						\
	malloc_printf(							\
	    "<jemalloc>: %s:%d: Unreachable code reached\n",		\
	    __FILE__, __LINE__);					\
	abort();							\
} while (0)

#define	not_implemented() do {						\
	malloc_printf("<jemalloc>: %s:%d: Not implemented\n",		\
	    __FILE__, __LINE__);					\
	abort();							\
} while (0)

#define	assert_not_implemented(e) do {					\
	if (!(e))							\
		not_implemented();					\
} while (0)

#include "test/jemalloc_test_defs.h"

#ifdef JEMALLOC_OSSPIN
#  include <libkern/OSAtomic.h>
#endif

#if defined(HAVE_ALTIVEC) && !defined(__APPLE__)
#  include <altivec.h>
#endif
#ifdef HAVE_SSE2
#  include <emmintrin.h>
#endif

/******************************************************************************/
/*
 * For unit tests, expose all public and private interfaces.
 */
#ifdef JEMALLOC_UNIT_TEST
#  define JEMALLOC_JET
#  define JEMALLOC_MANGLE
#  include "jemalloc/internal/jemalloc_internal.h"

/******************************************************************************/
/*
 * For integration tests, expose the public jemalloc interfaces, but only
 * expose the minimum necessary internal utility code (to avoid re-implementing
 * essentially identical code within the test infrastructure).
 */
#elif defined(JEMALLOC_INTEGRATION_TEST)
#  define JEMALLOC_MANGLE
#  include "jemalloc/jemalloc.h"
#  include "jemalloc/internal/jemalloc_internal_defs.h"
#  include "jemalloc/internal/jemalloc_internal_macros.h"

#  define JEMALLOC_N(n) je_cto_je_##n
#  include "jemalloc/internal/private_namespace.h"

#  define JEMALLOC_H_TYPES
#  define JEMALLOC_H_STRUCTS
#  define JEMALLOC_H_EXTERNS
#  define JEMALLOC_H_INLINES
#  include "jemalloc/internal/util.h"
#  include "jemalloc/internal/qr.h"
#  include "jemalloc/internal/ql.h"
#  undef JEMALLOC_H_TYPES
#  undef JEMALLOC_H_STRUCTS
#  undef JEMALLOC_H_EXTERNS
#  undef JEMALLOC_H_INLINES

/******************************************************************************/
/*
 * For stress tests, expose the public jemalloc interfaces with name mangling
 * so that they can be tested as e.g. malloc() and free().  Also expose the
 * public jemalloc interfaces with jet_ prefixes, so that stress tests can use
 * a separate allocator for their internal data structures.
 */
#elif defined(JEMALLOC_STRESS_TEST)
#  include "jemalloc/jemalloc.h"

#  include "jemalloc/jemalloc_protos_jet.h"

#  define JEMALLOC_JET
#  include "jemalloc/internal/jemalloc_internal.h"
#  include "jemalloc/internal/public_unnamespace.h"
#  undef JEMALLOC_JET

#  include "jemalloc/jemalloc_rename.h"
#  define JEMALLOC_MANGLE
#  ifdef JEMALLOC_STRESS_TESTLIB
#    include "jemalloc/jemalloc_mangle_jet.h"
#  else
#    include "jemalloc/jemalloc_mangle.h"
#  endif

/******************************************************************************/
/*
 * This header does dangerous things, the effects of which only test code
 * should be subject to.
 */
#else
#  error "This header cannot be included outside a testing context"
#endif

/******************************************************************************/
/*
 * Common test utilities.
 */
#include "test/math.h"
#include "test/mtx.h"
#include "test/mq.h"
#include "test/test.h"
#include "test/thd.h"
#define	MEXP 19937
#include "test/SFMT.h"
