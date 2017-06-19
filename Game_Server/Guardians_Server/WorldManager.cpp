#include "stdafx.h"
#include "WorldManager.h"
#include "Memory.h"
#include "SessionManager.h"
#include "SRWLock.h"
#include "Monster.h"
#include "ObjectManager.h"

CWorldManager::CWorldManager()
{
}


CWorldManager::~CWorldManager()
{
}

bool CWorldManager::Start()
{
	for (int i = 0; i < WORLD_TYPE::WORLD_END; ++i)
	{
		m_worlds[i] = new CWorld();
		m_worlds[i]->SetWorldType(i);
	}
	return true;
}

void CWorldManager::ShutDown()
{
	for (auto world : m_worlds)
	{
		Memory::SAFE_DELETE(world.second);
	}
	m_worlds.clear();
}

bool CWorldManager::UpdateSector(CClientSession * pOwnerSession)
{
	CPlayer *owner_player = pOwnerSession->GetPlayer();

	int sector_x = WORLD_MANAGER->GetNewSectorXWithObject(owner_player);
	int sector_y = WORLD_MANAGER->GetNewSectorYWithObject(owner_player);

	return owner_player->UpdateSector(sector_x, sector_y);
}

void CWorldManager::ViewProcess(CClientSession *pOwnerSession)
{
	assert(pOwnerSession != nullptr);

	UpdateSector(pOwnerSession);
	PlayerViewProcess(pOwnerSession);		// �÷��̾� -> ���� -> ������ �Լ�ȣ�� ���� �ٲٸ�ȵ�!
	MonsterViewProcess(pOwnerSession);
	//ItemViewProcess(pOwnerSession);
}

void CWorldManager::PlayerViewProcess(CClientSession * pOwnerSession)
{
	sc_packet_player_pos pos_pkt;
	sc_packet_put_player put_pkt;

	put_pkt.size = sizeof(sc_packet_put_player);
	put_pkt.type = SCPacketType::SC_PUT_PLAYER;
	pos_pkt.size = sizeof(pos_pkt);
	pos_pkt.type = SCPacketType::SC_PLAYER_POS;

	CPlayer *owner_player = pOwnerSession->GetPlayer();
	int      owner_id = owner_player->GetID();

	CSRWLock& owner_view_lock = owner_player->GetPlayerViewLock();

	int sector_x = owner_player->GetSectorX();
	int sector_y = owner_player->GetSectorY();

	CPlayer::ViewList new_player_list;

	for (int y = sector_y - 1; y <= sector_y + 1; ++y)				// �� ���߿� 9���� ���͸� ���Ϳ� ��� ĳ����Ʈ�� �÷����� -> �׷��� �̰��� ������ �ö����� �� �����ؾ� �� ����
	{
		if (y < 0 || y >= SECTOR_Y_LENGTH) continue;
		for (int x = sector_x - 1; x <= sector_x + 1; ++x)
		{
			if (x < 0 || x >= SECTOR_X_LENGTH) continue;

			CSector* sector = owner_player->GetWorld()->GetSector(x, y);
			sector->RegisterPlayerInRange(owner_player, new_player_list);
		}
	}

	//owner_view_lock.ReadEnter();
	//CPlayer::ViewList old_player_list = owner_player->GetOldViewPlayerList();
	//owner_view_lock.ReadLeave();

	for (auto target_id : new_player_list)
	{
		CClientSession *target_session = SESSION_MANAGER->FindSession(target_id);
		CPlayer        *target_obj = target_session->GetPlayer();
		CSRWLock       &target_view_list_lock = target_obj->GetPlayerViewLock();

		//owner_view_lock.ReadEnter();
		if (!owner_player->GetOldViewPlayerList().count(target_id))					// �÷��̾ �� ����Ʈ���� �����ϴµ� ���� ����Ʈ���� �������� ������
		{
			//owner_view_lock.ReadLeave();

			owner_player->AddPlayerInList(target_id);

			put_pkt.player_id = target_obj->GetID();
			put_pkt.x = target_obj->GetPositionX();
			put_pkt.y = target_obj->GetPositionY();
			put_pkt.z = target_obj->GetPositionZ();
			put_pkt.hp = target_obj->GetHP();

			pOwnerSession->OnceSend((char *)&put_pkt);		 	// �ڽſ� �þ߿� Ÿ�� �÷��̾ ���̰� �Ѵ�.

			target_view_list_lock.ReadEnter();
			if (!target_obj->GetOldViewPlayerList().count(owner_id))		// Ÿ�� �÷��̾��� �þ� ó��
			{
				target_view_list_lock.ReadLeave();

				target_obj->AddPlayerInList(owner_id);

				put_pkt.player_id = owner_player->GetID();
				put_pkt.x = owner_player->GetPositionX();
				put_pkt.y = owner_player->GetPositionY();
				put_pkt.z = owner_player->GetPositionZ();
				put_pkt.hp = owner_player->GetHP();

				target_session->OnceSend((char *)&put_pkt);
			}
			else
			{
				target_view_list_lock.ReadLeave();

				pos_pkt.player_id = owner_player->GetID();
				pos_pkt.x = owner_player->GetPositionX();
				pos_pkt.y = owner_player->GetPositionY();
				pos_pkt.z = owner_player->GetPositionZ();

				target_session->OnceSend((char *)&pos_pkt);		// �̹� �ҽ� �÷��̾ ���̹Ƿ� ��ġ�� ����
			}
		}
		else
		{
			//owner_view_lock.ReadLeave();
			// �÷��̾ �� ����Ʈ�� �����ϰ� ���� ����Ʈ������ �����Ҷ�
			pos_pkt.player_id = target_obj->GetID();
			pos_pkt.x = target_obj->GetPositionX();
			pos_pkt.y = target_obj->GetPositionY();
			pos_pkt.z = target_obj->GetPositionZ();
			pOwnerSession->OnceSend((char *)&pos_pkt);

			target_view_list_lock.ReadEnter();
			if (!target_obj->GetOldViewPlayerList().count(owner_id))		// Ÿ�� �÷��̾��� �þ� ó��
			{
				target_view_list_lock.ReadLeave();

				target_obj->AddPlayerInList(owner_id);

				put_pkt.player_id = owner_player->GetID();
				put_pkt.x = owner_player->GetPositionX();
				put_pkt.y = owner_player->GetPositionY();
				put_pkt.z = owner_player->GetPositionZ();
				put_pkt.hp = owner_player->GetHP();

				target_session->OnceSend((char *)&put_pkt);		// Ÿ�� �÷��̾� �þ߿� �ҽ� �÷��̾ ���̰� �Ѵ�.
			}
			else
			{
				target_view_list_lock.ReadLeave();

				pos_pkt.player_id = owner_player->GetID();
				pos_pkt.x = owner_player->GetPositionX();
				pos_pkt.y = owner_player->GetPositionY();
				pos_pkt.z = owner_player->GetPositionZ();

				target_session->OnceSend((char *)&pos_pkt);
			}
		}
	}

	vector<UINT> remove_list;
	remove_list.reserve(64);

	sc_packet_remove_player rem_pkt;
	rem_pkt.size = sizeof(rem_pkt);
	rem_pkt.type = SC_REMOVE_PLAYER;

	// ���ο� ����Ʈ���� ���µ� ���� ����Ʈ�� ���� ��� -> ������Ʈ ����
	owner_view_lock.ReadEnter();
	for (auto i : owner_player->GetOldViewPlayerList())
	{
		if (new_player_list.count(i)) continue;
		remove_list.push_back(i);
	}
	owner_view_lock.ReadLeave();

	for (auto i : remove_list)
	{
		owner_player->DelPlayerInList(i);

		rem_pkt.player_id = i;
		pOwnerSession->OnceSend((char *)&rem_pkt);
	}

	for (auto i : remove_list)
	{
		CClientSession *target_session = SESSION_MANAGER->FindSession(i);
		CPlayer        *target_obj = target_session->GetPlayer();
		CSRWLock&       target_view_lock = target_obj->GetPlayerViewLock();

		target_view_lock.ReadEnter();
		if (target_obj->GetOldViewPlayerList().count(owner_id))
		{
			target_view_lock.ReadLeave();
			
			target_obj->DelPlayerInList(owner_id);

			rem_pkt.player_id = owner_id;
			target_session->OnceSend((char *)&rem_pkt);
		}
		else
		{
			target_view_lock.ReadLeave();
		}
	}
}

void CWorldManager::MonsterViewProcess(CClientSession * pOwnerSession)
{
	sc_packet_monster_pos pos_pkt;
	sc_packet_put_monster put_pkt;

	put_pkt.size = sizeof(sc_packet_put_monster);
	put_pkt.type = SCPacketType::SC_PUT_MONSTER;
	pos_pkt.size = sizeof(sc_packet_monster_pos);
	pos_pkt.type = SCPacketType::SC_MONSTER_POS;

	CPlayer *owner_player = pOwnerSession->GetPlayer();
	CSRWLock& mon_list_lock = owner_player->GetMonsterViewLock();

	mon_list_lock.ReadEnter();
	CSector::ObjectList old_monster_list = owner_player->GetOldViewMonsterList();
	mon_list_lock.ReadLeave();

	int sector_x = owner_player->GetSectorX();
	int sector_y = owner_player->GetSectorY();

	CSector::ObjectList new_monster_list;

	for (int y = sector_y - 1; y <= sector_y + 1; ++y)				
	{
		if (y < 0 || y >= SECTOR_Y_LENGTH) continue;
		for (int x = sector_x - 1; x <= sector_x + 1; ++x)
		{
			if (x < 0 || x >= SECTOR_X_LENGTH) continue;

			CSector* sector = owner_player->GetWorld()->GetSector(x, y);
			sector->RegisterMonsterInRange(owner_player, new_monster_list);
		}
	}

	for (auto mon_id : new_monster_list)
	{
		CMonster* target_monster = OBJECT_MANAGER->FindMonster(mon_id);
		
		if (!old_monster_list.count(mon_id))
		{	
			owner_player->AddMonsterInList(mon_id);

			put_pkt.monster_id   = mon_id;
			put_pkt.monster_type = target_monster->GetMonsterType();
			put_pkt.x            = target_monster->GetPositionX();
			put_pkt.y            = target_monster->GetPositionY();
			put_pkt.z            = target_monster->GetPositionZ();
			put_pkt.hp           = target_monster->GetHP();

			pOwnerSession->OnceSend((char *)&put_pkt);		
			//target_monster->SetAlive(true);
		}
		else
		{
			//target_monster->SetAlive(true);
			// �÷��̾ �� ����Ʈ�� �����ϰ� ���� ����Ʈ������ �����Ҷ�
			pos_pkt.monster_id = mon_id;
			pos_pkt.x		   = target_monster->GetPositionX();
			pos_pkt.y		   = target_monster->GetPositionY();
			pos_pkt.z		   = target_monster->GetPositionZ();
			pOwnerSession->OnceSend((char *)&pos_pkt);

			//target_monster->SetAlive(true);
		}
	}

//	vector<int> remove_list;
//	remove_list.reserve(64);
//
//#ifdef _DEBUG
//	if (remove_list.size() >= 64) { cout << "�����ؾ��� ����� 64�� �Ѿ\n"; }
//#endif
//
	sc_packet_remove_monster rem_pkt;
	rem_pkt.size = sizeof(rem_pkt);
	rem_pkt.type = SC_REMOVE_MONSTER;
//
//	mon_list_lock.ReadEnter();
//	for (auto i : owner_player->GetOldViewMonsterList())
//	{
//		if (new_monster_list.count(i)) continue;
//		remove_list.push_back(i);
//	}
//	mon_list_lock.ReadLeave();
//
//	for (auto i : remove_list)
//	{
//		CMonster *target_monster = OBJECT_MANAGER->FindMonster(i);
//
//		rem_pkt.monster_id = i;
//		pOwnerSession->OnceSend((char *)&rem_pkt);
//
//		owner_player->DelMonsterInList(i);
//
//		target_monster->SetAlive(false);
//	}
//
	for (auto mon_id : old_monster_list)
	{
		if (new_monster_list.count(mon_id)) continue;
		CMonster *target_monster = OBJECT_MANAGER->FindMonster(mon_id);

		rem_pkt.monster_id = mon_id;
		pOwnerSession->OnceSend((char *)&rem_pkt);
		
		owner_player->DelMonsterInList(mon_id);
		//target_monster->SetAlive(false);
	}
}

void CWorldManager::ItemViewProcess(CClientSession * pOwnerSession)
{
}
