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

#ifndef MPIRESOURCE_H
#define MPIRESOURCE_H

#include "condor_common.h"
#include "remoteresource.h"

/** Here we have a remote resource that is specific to an MPI job. */

class MpiResource : public RemoteResource {

 public:

		/** See the RemoteResource's constructor.
		*/
	MpiResource( BaseShadow *shadow );

		/// Destructor
	~MpiResource() {};

		/** Call RemoteResource::activateClaim() and log a
			NodeExecuteEvent */
	virtual bool activateClaim( int starterVersion = 2 );

		/** Special format... */
	virtual void printExit( FILE *fp );

	int node( void ) { return node_num; };
	void setNode( int node ) { node_num = node; };

		/** Call RemoteResource::resourceExit() and log a
			NodeTerminatedEvent to the UserLog
		*/
	void resourceExit( int reason, int status );

		/** Before we log anything to the UserLog, we want to
			initialize the UserLog with our node number.  
		*/
	virtual bool writeULogEvent( ULogEvent* event );

 private:

		// Making these private PREVENTS copying.
	MpiResource( const MpiResource& );
	MpiResource& operator = ( const MpiResource& );

	int node_num;
};


#endif
