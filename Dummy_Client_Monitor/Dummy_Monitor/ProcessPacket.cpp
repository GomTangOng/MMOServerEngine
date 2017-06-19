#include "stdafx.h"

void ProcessMovePacket(int user_id, char *packet)
{
	sc_packet_player_pos *movePacket = (sc_packet_player_pos *)packet;

	objectList[movePacket->player_id].x = movePacket->x;
	objectList[movePacket->player_id].y = movePacket->y;
	//if (player[user_id].obj_id == movePacket->obj_id) {  // 서버가 나를 강제이동 시켰다!!
	//	player[movePacket->obj_id].SetX(movePacket->x);
	//	player[movePacket->obj_id].SetY(movePacket->y);
	//}
//	printf(	"Move - id : %d, x : %d, y :%d\n", 
//			movePacket->obj_id, 
//			player[movePacket->obj_id].GetX(), 
//			player[movePacket->obj_id].GetY() );
}

void ProcessLoginOkPacket(int user_id, char *packet)
{
	sc_packet_login *LoginOk = reinterpret_cast<sc_packet_login *> (packet);

	player[user_id].m_bConnect = true;
	printf("%d user : Login Ok!\n", user_id);
	objectList[LoginOk->player_id].x = LoginOk->x;
	objectList[LoginOk->player_id].y = LoginOk->y;
	player[user_id].obj_id = LoginOk->player_id;
#ifdef HOT_SPOT_OFF
			struct CS_Teleport_Packet TelePortPacket;
			TelePortPacket.size = sizeof(CS_Teleport_Packet);
			TelePortPacket.type = CS_TELEPORT;

			unsigned int randNum;

			rand_s(&randNum);
			int rand_x = randNum % WORLD_WIDTH; 
			player[user_id].SetX(rand_x);
			rand_s(&randNum);
			int rand_y = randNum % WORLD_HEIGHT; 
			player[user_id].SetY(rand_y);


			TelePortPacket.x = player[user_id].GetX();
			TelePortPacket.y = player[user_id].GetY();
			player[user_id].SendPacket( reinterpret_cast<CHAR*>(&TelePortPacket), TelePortPacket.size );
#endif 
}

void ProcessPutObjectPacket(int user_id, char *packet)
{
	struct sc_packet_put_player *PutObject = reinterpret_cast<sc_packet_put_player *>(packet);

	objectList[PutObject->player_id].x = PutObject->x;
	objectList[PutObject->player_id].y = PutObject->y;
	// printf("PutObj - id : %d, x : %d, y : %d\n", 
	//		PutObject->obj_id, 
	//		player[PutObject->obj_id].GetX(), 
	//		player[PutObject->obj_id].GetY());		
}

void ProcessRemovePacket(int user_id, char *packet)
{

}

bool ProcessPacket(int user_id, char *packet)
{
	switch (packet[1]) {

	case SCPacketType::SC_LOGIN:
			ProcessLoginOkPacket(user_id, packet);
			break;

//		case SC_LOGIN_FAIL:
//			ProcessPlayerInfoPacket(user_id, packet);
//			break;

		// put_obj, move_obj, remove_obj로 관리 필요
	    // object 리스트를 따로 만들고(object id의 map), put_obj시 object추가, remove_obj시 object삭제
		// 맵의 object 배열정보는 put_obj를 가지고 한다.
	    // 맵의 object 배열정보에 dummy clinet에서 관리하는 플레이어 좌표정보도 추가해야한다. (나홀로 플레이시 put_object)에서 빠짐
		// object 리스트를 한바퀴 순회하면서 해당 맵 원소를 ++      ..nhjung
	//case SCPacketType::SC_PUT_PLAYER:
	//		ProcessPutObjectPacket(user_id, packet);
	//		break;

	case SCPacketType::SC_PLAYER_POS:
			ProcessMovePacket(user_id, packet);
			break;

	//case SCPacketType::SC_REMOVE_PLAYER:
	//		ProcessRemovePacket(user_id, packet);
	//		break;

		//case SC_CHAT:
		//	ProcessChatPacket(packet);
		//	break;
	default :
		break;
	}
	

	return TRUE;
}