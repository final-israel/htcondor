/***************************Copyright-DO-NOT-REMOVE-THIS-LINE**
  *
  * Condor Software Copyright Notice
  * Copyright (C) 1990-2004, Condor Team, Computer Sciences Department,
  * University of Wisconsin-Madison, WI.
  *
  * This source code is covered by the Condor Public License, which can
  * be found in the accompanying LICENSE.TXT file, or online at
  * www.condorproject.org.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * AND THE UNIVERSITY OF WISCONSIN-MADISON "AS IS" AND ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY, OF SATISFACTORY QUALITY, AND FITNESS
  * FOR A PARTICULAR PURPOSE OR USE ARE DISCLAIMED. THE COPYRIGHT
  * HOLDERS AND CONTRIBUTORS AND THE UNIVERSITY OF WISCONSIN-MADISON
  * MAKE NO MAKE NO REPRESENTATION THAT THE SOFTWARE, MODIFICATIONS,
  * ENHANCEMENTS OR DERIVATIVE WORKS THEREOF, WILL NOT INFRINGE ANY
  * PATENT, COPYRIGHT, TRADEMARK, TRADE SECRET OR OTHER PROPRIETARY
  * RIGHT.
  *
  ****************************Copyright-DO-NOT-REMOVE-THIS-LINE**/

#ifndef DAG_H
#define DAG_H

#include "condor_common.h"
#include "list.h"
#include "job.h"
#include "scriptQ.h"
#include "user_log.c++.h"          /* from condor_c++_util/ directory */
#include "condor_constants.h"      /* from condor_includes/ directory */
#include "HashTable.h"
#include "../condor_daemon_core.V6/condor_daemon_core.h"
#include "read_multiple_logs.h"
#include "check_events.h"

// for DAGMAN_RUN_POST_ON_FAILURE config setting
extern bool run_post_on_failure;

// the name of the attr we insert in job ads, recording DAGMan's job id
extern const char* DAGManJobIdAttrName;

// for the Condor job id of the DAGMan job
extern char* DAGManJobId;

// NOTE: must be kept in sync with _job_type_t
enum Log_source{
  CONDORLOG,
  DAPLOG
};

class Dagman;

//------------------------------------------------------------------------
/** A Dag instance stores information about a job dependency graph,
    and the state of jobs as they are running. Jobs are run by
    submitting them to condor using the UserLog API<p>
    
    The public interface to a Dag allows you to add jobs, add
    dependencies, submit jobs, control the locations of log files,
    query the state of a job, and process any new events that have
    appeared in the condor log file.
*/
class Dag {
  public:
  
    /** Create a DAG
		@param dagFile the DAG file name
		@param condorLogName the log file name specified by the -Condorlog
		       command line argument
        @param maxJobsSubmitted the maximum number of jobs to submit to Condor
               at one time
        @param maxPreScripts the maximum number of PRE scripts to spawn at
		       one time
        @param maxPostScripts the maximum number of POST scripts to spawn at
		       one time
		@param allowExtraRuns whether to tolerate the Condor "submit once,
			   run twice" bug
		@param dapLogName the name of the Stork (DaP) log file
		@param allowLogError whether to allow the DAG to run even if we
			   have an error determining the job log files
    */

    Dag( char *dagFile, char *condorLogName, const int maxJobsSubmitted,
		 const int maxPreScripts, const int maxPostScripts, 
		 bool allowExtraRuns, const char *dapLogName, bool allowLogError );

    ///
    ~Dag();

	/** Initialize log files, lock files, etc.
	*/
	void InitializeDagFiles( char *lockFileName );

    /** Prepare DAG for initial run.  Call this function ONLY ONCE.
		@param the appropriate Dagman object
        @param recovery specifies if this is a recovery
        @return true: successful, false: failure
    */
    bool Bootstrap (const Dagman &dm, bool recovery);

    /// Add a job to the collection of jobs managed by this Dag.
    bool Add( Job& job );
  
    /** Specify a dependency between two jobs. The child job will only
        run after the parent job has finished.
        @param parent The parent job
        @param child The child job (depends on the parent)
        @return true: successful, false: failure
    */
    bool AddDependency (Job * parent, Job * child);
  
    /** Blocks until the Condor Log file grows.
        @return true: log file grew, false: timeout or shrinkage
    */

    
    //    bool DetectLogGrowth();
    bool DetectCondorLogGrowth();         //<--DAP 
    bool DetectDaPLogGrowth();            //<--DAP

    /** Force the Dag to process all new events in the condor log file.
        This may cause the state of some jobs to change.

		@param the appropriate Dagman object
        @param recover Process Log in Recover Mode, from beginning to end
        @return true on success, false on failure
    */
    //    bool ProcessLogEvents (bool recover = false);

    bool ProcessLogEvents (const Dagman &dm, int logsource,
			bool recovery = false); //<--DAP

	/** Process a single event.  Note that this is called every time
			we attempt to read the user log, so we may or may not have
			a valid event here.
		@param The outcome from the attempt to read the user log.
	    @param The event.
		@param Whether we're in recovery mode.
		@param Whether to do event checks.
		@param Whether we're done trying to read events (set by this
			function).
		@return True if the DAG should continue, false if we should abort.
	*/
	bool ProcessOneEvent (ULogEventOutcome outcome, const ULogEvent *event,
			bool recovery, bool doEventChecks, bool &done);

	/** Print a submit event (we try to match it up with the right DAG
			node).
	    @param The event.
		@param The name of the event.
		@param The job corresponding to this event (NOTE: may be changed
				by the function!).
		@param	The job ID of the event.
		@param Whether we're in recovery mode.
	*/
	void PrintSubmitEvent(const ULogEvent *event, const char *eventName,
			Job *&job, const CondorID &condorID, bool recovery);

	/** Process an abort or executable error event.
	    @param The event.
		@param The job corresponding to this event.
		@param Whether we're in recovery mode.
	*/
	void ProcessAbortEvent(const ULogEvent *event, Job *job,
			bool recovery);

	/** Process a terminated event.
	    @param The event.
		@param The job corresponding to this event.
		@param Whether we're in recovery mode.
	*/
	void ProcessTerminatedEvent(const ULogEvent *event, Job *job,
			bool recovery);

	/** Process a post script terminated event.
	    @param The event.
		@param The job corresponding to this event.
		@param Whether we're in recovery mode.
	*/
	void ProcessPostTermEvent(const ULogEvent *event, Job *job,
			bool recovery);

	/** Process a submit event.
	    @param The event.
		@param The event name.
		@param The job corresponding to this event.
		@param The Condor ID corresponding to this event.
		@param Whether we're in recovery mode.
	*/
	void ProcessSubmitEvent(const ULogEvent *event, const char *eventName,
			Job *job, const CondorID &condorID, bool recovery);

    /** Get pointer to job with id jobID
        @param the handle of the job in the DAG
        @return address of Job object, or NULL if not found
    */
    Job * GetJob (const JobID_t jobID) const;

    /** Get pointer to job with name jobName
        @param jobName the name of the job in the DAG
        @return address of Job object, or NULL if not found
    */
    Job * GetJob (const char * jobName) const;

    /** Get pointer to job with condor ID condorID
        @param condorID the CondorID of the job in the DAG
        @return address of Job object, or NULL if not found
    */
    Job * GetJob (const CondorID condorID) const;

    /** Ask whether a node name exists in the DAG
        @param nodeName the name of the node in the DAG
        @return true if the node exists, false otherwise
    */
    bool NodeExists( const char *nodeName ) const;

    /// Print the list of jobs to stdout (for debugging).
    void PrintJobList() const;
    void PrintJobList( Job::status_t status ) const;

    /** @return the total number of jobs in the DAG
     */
    inline int NumJobs() const { return _jobs.Number(); }

    /** @return the number of jobs completed
     */
    inline int NumJobsDone() const { return _numJobsDone; }

    /** @return the number of jobs that failed in the DAG
     */
    inline int NumJobsFailed() const { return _numJobsFailed; }

    /** @return the number of jobs currently submitted to Condor
     */
    inline int NumJobsSubmitted() const { return _numJobsSubmitted; }

    /** @return the number of jobs ready to submit to Condor
     */
    inline int NumJobsReady() const { return _readyQ->Number(); }

    /** @return the number of PRE scripts currently running
     */
    inline int NumPreScriptsRunning() const
		{ return _preScriptQ->NumScriptsRunning(); }

    /** @return the number of POST scripts currently running
     */
    inline int NumPostScriptsRunning() const
		{ return _postScriptQ->NumScriptsRunning(); }

    /** @return the total number of PRE/POST scripts currently running
     */
    inline int NumScriptsRunning() const
		{ return NumPreScriptsRunning() + NumPostScriptsRunning(); }

	inline bool Done() const { return NumJobsDone() == NumJobs(); }

		/** Submit all ready jobs, provided they are not waiting on a
			parent job or being throttled.
			@param the appropriate Dagman object
			@return number of jobs successfully submitted
		*/
    int SubmitReadyJobs(const Dagman &dm);
  
    /** Remove all jobs (using condor_rm) that are currently running.
        All jobs currently marked Job::STATUS_SUBMITTED will be fed
        as arguments to condor_rm via popen.  This function is called
        when the Dagman Condor job itself is removed by the user via
        condor_rm.  This function <b>is not</b> called when the schedd
        kills Dagman.
    */
    void RemoveRunningJobs ( const Dagman & ) const;

    /** Remove all pre- and post-scripts that are currently running.
	All currently running scripts will be killed via daemoncore.
	This function is called when the Dagman Condor job itself is
	removed by the user via condor_rm.  This function <b>is not</b>
	called when the schedd kills Dagman.
    */
    void RemoveRunningScripts ( const Dagman & ) const;

    /** Creates a DAG file based on the DAG in memory, except all
        completed jobs are premarked as DONE.
        @param rescue_file The name of the rescue file to generate
        @param datafile The original DAG config file to read from
    */
    void Rescue (const char * rescue_file, const char * datafile) const;

	int PreScriptReaper( const char* nodeName, int status );
	int PostScriptReaper( const char* nodeName, int status );

	void PrintReadyQ( debug_level_t level ) const;

	bool RemoveNode( const char *name, MyString &whynot );

	bool RemoveDependency( Job *parent, Job *child );
	bool RemoveDependency( Job *parent, Job *child, MyString &whynot );
	
    /* Detects cycle within dag submitted by user
	   @return true if there is cycle
	*/
	bool isCycle ();

	// max number of PRE & POST scripts to run at once (0 means no limit)
    const int _maxPreScripts;
    const int _maxPostScripts;

	void SetDotFileName(char *dot_file_name);
	void SetDotIncludeFileName(char *include_file_name);
	void SetDotFileUpdate(bool update_dot_file)       { _update_dot_file    = update_dot_file; }
	void SetDotFileOverwrite(bool overwrite_dot_file) { _overwrite_dot_file = overwrite_dot_file; }
	bool GetDotFileUpdate(void)                       { return _update_dot_file; }
	void DumpDotFile(void);

	void CheckAllJobs(const Dagman &dm);
	
  protected:

	/** Find all Condor (not DaP) log files associated with this DAG.
	*/
	void FindLogFiles();

    /* Prepares to submit job by running its PRE Script if one exists,
       otherwise adds job to _readyQ and calls SubmitReadyJobs()
	   @return true on success, false on failure
    */
    bool StartNode( Job *node );

    // add job to termination queue and report termination to all
    // child jobs by removing job ID from each child's waiting queue
    void TerminateJob( Job* job, bool bootstrap = false );
  
    /*  Get the first appearing job in the termination queue marked SUBMITTED.
        This function is called by ProcessLogEvents when a SUBMIT log
        entry is read.  The challenge is to correctly match the condorID
        found in the SUBMIT log event written by Condor with the Job object
        (currently with condorID (-1,-1,-1) that was recently submitted
        with condor_submit.<p>

        @return address of Job object, or NULL if not found
    */
    Job * GetSubmittedJob (bool recovery);

	void PrintEvent( debug_level_t level, const char* eventName, Job* job,
					 CondorID condorID );

	void RestartNode( Job *node, bool recovery );

	/* DFS number the jobs in the DAG in order to detect cycle*/
	void DFSVisit (Job * job);

		/** Check whether we got an exit value that should abort the DAG.
			@param The job associated with either the PRE script, POST
				script, or "main" job that just finished.
			@param The exit value of the relevant script or job.
			@param The "type" of script or job we're dealing with (PRE
				script, POST script, or node).
		*/
	static void CheckForDagAbort(Job *job, int exitVal, const char *type);
	
		// The DAG file name.
	char *		_dagFile;

		// The log file name specified by the -Condorlog command line
		// argument (not used for much anymore).
	char *		_condorLogName;

    // name of consolidated condor log
	StringList _condorLogFiles;

    // Documentation on ReadUserLog is present in condor_c++_util
	ReadMultipleUserLogs _condorLogRdr;

    //
    bool          _condorLogInitialized;

    //-->DAP
    const char* _dapLogName;
    ReadUserLog   _dapLog;
    bool          _dapLogInitialized;
    off_t         _dapLogSize;
    //<--DAP

    /// List of Job objects
    List<Job>     _jobs;

    // Number of Jobs that are done (completed execution)
    int _numJobsDone;
    
    // Number of Jobs that failed (or their PRE or POST script failed).
    int _numJobsFailed;

    // Number of Jobs currently running (submitted to Condor)
    int _numJobsSubmitted;

    /*  Maximum number of jobs to submit at once.  Non-negative.  Zero means
        unlimited
    */
    const int _maxJobsSubmitted;

		// Whether to allow the DAG to run even if we have an error
		// determining the job log files.
	bool		_allowLogError;

	// queue of jobs ready to be submitted to Condor
	SimpleList<Job*>* _readyQ;

	// queue of submitted jobs not yet matched with submit events in
	// the Condor job log
    Queue<Job*>* _submitQ;

	ScriptQ* _preScriptQ;
	ScriptQ* _postScriptQ;
	
	int DFS_ORDER; 

	// Information for producing dot files, which can be used to visualize
	// DAG files. Dot is part of the graphviz package, which is available from
	// http://www.research.att.com/sw/tools/graphviz/
	char *_dot_file_name;
	char *_dot_include_file_name;
	bool  _update_dot_file;
	bool  _overwrite_dot_file;
	int   _dot_file_name_suffix;

	void IncludeExtraDotCommands(FILE *dot_file);
	void DumpDotFileNodes(FILE *temp_dot_file);
	void DumpDotFileArcs(FILE *temp_dot_file);
	void ChooseDotFileName(MyString &dot_file_name);

	CheckEvents	_ce;

		// The next time we're allowed to try submitting a job -- 0 means
		// go ahead and submit right away.
	time_t		_nextSubmitTime;

		// The delay we use the next time a submit fails -- _nextSubmitTime
		// becomes the current time plus _nextSubmitDelay (this is in
		// seconds).
	int			_nextSubmitDelay;
};

#endif /* #ifndef DAG_H */
