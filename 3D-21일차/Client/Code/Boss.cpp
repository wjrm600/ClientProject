#include "stdafx.h"
#include "Boss.h"
#include "Export_Function.h"
#include "ColliderMesh.h"
#include "RuinBox.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
{
}

CBoss::~CBoss(void)
{

}

Client::_vec3 Client::CBoss::PickUp_OnTerrain(void)
{
	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	Engine::CTransform*		pTerrainTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransformCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransformCom);
}

HRESULT Client::CBoss::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Boss"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_pTransformCom->Set_Pos(&_vec3(45.f, -20.7f, 38.f));
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>(Engine::Clone(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_NumVtx(), m_pMeshCom->Get_Stride());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);


	Set_ColliderFrame("Beetle_Head");
	Set_ColliderFrame("Beetle_LowerClaw_Left_Thumb");
	Set_ColliderFrame("Beetle_LowerClaw_Right");
	Set_ColliderFrame("Beetle_LowerClaw_Right_Gun");
	Set_ColliderFrame("Beetle_LowerClaw_Right_Gun2");
	return S_OK;
}

HRESULT CBoss::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);	

	return S_OK;
}

void Client::CBoss::Key_Input(const _float& fTimeDelta)
{
	if (m_bIntro)
	{
		if (!m_bIntroEvent)
		{
			if ((dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC))->m_vInfo[Engine::INFO_POS].z) > 20.f)
			{
				m_bIntroEvent = true;
			}
		}
		else
		{
			if (m_fIntroTime > 2.f)
			{
				m_pTransformCom->Set_Pos(&_vec3(45.f, -4.7f, 38.f));
				m_pMeshCom->Set_AnimationSet(30);
				m_eBossMoveState = Engine::BMS_INTRO;
				m_bIntro = false;
			}
			m_fIntroTime += fTimeDelta;
		}
	}

	if (m_bDamaged)
	{
		if (!m_bOneAttack)
		{
			switch (m_eBossPattern)
			{
			case Engine::BP_ONE:
				m_pMeshCom->Set_AnimationSet(4);
				m_eBossPattern = Engine::BP_TWO;
				m_bDamaged = false;
				m_bOneAttack = true;
				break;
			case Engine::BP_TWO:
				m_pMeshCom->Set_AnimationSet(3);
				m_eBossPattern = Engine::BP_THREE;
				m_bDamaged = false;
				m_bOneAttack = true;
				break;
			case Engine::BP_THREE:
				m_eBossMoveState = Engine::BMS_DEAD;
				m_bDamaged = false;
				m_bOneAttack = true;
				break;
			}
		}
	}
	else
	{
		m_bOneAttack = false;
	}

		if (m_eBossMoveState == Engine::BMS_INTRO)
		{
			if ((m_fIntroTime > 9.8f) && (m_fIntroTime < 13.f))
			{
				m_pTransformCom->Move_Pos(&(_vec3(0.f, 1.6f, 1.5f)*fTimeDelta));
				m_fAddHeight += 2.f * fTimeDelta;
			}
			m_fIntroTime += fTimeDelta;
		}
		else if (m_eBossMoveState == Engine::BMS_ATTACK)
		{
			if (m_bAttackOn)
			{
				Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
				_vec3 AtPlayerDirection = pPlayerTransform->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
				_vec3 AtPlayerLookDirection = _vec3(0.f, 0.f, -1.f);
				D3DXVec3Normalize(&AtPlayerDirection, &AtPlayerDirection);
				_float AtPlayerAngle = D3DXVec3Dot(&AtPlayerDirection, &AtPlayerLookDirection);
				D3DXVec3Cross(&AtPlayerLookDirection, &AtPlayerDirection, &AtPlayerLookDirection);
				AtPlayerAngle = acosf(AtPlayerAngle);
				AtPlayerAngle = AtPlayerAngle;
				if (AtPlayerLookDirection.y > 0)
				{
					m_pTransformCom->m_vAngle.y = -AtPlayerAngle;
				}
				else
				{
					m_pTransformCom->m_vAngle.y = AtPlayerAngle;
				}
				m_pMeshCom->Set_AnimationSet(34);
				m_bAttackOn = false;
			}
		}
		else if (m_eBossMoveState == Engine::BMS_FIRE)
		{
			if (m_bAttackOn)
			{
				Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
				_vec3 AtPlayerDirection = _vec3(40.f,0.f,40.f) - m_pTransformCom->m_vInfo[Engine::INFO_POS];
				_float AtPlayerLength = D3DXVec3Length(&AtPlayerDirection);
				_vec3 AtPlayerLookDirection = _vec3(0.f, 0.f, -1.f);
				D3DXVec3Normalize(&AtPlayerDirection, &AtPlayerDirection);
				_float AtPlayerAngle = D3DXVec3Dot(&AtPlayerDirection, &AtPlayerLookDirection);
				D3DXVec3Cross(&AtPlayerLookDirection, &AtPlayerDirection, &AtPlayerLookDirection);
				AtPlayerAngle = acosf(AtPlayerAngle);
				AtPlayerAngle = AtPlayerAngle;
				if (AtPlayerLookDirection.y > 0)
				{
					m_pTransformCom->m_vAngle.y = -AtPlayerAngle;
				}
				else
				{
					m_pTransformCom->m_vAngle.y = AtPlayerAngle;
				}
				m_pTransformCom->Move_Pos(&(AtPlayerDirection * fTimeDelta * m_fSpeed));
				m_pMeshCom->Set_AnimationSet(34);
				if (AtPlayerLength < 1)
				{
					m_bAttackOn = false;
					m_bFire = true;
				}
			}
			if (m_bFire)
			{
				Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
				_vec3 AtPlayerDirection = pPlayerTransform->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
				_vec3 AtPlayerLookDirection = _vec3(0.f, 0.f, -1.f);
				D3DXVec3Normalize(&AtPlayerDirection, &AtPlayerDirection);
				_float AtPlayerAngle = D3DXVec3Dot(&AtPlayerDirection, &AtPlayerLookDirection);
				D3DXVec3Cross(&AtPlayerLookDirection, &AtPlayerDirection, &AtPlayerLookDirection);
				AtPlayerAngle = acosf(AtPlayerAngle);
				AtPlayerAngle = AtPlayerAngle;
				if (AtPlayerLookDirection.y > 0)
				{
					m_pTransformCom->m_vAngle.y = -AtPlayerAngle;
				}
				else
				{
					m_pTransformCom->m_vAngle.y = AtPlayerAngle;
				}
				m_pMeshCom->Set_AnimationSet(5);
				m_bFire = false;
			}
		}
		else if (m_eBossMoveState == Engine::BMS_TARGETMOVE)
		{
			if (!m_bTargetAniMoveOn)
			{
				m_pMeshCom->Set_AnimationSet(9);
				m_bTargetAniMoveOn = true;
			}
			Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
			_vec3 AtPlayerDirection = pPlayerTransform->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
			if (D3DXVec3Length(&AtPlayerDirection) > 5.f)
			{
				_vec3 AtPlayerLookDirection = _vec3(0.f, 0.f, -1.f);
				D3DXVec3Normalize(&AtPlayerDirection, &AtPlayerDirection);
				_float AtPlayerAngle = D3DXVec3Dot(&AtPlayerDirection, &AtPlayerLookDirection);
				D3DXVec3Cross(&AtPlayerLookDirection, &AtPlayerDirection, &AtPlayerLookDirection);
				AtPlayerAngle = acosf(AtPlayerAngle);
				AtPlayerAngle = AtPlayerAngle;
				if (AtPlayerLookDirection.y > 0)
				{
					m_pTransformCom->m_vAngle.y = -AtPlayerAngle;
				}
				else
				{
					m_pTransformCom->m_vAngle.y = AtPlayerAngle;
				}
				m_pTransformCom->Move_Pos(&(AtPlayerDirection * fTimeDelta));
			}
			else
			{
				m_bAttackOn = true;
			}
		}
		else if (m_eBossMoveState == Engine::BMS_DEAD)
		{
			m_pMeshCom->Set_AnimationSet(31);
			m_bDamaged = false;
		}
		else if (m_eBossMoveState == Engine::BMS_END)
		{
			m_pMeshCom->Set_AnimationSet(27);
		}
	if (true == m_pMeshCom->Is_AnimationSetEnd())
	{
		if (m_eBossMoveState == Engine::BMS_INTRO)
		{
			m_eBossMoveState = Engine::BMS_TARGETMOVE;
			m_fIntroTime = 0.f;
			m_fAddHeight = 0.f;
			//m_pMeshCom->Set_AnimationSet(34);
		}
		else if (m_eBossMoveState == Engine::BMS_TARGETMOVE)
		{
			if (m_bAttackOn)
			{
				m_eBossMoveState = Engine::BMS_ATTACK;
			}
			m_bTargetAniMoveOn = false;
		}
		else if (m_eBossMoveState == Engine::BMS_ATTACK)
		{
			m_bPatterning = true;
			m_eBossMoveState = Engine::BMS_END;
		}
		else if (m_eBossMoveState == Engine::BMS_FIRE)
		{
			m_bFire = true;
			m_eBossMoveState == Engine::BMS_END;
		}
		else if(m_eBossMoveState == Engine::BMS_END)
		{
			PatternCall(fTimeDelta);
			m_pMeshCom->Set_AnimationSet(27);
		}
		else if (m_eBossMoveState == Engine::BMS_DEAD)
		{
			m_bDead = true;
		}
	}

	m_bResearchCell = CellResearch();
}

void CBoss::Load_Object(_vec3 Position)
{
	m_vLoadPosition = Position;
	m_pTransformCom->Set_Pos(&m_vLoadPosition);
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBoss*	pInstance = new CBoss(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CBoss::Set_ColliderFrame(char * FrameName)
{
	Engine::CComponent*		pComponent = nullptr;
	std::vector<char> vname;
	for (_uint repeat = 0; repeat < strlen(FrameName) + 1; repeat++)
	{
		vname.push_back(FrameName[repeat]);
	}
	m_vColliderName.push_back(vname);

	pComponent = m_pColliderCom->Clone();
	m_mapCollider.emplace(&(m_vColliderName[m_iColliderCount][0]), dynamic_cast<Engine::CCollider*>(pComponent));
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);
	m_iColliderCount++;
}

bool CBoss::SphereCollision_ToObject(CGameObject* GameObject)
{
	CColliderMesh* pObjColliderCom = dynamic_cast<CColliderMesh*>(GameObject);
	NULL_CHECK_RETURN(pObjColliderCom, false);

	_vec3 ObjectDistance = pObjColliderCom->m_pTransformCom->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
	_float DistanceNorm = D3DXVec3Length(&ObjectDistance);


	auto iter = m_mapCollider.begin();
	for (; iter != m_mapCollider.end(); iter++)
	{
		_float ColliderDis = (pObjColliderCom->m_pColliderCom->m_fSphereRadius*0.01f) + (iter->second->m_fSphereRadius*0.01f);
		if (ColliderDis > DistanceNorm)
		{
			return true;
		}
	}
	return false;
}

bool	CBoss::BoxCollision_ToObject(CGameObject* GameObject, const _tchar* ObjectKinds)
{
	if (ObjectKinds == L"CollMesh")
	{
		CColliderMesh* pObjColliderCom = dynamic_cast<CColliderMesh*>(GameObject);
		NULL_CHECK_RETURN(pObjColliderCom, false);

		_vec3 vBoxMin = _vec3(-(pObjColliderCom->m_pColliderCom->m_fBoxWidth*0.5f), -(pObjColliderCom->m_pColliderCom->m_fBoxHeight)*0.5f, -(pObjColliderCom->m_pColliderCom->m_fBoxDepth)*0.5f);
		_vec3 vBoxMax = _vec3((pObjColliderCom->m_pColliderCom->m_fBoxWidth*0.5f), (pObjColliderCom->m_pColliderCom->m_fBoxHeight)*0.5f, (pObjColliderCom->m_pColliderCom->m_fBoxDepth)*0.5f);

		bool a;
		auto iter = m_mapCollider.begin();
		for (; iter != m_mapCollider.end(); iter++)
		{
			if (a = (m_pCalculatorCom->Collision_SphereOBB(&vBoxMin, &vBoxMax, pObjColliderCom->m_pTransformCom->Get_WorldMatrix(), &m_pTransformCom->m_vInfo[Engine::INFO_POS], &iter->second->m_fSphereRadius)))
			{
				if ((pObjColliderCom->m_pTransformCom->m_vInfo[Engine::INFO_POS].y + (pObjColliderCom->m_pColliderCom->m_fBoxHeight * 0.25f * 0.01f)) < m_pTransformCom->m_vInfo[Engine::INFO_POS].y)
				{
					//m_eBossHeightCollisionState = Engine::PHCS_ON;
				}
				else
				{
					//m_eBossHeightCollisionState = Engine::PHCS_SIDE;
					if (pObjColliderCom->m_eColliderHeight == Engine::CH_HIGH)
					{
						m_bColliderHigher = true;
						m_bColliderFrontJump = false;
					}
					else if (pObjColliderCom->m_eColliderHeight == Engine::CH_LOW)
					{
						m_bColliderHigher = false;
						m_bColliderFrontJump = false;
					}
					else if (pObjColliderCom->m_eColliderHeight == Engine::CH_FRONTJUMP)
					{
						m_bColliderHigher = false;
						m_bColliderFrontJump = true;
					}
					else if (pObjColliderCom->m_eColliderHeight == Engine::CH_NEXTSTAGE)
					{
						m_bColliderHigher = false;
						m_bColliderFrontJump = false;
						m_bNextStage = true;
					}
				}
				return true;
			}
		}
		return false;
	}
	else if (ObjectKinds == L"RuinBox")
	{
		m_bOnRuinBox = false;
		CRuinBox* pObjColliderCom = dynamic_cast<CRuinBox*>(GameObject);
		NULL_CHECK_RETURN(pObjColliderCom, false);

		_vec3 vBoxMin = _vec3(-(pObjColliderCom->m_pColliderCom->m_fBoxWidth*0.5f), -(pObjColliderCom->m_pColliderCom->m_fBoxHeight)*0.5f, -(pObjColliderCom->m_pColliderCom->m_fBoxDepth)*0.5f);
		_vec3 vBoxMax = _vec3((pObjColliderCom->m_pColliderCom->m_fBoxWidth*0.5f), (pObjColliderCom->m_pColliderCom->m_fBoxHeight)*0.5f, (pObjColliderCom->m_pColliderCom->m_fBoxDepth)*0.5f);

		bool a;
		auto iter = m_mapCollider.begin();
		for (; iter != m_mapCollider.end(); iter++)
		{
			if (a = (m_pCalculatorCom->Collision_SphereOBB(&vBoxMin, &vBoxMax, pObjColliderCom->m_pTransformCom->Get_WorldMatrix(), &m_pTransformCom->m_vInfo[Engine::INFO_POS], &iter->second->m_fSphereRadius)))
			{
				if ((pObjColliderCom->m_pTransformCom->m_vInfo[Engine::INFO_POS].y + (pObjColliderCom->m_pColliderCom->m_fBoxHeight * 0.25f * 0.01f)) < m_pTransformCom->m_vInfo[Engine::INFO_POS].y)
				{
					//m_eBossHeightCollisionState = Engine::PHCS_ON;
					m_bOnRuinBox = true;
				}
				else
				{
					//m_eBossHeightCollisionState = Engine::PHCS_SIDE;
					m_bOnRuinBox = true;
				}
				return true;
			}
		}
		return false;
	}
}

bool CBoss::CellResearch()
{
	_bool bCellGet = false;
	for (_ulong i = 0; i < m_pNaviMeshCom->GetVecCell()->size(); ++i)
	{
		bCellGet = (*(m_pNaviMeshCom->GetVecCell()))[i]->Compare_SearchCell(&m_pTransformCom->m_vInfo[Engine::INFO_POS], &(m_pNaviMeshCom->m_dwIndex));
	}
	return bCellGet;
}

void CBoss::PatternCall(const _float& fTimeDelta)
{
	if (m_bPatterning)
	{
		if (m_fPatternDelay > 2.f)
		{
			switch (m_eBossPattern)
			{
			case Engine::BP_ONE:
				m_eBossMoveState = Engine::BMS_TARGETMOVE;
				break;
			case Engine::BP_TWO:
				m_eBossMoveState = Engine::BMS_FIRE;
				break;
			case Engine::BP_THREE:
				m_eBossMoveState = Engine::BMS_TARGETMOVE;
				break;
			case Engine::BP_END:
				m_eBossMoveState = Engine::BMS_DEAD;
				break;
			}
		}
		m_fPatternDelay += fTimeDelta;
	}
	else
	{
		m_fPatternDelay = 0;
	}
}

void CBoss::Free(void)
{
	auto iter = m_mapCollider.begin();
	for (; iter != m_mapCollider.end(); iter++)
	{
		Engine::Safe_Release(iter->second);
	}
	Engine::CGameObject::Free();
}


HRESULT Client::CBoss::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.05f, 0.05f, 0.05f);
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(10.f));
	m_pMeshCom->Set_AnimationSet(27);
	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}
Client::_int Client::CBoss::Update_Object(const _float& fTimeDelta)
{
	//SetUp_OnTerrain();
	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);
	
	m_pMeshCom->Play_Animation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	//SetUp_OnTerrain();
	return 0;
}
void Client::CBoss::Render_Object(void)
{
		LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);
		Engine::Safe_AddRef(pEffect);
	if (!m_bDead)
	{
		_uint	iMaxPass = 0;

		pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
		pEffect->BeginPass(0);

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
		pEffect->End();
	}
	//m_pNaviMeshCom->Render_NaviMeshes();

	Engine::_matrix matWorld;
	auto iter = m_mapCollider.begin();
	int mrepeat = 0;
	char* ColName;
	for (; iter != m_mapCollider.end(); iter++)
	{
		ColName = new char[40];
		for (_uint nrepeat = 0; nrepeat < m_vColliderName[mrepeat].size(); nrepeat++)
		{
			ColName[nrepeat] = m_vColliderName[mrepeat][nrepeat];
		}
		const Engine::D3DXFRAME_DERIVED* pFrame = m_pMeshCom->Get_FrameByName(ColName);
		m_pTransformCom->Get_WorldMatrix(&matWorld);
		matWorld = pFrame->CombinedTransformationMatrix * matWorld;
		if (m_bHeightCollider)
		{
			iter->second->Render_Collider(Engine::COL_TRUE, &matWorld);
		}
		else
		{
			iter->second->Render_Collider(Engine::COL_FALSE, &matWorld);
		}
		mrepeat++;
		Engine::Safe_Delete_Array(ColName);
	}

	Engine::Safe_Release(pEffect);
}
void Client::CBoss::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	_vec3	vPosiDir = _vec3(0.f, -1.f, 0.f);
	BOOL	bHeightColl;
	_float	MeshDistance;
	_float	fSaveY;

	LPD3DXMESH HeightMesh = dynamic_cast<Engine::CStaticMesh*>(Engine::Get_Component(L"GameLogic", L"BossRoom", L"Com_Mesh", Engine::ID_STATIC))->m_pMesh;

	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	vPosition.y = 0.5f+vPosition.y;
	fSaveY = vPosition.y;

	_matrix	matWorld;
	dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"BossRoom", L"Com_Transform", Engine::ID_DYNAMIC))->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	
	D3DXVec3TransformCoord(&vPosition, &vPosition, &matWorld);
	D3DXVec3TransformNormal(&vPosiDir, &vPosiDir, &matWorld);

	D3DXIntersect(HeightMesh,
					&vPosition,
					&vPosiDir,
					&bHeightColl,
					nullptr,
					nullptr,
					nullptr,
					&MeshDistance,
					nullptr,
					nullptr);
	if(bHeightColl)
	{
		m_pTransformCom->m_vInfo[Engine::INFO_POS].y = fSaveY - MeshDistance;
	}
}