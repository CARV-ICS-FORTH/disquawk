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
  char          *fakeArgv[10];
  int           fakeArgc;

  fakeArgv[0] = "dummy";
  fakeArgv[1] = "-spotsuite:FormicApp";
  fakeArgv[2] = "-stats";
  fakeArgv[3] = "-verbose";
  fakeArgv[4] = "jembench.Main";
  /* fakeArgv[5] = "-micro"; */
  fakeArgv[5] = "-kernel";
  /* fakeArgv[5] = "-application"; */
  /* fakeArgv[5] = "-parallel"; */
  /* fakeArgv[5] = "-stream"; */
  fakeArgc    = 6;

  Squawk_main_wrapper(fakeArgc, fakeArgv);

  return;
}
