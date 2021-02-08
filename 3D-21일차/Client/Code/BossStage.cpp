#include "stdafx.h"
#include "BossStage.h"
#include "Export_Function.h"
#include "strsafe.h"

CBossStage::CBossStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CBossStage::~CBossStage(void)
{

}

HRESULT CBossStage::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Ready_Environment_Layer(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BGM);
	CSoundMgr::Get_Instance()->PlayBGM(L"BossStage.ogg");
	//_crtBreakAlloc = 56398;
	return S_OK;
}

Engine::_int CBossStage::Update_Scene(const _float& fTimeDelta)
{
	bool IsColl = false;
	bool AttackColl = false;
	_uint CollCount = 0;
	auto Playeriter = pGameLogicLayer->m_mapObject.begin();
	auto Colliter = m_vecColliderMesh.begin();
	auto EnvironmentIter = pEnvironmentLayer->m_mapObject.begin();
	CBossStagePlayer* BSP = nullptr;
	CBoss*	BO = nullptr;
	CSword* Swo = nullptr;
	for (; Playeriter != pGameLogicLayer->m_mapObject.end(); Playeriter++)
	{
		if (Playeriter->first == L"Player")
		{
	//		for (; Colliter != m_vecColliderMesh.end(); Colliter++)
	//		{
	//			if (Colliter->first == L"CollMesh")
	//			{
	//				IsColl = dynamic_cast<CPlayer*>(Playeriter->second)->BoxCollision_ToObject(Colliter->second, L"CollMesh");
	//				dynamic_cast<CColliderMesh*>(Colliter->second)->m_bHeightCollider = IsColl;
	//				if (IsColl == true)
	//				{
	//					CollCount++;
	//				}
	//			}
	//			else if (Colliter->first == L"RuinBox")
	//			{
	//				IsColl = dynamic_cast<CPlayer*>(Playeriter->second)->BoxCollision_ToObject(Colliter->second, L"RuinBox");
	//				dynamic_cast<CRuinBox*>(Colliter->second)->m_bHeightCollider = IsColl;
	//				if (IsColl == true)
	//				{
	//					CollCount++;
	//				}
	//			}
	//		}
	//		if (CollCount != 0)
	//		{
	//			dynamic_cast<CPlayer*>(Playeriter->second)->m_bHeightCollider = true;
	//		}
	//		else
	//		{
	//			dynamic_cast<CPlayer*>(Playeriter->second)->m_ePlayerHeightCollisionState = Engine::PHCS_END;
	//			dynamic_cast<CPlayer*>(Playeriter->second)->m_bHeightCollider = false;
	//		}
			BSP = dynamic_cast<CBossStagePlayer*>(Playeriter->second);
		}
		if (Playeriter->first == L"Sword")
		{
			Swo = dynamic_cast<CSword*>(Playeriter->second);
		}
		else if (Playeriter->first == L"Boss")
		{
			BO = dynamic_cast<CBoss*>(Playeriter->second);
		}
	}

	if (!m_bMeetBoss)
	{
		_float a = D3DXVec3Length(&((BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS]) - (BO->m_pTransformCom->m_vInfo[Engine::INFO_POS])));
		m_vEye = BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS];
		m_vEye.z -= 5.f;
		m_vEye.y += 2.f;
		m_vAt = BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS];
		m_vUp = _vec3(0.f, 1.f, 0.f);
		pStaticCamera->CameraPositionUpdate(&m_vEye, &m_vAt, &m_vUp);
		if (a < 7)
		{
			m_bMeetBoss = true;
		}
	}
	else
	{
		m_vEye.x = BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS].x + (BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS] - BO->m_pTransformCom->m_vInfo[Engine::INFO_POS]).x + 1.f;
		m_vEye.z = BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS].z - (BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS] - BO->m_pTransformCom->m_vInfo[Engine::INFO_POS]).z + 4.f;
		m_vEye.y = BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS].y + (BSP->m_pTransformCom->m_vInfo[Engine::INFO_POS] - BO->m_pTransformCom->m_vInfo[Engine::INFO_POS]).y + 4.f;
		m_vAt = BO->m_pTransformCom->m_vInfo[Engine::INFO_POS];
		m_vUp = _vec3(0.f, 1.f, 0.f);
		pStaticCamera->CameraPositionUpdate(&m_vEye, &m_vAt, &m_vUp);
	}

	if (m_bStageFirstCall)
	{
		_float d = BO->m_pColliderCom->m_fSphereRadius * 4.f;
		AttackColl = Swo->m_pCalculatorCom->Collision_SphereOBB(Swo->m_pColliderCom->Get_Min(), Swo->m_pColliderCom->Get_Max(), &Swo->m_pTransformCom->m_matWorld, &BO->m_pTransformCom->m_vInfo[Engine::INFO_POS], &d);
	}
	else
	{
		m_bStageFirstCall = true;
	}
	
	IsColl = BSP->BossSphereCollision_ToObject(BO);
	BSP->m_bHeightCollider = IsColl;
	BO->m_bHeightCollider = IsColl;

	if (BSP->m_bHeightCollider)
	{
		if (BO->m_eBossMoveState == Engine::BMS_ATTACK)
		{
			BSP->m_ePlayerMoveState = Engine::PMS_DAMAGE;
		}
	}

	if (AttackColl)
	{
		if (BSP->m_ePlayerMoveState == Engine::PMS_ATTACK)
		{
			BO->m_bDamaged = true;
		}
		else
		{
			BO->m_bDamaged = false;
		}
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

void CBossStage::Render_Scene(void)
{

}

HRESULT CBossStage::Ready_Environment_Layer(const _tchar * pLayerTag)
{
	pEnvironmentLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pEnvironmentLayer, E_FAIL);
	
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pEnvironmentLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);
	//
	//pGameObject = CRuinBox::Create(m_pGraphicDev, L"RuinBox");
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//m_vecColliderMesh.emplace(L"RuinBox", pGameObject);
	//FAILED_CHECK_RETURN(pEnvironmentLayer->Add_GameObject(L"RuinBox", pGameObject), E_FAIL);

	//pGameObject = CTerrain::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pEnvironmentLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	//pGameObject = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f),
	//													&_vec3(0.f, 0.f, 10.f),
	//													&_vec3(0.f, 1.f, 0.f));
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pEnvironmentLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	pGameObject = CStaticCamera::Create(m_pGraphicDev, &m_vEye,
		&_vec3(0.f, 0.f, 10.f),
		&_vec3(0.f, 1.f, 0.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pStaticCamera = dynamic_cast<CStaticCamera*>(pGameObject);
	FAILED_CHECK_RETURN(pEnvironmentLayer->Add_GameObject(L"StaticCamera", pGameObject), E_FAIL);
	
	m_mapLayer.emplace(pLayerTag, pEnvironmentLayer);
	
	return S_OK;
}

HRESULT CBossStage::Ready_GameLogic_Layer(const _tchar * pLayerTag)
{
	pGameLogicLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pGameLogicLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CBoss::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Boss", pGameObject), E_FAIL);
	//
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);

	/*for (_uint i = 0; i < 50; ++i)
	{
		pGameObject = CMonster::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monster", pGameObject), E_FAIL);
	}*/

	//pGameObject = CStone::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Stone", pGameObject), E_FAIL);

	//pGameObject = CTree::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Tree", pGameObject), E_FAIL);
	//
	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObject = CEffect::Create(m_pGraphicDev);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	//}

	//pGameObject = CUI::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"UI", pGameObject), E_FAIL);


	m_mapLayer.emplace(pLayerTag, pGameLogicLayer);

	return S_OK;
}

HRESULT CBossStage::Ready_UI_Layer(const _tchar * pLayerTag)
{
	return S_OK;
}



HRESULT CBossStage::Ready_LightInfo(void)
{
	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	 //0번 조명
	//tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	//
	//tLightInfo.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.1f);
	//tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 0.1f);
	//tLightInfo.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.1f);
	//
	//tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);
	//
	//if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
	//	return E_FAIL;

	// 1번 조명
	//tLightInfo.Type = D3DLIGHT_POINT;
	//tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 0.1f);
	//tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 0.1f);
	//tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 0.1f);
	//tLightInfo.Position = _vec3(5.f, 5.f, -5.f);
	//tLightInfo.Range = 10.f;
	//
	//if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 1)))
	//	return E_FAIL;
	//	
	//// 2번 조명
	//tLightInfo.Type = D3DLIGHT_POINT;
	//tLightInfo.Diffuse = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	//tLightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.2f, 1.f);
	//tLightInfo.Position = _vec3(10.f, 5.f, 10.f);
	//tLightInfo.Range = 10.f;
	//
	//if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 2)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CBossStage::Ready_LoadObjectFile(void)
{
	Engine::MESHSAVESTRUCT LoadMeshData;
	//ZeroMemory(&LoadMeshData, sizeof(LoadMeshData));
	FILE *fpMesh;
	char ColliderLast[5];
	_uint CharCount = 0;
	char CharLoad;
	_wfopen_s(&fpMesh, L"../Bin/Resource/ObjectSave/BossMeshData.M", L"rb");
	//메쉬 카운트
	_uint RepeatCount;
	fseek(fpMesh, -(_int(sizeof(_uint))), SEEK_END);
	fread(&RepeatCount, sizeof(_uint), 1, fpMesh);
	fseek(fpMesh, 0, SEEK_SET);
	//메쉬 로드
		for (_uint repeat = 0; repeat < RepeatCount; repeat++)
		{
			fread(&LoadMeshData.MeshPosition, sizeof(Engine::_vec3), 1, fpMesh);
			fread(&LoadMeshData.MeshTag, sizeof(Engine::_tchar) * 50, 1, fpMesh);
			fread(&LoadMeshData.MeshType, sizeof(Engine::MESHTYPE), 1, fpMesh);
			fread(&ColliderLast, sizeof(char) * 5, 1, fpMesh);
			while (strcmp(ColliderLast, "Last"))
			{
				vector<char> VecChar;
				fseek(fpMesh, -(Engine::_int(sizeof(char) * 5)), SEEK_CUR);
				CharCount = 0;
				while (true)
				{
					fread(&CharLoad, sizeof(char), 1, fpMesh);
					CharCount++;
					if (CharCount == 1)
					{
						VecChar.push_back(CharLoad);
					}
					else
					{
						VecChar.push_back(CharLoad);
					}
					if ('\0' == CharLoad)
					{
						break;
					}
				}
				LoadMeshData.ColliderTarget.push_back(VecChar);
				ZeroMemory(ColliderLast, sizeof(char) * 5);
				fread(&ColliderLast, sizeof(char) * 5, 1, fpMesh);
			}
			_tchar Name[20];
			lstrcpyW(Name, LoadMeshData.MeshTag);
			if (!lstrcmpW(Name, L"TombStone"))
			{
				CStone* pStone = CStone::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pStone, E_FAIL);
				pStone->Load_Object(LoadMeshData.MeshPosition);
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Stone", pStone), E_FAIL);
			}
			else if (!lstrcmpW(Name, L"Player"))
			{
				char FrameNameText[20];
				CBossStagePlayer* pPlayer = CBossStagePlayer::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pPlayer, E_FAIL);
				pPlayer->Load_Object(LoadMeshData.MeshPosition);
				
				for (int repeat = 0; repeat < LoadMeshData.ColliderTarget.size(); repeat++)
				{
					ZeroMemory(FrameNameText, 20);
					for (int repeatN = 0; repeatN < LoadMeshData.ColliderTarget[repeat].size(); repeatN++)
					{
						FrameNameText[repeatN] = LoadMeshData.ColliderTarget[repeat][repeatN];
					}
					pPlayer->Set_ColliderFrame(FrameNameText);
				}
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Player", pPlayer), E_FAIL);
			}
			else if (!lstrcmpW(Name, L"Intro010"))
			{
				CIntro010* pPlayer = CIntro010::Create(m_pGraphicDev, Name);
				NULL_CHECK_RETURN(pPlayer, E_FAIL);
				pPlayer->Load_Object(LoadMeshData.MeshPosition);
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Intro010", pPlayer), E_FAIL);
			}
			else if (!lstrcmpW(Name, L"BossRoom"))
			{
				CIntro010* pPlayer = CIntro010::Create(m_pGraphicDev, Name);
				NULL_CHECK_RETURN(pPlayer, E_FAIL);
				pPlayer->Load_Object(LoadMeshData.MeshPosition);
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"BossRoom", pPlayer), E_FAIL);
			}
			else if (!lstrcmpW(Name, L"Intro010_Brick"))
			{
				CIntro010* pPlayer = CIntro010::Create(m_pGraphicDev, Name);
				NULL_CHECK_RETURN(pPlayer, E_FAIL);
				pPlayer->Load_Object(LoadMeshData.MeshPosition);
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Intro010_Brick", pPlayer), E_FAIL);
			}
			else if (!lstrcmpW(Name, L"Intro010_Back"))
			{
				CIntro010* pPlayer = CIntro010::Create(m_pGraphicDev, Name);
				NULL_CHECK_RETURN(pPlayer, E_FAIL);
				pPlayer->Load_Object(LoadMeshData.MeshPosition);
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"Intro010_Back", pPlayer), E_FAIL);
			}
			else if (!lstrcmpW(Name, L"CollMesh"))
			{
				_vec3 Angle, Parameter;
				fread(&Angle, sizeof(Engine::_vec3), 1, fpMesh);
				fread(&Parameter, sizeof(Engine::_vec3), 1, fpMesh);
 				CColliderMesh* pMesh = CColliderMesh::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pMesh, E_FAIL);
				pMesh->Load_Object(LoadMeshData.MeshPosition);
				pMesh->m_pTransformCom->m_vAngle = Angle;
				pMesh->m_pColliderCom->Set_BoxParameter(Parameter.x, Parameter.y, Parameter.z);
				if (pMesh->m_pColliderCom->m_fBoxHeight <= 50)
				{
					pMesh->m_eColliderHeight = Engine::CH_LOW;
				}
				else
				{
					pMesh->m_eColliderHeight = Engine::CH_HIGH;
				}
				if (pMesh->m_pColliderCom->m_fBoxHeight == 1)
				{
					pMesh->m_eColliderHeight = Engine::CH_FRONTJUMP;
				}
				if (pMesh->m_pColliderCom->m_fBoxDepth == 1)
				{
					pMesh->m_eColliderHeight = Engine::CH_NEXTSTAGE;
				}
				FAILED_CHECK_RETURN(pGameLogicLayer->Add_GameObject(L"CollMesh", pMesh), E_FAIL);
				m_vecColliderMesh.emplace(L"CollMesh", pMesh);
			}
			else
			{
				return E_FAIL;
			}
		}
	fclose(fpMesh);

	return S_OK;
}

HRESULT CBossStage::Ready_LoadNaviMeshFile(void)
{
	Engine::NAVIMESHSAVESTRUCT LoadNaviData;
	ZeroMemory(&LoadNaviData, sizeof(Engine::NAVIMESHSAVESTRUCT));
	FILE *fpNaviMesh;
	FILE *fpNaviMeshBrick;

	fopen_s(&fpNaviMesh, "../Bin/Resource/ObjectSave/BossNaviData.NM", "rb");
	//네비메쉬 카운트
	_uint NaviRepeatCount;
	Engine::CComponent* pNaviMeshCom = nullptr;
	fseek(fpNaviMesh, -(_int(sizeof(_uint))), SEEK_END);
	fread(&NaviRepeatCount, sizeof(_uint), 1, fpNaviMesh);
	fseek(fpNaviMesh, 0, SEEK_SET);
	//네비 메쉬 로드
	Engine::CNaviMesh* pNaviMesh = nullptr;
	pNaviMesh = dynamic_cast<Engine::CNaviMesh*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Navi", Engine::ID_STATIC));
	for (Engine::_uint repeat = 0; repeat < NaviRepeatCount + 1; repeat++)
	{
		fread(&LoadNaviData.CellPointOne, sizeof(Engine::_vec3), 1, fpNaviMesh);
		fread(&LoadNaviData.CellPointTwo, sizeof(Engine::_vec3), 1, fpNaviMesh);
		fread(&LoadNaviData.CellPointThree, sizeof(Engine::_vec3), 1, fpNaviMesh);
		fread(&LoadNaviData.SaveNaviMeshIndex, sizeof(Engine::_uint), 1, fpNaviMesh);
		pNaviMesh->SetAddCell(&LoadNaviData.CellPointOne, &LoadNaviData.CellPointTwo, &LoadNaviData.CellPointThree);
	}
	fclose(fpNaviMesh);

	//fopen_s(&fpNaviMeshBrick, "../Bin/Resource/ObjectSave/NaviDataBrick.NM", "rb");
	//fseek(fpNaviMeshBrick, -(_int(sizeof(_uint))), SEEK_END);
	//fread(&NaviRepeatCount, sizeof(_uint), 1, fpNaviMeshBrick);
	//fseek(fpNaviMeshBrick, 0, SEEK_SET);
	//
	//for (Engine::_uint repeat = 0; repeat < NaviRepeatCount + 1; repeat++)
	//{
	//	fread(&LoadNaviData.CellPointOne, sizeof(Engine::_vec3), 1, fpNaviMesh);
	//	fread(&LoadNaviData.CellPointTwo, sizeof(Engine::_vec3), 1, fpNaviMesh);
	//	fread(&LoadNaviData.CellPointThree, sizeof(Engine::_vec3), 1, fpNaviMesh);
	//	fread(&LoadNaviData.SaveNaviMeshIndex, sizeof(Engine::_uint), 1, fpNaviMesh);
	//	pNaviMesh->SetAddCell(&LoadNaviData.CellPointOne, &LoadNaviData.CellPointTwo, &LoadNaviData.CellPointThree);
	//}
	//fclose(fpNaviMeshBrick);
	pNaviMesh->Set_NaviIndex(0);
	return S_OK;
}

CBossStage* CBossStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossStage*	pInstance = new CBossStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CBossStage::Free(void)
{
	Engine::CScene::Free();
}

