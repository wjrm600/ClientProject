#include "stdafx.h"
#include "RuinBox.h"
#include "Export_Function.h"

CRuinBox::CRuinBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_sObjectName(L"Mesh_")
{

}

CRuinBox::~CRuinBox(void)
{

}

HRESULT Client::CRuinBox::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, m_sObjectName));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

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

	// Collider 
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_NumVtx(), m_pMeshCom->Get_Stride());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	// Optimization
	pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>(Engine::Clone(L"Proto_Optimization"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}


CRuinBox* CRuinBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _tchar* pObjectName)
{
	CRuinBox*	pInstance = new CRuinBox(pGraphicDev);

	lstrcatW(pInstance->m_sObjectName,pObjectName);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CRuinBox::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CRuinBox::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	

	m_pTransformCom->Set_Pos(&_vec3(33.2f, 2.5f, 7.9f));
	m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(90.f));
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-10.f));
	m_pTransformCom->Set_Scale(0.009f, 0.009f, 0.009f);
	m_vOriPosition = m_pTransformCom->m_vInfo[Engine::INFO_POS];
	return S_OK;
}
Client::_int Client::CRuinBox::Update_Object(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);

	_vec3	vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	m_bDraw = m_pOptimizationCom->Is_InFrustumForObject(&vPos, 0.f);


	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CRuinBox::Render_Object(void)
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
	
	Engine::Safe_Release(pEffect);

	//m_pTransformCom->Set_Transform(m_pGraphicDev);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//
	//m_pMeshCom->Render_Meshes();
	//
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuinBox::Load_Object(_vec3 Position)
{
	m_vLoadPosition = Position;
	m_pTransformCom->Set_Pos(&m_vLoadPosition);
}

void Client::CRuinBox::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}

HRESULT Client::CRuinBox::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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

void CRuinBox::Key_Input(const _float & fTimeDelta)
{
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)
		{
			Engine::PICKING_RAY PickRuinBox = m_pCalculatorCom->GetPicking_Ray(g_hWnd);
			_matrix RuinBoxInverseMatrix = m_pTransformCom->m_matWorld;
			BOOL	bHitRuin = false;
			D3DXMatrixInverse(&RuinBoxInverseMatrix, 0, &RuinBoxInverseMatrix);
			D3DXVec3TransformCoord(&PickRuinBox.Raypos, &PickRuinBox.Raypos, &RuinBoxInverseMatrix);
			D3DXVec3TransformNormal(&PickRuinBox.Raydir, &PickRuinBox.Raydir, &RuinBoxInverseMatrix);

			D3DXIntersect(m_pMeshCom->m_pMesh, 
				&PickRuinBox.Raypos, 
				&PickRuinBox.Raydir, 
				&bHitRuin, 
				nullptr, 
				nullptr, 
				nullptr, 
				nullptr, 
				nullptr, 
				nullptr);
			if(bHitRuin)
			{
				if (!m_bPickDrag)
				{
					m_stPickRuinBox = m_pCalculatorCom->GetPicking_Ray(g_hWnd);
					m_bPickDrag = true;
				}
			}
			else
			{
				_float MoveDis = m_stPickRuinBox.Raypos.z - m_pCalculatorCom->GetPicking_Ray(g_hWnd).Raypos.z;
				_vec3 MoveRuinUpVec = m_pTransformCom->m_vInfo[Engine::INFO_UP] * fTimeDelta * 300;
				m_pTransformCom->Set_Pos(&(m_pTransformCom->m_vInfo[Engine::INFO_POS] - MoveRuinUpVec));
			}
		}
	}
	else
	{
		m_bPickDrag = false;
	}
}

_bool CRuinBox::Collision_ToObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	Engine::CCollider*	pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerColliderCom, false);
	
	/*return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(),
											pPlayerColliderCom->Get_Max(),
											pPlayerColliderCom->Get_CollMatrix(),
											m_pColliderCom->Get_Min(), 
											m_pColliderCom->Get_Max(),
											m_pColliderCom->Get_CollMatrix());*/

	return m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(),
		pPlayerColliderCom->Get_Max(),
		pPlayerColliderCom->Get_CollMatrix(),
		m_pColliderCom->Get_Min(),
		m_pColliderCom->Get_Max(),
		m_pColliderCom->Get_CollMatrix());
}
