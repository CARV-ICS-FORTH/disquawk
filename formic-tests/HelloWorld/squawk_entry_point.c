/****************************************************************************/
/*                                                                          */
/*                             FORTH-ICS / CARV                             */
/*                                                                          */
/*                       Proprietary and confidential                       */
/*                            Copyright (c) 2013                            */
/*                                                                          */
/* ======================================================================== */
/*                                                                          */
/* Author        : Foivos S. Zakkak                                         */
/*                                                                          */
/* Abstract      : Main Squawk entry point, responsible for passing the     */
/*                 appropriate arguments to the JVM.                        */
/*                                                                          */
/****************************************************************************/

#include <kernel_toolset.h>

void squawk_entry_point(void)
{
	extern int __linker_squawk_suite_start;
	char          *fakeArgv[4];
	int           fakeArgc;

	fakeArgv[0] = "dummy";
	fakeArgv[1] = "-spotsuite:FormicApp";
	fakeArgv[2] = "HelloWorld";
	fakeArgc    = 3;

	Squawk_main_wrapper(fakeArgc, fakeArgv);

	return;
}
