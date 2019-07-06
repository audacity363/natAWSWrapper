
/*****************************************************************************
**
** file    : nattab.h
** purpose : NATURAL link interface to user supplied C functions
**
** (c) copyright 1991-97 by Software AG
**
******************************************************************************
**
** DO NOT CHANGE ANYTHING IN THIS FILE !
**
*****************************************************************************/

#ifndef DEF_NATTAB
#define DEF_NATTAB	1

#define NAME_SIZE 9 /* 8 chars and terminator */

/*
** structure for user exit call table
*/

typedef struct nc_tab_struct {

  char fctn_name[NAME_SIZE]; /* natural name of function */
  long (*proc_ptr)(); /* pointer to function entry */

} NC_TAB_STRUCT;

/*
** if we use shared library access ==>
** we have to define tha data as static
*/
#ifndef SHARED_MODE

#define TAB_STRUCT NC_TAB_STRUCT  
#define TAB_SIZE   int

#else

#define TAB_STRUCT static NC_TAB_STRUCT  
#define TAB_SIZE   static int

#endif

#endif /* DEF_NATTAB */
/*
** end of file nattab.h
*/

