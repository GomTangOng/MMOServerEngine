#include "stdafx.h"
#include "Monster.h"
#include "WorldManager.h"
#include "SessionManager.h"
#include "ObjectManager.h"
#include "NetEngine.h"

CMonster::CMonster() : CEntity(EntityType::MONSTER)
{
	m_index     = 0;
	m_bAlive    = false;
	m_bPet      = false;
	m_bExecute  = false;
	m_position  = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_speed     = 10;

	// ToDo : 나중에 Collision 추가
	m_pWorld   = WORLD_MANAGER->GetWorld(WORLD_TYPE::FOREST);
	m_sector_x = 0;
	m_sector_y = 0;
	m_pSector  = nullptr;

	m_viewBoundingSphere.Center.x = 0;
	m_viewBoundingSphere.Center.y = 0;
	m_viewBoundingSphere.Center.z = 0;
	m_viewBoundingSphere.Radius = VIEW_RADIUS;

	SetHP(100);
}


CMonster::~CMonster()
{
}

void CMonster::Initalize()
{
	// XML로 받아올 예정...
	//Clear();
}

void CMonster::Clear()
{
	GetSector()->DelMonster(GetID());
	CEntity::Clear();

	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	m_sector_x = m_position.x / SECTOR_WIDTH;
	m_sector_y = m_position.y / SECTOR_HEIGHT;
	m_pSector  = nullptr;	// ★ nullptr 로 해야 처음 만들어졌을때 섹터안에 리스트에 들어가진다.
	m_bAlive   = false;		// false로..? 나중에 리스폰함수 따로 만들자
	m_bExecute = false;

	SetHP(100);
}

void CMonster::Move()
{	
	sc_packet_monster_pos pos_pkt;
	sc_packet_put_monster put_pkt;
	UINT monster_id = GetID();

	put_pkt.size         = sizeof(sc_packet_put_monster);
	put_pkt.type         = SCPacketType::SC_PUT_MONSTER;
	put_pkt.monster_id   = monster_id;
	put_pkt.monster_type = GetMonsterType();
	put_pkt.hp           = GetHP();

	pos_pkt.size	   = sizeof(sc_packet_monster_pos);
	pos_pkt.type	   = SCPacketType::SC_MONSTER_POS;
	pos_pkt.monster_id = monster_id;

	
	XMFLOAT3 old_pos = m_position;
	XMFLOAT3 new_pos;

	int sector_x = WORLD_MANAGER->GetNewSectorXWithObject(this);
	int sector_y = WORLD_MANAGER->GetNewSectorYWithObject(this);
	
	UpdateSector(sector_x, sector_y);

	CPlayer::ViewList old_player_view_list;

	// 이동시키기전에 OldList를 만들어서 나중에 NewList랑 비교
	for (int y = sector_y - 1; y <= sector_y + 1; ++y)				
	{
		if (y < 0 || y >= SECTOR_Y_LENGTH) continue;
		for (int x = sector_x - 1; x <= sector_x + 1; ++x)
		{
			if (x < 0 || x >= SECTOR_X_LENGTH) continue;

			CSector* sector = GetWorld()->GetSector(x, y);
			
			sector->RegisterPlayerInRange(this, old_player_view_list);
		}
	}

	new_pos.x = old_pos.x + rand() % 20 - 10;
	new_pos.y = old_pos.y + rand() % 20 - 10;	// 나중에 Z로 수정
	new_pos.z = 0;

	if (new_pos.x < 0) new_pos.x = 0;
	if (new_pos.x >= WORLD_WIDTH) new_pos.x = WORLD_WIDTH - 1;
	if (new_pos.y < 0) new_pos.y = 0;
	if (new_pos.y >= WORLD_HEIGHT) new_pos.y = WORLD_HEIGHT - 1;

	put_pkt.x = pos_pkt.x = new_pos.x;
	put_pkt.x = pos_pkt.y = new_pos.y;
	put_pkt.x = pos_pkt.z = new_pos.z;

	SetPosition(new_pos);

	// NewViewList를 만들기 위해 갱신한 값을 이용해서 다시 Sector 업데이트
	sector_x = WORLD_MANAGER->GetNewSectorXWithObject(this);
	sector_y = WORLD_MANAGER->GetNewSectorYWithObject(this);

	UpdateSector(sector_x, sector_y);

	CSector::ObjectList new_player_view_list;

	for (int y = sector_y - 1; y <= sector_y + 1; ++y)
	{
		if (y < 0 || y >= SECTOR_Y_LENGTH) continue;
		for (int x = sector_x - 1; x <= sector_x + 1; ++x)
		{
			if (x < 0 || x >= SECTOR_X_LENGTH) continue;

			CSector* sector = GetWorld()->GetSector(x, y);

			sector->RegisterPlayerInRange(this, new_player_view_list);
		}
	}

	// 새로 갱신 된 플레이어 리스트에서 그 플레이어가 갖고 있는 몬스터 리스트에 이 몬스터가 있나 없나에 따른 처리
	for (auto player_id : new_player_view_list)
	{
		CClientSession* target_session = SESSION_MANAGER->FindSession(player_id);
		CPlayer*        player         = target_session->GetPlayer();
		
		//player->GetMonsterViewLock().ReadEnter();
		if (!player->GetOldViewMonsterList().count(monster_id))			//  플레이어가 몬스터 리스트에 해당 몬스터가 존재하지 않을때
		{
			//player->GetMonsterViewLock().ReadLeave();

			target_session->OnceSend((char *)&put_pkt);		 	// 자신에 시야에 타겟 플레이어가 보이게 한다.
			//this->SetAlive(true);
		}
		else
		{
			//player->GetMonsterViewLock().ReadLeave();
			target_session->OnceSend((char *)&pos_pkt);
			//this->SetAlive(true);
		}
	}

	sc_packet_remove_monster rem_pkt;
	rem_pkt.size       = sizeof(rem_pkt);
	rem_pkt.type       = SC_REMOVE_MONSTER;
	rem_pkt.monster_id = this->GetID();

	for (auto player_id : old_player_view_list)
	{
		if (new_player_view_list.count(player_id)) continue;

		CClientSession* target_session = SESSION_MANAGER->FindSession(player_id);
		CPlayer*        player         = target_session->GetPlayer();

		target_session->OnceSend((char *)&rem_pkt);

		player->DelMonsterInList(monster_id);
	}
}

bool CMonster::UpdateSector(int new_sector_x, int new_sector_y)
{
	if (m_pSector == nullptr)
	{
		m_pSector = m_pWorld->GetSector(new_sector_x, new_sector_y);		// Warning : 밑에 문장이랑 똑같은게 있다고 if문 밖으로 빼내면 안됨!
		m_sector_x = new_sector_x;
		m_sector_y = new_sector_y;

		m_pSector->AddMonster(GetID());
		return false;
	}
	if (m_sector_x == new_sector_x && m_sector_y == new_sector_y) return true;

	m_pSector->DelMonster(GetID());
	m_pSector = m_pWorld->GetSector(new_sector_x, new_sector_y);

	m_sector_x = new_sector_x;
	m_sector_y = new_sector_y;

	m_pSector->AddMonster(GetID());

	return false;
}

void CMonster::SetAlive(const bool alive)
{
	//if (alive == m_bAlive) return;

	m_bAlive = alive;

	//if (m_bAlive == true)
	//{
	//	CEventMessage* evt = new CEventMessage(GetID(), GetID(), GetTickCount(), EventType::MOVE, GetEntityType(), GetEntityType());
	//	NETWORK_ENGINE->GetTimerThread()->AddEvent(evt);
	//}
}
