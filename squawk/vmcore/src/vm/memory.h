#include "address.h"

int getShort(Address base, Offset offset);
void setInt(Address base, Offset offset, int value);
int getInt(Address base, Offset offset);
Address getObject(Address base, Offset offset);
void zeroWords(UWordAddress start, UWordAddress end);
int getDataSize(Address klass);
 * Copyright     2015, FORTH-ICS / CARV
 *                    (Foundation for Research & Technology -- Hellas,
 *                     Institute of Computer Science,
 *                     Computer Architecture & VLSI Systems Laboratory)
