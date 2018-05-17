#ifndef libiomp_h_INCLUDED
#define libiomp_h_INCLUDED

#include <stdbool.h>

#define OMP_40_ENABLED 1
#define OMP_45_ENABLED 1


#define KMP_EXPORT     extern  /* export declaration in guide libraries */

typedef char               kmp_int8;
typedef unsigned char      kmp_uint8;
typedef short              kmp_int16;
typedef unsigned short     kmp_uint16;
typedef int                kmp_int32;
typedef unsigned int       kmp_uint32;
typedef long long          kmp_int64;
typedef unsigned long long kmp_uint64;
typedef void (*kmpc_micro)              ( kmp_int32 * global_tid, kmp_int32 * bound_tid, ... );

typedef kmp_uint32 kmp_lock_flags_t;

typedef kmp_int32 kmp_critical_name[8];

/*! Use trampoline for internal microtasks */
#define KMP_IDENT_IMB             0x01
/*! Use c-style ident structure */
#define KMP_IDENT_KMPC            0x02
/*! Entry point generated by auto-parallelization */
#define KMP_IDENT_AUTOPAR         0x08
/*! Compiler generates atomic reduction option for kmpc_reduce* */
#define KMP_IDENT_ATOMIC_REDUCE   0x10
/*! To mark a 'barrier' directive in user code */
#define KMP_IDENT_BARRIER_EXPL    0x20
/*! To Mark implicit barriers. */
#define KMP_IDENT_BARRIER_IMPL           0x0040
#define KMP_IDENT_BARRIER_IMPL_MASK      0x01C0
#define KMP_IDENT_BARRIER_IMPL_FOR       0x0040
#define KMP_IDENT_BARRIER_IMPL_SECTIONS  0x00C0

#define KMP_IDENT_BARRIER_IMPL_SINGLE    0x0140
#define KMP_IDENT_BARRIER_IMPL_WORKSHARE 0x01C0
enum sched_type {
    kmp_sch_lower                     = 32,   /**< lower bound for unordered values */
    kmp_sch_static_chunked            = 33,
    kmp_sch_static                    = 34,   /**< static unspecialized */
    kmp_sch_dynamic_chunked           = 35,
    kmp_sch_guided_chunked            = 36,   /**< guided unspecialized */
    kmp_sch_runtime                   = 37,
    kmp_sch_auto                      = 38,   /**< auto */
    kmp_sch_trapezoidal               = 39,

    /* accessible only through KMP_SCHEDULE environment variable */
    kmp_sch_static_greedy             = 40,
    kmp_sch_static_balanced           = 41,
    /* accessible only through KMP_SCHEDULE environment variable */
    kmp_sch_guided_iterative_chunked  = 42,
    kmp_sch_guided_analytical_chunked = 43,

    kmp_sch_static_steal              = 44,   /**< accessible only through KMP_SCHEDULE environment variable */

#if OMP_45_ENABLED
    kmp_sch_static_balanced_chunked   = 45,   /**< static with chunk adjustment (e.g., simd) */
#endif

    /* accessible only through KMP_SCHEDULE environment variable */
    kmp_sch_upper                     = 46,   /**< upper bound for unordered values */

    kmp_ord_lower                     = 64,   /**< lower bound for ordered values, must be power of 2 */
    kmp_ord_static_chunked            = 65,
    kmp_ord_static                    = 66,   /**< ordered static unspecialized */
    kmp_ord_dynamic_chunked           = 67,
    kmp_ord_guided_chunked            = 68,
    kmp_ord_runtime                   = 69,
    kmp_ord_auto                      = 70,   /**< ordered auto */
    kmp_ord_trapezoidal               = 71,
    kmp_ord_upper                     = 72,   /**< upper bound for ordered values */

#if OMP_40_ENABLED
    /* Schedules for Distribute construct */
    kmp_distribute_static_chunked     = 91,   /**< distribute static chunked */
    kmp_distribute_static             = 92,   /**< distribute static unspecialized */
#endif

    /*
     * For the "nomerge" versions, kmp_dispatch_next*() will always return
     * a single iteration/chunk, even if the loop is serialized.  For the
     * schedule types listed above, the entire iteration vector is returned
     * if the loop is serialized.  This doesn't work for gcc/gcomp sections.
     */
    kmp_nm_lower                      = 160,  /**< lower bound for nomerge values */

    kmp_nm_static_chunked             = (kmp_sch_static_chunked - kmp_sch_lower + kmp_nm_lower),
    kmp_nm_static                     = 162,  /**< static unspecialized */
    kmp_nm_dynamic_chunked            = 163,
    kmp_nm_guided_chunked             = 164,  /**< guided unspecialized */
    kmp_nm_runtime                    = 165,
    kmp_nm_auto                       = 166,  /**< auto */
    kmp_nm_trapezoidal                = 167,

    /* accessible only through KMP_SCHEDULE environment variable */
    kmp_nm_static_greedy              = 168,
    kmp_nm_static_balanced            = 169,
    /* accessible only through KMP_SCHEDULE environment variable */
    kmp_nm_guided_iterative_chunked   = 170,
    kmp_nm_guided_analytical_chunked  = 171,
    kmp_nm_static_steal               = 172,  /* accessible only through OMP_SCHEDULE environment variable */

    kmp_nm_ord_static_chunked         = 193,
    kmp_nm_ord_static                 = 194,  /**< ordered static unspecialized */
    kmp_nm_ord_dynamic_chunked        = 195,
    kmp_nm_ord_guided_chunked         = 196,
    kmp_nm_ord_runtime                = 197,
    kmp_nm_ord_auto                   = 198,  /**< auto */
    kmp_nm_ord_trapezoidal            = 199,
    kmp_nm_upper                      = 200,  /**< upper bound for nomerge values */

#if OMP_45_ENABLED
    /* Support for OpenMP 4.5 monotonic and nonmonotonic schedule modifiers.
     * Since we need to distinguish the three possible cases (no modifier, monotonic modifier,
     * nonmonotonic modifier), we need separate bits for each modifier.
     * The absence of monotonic does not imply nonmonotonic, especially since 4.5 says
     * that the behaviour of the "no modifier" case is implementation defined in 4.5,
     * but will become "nonmonotonic" in 5.0.
     *
     * Since we're passing a full 32 bit value, we can use a couple of high bits for these
     * flags; out of paranoia we avoid the sign bit.
     *
     * These modifiers can be or-ed into non-static schedules by the compiler to pass
     * the additional information.
     * They will be stripped early in the processing in __kmp_dispatch_init when setting up schedules, so
     * most of the code won't ever see schedules with these bits set.
     */
    kmp_sch_modifier_monotonic      = (1<<29), /**< Set if the monotonic schedule modifier was present */
    kmp_sch_modifier_nonmonotonic   = (1<<30), /**< Set if the nonmonotonic schedule modifier was present */

# define SCHEDULE_WITHOUT_MODIFIERS(s) (enum sched_type)((s) & ~ (kmp_sch_modifier_nonmonotonic | kmp_sch_modifier_monotonic))
# define SCHEDULE_HAS_MONOTONIC(s)     (((s) & kmp_sch_modifier_monotonic)    != 0)
# define SCHEDULE_HAS_NONMONOTONIC(s)  (((s) & kmp_sch_modifier_nonmonotonic) != 0)
# define SCHEDULE_HAS_NO_MODIFIERS(s)  (((s) & (kmp_sch_modifier_nonmonotonic | kmp_sch_modifier_monotonic)) == 0)
#else
    /* By doing this we hope to avoid multiple tests on OMP_45_ENABLED. Compilers can now eliminate tests on compile time
     * constants and dead code that results from them, so we can leave code guarded by such an if in place.
     */
# define SCHEDULE_WITHOUT_MODIFIERS(s) (s)
# define SCHEDULE_HAS_MONOTONIC(s)     false
# define SCHEDULE_HAS_NONMONOTONIC(s)  false
# define SCHEDULE_HAS_NO_MODIFIERS(s)  true
#endif

    kmp_sch_default = kmp_sch_static  /**< default scheduling algorithm */
};
typedef struct ident {
	kmp_int32 reserved_1;   /**<  might be used in Fortran; see above  */
	kmp_int32 flags;        /**<  also f.flags; KMP_IDENT_xxx flags; KMP_IDENT_KMPC identifies this union member  */
	kmp_int32 reserved_2;   /**<  not really used in Fortran any more; see above */
	kmp_int32 reserved_3;   /**<  source[4] in Fortran, do not use for C++  */
	char const *psource;    /**<  String describing the source location.
                                      The string is composed of semi-colon separated fields which describe the source file,
                                      the function and a pair of line numbers that delimit the construct.
                                */
} ident_t;

KMP_EXPORT kmp_int32  __kmpc_global_thread_num  ( ident_t * );
KMP_EXPORT void   __kmpc_fork_call            ( ident_t *, kmp_int32 nargs, kmpc_micro microtask, ... );
KMP_EXPORT void   __kmpc_begin                ( ident_t *, kmp_int32 flags );
KMP_EXPORT void   __kmpc_end                  ( ident_t * );
KMP_EXPORT void __kmpc_barrier(ident_t *loc, kmp_int32 global_tid);
KMP_EXPORT void __kmpc_for_static_init_4(ident_t *loc, kmp_int32 gtid, kmp_int32 schedtype, kmp_int32 *plastiter,
                            kmp_int32 *plower, kmp_int32 *pupper, kmp_int32 *pstride, kmp_int32 incr,
                            kmp_int32 chunk);

KMP_EXPORT void __kmpc_for_static_fini(ident_t *loc, kmp_int32 global_tid);

KMP_EXPORT void __kmpc_dispatch_init_4( ident_t *loc, kmp_int32 gtid,
    enum sched_type schedule, kmp_int32 lb, kmp_int32 ub, kmp_int32 st,
    kmp_int32 chunk );

KMP_EXPORT void __kmpc_dispatch_fini_4( ident_t *loc, kmp_int32 gtid );
KMP_EXPORT int __kmpc_dispatch_next_4( ident_t *loc, kmp_int32 gtid,
    kmp_int32 *p_last, kmp_int32 *p_lb, kmp_int32 *p_ub, kmp_int32 *p_st );

/*
 * Interface to fast scalable reduce methods routines
 */

KMP_EXPORT kmp_int32 __kmpc_reduce_nowait( ident_t *loc, kmp_int32 global_tid,
                                           kmp_int32 num_vars, size_t reduce_size,
                                           void *reduce_data, void (*reduce_func)(void *lhs_data, void *rhs_data),
                                           kmp_critical_name *lck );
KMP_EXPORT void __kmpc_end_reduce_nowait( ident_t *loc, kmp_int32 global_tid, kmp_critical_name *lck );
KMP_EXPORT kmp_int32 __kmpc_reduce( ident_t *loc, kmp_int32 global_tid,
                                    kmp_int32 num_vars, size_t reduce_size,
                                    void *reduce_data, void (*reduce_func)(void *lhs_data, void *rhs_data),
                                    kmp_critical_name *lck );
KMP_EXPORT void __kmpc_end_reduce( ident_t *loc, kmp_int32 global_tid, kmp_critical_name *lck );

KMP_EXPORT kmp_int32  __kmpc_single         ( ident_t *, kmp_int32 global_tid );
KMP_EXPORT void   __kmpc_end_single         ( ident_t *, kmp_int32 global_tid );

KMP_EXPORT void   __kmpc_critical           ( ident_t *, kmp_int32 global_tid, kmp_critical_name * );
KMP_EXPORT void   __kmpc_end_critical       ( ident_t *, kmp_int32 global_tid, kmp_critical_name * );

// 4-byte add / sub fixed
void __kmpc_atomic_fixed4_add(  ident_t *id_ref, int gtid, kmp_int32 * lhs, kmp_int32 rhs );
void __kmpc_atomic_fixed4_sub(  ident_t *id_ref, int gtid, kmp_int32 * lhs, kmp_int32 rhs );

struct kmp_base_ticket_lock {
    // `initialized' must be the first entry in the lock data structure!
    volatile union kmp_ticket_lock * initialized;  // points to the lock union if in initialized state
    ident_t const *     location;     // Source code location of omp_init_lock().
    volatile kmp_uint32 next_ticket;  // ticket number to give to next thread which acquires
    volatile kmp_uint32 now_serving;  // ticket number for thread which holds the lock
    volatile kmp_int32  owner_id;     // (gtid+1) of owning thread, 0 if unlocked
    kmp_int32           depth_locked; // depth locked, for nested locks only
    kmp_lock_flags_t    flags;        // lock specifics, e.g. critical section lock
};

typedef struct kmp_base_ticket_lock kmp_base_ticket_lock_t;

/* Define the default size of the cache line */
#ifndef CACHE_LINE
    #define CACHE_LINE                  128         /* cache line size in bytes */
#else
    #if ( CACHE_LINE < 64 ) && ! defined( KMP_OS_DARWIN )
        // 2006-02-13: This produces too many warnings on OS X*. Disable it for a while...
        #warning CACHE_LINE is too small.
    #endif
#endif /* CACHE_LINE */

# define KMP_ALIGN_CACHE      __attribute__((aligned(CACHE_LINE)))

//
// When a lock table is used, the indices are of kmp_lock_index_t
//
typedef kmp_uint32 kmp_lock_index_t;

//
// When memory allocated for locks are on the lock pool (free list),
// it is treated as structs of this type.
//
struct kmp_lock_pool {
    union kmp_user_lock *next;
    kmp_lock_index_t index;
};

typedef struct kmp_lock_pool kmp_lock_pool_t;

#define KMP_PAD(type, sz)     (sizeof(type) + (sz - ((sizeof(type) - 1) % (sz)) - 1))

union KMP_ALIGN_CACHE kmp_ticket_lock {
    kmp_base_ticket_lock_t lk;       // This field must be first to allow static initializing.
    kmp_lock_pool_t pool;
    double                 lk_align; // use worst case alignment
    char                   lk_pad[ KMP_PAD( kmp_base_ticket_lock_t, CACHE_LINE ) ];
};

typedef union kmp_ticket_lock kmp_ticket_lock_t;

typedef kmp_ticket_lock_t kmp_lock_t;

typedef struct KMP_ALIGN_CACHE kmpc_aligned_queue_slot_t {
    struct kmpc_thunk_t   *qs_thunk;
} kmpc_aligned_queue_slot_t;

/*
 * OMP 3.0 tasking interface routines
 */

/* Defines for OpenMP 3.0 tasking and auto scheduling */

# ifndef KMP_STATIC_STEAL_ENABLED
#  define KMP_STATIC_STEAL_ENABLED 1
# endif

#define TASK_CURRENT_NOT_QUEUED  0
#define TASK_CURRENT_QUEUED      1

#ifdef BUILD_TIED_TASK_STACK
#define TASK_STACK_EMPTY         0  // entries when the stack is empty

#define TASK_STACK_BLOCK_BITS    5  // Used to define TASK_STACK_SIZE and TASK_STACK_MASK
#define TASK_STACK_BLOCK_SIZE    ( 1 << TASK_STACK_BLOCK_BITS ) // Number of entries in each task stack array
#define TASK_STACK_INDEX_MASK    ( TASK_STACK_BLOCK_SIZE - 1 )  // Mask for determining index into stack block
#endif // BUILD_TIED_TASK_STACK

#define TASK_NOT_PUSHED          1
#define TASK_SUCCESSFULLY_PUSHED 0
#define TASK_TIED                1
#define TASK_UNTIED              0
#define TASK_EXPLICIT            1
#define TASK_IMPLICIT            0
#define TASK_PROXY               1
#define TASK_FULL                0

#define KMP_CANCEL_THREADS
#define KMP_THREAD_ATTR

typedef kmp_int32 (* kmp_routine_entry_t)( kmp_int32, void * );

#if OMP_40_ENABLED || OMP_45_ENABLED
typedef union kmp_cmplrdata {
#if OMP_45_ENABLED
    kmp_int32           priority;           /**< priority specified by user for the task */
#endif // OMP_45_ENABLED
#if OMP_40_ENABLED
    kmp_routine_entry_t destructors;        /* pointer to function to invoke deconstructors of firstprivate C++ objects */
#endif // OMP_40_ENABLED
    /* future data */
} kmp_cmplrdata_t;
#endif

typedef struct kmpc_task_queue_t {
        /* task queue linkage fields for n-ary tree of queues (locked with global taskq_tree_lck) */
    kmp_lock_t                    tq_link_lck;          /*  lock for child link, child next/prev links and child ref counts */
    union {
        struct kmpc_task_queue_t *tq_parent;            /*  pointer to parent taskq, not locked */
        struct kmpc_task_queue_t *tq_next_free;         /*  for taskq internal freelists, locked with global taskq_freelist_lck */
    } tq;
    volatile struct kmpc_task_queue_t *tq_first_child;  /*  pointer to linked-list of children, locked by tq's tq_link_lck */
    struct kmpc_task_queue_t     *tq_next_child;        /*  next child in linked-list, locked by parent tq's tq_link_lck */
    struct kmpc_task_queue_t     *tq_prev_child;        /*  previous child in linked-list, locked by parent tq's tq_link_lck */
    volatile kmp_int32            tq_ref_count;         /*  reference count of threads with access to this task queue */
                                                        /*  (other than the thread executing the kmpc_end_taskq call) */
                                                        /*  locked by parent tq's tq_link_lck */

        /* shared data for task queue */
    struct kmpc_aligned_shared_vars_t    *tq_shareds;   /*  per-thread array of pointers to shared variable structures */
                                                        /*  only one array element exists for all but outermost taskq */

        /* bookkeeping for ordered task queue */
    kmp_uint32                    tq_tasknum_queuing;   /*  ordered task number assigned while queuing tasks */
    volatile kmp_uint32           tq_tasknum_serving;   /*  ordered number of next task to be served (executed) */

        /* thunk storage management for task queue */
    kmp_lock_t                    tq_free_thunks_lck;   /*  lock for thunk freelist manipulation */
    struct kmpc_thunk_t          *tq_free_thunks;       /*  thunk freelist, chained via th.th_next_free  */
    struct kmpc_thunk_t          *tq_thunk_space;       /*  space allocated for thunks for this task queue  */

        /* data fields for queue itself */
    kmp_lock_t                    tq_queue_lck;         /*  lock for [de]enqueue operations: tq_queue, tq_head, tq_tail, tq_nfull */
    kmpc_aligned_queue_slot_t    *tq_queue;             /*  array of queue slots to hold thunks for tasks */
    volatile struct kmpc_thunk_t *tq_taskq_slot;        /*  special slot for taskq task thunk, occupied if not NULL  */
    kmp_int32                     tq_nslots;            /*  # of tq_thunk_space thunks alloc'd (not incl. tq_taskq_slot space)  */
    kmp_int32                     tq_head;              /*  enqueue puts next item in here (index into tq_queue array) */
    kmp_int32                     tq_tail;              /*  dequeue takes next item out of here (index into tq_queue array) */
    volatile kmp_int32            tq_nfull;             /*  # of occupied entries in task queue right now  */
    kmp_int32                     tq_hiwat;             /*  high-water mark for tq_nfull and queue scheduling  */
    volatile kmp_int32            tq_flags;             /*  TQF_xxx  */

        /* bookkeeping for outstanding thunks */
    struct kmpc_aligned_int32_t  *tq_th_thunks;         /*  per-thread array for # of regular thunks currently being executed */
    kmp_int32                     tq_nproc;             /*  number of thunks in the th_thunks array */

        /* statistics library bookkeeping */
    ident_t                       *tq_loc;              /*  source location information for taskq directive */
} kmpc_task_queue_t;

/*  sizeof_kmp_task_t passed as arg to kmpc_omp_task call  */
typedef struct kmp_task {                   /* GEH: Shouldn't this be aligned somehow? */
    void *              shareds;            /**< pointer to block of pointers to shared vars   */
    kmp_routine_entry_t routine;            /**< pointer to routine to call for executing task */
    kmp_int32           part_id;            /**< part id for the task                          */
#if OMP_40_ENABLED || OMP_45_ENABLED
    kmp_cmplrdata_t data1;                  /* Two known optional additions: destructors and priority */
    kmp_cmplrdata_t data2;                  /* Process destructors first, priority second */
    /* future data */
#endif
    /*  private vars  */
} kmp_task_t;

/*  sizeof_shareds passed as arg to __kmpc_taskq call  */
typedef struct kmpc_shared_vars_t {             /*  aligned during dynamic allocation */
    kmpc_task_queue_t         *sv_queue;
    /*  (pointers to) shared vars  */
} kmpc_shared_vars_t;


KMP_EXPORT kmp_int32
__kmpc_omp_task( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t * new_task );
KMP_EXPORT kmp_task_t*
__kmpc_omp_task_alloc( ident_t *loc_ref, kmp_int32 gtid, kmp_int32 flags,
                       size_t sizeof_kmp_task_t, size_t sizeof_shareds,
                       kmp_routine_entry_t task_entry );
KMP_EXPORT void
__kmpc_omp_task_begin_if0( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t * task );
KMP_EXPORT void
__kmpc_omp_task_complete_if0( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t *task );
KMP_EXPORT kmp_int32
__kmpc_omp_task_parts( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t * new_task );
KMP_EXPORT kmp_int32
__kmpc_omp_taskwait( ident_t *loc_ref, kmp_int32 gtid );

KMP_EXPORT kmp_int32
__kmpc_omp_taskyield( ident_t *loc_ref, kmp_int32 gtid, int end_part );

#if TASK_UNUSED
void __kmpc_omp_task_begin( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t * task );
void __kmpc_omp_task_complete( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t *task );
#endif // TASK_UNUSED

/* ------------------------------------------------------------------------ */

#if OMP_40_ENABLED

typedef long             kmp_intptr_t;
typedef unsigned long    kmp_uintptr_t;

/* ------------------------------------------------------------------------ */
// OpenMP tasking data structures
//

typedef union  kmp_team      kmp_team_t;
typedef struct kmp_taskdata  kmp_taskdata_t;
typedef union  kmp_task_team kmp_task_team_t;
typedef union  kmp_team      kmp_team_p;
typedef union  kmp_info      kmp_info_p;
typedef union  kmp_root      kmp_root_p;

typedef enum kmp_proc_bind_t {
    proc_bind_false = 0,
    proc_bind_true,
    proc_bind_master,
    proc_bind_close,
    proc_bind_spread,
    proc_bind_intel,    // use KMP_AFFINITY interface
    proc_bind_default
} kmp_proc_bind_t;

/* Type to keep runtime schedule set via OMP_SCHEDULE or omp_set_schedule() */
typedef struct kmp_r_sched {
    enum sched_type r_sched_type;
    int             chunk;
} kmp_r_sched_t;

typedef struct kmp_internal_control {
    int           serial_nesting_level;  /* corresponds to the value of the th_team_serialized field */
    kmp_int8      nested;                /* internal control for nested parallelism (per thread) */
    kmp_int8      dynamic;               /* internal control for dynamic adjustment of threads (per thread) */
    kmp_int8      bt_set;                /* internal control for whether blocktime is explicitly set */
    int           blocktime;             /* internal control for blocktime */
    int           bt_intervals;          /* internal control for blocktime intervals */
    int           nproc;                 /* internal control for #threads for next parallel region (per thread) */
    int           max_active_levels;     /* internal control for max_active_levels */
    kmp_r_sched_t sched;                 /* internal control for runtime schedule {sched,chunk} pair */
#if OMP_40_ENABLED
    kmp_proc_bind_t proc_bind;           /* internal control for affinity  */
#endif // OMP_40_ENABLED
    struct kmp_internal_control *next;
} kmp_internal_control_t;

typedef enum kmp_tasking_mode {
    tskm_immediate_exec = 0,
    tskm_extra_barrier = 1,
    tskm_task_teams = 2,
    tskm_max = 2
} kmp_tasking_mode_t;

extern kmp_tasking_mode_t __kmp_tasking_mode;         /* determines how/when to execute tasks */
extern kmp_int32 __kmp_task_stealing_constraint;
#if OMP_45_ENABLED
    extern kmp_int32 __kmp_max_task_priority; // Set via OMP_MAX_TASK_PRIORITY if specified, defaults to 0 otherwise
#endif

/* NOTE: kmp_taskdata_t and kmp_task_t structures allocated in single block with taskdata first */
#define KMP_TASK_TO_TASKDATA(task)     (((kmp_taskdata_t *) task) - 1)
#define KMP_TASKDATA_TO_TASK(taskdata) (kmp_task_t *) (taskdata + 1)

// The tt_found_tasks flag is a signal to all threads in the team that tasks were spawned and
// queued since the previous barrier release.
#define KMP_TASKING_ENABLED(task_team) \
    (TCR_SYNC_4((task_team)->tt.tt_found_tasks) == TRUE)
/*!
@ingroup BASIC_TYPES
@{
*/

/*!
 */
typedef kmp_int32 (* kmp_routine_entry_t)( kmp_int32, void * );

#if OMP_40_ENABLED
typedef struct kmp_taskgroup {
    kmp_uint32            count;   // number of allocated and not yet complete tasks
    kmp_int32             cancel_request; // request for cancellation of this taskgroup
    struct kmp_taskgroup *parent;  // parent taskgroup
} kmp_taskgroup_t;


// forward declarations
typedef union kmp_depnode       kmp_depnode_t;
typedef struct kmp_depnode_list  kmp_depnode_list_t;
typedef struct kmp_dephash_entry kmp_dephash_entry_t;

typedef struct kmp_depend_info {
     kmp_intptr_t               base_addr;
     size_t                     len;
     struct {
         bool                   in:1;
         bool                   out:1;
     } flags;
} kmp_depend_info_t;

struct kmp_depnode_list {
   kmp_depnode_t *              node;
   kmp_depnode_list_t *         next;
};

typedef struct kmp_base_depnode {
    kmp_depnode_list_t        * successors;
    kmp_task_t                * task;

    kmp_lock_t                  lock;

#if KMP_SUPPORT_GRAPH_OUTPUT
    kmp_uint32                  id;
#endif

    volatile kmp_int32          npredecessors;
    volatile kmp_int32          nrefs;
} kmp_base_depnode_t;

union KMP_ALIGN_CACHE kmp_depnode {
    double          dn_align;        /* use worst case alignment */
    char            dn_pad[ KMP_PAD(kmp_base_depnode_t, CACHE_LINE) ];
    kmp_base_depnode_t dn;
};

struct kmp_dephash_entry {
    kmp_intptr_t               addr;
    kmp_depnode_t            * last_out;
    kmp_depnode_list_t       * last_ins;
    kmp_dephash_entry_t      * next_in_bucket;
};

typedef struct kmp_dephash {
   kmp_dephash_entry_t     ** buckets;
   size_t		      size;
#ifdef KMP_DEBUG
   kmp_uint32                 nelements;
   kmp_uint32                 nconflicts;
#endif
} kmp_dephash_t;

#endif

#ifdef BUILD_TIED_TASK_STACK

/* Tied Task stack definitions */
typedef struct kmp_stack_block {
    kmp_taskdata_t *          sb_block[ TASK_STACK_BLOCK_SIZE ];
    struct kmp_stack_block *  sb_next;
    struct kmp_stack_block *  sb_prev;
} kmp_stack_block_t;

typedef struct kmp_task_stack {
    kmp_stack_block_t         ts_first_block;  // first block of stack entries
    kmp_taskdata_t **         ts_top;          // pointer to the top of stack
    kmp_int32                 ts_entries;      // number of entries on the stack
} kmp_task_stack_t;

#endif // BUILD_TIED_TASK_STACK

typedef struct kmp_tasking_flags {          /* Total struct must be exactly 32 bits */
    /* Compiler flags */                    /* Total compiler flags must be 16 bits */
    unsigned tiedness    : 1;               /* task is either tied (1) or untied (0) */
    unsigned final       : 1;               /* task is final(1) so execute immediately */
    unsigned merged_if0  : 1;               /* no __kmpc_task_{begin/complete}_if0 calls in if0 code path */
#if OMP_40_ENABLED
    unsigned destructors_thunk : 1;         /* set if the compiler creates a thunk to invoke destructors from the runtime */
#if OMP_45_ENABLED
    unsigned proxy       : 1;               /* task is a proxy task (it will be executed outside the context of the RTL) */
    unsigned priority_specified :1;         /* set if the compiler provides priority setting for the task */
    unsigned reserved    : 10;              /* reserved for compiler use */
#else
    unsigned reserved    : 12;              /* reserved for compiler use */
#endif
#else // OMP_40_ENABLED
    unsigned reserved    : 13;              /* reserved for compiler use */
#endif // OMP_40_ENABLED

    /* Library flags */                     /* Total library flags must be 16 bits */
    unsigned tasktype    : 1;               /* task is either explicit(1) or implicit (0) */
    unsigned task_serial : 1;               /* this task is executed immediately (1) or deferred (0) */
    unsigned tasking_ser : 1;               /* all tasks in team are either executed immediately (1) or may be deferred (0) */
    unsigned team_serial : 1;               /* entire team is serial (1) [1 thread] or parallel (0) [>= 2 threads] */
                                            /* If either team_serial or tasking_ser is set, task team may be NULL */
    /* Task State Flags: */
    unsigned started     : 1;               /* 1==started, 0==not started     */
    unsigned executing   : 1;               /* 1==executing, 0==not executing */
    unsigned complete    : 1;               /* 1==complete, 0==not complete   */
    unsigned freed       : 1;               /* 1==freed, 0==allocateed        */
    unsigned native      : 1;               /* 1==gcc-compiled task, 0==intel */
    unsigned reserved31  : 7;               /* reserved for library use */

} kmp_tasking_flags_t;


struct kmp_taskdata {                                 /* aligned during dynamic allocation       */
    kmp_int32               td_task_id;               /* id, assigned by debugger                */
    kmp_tasking_flags_t     td_flags;                 /* task flags                              */
    kmp_team_t *            td_team;                  /* team for this task                      */
    kmp_info_p *            td_alloc_thread;          /* thread that allocated data structures   */
                                                      /* Currently not used except for perhaps IDB */
    kmp_taskdata_t *        td_parent;                /* parent task                             */
    kmp_int32               td_level;                 /* task nesting level                      */
    kmp_int32               td_untied_count;          /* untied task active parts counter        */
    ident_t *               td_ident;                 /* task identifier                         */
                            // Taskwait data.
    ident_t *               td_taskwait_ident;
    kmp_uint32              td_taskwait_counter;
    kmp_int32               td_taskwait_thread;       /* gtid + 1 of thread encountered taskwait */
    KMP_ALIGN_CACHE kmp_internal_control_t  td_icvs;  /* Internal control variables for the task */
    KMP_ALIGN_CACHE volatile kmp_uint32 td_allocated_child_tasks;  /* Child tasks (+ current task) not yet deallocated */
    volatile kmp_uint32     td_incomplete_child_tasks; /* Child tasks not yet complete */
#if OMP_40_ENABLED
    kmp_taskgroup_t *       td_taskgroup;         // Each task keeps pointer to its current taskgroup
    kmp_dephash_t *         td_dephash;           // Dependencies for children tasks are tracked from here
    kmp_depnode_t *         td_depnode;           // Pointer to graph node if this task has dependencies
#endif
#if OMPT_SUPPORT
    ompt_task_info_t        ompt_task_info;
#endif
#if OMP_45_ENABLED
    kmp_task_team_t *       td_task_team;
    kmp_int32               td_size_alloc;        // The size of task structure, including shareds etc.
#endif
}; // struct kmp_taskdata

typedef struct kmp_task_red_flags {
    unsigned lazy_priv : 1;     // hint: (1) use lazy allocation (big objects)
    unsigned reserved31 : 31;
} kmp_task_red_flags_t;

// internal structure for reduction data item related info
typedef struct kmp_task_red_data {
    void *reduce_shar;          // shared reduction item
    size_t reduce_size;         // size of data item
    void *reduce_priv;          // thread specific data
    void *reduce_pend;          // end of private data for comparison op
    void *reduce_init;          // data initialization routine
    void *reduce_fini;          // data finalization routine
    void *reduce_comb;          // data combiner routine
    kmp_task_red_flags_t flags; // flags for additional info from compiler
} kmp_task_red_data_t;

// structure sent us by compiler - one per reduction item
typedef struct kmp_task_red_input {
    void *reduce_shar;          // shared reduction item
    size_t reduce_size;         // size of data item
    void *reduce_init;          // data initialization routine
    void *reduce_fini;          // data finalization routine
    void *reduce_comb;          // data combiner routine
    kmp_task_red_flags_t flags; // flags for additional info from compiler
} kmp_task_red_input_t;

KMP_EXPORT void *__kmpc_task_reduction_init(int gtid, int num_data, void *data);
KMP_EXPORT void *__kmpc_task_reduction_get_th_data(int gtid, void *tg, void *d);

KMP_EXPORT void __kmpc_taskgroup( ident_t * loc, int gtid );
KMP_EXPORT void __kmpc_end_taskgroup( ident_t * loc, int gtid );

KMP_EXPORT kmp_int32 __kmpc_omp_task_with_deps ( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t * new_task,
                                                 kmp_int32 ndeps, kmp_depend_info_t *dep_list,
                                                 kmp_int32 ndeps_noalias, kmp_depend_info_t *noalias_dep_list );
KMP_EXPORT void __kmpc_omp_wait_deps ( ident_t *loc_ref, kmp_int32 gtid, kmp_int32 ndeps, kmp_depend_info_t *dep_list,
                                          kmp_int32 ndeps_noalias, kmp_depend_info_t *noalias_dep_list );
extern void __kmp_release_deps ( kmp_int32 gtid, kmp_taskdata_t *task );

extern kmp_int32 __kmp_omp_task( kmp_int32 gtid, kmp_task_t * new_task, bool serialize_immediate );

KMP_EXPORT kmp_int32 __kmpc_cancel(ident_t* loc_ref, kmp_int32 gtid, kmp_int32 cncl_kind);
KMP_EXPORT kmp_int32 __kmpc_cancellationpoint(ident_t* loc_ref, kmp_int32 gtid, kmp_int32 cncl_kind);
KMP_EXPORT kmp_int32 __kmpc_cancel_barrier(ident_t* loc_ref, kmp_int32 gtid);
KMP_EXPORT int __kmp_get_cancellation_status(int cancel_kind);

#if OMP_45_ENABLED

KMP_EXPORT void __kmpc_proxy_task_completed( kmp_int32 gtid, kmp_task_t *ptask );
KMP_EXPORT void __kmpc_proxy_task_completed_ooo ( kmp_task_t *ptask );
KMP_EXPORT void __kmpc_taskloop(ident_t *loc, kmp_int32 gtid, kmp_task_t *task, kmp_int32 if_val,
                kmp_uint64 *lb, kmp_uint64 *ub, kmp_int64 st,
                kmp_int32 nogroup, kmp_int32 sched, kmp_uint64 grainsize, void * task_dup );
#endif
#endif

#endif // libiomp_h_INCLUDED

