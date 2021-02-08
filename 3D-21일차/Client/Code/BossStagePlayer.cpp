#include "stdafx.h"
#include "BossStagePlayer.h"
#include "Export_Function.h"
#include "ColliderMesh.h"
#include "RuinBox.h"
#include "Boss.h"

CBossStagePlayer::CBossStagePlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
	, m_ePlayerMoveState(Engine::PMS_END)
{
}

CBossStagePlayer::~CBossStagePlayer(void)
{

}

Client::_vec3 Client::CBossStagePlayer::PickUp_OnTerrain(void)
{
	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	Engine::CTransform*		pTerrainTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransformCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransformCom);
}

HRESULT Client::CBossStagePlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Player"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
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

	return S_OK;
}

HRESULT CBossStagePlayer::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
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

void Client::CBossStagePlayer::Key_Input(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);

	if (m_ePlayerMoveState == Engine::PMS_END)
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			if (GetAsyncKeyState('A') & 0x8000)
			{
				m_pTransformCom->m_vAngle.y = D3DXToRadian(-45.f);
			}
			else if (GetAsyncKeyState('D') & 0x8000)
			{
				m_pTransformCom->m_vAngle.y = D3DXToRadian(45.f);
			}
			else
			{
				m_pTransformCom->m_vAngle.y = D3DXToRadian(0.f);
			}
			_vec3	vPos, vDir;
			m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
			m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), fTimeDelta));

			if (!m_bResearchCell)
			{
				m_bResearchCell = CellResearch();
			}

			m_pMeshCom->Set_AnimationSet(34);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			if (GetAsyncKeyState('A') & 0x8000)
			{
				m_pTransformCom->m_vAngle.y = D3DXToRadian(225.f);
			}
			else if (GetAsyncKeyState('D') & 0x8000)
			{
				m_pTransformCom->m_vAngle.y = D3DXToRadian(135.f);
			}
			else
			{
				m_pTransformCom->m_vAngle.y = D3DXToRadian(180.f);
			}
			_vec3	vPos, vDir;
			m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
			m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), fTimeDelta));

			if (!m_bResearchCell)
			{
				m_bResearchCell = CellResearch();
			}

			m_pMeshCom->Set_AnimationSet(34);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			m_pTransformCom->m_vAngle.y = D3DXToRadian(270.f);
			_vec3	vPos, vDir;
			m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
			m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), fTimeDelta));

			if (!m_bResearchCell)
			{
				m_bResearchCell = CellResearch();
			}

			m_pMeshCom->Set_AnimationSet(34);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			m_pTransformCom->m_vAngle.y = D3DXToRadian(90.f);
			_vec3	vPos, vDir;
			m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
			m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
			D3DXVec3Normalize(&vDir, &vDir);

			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), fTimeDelta));

			if (!m_bResearchCell)
			{
				m_bResearchCell = CellResearch();
			}

			m_pMeshCom->Set_AnimationSet(34);
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			if (m_bHeightCollider)
			{
				if (m_ePlayerHeightCollisionState == Engine::PHCS_SIDE)
				{
					m_FrameMatrix = m_pMeshCom->Get_FrameByName("mouse_spine1")->CombinedTransformationMatrix;
					m_ePlayerMoveState = Engine::PMS_CLIMBUP;
					if (m_bColliderHigher)
					{
						m_pMeshCom->Set_AnimationSet(95);
					}
					else
					{
						m_pMeshCom->Set_AnimationSet(46);
					}

					if (m_bColliderFrontJump)
					{
						m_pMeshCom->Set_AnimationSet(38);
					}

					if (m_bOnRuinBox)
					{
						m_pMeshCom->Set_AnimationSet(95);
					}
				}
				else if (m_ePlayerHeightCollisionState == Engine::PHCS_ON)
				{
					m_FrameMatrix = m_pMeshCom->Get_FrameByName("mouse_spine1")->CombinedTransformationMatrix;
					if (m_bOnRuinBox)
					{
						m_ePlayerMoveState = Engine::PMS_CLIMBUP;
						m_pMeshCom->Set_AnimationSet(38);
					}
					else
					{
						m_ePlayerMoveState = Engine::PMS_CLIMBDOWN;
						m_pMeshCom->Set_AnimationSet(106);
					}
				}
			}
		}

		if (!(GetAsyncKeyState('F') & 0x8000))
		{
			if (Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)
			{
				m_ePlayerMoveState = Engine::PMS_ATTACK;
				m_uiComboStack++;
				if (m_uiComboStack == 1)
				{
					m_pMeshCom->Set_AnimationSet(134);
					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::PLAYER);
					CSoundMgr::Get_Instance()->PlaySound(L"Attack1.ogg", CSoundMgr::CHANNELID::PLAYER);
				}
				else if (m_uiComboStack == 2)
				{
					m_pMeshCom->Set_AnimationSet(132);
					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::PLAYER);
					CSoundMgr::Get_Instance()->PlaySound(L"Attack2.ogg", CSoundMgr::CHANNELID::PLAYER);
				}
				else if (m_uiComboStack == 3)
				{
					m_pMeshCom->Set_AnimationSet(130);
					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::PLAYER);
					CSoundMgr::Get_Instance()->PlaySound(L"Attack3.ogg", CSoundMgr::CHANNELID::PLAYER);
					m_uiComboStack = 0;
				}
			}
		}
	}

	if (m_ePlayerMoveState == Engine::PMS_DAMAGE)
	{
		m_FrameMatrix = m_pMeshCom->Get_FrameByName("mouse_head")->CombinedTransformationMatrix;
		m_pMeshCom->Set_AnimationSet(41);
	}

	if ((m_ePlayerMoveState == Engine::PMS_CLIMBDOWN) && (m_bHanging == true))
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			m_bHanging = false;
			m_ePlayerMoveState = Engine::PMS_END;
			m_pMeshCom->Set_AnimationSet(99);
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			m_bHanging = false;
			m_ePlayerMoveState = Engine::PMS_END;
			m_pMeshCom->Set_AnimationSet(105);
		}
	}

	if (true == m_pMeshCom->Is_AnimationSetEnd())
	{
		if(m_ePlayerMoveState == Engine::PMS_CLIMBUP)
		{
			_vec3 PostVec = _vec3(0.f, 0.f, 0.f);
			_vec3 CurrentVec = _vec3(0.f, 0.f, 0.f);
			_matrix FrameMatrix = m_pMeshCom->Get_FrameByName("mouse_spine1")->CombinedTransformationMatrix;
			_matrix TransformMatrix = *m_pTransformCom->Get_WorldMatrix();
			_bool bCellGet = false;

			m_FrameMatrix *= TransformMatrix;
			FrameMatrix *= TransformMatrix;

			D3DXVec3TransformCoord(&PostVec, &PostVec, &m_FrameMatrix);
			D3DXVec3TransformCoord(&CurrentVec, &CurrentVec, &FrameMatrix);
			_vec3 FramePosition = CurrentVec - PostVec;
			m_pTransformCom->Move_Pos(&FramePosition);
			m_ePlayerMoveState = Engine::PMS_END;
		}
		else if (m_ePlayerMoveState == Engine::PMS_CLIMBDOWN)
		{
			if (m_bHanging == false)
			{
				_vec3 PostVec = _vec3(0.f, 0.f, 0.f);
				_vec3 CurrentVec = _vec3(0.f, 0.f, 0.f);
				_matrix FrameMatrix = m_pMeshCom->Get_FrameByName("mouse_spine1")->CombinedTransformationMatrix;
				_matrix TransformMatrix = *m_pTransformCom->Get_WorldMatrix();
				_bool bCellGet = false;

				m_FrameMatrix *= TransformMatrix;
				FrameMatrix *= TransformMatrix;

				D3DXVec3TransformCoord(&PostVec, &PostVec, &m_FrameMatrix);
				D3DXVec3TransformCoord(&CurrentVec, &CurrentVec, &FrameMatrix);
				_vec3 FramePosition = CurrentVec - PostVec;
				m_pTransformCom->Move_Pos(&FramePosition);
				m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(180.f));
				m_pMeshCom->Set_AnimationSet(104);
				m_bHanging = true;
			}
			else
			{
				m_pMeshCom->Set_AnimationSet(104);
			}
		}
		else if (m_ePlayerMoveState == Engine::PMS_ATTACK)
		{
			m_ePlayerMoveState = Engine::PMS_END;
		}
		else if (m_ePlayerMoveState == Engine::PMS_DAMAGE)
		{
			_vec3 PostVec = _vec3(0.f, 0.f, 0.f);
			_vec3 CurrentVec = _vec3(0.f, 0.f, 0.f);
			_matrix FrameMatrix = m_pMeshCom->Get_FrameByName("mouse_head")->CombinedTransformationMatrix;
			_matrix TransformMatrix = *m_pTransformCom->Get_WorldMatrix();
			_bool bCellGet = false;
			
			m_FrameMatrix *= TransformMatrix;
			FrameMatrix *= TransformMatrix;
			
			D3DXVec3TransformCoord(&PostVec, &PostVec, &m_FrameMatrix);
			D3DXVec3TransformCoord(&CurrentVec, &CurrentVec, &FrameMatrix);
			_vec3 FramePosition = CurrentVec - PostVec;
			m_pTransformCom->Move_Pos(&FramePosition);
			m_ePlayerMoveState = Engine::PMS_END;
		}
		else
		{
			m_pMeshCom->Set_AnimationSet(80);
		}
	}

	m_bResearchCell = CellResearch();
}

void CBossStagePlayer::Load_Object(_vec3 Position)
{
	m_vLoadPosition = Position;
	m_pTransformCom->Set_Pos(&m_vLoadPosition);
}

CBossStagePlayer* CBossStagePlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossStagePlayer*	pInstance = new CBossStagePlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CBossStagePlayer::Set_ColliderFrame(char * FrameName)
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

bool CBossStagePlayer::BossSphereCollision_ToObject(CGameObject* GameObject)
{
	CBoss* pObjColliderCom = dynamic_cast<CBoss*>(GameObject);
	const Engine::D3DXFRAME_DERIVED* pFrame;
	const Engine::D3DXFRAME_DERIVED* pPFrame;
	NULL_CHECK_RETURN(pObjColliderCom, false);
	Engine::_matrix matWorld;
	Engine::_matrix matPWorld;
	Engine::_matrix matInverse;
	_vec3 LocalObject;
	_vec3 CompareVec;
	_float ObjectDistance;
	_float SphereDistance;
	auto ObjectColliderIter = pObjColliderCom->m_mapCollider.begin();
	for (; ObjectColliderIter != pObjColliderCom->m_mapCollider.end(); ObjectColliderIter++)
	{
		if ((!strcmp(ObjectColliderIter->first, "Beetle_LowerClaw_Right")) ||
			(!strcmp(ObjectColliderIter->first, "Beetle_LowerClaw_Right_Gun")) ||
			(!strcmp(ObjectColliderIter->first, "Beetle_LowerClaw_Right_Gun2")))
		{
			LocalObject = pObjColliderCom->m_pTransformCom->m_vInfo[Engine::INFO_POS];
			pObjColliderCom->m_pTransformCom->Get_WorldMatrix(&matInverse);
			D3DXMatrixInverse(&matInverse, 0, &matInverse);
			D3DXVec3TransformCoord(&LocalObject, &LocalObject, &matInverse);
			pFrame = pObjColliderCom->m_pMeshCom->Get_FrameByName(ObjectColliderIter->first);
			pObjColliderCom->m_pTransformCom->Get_WorldMatrix(&matWorld);
			matWorld = pFrame->CombinedTransformationMatrix * matWorld;
			D3DXVec3TransformCoord(&LocalObject, &LocalObject, &matWorld);
			
			pPFrame = m_pMeshCom->Get_FrameByName("mouse_hips");
			CompareVec = m_pTransformCom->m_vInfo[Engine::INFO_POS];
			m_pTransformCom->Get_WorldMatrix(&matPWorld);
			D3DXMatrixInverse(&matPWorld, 0, &matPWorld);
			D3DXVec3TransformCoord(&CompareVec, &CompareVec, &matPWorld);
			m_pTransformCom->Get_WorldMatrix(&matPWorld);
			matPWorld = pPFrame->CombinedTransformationMatrix * matPWorld;
			D3DXVec3TransformCoord(&CompareVec, &CompareVec, &matPWorld);


			CompareVec = CompareVec - LocalObject;
			ObjectDistance = D3DXVec3Length(&CompareVec);
			
			SphereDistance = ((ObjectColliderIter->second->m_fSphereRadius) * 0.05f) + ((m_pColliderCom->m_fSphereRadius) * 0.05f);

			if (ObjectDistance < SphereDistance)
			{
				return true;
			}
		}
	}
	return false;
}

bool	CBossStagePlayer::BoxCollision_ToObject(CGameObject* GameObject, const _tchar* ObjectKinds)
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
					m_ePlayerHeightCollisionState = Engine::PHCS_ON;
				}
				else
				{
					m_ePlayerHeightCollisionState = Engine::PHCS_SIDE;
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
					m_ePlayerHeightCollisionState = Engine::PHCS_ON;
					m_bOnRuinBox = true;
				}
				else
				{
					m_ePlayerHeightCollisionState = Engine::PHCS_SIDE;
					m_bOnRuinBox = true;
				}
				return true;
			}
		}
		return false;
	}
	return false;
}

bool CBossStagePlayer::CellResearch()
{
	_bool bCellGet = false;
	for (_ulong i = 0; i < m_pNaviMeshCom->GetVecCell()->size(); ++i)
	{
		bCellGet = (*(m_pNaviMeshCom->GetVecCell()))[i]->Compare_SearchCell(&m_pTransformCom->m_vInfo[Engine::INFO_POS], &(m_pNaviMeshCom->m_dwIndex));
	}
	return bCellGet;
}

void CBossStagePlayer::Free(void)
{
	auto iter = m_mapCollider.begin();
	for (; iter != m_mapCollider.end(); iter++)
	{
		Engine::Safe_Release(iter->second);
	}
	Engine::CGameObject::Free();
}


HRESULT Client::CBossStagePlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(180.f));
	m_pMeshCom->Set_AnimationSet(44);

	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}
Client::_int Client::CBossStagePlayer::Update_Object(const _float& fTimeDelta)
{
	SetUp_OnTerrain();
	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);
	
	m_pMeshCom->Play_Animation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	//SetUp_OnTerrain();
	return 0;
}
void Client::CBossStagePlayer::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );
		
	m_pMeshCom->Render_Meshes(pEffect);
	
	pEffect->EndPass();
	pEffect->End();

	if (false)
	{
		m_pNaviMeshCom->Render_NaviMeshes();
		Engine::_matrix matWorld;
		auto iter = m_mapCollider.begin();
		int mrepeat = 0;
		char* ColName;
		for (; iter != m_mapCollider.end(); iter++)
		{
			ColName = new char[20];
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
	}
	Engine::Safe_Release(pEffect);
}
void Client::CBossStagePlayer::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	_vec3	vPosiDir = _vec3(0.f, -1.f, 0.f);
	BOOL	bHeightColl;
	_float	MeshDistance;
	_float	fSaveY;

	LPD3DXMESH HeightMesh = dynamic_cast<Engine::CStaticMesh*>(Engine::Get_Component(L"GameLogic", L"BossRoom", L"Com_Mesh", Engine::ID_STATIC))->m_pMesh;

	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	vPosition.y = 0.5f + vPosition.y;
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
	if (bHeightColl)
	{
		m_pTransformCom->m_vInfo[Engine::INFO_POS].y = fSaveY - MeshDistance;
	}
}