#include "stdafx.h"
#include "ColliderMesh.h"
#include "Export_Function.h"

CColliderMesh::CColliderMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CColliderMesh::~CColliderMesh(void)
{

}

HRESULT CColliderMesh::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	
	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>(Engine::Clone(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	//Collider 
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, nullptr, NULL, NULL);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	return S_OK;
}

CColliderMesh* CColliderMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CColliderMesh*	pInstance = new CColliderMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CColliderMesh::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT CColliderMesh::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}
Engine::_int CColliderMesh::Update_Object(const Engine::_float& fTimeDelta)
{

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void CColliderMesh::Render_Object(void)
{
	m_pTransformCom->Set_Transform(m_pGraphicDev);

	if (m_bHeightCollider)
	{
		m_pColliderCom->Render_Collider(Engine::COL_TRUE, &(m_pTransformCom->m_matWorld));
	}
	else
	{
		m_pColliderCom->Render_Collider(Engine::COL_FALSE, &(m_pTransformCom->m_matWorld));
	}
}

void CColliderMesh::Load_Object(_vec3 Position)
{
	m_vLoadPosition = Position;
	m_pTransformCom->Set_Pos(&m_vLoadPosition);
}
