
/*****************************************************************************
**
** file    : ncuxinit.c
** purpose : default entry point for each shared lib with
**           user exits
**
** (c) copyright 1991-97 by Software AG
**
******************************************************************************
**
** DO NOT CHANGE ANYTHING IN THIS FILE !
**
*****************************************************************************/

#define SHARED_MODE 1

/* 
** following include is not a good 'C' style but it is a
** possibility to avoid different sources for dynamic and static 
** linked user exits.
*/
#include "rh4n_nat_jumptab.c"

int ncsy_dyuxinit(table, size)
NC_TAB_STRUCT **table;
int *size;
{

  *table = (NC_TAB_STRUCT *)n_call_table;
  *size  = n_call_tab;

  return 0;

}

