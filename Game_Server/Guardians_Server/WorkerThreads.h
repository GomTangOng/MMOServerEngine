#pragma once
#include "Memory.h"
#include "ClientSession.h"

class CClientSession;
class CWorkerThreads : public CSRWLockGaurd<CWorkerThreads>
{
public:
	CWorkerThreads();
	~CWorkerThreads();

	// IO �Ϸ��� ����Ʈ �� , ID
	typedef function<void(const int, const UINT)>              OperationProcessFunc;
	typedef unordered_map<int, OperationProcessFunc>           OperationProcessFuncHashTable;	// IO TYPE�� ���� ó���Լ� �ؽ����̺� �ڷ���
	typedef unordered_map<int, function<void(OverlappedEx *)>> DelOverlappedExFunc;				// IO TYPE�� ���� Overlapped ���� ������ ó�� �Լ�

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

