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
  char          *fakeArgv[4];
  int           fakeArgc, i;

  fakeArgv[0] = "dummy";
  fakeArgv[1] = "-Xtgc:1";
  fakeArgv[2] = "-spotsuite:FormicApp";
// Must also change the MAKEFILE
#if 0
  fakeArgv[3] = "spec.benchmarks.scimark.lu.Main";
#endif
#if 1
  fakeArgv[3] = "spec.benchmarks.scimark.fft.Main";
#endif
#if 0
  fakeArgv[3] = "spec.benchmarks.scimark.sor.Main";
#endif
#if 0
  fakeArgv[3] = "spec.benchmarks.scimark.sparse.Main";
#endif
  fakeArgc    = 4;

  kt_printf("Invoking squawk with:");
  for(i=0; i<fakeArgc; ++i)
    kt_printf(" %s", fakeArgv[i]);
  kt_printf("\n");

  Squawk_main_wrapper(fakeArgc, fakeArgv);

  return;
}
