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

	retval = listen(m_hListenSocket, SOMAXCONN);	// Note : 두번째 인자에 대해 좀 고민 할 필요가 있을 것 같다.
													//        교수님 코드에는 인자가 10으로 되어있음... SOMAXCONN을 쓰면 무슨 문제가,..?

	if (retval == SOCKET_ERROR) return false;

	// Note : Linger Option은 소켓을 Close할 때 이미 받고있거나 보내고 있는 데이터가 있을 경우 
	//        저걸 다 처리하고 Close 해주는 옵션
	//        하지만 너무 많은 데이터를 보내고 받고 있으면 Close가 늦어진다는 점 유의

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
		// Note : 실패이지만 대기상태라고 결과가 나왔을 경우 성공으로 인식
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
		cout << "Listen Socket 생성되지 않았음\n";
		return;
	}

	while (1)
	{
		int addr_len = sizeof(sockaddr);
		SOCKET client_sock = WSAAccept(m_hListenSocket, (SOCKADDR *)&sockaddr, &addr_len, 0, 0);
		if (client_sock == INVALID_SOCKET)
		{
			// 에러 처리
			closesocket(client_sock);
			err_display(L"Accept에서 클라이언트 소켓 생성 실패\n");
			continue;
		}

		// 플레이어 생성
		CClientSession *pNewSession = SESSION_MANAGER->GetNewClientFromPool(client_sock, sockaddr);	// Note : ID는 이미 SessionManager에서 
																									//        Start()할때 지정해줬다.
		if (pNewSession == nullptr)
		{
			closesocket(client_sock);
			cout << "클라이언트가 꽉 찼습니다.\n";
			continue;

		}

		if (pNewSession->AcceptCompletion())
		{
			// 데이터 초기화
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
		
			// 로그인 OK 송신
			pNewSession->OnceSend((char *)&login_pkt);

			pNewSession->SetConnect(true);				// ★
#ifdef _DEBUG
			printf("[접속] ID/INDEX : [%d, %d]\n", pNewSession->GetID(), pNewSession->GetIndex());
#endif
			////////////////////////////////////////////

#ifdef VIEW_PROCCESS
			WORLD_MANAGER->ViewProcess(pNewSession);
#else

#endif
			// TimerThread 잘 돌아가는지 실험
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
	flag = !flag;	// Note : 함수이름 의미에 맞게 하기 위해서 플래그값을 반대로 바꿔줌

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