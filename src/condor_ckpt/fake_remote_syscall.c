/***************************Copyright-DO-NOT-REMOVE-THIS-LINE**
 * CONDOR Copyright Notice
 *
 * See LICENSE.TXT for additional notices and disclaimers.
 *
 * Copyright (c)1990-1998 CONDOR Team, Computer Sciences Department, 
 * University of Wisconsin-Madison, Madison, WI.  All Rights Reserved.  
 * No use of the CONDOR Software Program Source Code is authorized 
 * without the express consent of the CONDOR Team.  For more information 
 * contact: CONDOR Team, Attention: Professor Miron Livny, 
 * 7367 Computer Sciences, 1210 W. Dayton St., Madison, WI 53706-1685, 
 * (608) 262-0856 or miron@cs.wisc.edu.
 *
 * U.S. Government Rights Restrictions: Use, duplication, or disclosure 
 * by the U.S. Government is subject to restrictions as set forth in 
 * subparagraph (c)(1)(ii) of The Rights in Technical Data and Computer 
 * Software clause at DFARS 252.227-7013 or subparagraphs (c)(1) and 
 * (2) of Commercial Computer Software-Restricted Rights at 48 CFR 
 * 52.227-19, as applicable, CONDOR Team, Attention: Professor Miron 
 * Livny, 7367 Computer Sciences, 1210 W. Dayton St., Madison, 
 * WI 53706-1685, (608) 262-0856 or miron@cs.wisc.edu.
****************************Copyright-DO-NOT-REMOVE-THIS-LINE**/

 

/*
  This routine is for linking with the standalone checkpointing library.  Since
  programs linked that way should never attempt remote system calls, it is
  a fatal error if this routine ever gets called.
*/

#include "condor_common.h"
#include "condor_debug.h"
#include "syscall_numbers.h"

int
REMOTE_syscall( int syscall_num, ... )
{
	char *text;

	va_list args;
	va_start( args, syscall_num );

	text = va_arg( args, char * );

	if( syscall_num==CONDOR_report_error ) {
		fprintf(stderr,"Condor Warning: %s\n",text);
	} else {
		EXCEPT( "Linked for standalone checkpointing, but called REMOTE_syscall()");
	}

	va_end(args);
}
