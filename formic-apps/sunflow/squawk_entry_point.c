/****************************************************************************/
/*                                                                          */
/*                             FORTH-ICS / CARV                             */
/*                                                                          */
/*                       Proprietary and confidential                       */
/*                            Copyright (c) 2015                            */
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

extern void Squawk_main_wrapper(int fakeArgc, char** fakeArgv);

void squawk_entry_point(void)
{
  char          *fakeArgv[8];
  int           fakeArgc;

  fakeArgv[0] = "dummy";
  fakeArgv[1] = "-spotsuite:FormicApp";
  fakeArgv[2] = "-stats";
  fakeArgv[3] = "-verbose";
  fakeArgv[4] = "org.sunflow.Benchmark";
  fakeArgv[5] = "-bench";
  fakeArgv[6] = "7";            /* Number of threads */
  fakeArgv[7] = "32";           /* Image resolution */
  /* fakeArgv[7] = "64";           /\* Image resolution *\/
   * fakeArgv[7] = "128";           /\* Image resolution *\/
   * fakeArgv[7] = "256";           /\* Image resolution *\/ */
  fakeArgc    = 8;

  Squawk_main_wrapper(fakeArgc, fakeArgv);

  return;
}
