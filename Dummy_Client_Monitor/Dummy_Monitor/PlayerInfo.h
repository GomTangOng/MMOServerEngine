#pragma once

class playerInfo
{
public:
	int			x;
	int			y;
	int			obj_id;
	WSADATA			m_wsaData;
	BOOL			m_bConnect;
	BOOL			m_bLogin;
	SOCKET			m_socket;
	HANDLE			m_threads[MAX_WORKER_THREAD+1];
	OVERLAPPED_EX	m_overExRecv;
	WSABUF			m_recv_wsa_buf;
	char			real_recv_buf[MAX_BUFF_SIZE];
	char			m_uncomplete_packet[MAX_BUFF_SIZE];
	int				m_current_packet_size;
	int				m_uncomplete_size;
	CRITICAL_SECTION	cs;

	playerInfo();
	~playerInfo() { }

	SOCKET	GetSocket() const { return m_socket; }
	BOOL	GetConnect() const { return m_bConnect; }
	BOOL	Connect(int i);
	BOOL	DisConnect();
	BOOL	SendPacket(CHAR *packet, WORD size);
	void	TelePort(int i);
	BOOL ProcessPacket(CHAR *packet);
	BOOL ProcessLoginPacket(CHAR *packet);
	BOOL ProcessPlayerInfoPacket(CHAR *packet);
	BOOL ProcessPutObjectPacket(CHAR *packet);
	BOOL ProcessRemovePacket(CHAR *packet);
	BOOL ProcessMovePacket(CHAR *packet);

	void SetX(int input_x) { x = input_x; }
	void SetY(int input_y) { y = input_y; }
	int GetX() { return (int)x; }
	int GetY() { return (int)y; }

	static DWORD WINAPI WorkerThread(LPVOID param);
};



