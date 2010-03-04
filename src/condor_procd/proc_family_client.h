/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/


#ifndef _PROC_FAMILY_CLIENT_H
#define _PROC_FAMILY_CLIENT_H

#include "proc_family_io.h"
#include "../condor_procapi/procapi.h"

#include <vector>

class LocalClient;

class ProcFamilyClient {

public:

	ProcFamilyClient() : m_initialized(false), m_client(NULL) { }

	// we use a plain old member function instead of the constructor
	// to do the real initialization work so that we can give our
	// caller an indication if something goes wrong
	//
	bool initialize(const char*);

	~ProcFamilyClient();

	// tell the procd to start tracking a new subfamily
	//
	bool register_subfamily(pid_t root_pid,
	                        pid_t watcher_pid,
	                        int   max_snapshot_interval,
	                        bool& response);

	// tell ProcD to track a family via environment variables
	//
	bool track_family_via_environment(pid_t pid, PidEnvID& penvid, bool&);

	// tell ProcD to track a family via login
	//
	bool track_family_via_login(pid_t pid, const char* login, bool&);

#if defined(LINUX)
	// tell ProcD to track a family via supplementary group id
	//
	bool track_family_via_supplementary_group(pid_t pid, bool&, gid_t&);
#endif

	// tell the ProcD it needs to use glexec (along with the given
	// proxy) in order to signal the specified family
	//
	bool use_glexec_for_family(pid_t pid,
	                           const char* proxy,
	                           bool& response);

	// ask the procd for usage information about a process
	// family
	//
	bool get_usage(pid_t, ProcFamilyUsage&, bool&);

	// tell the procd to send a signal to a single process
	//
	bool signal_process(pid_t, int, bool&);

	// tell the procd to suspend a family tree
	//
	bool suspend_family(pid_t, bool&);

	// tell the procd to continue a suspended family tree
	//
	bool continue_family(pid_t, bool&);

	// tell the procd to kill an entire family (and all
	// subfamilies of that family)
	//
	bool kill_family(pid_t, bool&);
	
	// tell the procd to kill all subfamilies of a family
        bool signal_children(pid_t, int, bool&);
	
	// tell the procd we don't care about this family any
	// more
	//
	bool unregister_family(pid_t, bool&);

	// tell the procd to take a snapshot
	//
	bool snapshot(bool&);

	// tell the procd to exit
	//
	bool quit(bool&);

	// tell the procd to dump out its state
	//
	bool dump(pid_t, bool&, std::vector<ProcFamilyDump>&);

private:

	// common code to send a signal to a single process or 
	// children of a process 
	//
	bool signal_process(pid_t, int, proc_family_command_t, bool&);
	
	// common code for killing, suspending, and
	// continuing a family
	//
	bool signal_family(pid_t, proc_family_command_t, bool&);

	// set true once we're properly initialized
	//
	bool m_initialized;

	// object for managing our connection to the ProcD
	//
	LocalClient* m_client;
};

#endif
