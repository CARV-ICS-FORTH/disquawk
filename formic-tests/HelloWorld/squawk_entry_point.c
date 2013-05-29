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
  char          *fakeArgv[5];
  char          arg1[256];
  int           fakeArgc, i;

  kt_sprintf(arg1, "-Xboot:0x%p", &__linker_squawk_suite_start);
  fakeArgv[0] = "dummy";
  fakeArgv[1] = arg1;
  fakeArgv[2] = "-Xtr:0";
  fakeArgv[3] = "-spotsuite:FormicApp";
  fakeArgv[4] = "HelloWorld";
  fakeArgc    = 5;

  kt_printf("Invoking squawk with:");
  for(i=0; i<fakeArgc; ++i)
    kt_printf(" %s", fakeArgv[i]);
  kt_printf("\n");

  Squawk_main_wrapper(fakeArgc, fakeArgv);

  return;
}
