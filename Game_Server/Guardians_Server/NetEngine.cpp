#include "stdafx.h"
#include "NetEngine.h"
#include "protocol.h"
#include "PacketManager.h"
#include "WorldManager.h"
#include "ObjectManager.h"

extern "C" void PrintPropertyDevice();

CNetEngine::CNetEngine()
{
}


CNetEngine::~CNetEngine()
{
	
}

bool CNetEngine::Start()
{
	WSADATA wsa;
	_wsetlocale(LC_ALL, L"korean");

	m_pWorkerThreads = make_unique<CWorkerThreads>();
	m_pTimerThread = make_unique<CTimerThread>();
	m_pHeartBeatThread = make_unique<CHeartBeatThread>();

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		err_display(L"[DEBUG] WSAStartup() Error\n");
		return false;
	}

	if (!CreateIOCP())
	{
		err_display(L"[DEBUG] CreateIOCP() Error\n");
		return false;
	}

	if (!m_pWorkerThreads->Create(0))
	{
		printf("[DEBUG] CreateWorkerThread() Error\n");
		return false;
	}

	if (!m_pTimerThread->Create())
	{
		printf("[DEBUG] TimerThread() Error\n");
		return false;
	}

	if (!m_pHeartBeatThread->Create())
	{
		printf("[DEBUG] HeartBeatThread() Error\n");
		return false;
	}

	if (!CreateListenSocket())
	{
		err_display(L"[DEBUG] CreateListenSocket() Error\n");
		return false;
	}

	if (!Bind())
	{
		err_display(L"[DEBUG] Bind() Error\n");
		return false;
	}

	if (!Listen())
	{
		err_display(L"[DEBUG] Listen() Error\n");
		return false;
	}


	if (!WORLD_MANAGER->Start())
	{
		printf("[DEBUG] WORLD_MANAGER Class :: Start() Error\n");
		return false;
	}

	if (!PACKET_MANAGER->Start())
	{
		printf("[DEBUG] Packet_Manager Class :: Start() Error\n");
		return false;
	}

	if (!SESSION_MANAGER->Start())
	{
		printf("[DEBUG] Session_Manager Class :: Start() Error\n");
		return false;
	}

	if (!OBJECT_MANAGER->Start())
	{
		printf("[DEBUG] Monster_Manager Class :: Start() Error\n");
		return false;
	}
	
	PrintPropertyDevice();
	//PrintPropertyDevice();

	return true;
}

void CNetEngine::Run()
{
	CNetEngine::Accept();

	m_pTimerThread->WaitThreads();
	m_pWorkerThreads->WaitThreads();
	m_pHeartBeatThread->WaitThreads();
}

void CNetEngine::Shutdown()
{
	// Managers
	PACKET_MANAGER->ShutDown();
	WORLD_MANAGER->ShutDown();
	SESSION_MANAGER->ShutDown();
	OBJECT_MANAGER->ShutDown();
	
	// Threads
	m_pTimerThread->Release();
	m_pWorkerThreads->Release();
	m_pHeartBeatThread->Release();

	// Socket and WinSocket
	closesocket(m_hListenSocket);
	CloseHandle(m_hIocp);
	WSACleanup();
}

bool CNetEngine::CreateListenSocket()
{
	m_hListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_hListenSocket == NULL) return false;

	return true;
}

bool CNetEngine::Bind()
{
	int retval;

	SOCKADDR_IN sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(SERVER_PORT);

	retval = ::bind(m_hListenSocket, (SOCKADDR *)&sockaddr, sizeof(sockaddr));

	if (retval == SOCKET_ERROR) return false;

	return true;
}

bool CNetEngine::Listen()
{
	int retval;

	retval = listen(m_hListenSocket, SOMAXCONN);	// Note : �ι�° ���ڿ� ���� �� ��� �� �ʿ䰡 ���� �� ����.
													//        ������ �ڵ忡�� ���ڰ� 10���� �Ǿ�����... SOMAXCONN�� ���� ���� ������,..?

	if (retval == SOCKET_ERROR) return false;

	// Note : Linger Option�� ������ Close�� �� �̹� �ް��ְų� ������ �ִ� �����Ͱ� ���� ��� 
	//        ���� �� ó���ϰ� Close ���ִ� �ɼ�
	//        ������ �ʹ� ���� �����͸� ������ �ް� ������ Close�� �ʾ����ٴ� �� ����

	//LINGER linger;
	//linger.l_onoff  = 1;
	//linger.l_linger = 0;

	//retval = setsockopt(m_hListenSocket, IPPROTO_TCP, SO_LINGER, (char *)&linger, sizeof(linger));
	//if (retval == SOCKET_ERROR) return false;

	return true;
}

bool CNetEngine::Connect(char *ip, USHORT port)
{
	if (ip == NULL || port == NULL) return false;
	if (m_socket == NULL) return false;

	int retval;
	SOCKADDR_IN sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);

	retval = WSAConnect(m_socket, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL);

	if (retval == SOCKET_ERROR)
	{
		// Note : ���������� �����¶�� ����� ������ ��� �������� �ν�
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return false;
		}
	}
	return true;
}

void CNetEngine::Accept()
{
	SOCKADDR_IN sockaddr;
	// Monster Test
	// pre : 40, 40, 20, 15
	// pre2 : 80, 80, 10, 7

	//for (int i = 1; i < 100; ++i)
	//{
	//	for (int j = 1; j < 100; ++j)
	//	{
	//		CMonster* pMon = OBJECT_MANAGER->GetNewMonster(rand() % MonsterType::MONSTER_TYPE_END);
	//		pMon->SetPosition(j * 8, i * 6, 0);
	//		pMon->SetWorld(WORLD_MANAGER->GetWorld(WORLD_TYPE::FOREST));
	//		pMon->UpdateSector(pMon->GetPositionX() / SECTOR_WIDTH, pMon->GetPositionY() / SECTOR_HEIGHT);
	//		pMon->SetAlive(false);
	//	}
	//}

	if (m_hListenSocket == NULL)
	{
		cout << "Listen Socket �������� �ʾ���\n";
		return;
	}

	while (1)
	{
		int addr_len = sizeof(sockaddr);
		SOCKET client_sock = WSAAccept(m_hListenSocket, (SOCKADDR *)&sockaddr, &addr_len, 0, 0);
		if (client_sock == INVALID_SOCKET)
		{
			// ���� ó��
			closesocket(client_sock);
			err_display(L"Accept���� Ŭ���̾�Ʈ ���� ���� ����\n");
			continue;
		}

		// �÷��̾� ����
		CClientSession *pNewSession = SESSION_MANAGER->GetNewClientFromPool(client_sock, sockaddr);	// Note : ID�� �̹� SessionManager���� 
																									//        Start()�Ҷ� ���������.
		if (pNewSession == nullptr)
		{
			closesocket(client_sock);
			cout << "Ŭ���̾�Ʈ�� �� á���ϴ�.\n";
			continue;

		}

		if (pNewSession->AcceptCompletion())
		{
			// ������ �ʱ�ȭ
			CPlayer *player = pNewSession->GetPlayer();
			player->SetPosition(XMFLOAT3(rand() % SECTOR_WIDTH, rand() % SECTOR_HEIGHT, 0));
			player->SetWorld(WORLD_MANAGER->GetWorld(WORLD_TYPE::FOREST));
			player->UpdateSector(player->GetPositionX() / SECTOR_WIDTH, player->GetPositionY() / SECTOR_HEIGHT);

			// LOGIN OK PACKET
			///////////////////////////////////////////
			sc_packet_login login_pkt;
			login_pkt.size = sizeof(sc_packet_login);
			login_pkt.type = SC_LOGIN;
			login_pkt.player_id = pNewSession->GetID();
			login_pkt.x = player->GetPositionX();
			login_pkt.y = player->GetPositionY();
			login_pkt.z = 0;
			login_pkt.hp = 100;
		
			// �α��� OK �۽�
			pNewSession->OnceSend((char *)&login_pkt);

			pNewSession->SetConnect(true);				// ��
#ifdef _DEBUG
			printf("[����] ID/INDEX : [%d, %d]\n", pNewSession->GetID(), pNewSession->GetIndex());
#endif
			////////////////////////////////////////////

#ifdef VIEW_PROCCESS
			WORLD_MANAGER->ViewProcess(pNewSession);
#else

#endif
			// TimerThread �� ���ư����� ����
			//CEventMessage *pNewEvent = new CEventMessage(pNewSession->GetID(), 0, 5000 + GetTickCount(), EventType::INCREASE_HP, EntityType::PLAYER, EntityType::PLAYER);
			//m_pTimerThread->AddEvent(pNewEvent);			
			///////////////////////////////////////////////////////////////////////////		
		}
		else
		{
			SESSION_MANAGER->DeleteSession(pNewSession);
		}
	}
}

bool CNetEngine::CreateIOCP(int nThread)
{
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nThread);

	if (m_hIocp == NULL) return false;

	return true;
}

bool CNetEngine::AssociateSocketWithIOCP(CClientSession *m_pClientSession)
{
	if (CreateIoCompletionPort((HANDLE)m_pClientSession->GetSocket(), m_hIocp, m_pClientSession->GetID(), NULL) == NULL)
	{
		return false;
	}
	return true;
}

void CNetEngine::SetNagle(SOCKET sock, bool flag)
{
	flag = !flag;	// Note : �Լ��̸� �ǹ̿� �°� �ϱ� ���ؼ� �÷��װ��� �ݴ�� �ٲ���

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag));
}

void err_quit(TCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(TCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}