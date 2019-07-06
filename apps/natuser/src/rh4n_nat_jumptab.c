/**
 * @file jumptab.c
 *
 * @brief Lookup table for natural for the user exists in this shared Library
 *
 * (I shouldn't edit anything at this position in the file. Upsss....)
 *
 * @author SoftwareAG / Tom Engemann
 */

/***********************************************************************
**
** file    : jumptab.c
** purpose : linkage between natural and the user defined functions
**
** (c) copyright 1991,92,93 by Software AG
**
************************************************************************
**
** section 1: define structure of a jump table entry.
**
** DO NOT CHANGE ANYTHING IN THIS SECTION !
*/
#include "nattab.h"

/*
** section 2: define external example routines
**
** START OF CODE TO BE CHANGED BY THE USER
*/

extern long rh4nnatGenJSON();
extern long rh4nnatGetURLObj();
extern long rh4nnatGetURLVar();
extern long rh4nnatGetBodyObj();
extern long rh4nnatGetBodyVar();
extern long rh4nnatGetUser();
extern long rh4nnatDebugInit();
/*
** section 3: initialize the jumptable
**
** the function name visible to NATURAL must not be longer than 8
** uppercase characters.  they must contain only uppercase letters.
** the entries in the array must be alphabetically (!!!!!!!!!!!!!!) sorted by the
** function names.
*/

TAB_STRUCT n_call_table[] = {

  { "RHGENJS", rh4nnatGenJSON},      //Generate JSON
  { "RHGETUO", rh4nnatGetURLObj },   //Match by name (URL)
  { "RHGETUV", rh4nnatGetURLVar },   //Get URL variable
  { "RHGETBO", rh4nnatGetBodyObj },  //Match by name (BODY)
  { "RHGETBV", rh4nnatGetBodyVar },  //Get BODY variable
  { "RHGETUS", rh4nnatGetUser },     //Get the current user
  { "RHDBGINI", rh4nnatDebugInit},               //Placeholder for framework init (debugging in NATOne)
  { "RHSETRC", 0x00 }                //call interface4 'RHSETRC' rh4n-internals 204
};


/*
** section 4: make number of elements in the jump table availale to
** NATURAL.
**
** END OF CODE TO BE CHANGED BY THE USER
*/

TAB_SIZE n_call_tab = sizeof(n_call_table) / sizeof(n_call_table[0]);

/*
** end of file jumptab.c
*/
