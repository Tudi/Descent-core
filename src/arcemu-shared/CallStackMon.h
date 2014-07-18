#ifndef _CALLSTACK_MON_H_
#define _CALLSTACK_MON_H_

#include "Common.h"
#include "Singleton.h"

/*
Light Weight CallStack Monitor
It only monitors main function calls to detect deadlocks and if possible callstack corruptions
it is an independent callstack monitor which can dump all thread stacktraces
only some functions are monitored that could lead to issues
in future it could dump stack to file before the problem ocures
*/

#ifndef _DEBUG
	#define INSTRUMENTATION_CALLSTACK_MONITOR_ENTER(CM_FileName,CM_FunctionName,CM_FileLine,CM_GUID,CM_FUID,CM_FUNUID,CM_TUID,CM_TthreadID) ;
	#define INSTRUMENTATION_CALLSTACK_MONITOR_EXIT(CM_FileName,CM_FunctionName,CM_FileLine,CM_GUID,CM_FUID,CM_FUNUID,CM_TUID,CM_TthreadID) ;
#else
	#define INSTRUMENTATION_CALLSTACK_MONITOR_ENTER(CM_FileName,CM_FunctionName,CM_FileLine,CM_GUID,CM_FUID,CM_FUNUID,CM_TUID,CM_TthreadID) ;
	#define INSTRUMENTATION_CALLSTACK_MONITOR_EXIT(CM_FileName,CM_FunctionName,CM_FileLine,CM_GUID,CM_FUID,CM_FUNUID,CM_TUID,CM_TthreadID) ;
#endif

struct FuncCallEntry
{
	uint32 global_ID;		//unique in project
};

class SERVER_DECL CallMon : public Singleton< CallMon > 
{
public:
	CallMon(){};
	~CallMon(){}; 
	void DumpAllThreadStacks();		//iterate through each thread and dump their stacktrace.
private:
	FILE *m_file;
	std::map<unsigned int,std::vector<FuncCallEntry>> m_thread_stacks;
};

#define sCallMon CallMon::getSingleton()

#endif