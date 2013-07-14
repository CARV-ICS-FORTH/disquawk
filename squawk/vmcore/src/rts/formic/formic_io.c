/*
 * Copyright 2004-2010 Sun Microsystems, Inc. All Rights Reserved.
 * Copyright 2011 Oracle. All Rights Reserved.
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
 * Please contact Oracle, 16 Network Circle, Menlo Park, CA 94025 or
 * visit www.oracle.com if you need additional information or have
 * any questions.
 */


/*
 * Enter shallow sleep
 */
void doShallowSleep(long long targetMillis) { return; }

extern int __linker_formic_code;
void printStackTrace(const char *msg);

/**
 * Execute an IO operation for a Squawk isolate.
 */
static void ioExecute(void) {
//  int     context = com_sun_squawk_ServiceOperation_context;
    int     op      __attribute__((unused)) = com_sun_squawk_ServiceOperation_op;
//  int     channel = com_sun_squawk_ServiceOperation_channel;
    int     i1      __attribute__((unused)) = com_sun_squawk_ServiceOperation_i1;
    int     i2      __attribute__((unused)) = com_sun_squawk_ServiceOperation_i2;
    int     i3      __attribute__((unused)) = com_sun_squawk_ServiceOperation_i3;
    int     i4      __attribute__((unused)) = com_sun_squawk_ServiceOperation_i4;
    int     i5      __attribute__((unused)) = com_sun_squawk_ServiceOperation_i5;
    int     i6      __attribute__((unused)) = com_sun_squawk_ServiceOperation_i6;
    Address send    __attribute__((unused)) = com_sun_squawk_ServiceOperation_o1;
    Address receive __attribute__((unused)) = com_sun_squawk_ServiceOperation_o2;

    int res = ChannelConstants_RESULT_OK;

    switch (op) {
    case ChannelConstants_GET_CURRENT_TIME_ADDR:
      res = sysTimeMillis();
      break;
    case ChannelConstants_GLOBAL_GETEVENT:
      //res = getEvent();
      break;
    /* case ChannelConstants_GET_PUBLIC_KEY: */
    /*   break; */
    case ChannelConstants_GET_FILE_VIRTUAL_ADDRESS:
      res = (int)&__linker_formic_code;
      // res = 0xdeaddead;
      break;
    default:
      printf("ioExecute unknown op: %d\n", op);
      printStackTrace("");
      res = ChannelConstants_RESULT_BADPARAMETER;
    }
    com_sun_squawk_ServiceOperation_result = res;

}
