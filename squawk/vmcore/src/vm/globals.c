/*
 * Copyright (C) 2013-2014 FORTH-ICS / CARV
 *                         (Foundation for Research & Technology -- Hellas,
 *                          Institute of Computer Science,
 *                          Computer Architecture & VLSI Systems Laboratory)
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

#include <os.h>
#include "globals.h"

/*=======================================================================*\
 *                          Truly global globals                         *
\*=======================================================================*/

/* keep it thread/core local */
#ifdef __MICROBLAZE__
/* The pointer to the global execution context */
globals_box gps[AR_FORMIC_CORES_PER_BOARD];
#else
__thread Globals *gp;         /* The pointer to the global execution context */
__thread mmgrGlobals *mmgr_g;
#endif /* __MICROBLAZE__ */

#if KERNEL_SQUAWK
__thread Globals kernelGlobals;    /* The kernel mode execution context */
#endif	/* KERNEL_SQUAWK */


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

#if (defined VERY_VERBOSE)
	com_sun_squawk_VM_verboseLevel = 2;
#elif (defined VERBOSE)
	com_sun_squawk_VM_verboseLevel = 1;
#else
	com_sun_squawk_VM_verboseLevel = 0;
#endif

#ifndef FORMIC
	streams_g[com_sun_squawk_VM_STREAM_STDOUT] = stdout;
	streams_g[com_sun_squawk_VM_STREAM_STDERR] = stderr;
	currentStream_g = com_sun_squawk_VM_STREAM_STDERR;

#if TRACE
	setTraceStart(TRACESTART);
	setTraceEnd(TRACESTART);
	traceLastThreadID_g  = -2;
	traceServiceThread_g = true;
#endif /* TRACE */

#endif /* FORMIC */

#if PLATFORM_TYPE_SOCKET
	ioport = null;
	iosocket = -1;
#endif

	sleepManagerRunning_g    = 1;
	minimumDeepSleepMillis_g = 0x7FFFFFFFFFFFFFFFLL;

	BufferCount_g = 0;

	return 0;
}

/**
 * Prints the name and current value of all the globals.
 */
void printGlobals() {
#ifndef FORMIC
	FILE *vmOut = streams[currentStream_g];
#endif  /* FORMIC */
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
#ifndef FORMIC
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
#endif /* FORMIC */

/**
 * Closes all the open files used for VM printing.
 */
#ifndef FORMIC
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
#endif /* FORMIC */
