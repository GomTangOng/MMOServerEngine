#include "stdafx.h"

DWORD WINAPI WorkerThread(LPVOID para)
{
	DWORD	iosize;
	DWORD	flag = 0;
	ULONG	comp_key;
	OVERLAPPED_EX *over_ptr	= NULL;
	char *buf = NULL;
	int remained_io_data_size = 0;
	int	result;

	while (true) {
		GetQueuedCompletionStatus(g_iocp, &iosize, &comp_key, (LPOVERLAPPED *)&over_ptr, INFINITE);
		if (0 == iosize) {
			closesocket(player[comp_key].m_socket);
		//	Process_disconnect(comp_key);
			break;
		}

		switch (over_ptr->m_io_type) {		
			case IO_RECV:				
				remained_io_data_size = iosize;
				buf = player[comp_key].real_recv_buf;

				while (remained_io_data_size > 0) {

					if (0 == player[comp_key].m_current_packet_size) player[comp_key].m_current_packet_size = buf[0];
							
					int remained_packet_data_size = player[comp_key].m_current_packet_size - player[comp_key].m_uncomplete_size;

					if (remained_packet_data_size <= remained_io_data_size) {

						memcpy(player[comp_key].m_uncomplete_packet + player[comp_key].m_uncomplete_size,
							buf,
							remained_packet_data_size);

						//ProceessPacket
						ProcessPacket(comp_key, player[comp_key].m_uncomplete_packet);

						player[comp_key].m_uncomplete_size = 0;
						player[comp_key].m_current_packet_size = 0;
						buf += remained_packet_data_size;
						remained_io_data_size -= remained_packet_data_size;
					} else { 
						memcpy(player[comp_key].m_uncomplete_packet + player[comp_key].m_uncomplete_size,
							buf,
							remained_io_data_size);
						player[comp_key].m_uncomplete_size += remained_io_data_size;						
						break;
					}
				}
				result = WSARecv( player[comp_key].GetSocket(), &player[comp_key].m_recv_wsa_buf, 1,
							NULL, &flag, reinterpret_cast<LPWSAOVERLAPPED>(&player[comp_key].m_overExRecv), NULL);

				if (result == SOCKET_ERROR) {
					if (WSAGetLastError() != WSA_IO_PENDING) {
						printf("WSARecv() error!!\n");
						return FALSE;
					}
				}
				break;

			case IO_SEND:
				delete over_ptr;
				//printf("Send Complete on User:%d\n",comp_key);
				break;
			default:
				printf("IO type error!!\n");
				break;
				
		}

	} // while()

	return 0;
}
