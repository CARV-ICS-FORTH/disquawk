/*
 * Copyright 2004-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2011 Oracle Corporation. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * only, as published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Oracle Corporation, 500 Oracle Parkway, Redwood
 * Shores, CA 94065 or visit www.oracle.com if you need additional
 * information or have any questions.
 */

#ifndef __MICROBLAZE__
#define MAX_STREAMS 4
#endif /* __MICROBLAZE__ */

#if (defined(ASSUME) && ASSUME != 0) | TRACE
#define INTERPRETER_STATS 1
#endif /* ASSUME */

/* FIXME: Make a struct for read-only (after initialization) globals
 * to skip copies and double initialization
 */

/**
 * This struct encapsulates all the globals in the Squawk VM. This allows
 * a system to be built with several separate VM execution contexts.
 */
typedef struct globalsStruct {
	Address     _memory;        /* The buffer containing ROM, NVM, RAM and serviceChunk */
	Address     _memoryEnd;     /* The end of the memory buffer. */
	UWord       _memorySize;    /* The Size (In bytes) of the memory buffer. */

#ifdef SC_NATIVE
#ifdef __MICROBLAZE__
	/** The cache directory. */
	sc_object_st     *_cacheDirectory;
	/** Start address of the allocated memory to the software cache. */
	Address           _cacheStart;
	/** End address of the allocated memory to the software cache. */
	Address           _cacheEnd;
	/** Size of the software cache in bytes. */
	int               _cacheSize;
	/** The next allocation address. */
	Address           _cacheAllocTop;
	/** Temp pointer to the last allocated cache-line if it was not used. */
	Address           _cacheAllocTemp;
	/** The allocation address for read-only Objects. */
	Address           _cacheROAllocTop;
	/** The allocation threshold/limit address for read-only Objects. */
	Address           _cacheROThreshold;
	/** Counter for the number of flushes due to full cache. */
	int               _cacheFlushes;
	/** Counts how many times the cache was cleared. */
	int               _cacheClears;
	/** Counter for the number of cached objects. */
	int               _cacheObjects;
	/** Bitmap for pending write back DMAs. */
	int               _cachePendingWBs;
	/** Bitmap for pending fetch DMAs. */
	int               _cachePendingFEs;
#else
#error SC_NATIVE is only supported on Formic
#endif /* __MICROBLAZE__ */
#endif /* SC_NATIVE */

	/** Keeps the last core that the scheduler chose to assign a thread to */
	int          _schedulerLastCore;

#ifndef MACROIZE
	int          _iparm; /* The immediate operand value of the current bytecode. */
	ByteAddress  _ip;    /* The instruction pointer. */
	UWordAddress _fp;    /* The frame pointer. */
	UWordAddress _sp;    /* The stack pointer. */
#else
#if TRACE
	ByteAddress  _lastIP;
	UWordAddress _lastFP;
#endif /* TRACE */
#endif /* MACROIZE */

	UWordAddress _sl;           /* The stack limit. */
	UWordAddress _ss;           /* The stack start. */
	int          _bc;           /* The branch counter. */

	ByteAddress  _saved_ip;     /* The saved instruction pointer. */
	UWordAddress _saved_fp;     /* The saved frame pointer. */
	UWordAddress _saved_sp;     /* The saved stack pointer. */

	int         _Ints[GLOBAL_INT_COUNT];     /* Storage for the
	                                          * primitive typed Java
	                                          * globals. */
	Address     _Addrs[GLOBAL_ADDR_COUNT];   /* Storage for the
	                                          * primitive typed Java
	                                          * globals. */
	Address     _Oops[GLOBAL_OOP_COUNT];     /* Storage for the
	                                          * reference typed Java
	                                          * globals. */
	Address     _Buffers[MAX_BUFFERS];       /* Buffers that are
	                                          * allocated by native
	                                          * code. */
	int         _BufferCount;                /* Number of buffers that
	                                          * are currently
	                                          * allocated by native
	                                          * code. */

#ifdef __MICROBLAZE__
	char        _format_buf[FORMAT_BUF_LEN]; /* Buffer used for
	                                          * pre-formatting
	                                          * printf/fprintf format
	                                          * specifications */
#endif /* __MICROBLAZE__ */

#ifndef __MICROBLAZE__
	FILE       *_streams[MAX_STREAMS];       /* The file streams to
	                                          * which the VM printing
	                                          * directives sent. */
#endif /* __MICROBLAZE__ */
	int         _currentStream;              /* The currently selected stream */
	int         _internalLowResult;          /* Value for INTERNAL_LOW_RESULT */

#if KERNEL_SQUAWK
	/* Nothing yet... */
#endif

#if PLATFORM_TYPE_DELEGATING
	jclass      _channelIO_clazz;            /* JNI handle to com.sun.squawk.vm.ChannelIO. */
	jmethodID   _channelIO_execute;          /* JNI handle to com.sun.squawk.vm.ChannelIO.execute(...) */
#endif

#if PLATFORM_TYPE_SOCKET
	char       *_ioport;                     /* The [host and] port number of the optional I/O server. */
	int         _iosocket;                   /* The socket number of the optional I/O server. */
	int         _result_low;                 /* The low 32 bits of the last result */
	int         _result_high;                /* The high 32 bits of the last result */
	jlong       _io_ops_time;
	int         _io_ops_count;
#endif /* PLATFORM_TYPE_SOCKET */

	void*       _nativeFuncPtr;     /* Ptr to the function that is
	                                 * being called via
	                                 * NativeUnsafe.call, or null */

#ifdef PROFILING
	int         _sampleFrequency;   /* The profile sample frequency */
	jlong       _instructionCount;
#endif /* PROFILING */

#if TRACE
	FILE       *_traceFile;          /* The trace file name */
	boolean     _traceFileOpen;      /* Specifies if the trace file
	                                  * has been opened. */
	boolean     _traceServiceThread; /* Specifies if execution on the
	                                  * service thread is to be
	                                  * traced. */
	int         _traceLastThreadID;  /* Specifies the thread ID at the
	                                  * last call to trace() */

	int         _total_extends;      /* Total number of extends */
	int         _total_slots;        /* Total number of slots cleared */

	int         _lastThreadID;

	int         _statsFrequency;     /* The statistics output frequency */
#endif /* TRACE */

#if defined(PROFILING) | TRACE
	jlong       _lastStatCount;
#endif /* PROFILING */

	Address     _cachedClassState[CLASS_CACHE_SIZE > 0 ? CLASS_CACHE_SIZE : 1];
	Address     _cachedClass     [CLASS_CACHE_SIZE > 0 ? CLASS_CACHE_SIZE : 1];
	int         _cachedClassIndex;
#ifdef INTERPRETER_STATS
	int         _cachedClassAccesses;
	int         _cachedClassHits;
#endif /* INTERPRETER_STATS */

	Address    *_pendingMonitors;
	int         _pendingMonitorStackPointer;
#ifdef INTERPRETER_STATS
	int         _pendingMonitorAccesses;
	int         _pendingMonitorHits;
#endif /* INTERPRETER_STATS */

	boolean     _inFatalVMError;
	boolean     _squawk_inStop;

#if PLATFORM_TYPE_DELEGATING
	JNIEnv     *_JNI_env;                    /* The pointer to the
	                                          * table of JNI function
	                                          * pointers. */
	JavaVM     *_jvm;                        /* Handle to the JVM
	                                          * created via the
	                                          * Invocation API. This
	                                          * will be null if Squawk
	                                          * was called from Java
	                                          * code. */
#endif

	int         _interruptsDisabled;         /* Depth-count: for
	                                          * correct interrupt
	                                          * state changes */

#if KERNEL_SQUAWK
	boolean     _kernelMode;                 /* If true, kernel
	                                          * support for interrupts
	                                          * is enabled */
	int         _kernelSignal;               /* Signal number used for
	                                          * entering kernel
	                                          * mode */
	int         _kernelSignalCounter;        /* Count for number of
	                                          * signals received and
	                                          * not yet processed. */
	boolean     _kernelSendNotify;           /* Control whether to
	                                          * notify potential
	                                          * (user) waiters on
	                                          * return */
#endif /* KERNEL_SQUAWK */

#ifndef __MICROBLAZE__
	int         _deepSleepEnabled;           /* indicates whether the
	                                          * feature is currently
	                                          * enabled (=1) */
#endif
	int         _sleepManagerRunning;        /* assume that
	                                          * sleepManager is
	                                          * running until it calls
	                                          * WAIT_FOR_DEEP_SLEEP */
	int         _outstandingDeepSleepEvent;  /* whether the sleep
	                                          * manager thread should
	                                          * be unblocked at the
	                                          * next reschedule */
	long long   _storedDeepSleepWakeupTarget;/* The millis that the
	                                          * next deep sleep should
	                                          * end at */
	long long   _minimumDeepSleepMillis;     /* minimum time we're
	                                          * prepared to deep sleep
	                                          * for: avoid deep
	                                          * sleeping initially. */
	long long   _totalShallowSleepTime;      /* total time the SPOT
	                                          * has been shallow
	                                          * sleeping */

	boolean     _notrap;

	char        _service_stack[SERVICE_CHUNK_SIZE] __attribute__((aligned(MM_PAGE_SIZE)));
} Globals
#ifdef __MICROBLAZE__
__attribute__((aligned(MM_CACHELINE_SIZE)))
#endif
;

/*=======================================================================*\
 *                          Truly global globals                         *
\*=======================================================================*/

/* keep it thread/core local */
#ifdef __MICROBLAZE__
typedef struct {
	Globals* global_ctx;        /* pointer to the global context */
	/* Padding to avoid overlapping with other cores */
	char padding[MM_CACHELINE_SIZE-sizeof(Globals*)];
} globals_box __attribute__((aligned(MM_CACHELINE_SIZE)));
/* The pointer to the global execution context */
globals_box gps[AR_FORMIC_CORES_PER_BOARD];
#define gp gps[my_cid].global_ctx
#else
__thread Globals *gp;         /* The pointer to the global execution context */
#endif /* __MICROBLAZE__ */

#if KERNEL_SQUAWK
__thread Globals kernelGlobals;    /* The kernel mode execution context */
#endif	/* KERNEL_SQUAWK */

#define defineGlobal(x) gp->_##x
#define defineGlobalContext(c,x) c._##x


/*=======================================================================*\
 *                             Virtual globals                           *
\*=======================================================================*/

#define memory_g                            defineGlobal(memory)
#define memoryEnd_g                         defineGlobal(memoryEnd)
#define memorySize_g                        defineGlobal(memorySize)

#ifdef SC_NATIVE
#ifdef __MICROBLAZE__
#define cacheDirectory_g                    defineGlobal(cacheDirectory)
#define cacheStart_g                        defineGlobal(cacheStart)
#define cacheEnd_g                          defineGlobal(cacheEnd)
#define cacheSize_g                         defineGlobal(cacheSize)
#define cacheAllocTop_g                     defineGlobal(cacheAllocTop)
#define cacheAllocTemp_g                    defineGlobal(cacheAllocTemp)
#define cacheROAllocTop_g                   defineGlobal(cacheROAllocTop)
#define cacheROThreshold_g                  defineGlobal(cacheROThreshold)
#define cacheFlushes_g                      defineGlobal(cacheFlushes)
#define cacheClears_g                       defineGlobal(cacheClears)
#define cacheObjects_g                      defineGlobal(cacheObjects)
#define cachePendingWBs_g                   defineGlobal(cachePendingWBs)
#define cachePendingFEs_g                   defineGlobal(cachePendingFEs)
#else
#error SC_NATIVE is only supported on Formic
#endif /* __MICROBLAZE__ */
#endif /* SC_NATIVE */

#define schedulerLastCore_g                 defineGlobal(schedulerLastCore)

#ifndef MACROIZE
#define iparm_g                             defineGlobal(iparm)
#define ip_g                                defineGlobal(ip)
#define fp_g                                defineGlobal(fp)
#define sp_g                                defineGlobal(sp)
#else
#if TRACE
#define lastIP_g                            defineGlobal(lastIP)
#define lastFP_g                            defineGlobal(lastFP)
#endif /* TRACE */
#endif /* MACROIZE */

#define saved_ip_g                          defineGlobal(saved_ip)
#define saved_fp_g                          defineGlobal(saved_fp)
#define saved_sp_g                          defineGlobal(saved_sp)

#define sl_g                                defineGlobal(sl)
#define ss_g                                defineGlobal(ss)
#define bc_g                                defineGlobal(bc)
#define internalLowResult_g                 defineGlobal(internalLowResult)
#define Ints_g                              defineGlobal(Ints)
#define Addrs_g                             defineGlobal(Addrs)
#define Oops_g                              defineGlobal(Oops)
#define Buffers_g                           defineGlobal(Buffers)
#define BufferCount_g                       defineGlobal(BufferCount)

#ifdef __MICROBLAZE__
#define format_buf_g                        defineGlobal(format_buf)
#endif /* __MICROBLAZE__ */

#if KERNEL_SQUAWK
/* Nothing yet... */
#endif

#if PLATFORM_TYPE_SOCKET
#define ioport_g                            defineGlobal(ioport)
#define iosocket_g                          defineGlobal(iosocket)
#define result_low_g                        defineGlobal(result_low)
#define result_high_g                       defineGlobal(result_high)
#define io_ops_time_g                       defineGlobal(io_ops_time)
#define io_ops_count_g                      defineGlobal(io_ops_count)
#endif /* PLATFORM_TYPE_SOCKET */

#define cachedClassState_g                  defineGlobal(cachedClassState)
#define cachedClass_g                       defineGlobal(cachedClass)
#define cachedClassIndex_g                  defineGlobal(cachedClassIndex)
#ifdef INTERPRETER_STATS
#define cachedClassAccesses_g               defineGlobal(cachedClassAccesses)
#define cachedClassHits_g                   defineGlobal(cachedClassHits)
#endif /* INTERPRETER_STATS */

#define pendingMonitors_g                   defineGlobal(pendingMonitors)
#define pendingMonitorStackPointer_g        defineGlobal(pendingMonitorStackPointer)
#ifdef INTERPRETER_STATS
#define pendingMonitorAccesses_g            defineGlobal(pendingMonitorAccesses)
#define pendingMonitorHits_g                defineGlobal(pendingMonitorHits)
#endif /* INTERPRETER_STATS */

#ifndef __MICROBLAZE__
#define streams_g                           defineGlobal(streams)
#define currentStream_g                     defineGlobal(currentStream)
#endif /* __MICROBLAZE__ */

#if PLATFORM_TYPE_DELEGATING
#define channelIO_clazz_g                   defineGlobal(channelIO_clazz)
#define channelIO_execute_g                 defineGlobal(channelIO_execute)
#endif /* PLATFORM_TYPE_DELEGATING */

#define nativeFuncPtr_g                     defineGlobal(nativeFuncPtr)

#ifndef __MICROBLAZE__
#define STREAM_COUNT                        (sizeof(Streams) / sizeof(FILE*))
#endif /* __MICROBLAZE__ */

#if TRACE
#define traceFile_g                         defineGlobal(traceFile)
#define traceFileOpen_g                     defineGlobal(traceFileOpen)
#define traceServiceThread_g                defineGlobal(traceServiceThread)
#define traceLastThreadID_g                 defineGlobal(traceLastThreadID)
#define setLongCounter(high, low, x)        { high = (int)(x >> 32); low = (int)(x);}
#define getLongCounter(high, low)           ((((ujlong)(unsigned)high) << 32) | ((unsigned)low))
#define getBranchCount()                    getLongCounter(branchCountHigh_g, branchCountLow_g)
#define getTraceStart()                     getLongCounter(traceStartHigh_g, traceStartLow_g)
#define getTraceEnd()                       getLongCounter(traceEndHigh_g, traceEndLow_g)
#define setTraceStart(x)                    setLongCounter(traceStartHigh_g, traceStartLow_g, (x)); if ((x) == 0) { com_sun_squawk_VM_tracing = true; }
#define setTraceEnd(x)                      setLongCounter(traceEndHigh_g, traceEndLow_g, (x))
#define total_extends_g                     defineGlobal(total_extends)
#define total_slots_g                       defineGlobal(total_slots)
#define statsFrequency_g                    defineGlobal(statsFrequency)
#define lastThreadID_g                      defineGlobal(lastThreadID)
#else
#define getBranchCount()                    ((jlong)-1L)
#endif /* TRACE */

#ifdef PROFILING
#define sampleFrequency_g                   defineGlobal(sampleFrequency)
#define instructionCount_g                  defineGlobal(instructionCount)
#endif /* PROFILING */

#if defined(PROFILING) | TRACE
#define lastStatCount_g                     defineGlobal(lastStatCount)
#endif /* PROFILING */

#define inFatalVMError_g                    defineGlobal(inFatalVMError)
#define service_stack_g                     defineGlobal(service_stack)
#define squawk_inStop_g                     defineGlobal(squawk_inStop)

#if PLATFORM_TYPE_DELEGATING
#define JNI_env_g                           defineGlobal(JNI_env)
#define jvm_g                               defineGlobal(jvm)
#endif

#define interruptsDisabled_g                defineGlobal(interruptsDisabled)

#if KERNEL_SQUAWK
#define kernelMode_g                        defineGlobal(kernelMode)
#define kernelSignal_g                      defineGlobal(kernelSignal)
#define kernelSignalCounter_g               defineGlobal(kernelSignalCounter)
#define kernelSendNotify_g                  defineGlobal(kernelSendNotify)
#endif /* KERNEL_SQUAWK */

#ifndef __MICROBLAZE__
#define deepSleepEnabled_g                  defineGlobal(deepSleepEnabled)
#endif /* __MICROBLAZE__ */

#define sleepManagerRunning_g               defineGlobal(sleepManagerRunning)
#define outstandingDeepSleepEvent_g         defineGlobal(outstandingDeepSleepEvent)
#define storedDeepSleepWakeupTarget_g       defineGlobal(storedDeepSleepWakeupTarget)
#define minimumDeepSleepMillis_g            defineGlobal(minimumDeepSleepMillis)
#define totalShallowSleepTime_g             defineGlobal(totalShallowSleepTime)


#define notrap_g                            defineGlobal(notrap)

/**
 * Initialize/re-initialize the globals to their defaults.
 */
int initializeGlobals(Globals *globals) {
	gp = globals;
	memset(gp, 0, sizeof(Globals));

	/*
	 * Initialize the variables that have non-zero defaults.
	 */
	com_sun_squawk_VM_extendsEnabled = true;
	interruptsDisabled_g = 0;      /* enabled by default */
	runningOnServiceThread_g = true;
	pendingMonitors_g = &Oops_g[ROM_GLOBAL_OOP_COUNT];

#ifndef __MICROBLAZE__
	streams_g[com_sun_squawk_VM_STREAM_STDOUT] = stdout;
	streams_g[com_sun_squawk_VM_STREAM_STDERR] = stderr;
	currentStream_g = com_sun_squawk_VM_STREAM_STDERR;

#if TRACE
	setTraceStart(TRACESTART);
	setTraceEnd(TRACESTART);
	traceLastThreadID_g = -2;
	traceServiceThread_g = true;
#endif /* TRACE */

#endif /* __MICROBLAZE__ */

#if PLATFORM_TYPE_SOCKET
	ioport = null;
	iosocket = -1;
#endif

	sleepManagerRunning_g    = 1;
	minimumDeepSleepMillis_g = 0x7FFFFFFFFFFFFFFFLL;

	return 0;
}

/**
 * Prints the name and current value of all the globals.
 */
void printGlobals() {
#ifndef __MICROBLAZE__
	FILE *vmOut = streams[currentStream_g];
#endif  /* __MICROBLAZE__ */
#if TRACE
	int i;

	// Print the global integers
	fprintf(vmOut, "Global ints:\n");
	for (i = 0; i != GLOBAL_INT_COUNT; ++i) {
		fprintf(vmOut, "  %s = %d\n", getGlobalIntName(i), Ints_g[i]);
	}

	// Print the global oops
	fprintf(vmOut, "Global oops:\n");
	for (i = 0; i != GLOBAL_OOP_COUNT; ++i) {
		fprintf(vmOut, format("  %s = %A\n"), getGlobalOopName(i), Oops_g[i]);
	}

	// Print the global addresses
	fprintf(vmOut, "Global addresses:\n");
	for (i = 0; i != GLOBAL_ADDR_COUNT; ++i) {
		fprintf(vmOut, format("  %s = %A\n"), getGlobalAddrName(i), Addrs_g[i]);
	}
#else
	fprintf(vmOut, "printGlobals() requires tracing\n");
#endif /* TRACE */
}

/**
 * Sets the stream for the VM.print... methods to one of the com_sun_squawk_VM_STREAM_... constants.
 *
 * @param stream  the stream to use for the print... methods
 * @return the current stream used for VM printing
 *
 * @vm2c proxy( setStream )
 */
#ifdef __MICROBLAZE__
# define setStream(x)
#else
int setStream(int stream) {
	int result = currentStream_g;
	currentStream_g = stream;
	if (streams_g[currentStream_g] == null) {
		switch (currentStream_g) {
#if com_sun_squawk_Klass_ENABLE_DYNAMIC_CLASSLOADING
		case com_sun_squawk_VM_STREAM_SYMBOLS: {
			streams_g[currentStream_g] = fopen("squawk_dynamic.sym", "w");
			break;
		}
#endif /* ENABLE_DYNAMIC_CLASSLOADING */
		default: {
			NORETURN void fatalVMError(char *msg);
			fatalVMError("Bad INTERNAL_SETSTREAM");
		}
		}
	}
	return result;
}
#endif /* __MICROBLAZE__ */

/**
 * Closes all the open files used for VM printing.
 */
#ifdef __MICROBLAZE__
# define finalizeStreams()
#else
void finalizeStreams() {
	int i;
	for (i = 0 ; i < MAX_STREAMS ; i++) {
		FILE *file = streams_g[i];
		if (file != null) {
			fflush(file);
#ifndef FLASH_MEMORY
			if (file != stdout && file != stderr) {
				fclose(file);
			}
#endif /* FLASH_MEMORY */
		}
	}
}
#endif /* __MICROBLAZE__ */

/* These macros are useful for recording the current context
 * (e.g., user or kernel) in a data structure such as a message.
 */
#define setContext(v, t)           (v = (t)gp)
#define isCurrentContext(v, t)     ((v) == (t)gp)


typedef int (*funcPtr0)();
typedef int (*funcPtr1)(int);
typedef int (*funcPtr2)(int, int);
typedef int (*funcPtr3)(int, int, int);
typedef int (*funcPtr4)(int, int, int, int);
typedef int (*funcPtr5)(int, int, int, int, int);
typedef int (*funcPtr6)(int, int, int, int, int, int);
typedef int (*funcPtr7)(int, int, int, int, int, int, int);
typedef int (*funcPtr8)(int, int, int, int, int, int, int, int);
typedef int (*funcPtr9)(int, int, int, int, int, int, int, int, int);
typedef int (*funcPtr10)(int, int, int, int, int, int, int, int, int, int);
