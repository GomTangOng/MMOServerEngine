#pragma once
#include "Memory.h"
#include "ClientSession.h"

class CClientSession;
class CWorkerThreads : public CSRWLockGaurd<CWorkerThreads>
{
public:
	CWorkerThreads();
	~CWorkerThreads();

	// IO 완료한 바이트 수 , ID
	typedef function<void(const int, const UINT)>              OperationProcessFunc;
	typedef unordered_map<int, OperationProcessFunc>           OperationProcessFuncHashTable;	// IO TYPE에 따른 처리함수 해쉬테이블 자료형
	typedef unordered_map<int, function<void(OverlappedEx *)>> DelOverlappedExFunc;				// IO TYPE에 따른 Overlapped 변수 릴리즈 처리 함수

	void WaitThreads();
	bool Create(const int nWorkerThread);
	void Release();
	void RegisterProcessFunction();

	UINT Run();

	static void EntryPoint(void* arg);

	void ZeroRecvProcess(const DWORD transferred, const UINT id);
	void RecvProcess(const DWORD transferred, const UINT id);
	void OnceSendProcess(const DWORD transferred, const UINT id);
	void SendProcess(const DWORD transferred, const UINT id);
	void MoveProcess(const DWORD transferred, const UINT id);

	void DeleteZeroRecvOverlappedEx(OverlappedEx *overlappedEx);
	void DeleteRecvOverlappedEx(OverlappedEx *overlappedEx);
	void DeleteOnceSendOverlappedEx(OverlappedEx *overlappedEx);
	void DeleteSendOverlappedEx(OverlappedEx *overlappedEx);
	void DeleteMoveOverlappedEx(OverlappedEx *overlappedEx);
private :
	int              m_nWorkerThread;
	vector<thread *> m_vWorkerThreads;

	OperationProcessFuncHashTable m_operationFuncHashTable;
	DelOverlappedExFunc m_delOverlappedExFunc;

public :
	// Getter, Setter 

	int               GetWorkerThreadCount() const { return m_nWorkerThread; }
	vector<thread *>& GetWorkerThread()		       { return m_vWorkerThreads; }	
};

