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

extern int Squawk_main_wrapper(int, char **);

void squawk_entry_point(void)
{
  char          *fakeArgv[3];
  int           fakeArgc, i;

  fakeArgv[0] = "dummy";
  fakeArgv[1] = "-spotsuite:FormicApp";
  fakeArgv[2] = "Linpack";
  fakeArgc    = 3;

  kt_printf("Invoking squawk with:");
  for(i=0; i<fakeArgc; ++i)
    kt_printf(" %s", fakeArgv[i]);
  kt_printf("\n");

  Squawk_main_wrapper(fakeArgc, fakeArgv);

  return;
}
