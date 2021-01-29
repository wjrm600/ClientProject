#include "Collider.h"

USING(Engine)
Engine::CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)	
{
	Safe_AddRef(m_pGraphicDev);
}

Engine::CCollider::CCollider(const CCollider& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_fSphereRadius(rhs.m_fSphereRadius)
	, m_bColliderObject(rhs.m_bColliderObject)
	, m_pBox(rhs.m_pBox)
{

	for (_uint i = 0; i < COL_END; ++i)
	{
		m_pTexture[i] = rhs.m_pTexture[i];
		Safe_AddRef(m_pTexture[i]);
	}
	if (!m_bColliderObject)
	{
		Safe_AddRef(m_pVB);
		Safe_AddRef(m_pIB);
	}
	Safe_AddRef(m_pBox);
	Safe_AddRef(m_pGraphicDev);
}

Engine::CCollider::~CCollider(void)
{

}

HRESULT Engine::CCollider::Ready_Collider(const _vec3* pVtxPos, const _ulong& dwNumVtxCnt, const _ulong& dwStride)
{
	// 인자로 받아들인 메쉬에 맞는 사이즈의 박스를 생성
	D3DXComputeBoundingBox(pVtxPos, dwNumVtxCnt, sizeof(_vec3), &m_vMin, &m_vMax);
	D3DXComputeBoundingSphere(pVtxPos, dwNumVtxCnt, sizeof(_vec3), &m_vSphereCenter, &m_fSphereRadius);

	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXCUBE) * 8,
		0, // 정적버퍼로 할당하겠다는 옵션
		FVF_CUBE,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL),
		E_FAIL);
	
	FAILED_CHECK_RETURN(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 12,
		0, // 정적버퍼로 할당하겠다는 옵션
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL),
		E_FAIL);
	
	VTXCUBE*		pVertex = NULL;
	
	m_pVB->Lock(0, 0, (void**)&pVertex, NULL);
	
	// 전면
	pVertex[0].vPos = _vec3(m_vMin.x, m_vMax.y, m_vMin.z);
	pVertex[0].vTexUV = pVertex[0].vPos;
	
	pVertex[1].vPos = _vec3(m_vMax.x, m_vMax.y, m_vMin.z);
	pVertex[1].vTexUV = pVertex[1].vPos;
	
	pVertex[2].vPos = _vec3(m_vMax.x, m_vMin.y, m_vMin.z);
	pVertex[2].vTexUV = pVertex[2].vPos;
	
	pVertex[3].vPos = _vec3(m_vMin.x, m_vMin.y, m_vMin.z);
	pVertex[3].vTexUV = pVertex[3].vPos;
	
	// 후면
	pVertex[4].vPos = _vec3(m_vMin.x, m_vMax.y, m_vMax.z);
	pVertex[4].vTexUV = pVertex[4].vPos;
	
	pVertex[5].vPos = _vec3(m_vMax.x, m_vMax.y, m_vMax.z);
	pVertex[5].vTexUV = pVertex[5].vPos;
	
	pVertex[6].vPos = _vec3(m_vMax.x, m_vMin.y, m_vMax.z);
	pVertex[6].vTexUV = pVertex[6].vPos;
	
	pVertex[7].vPos = _vec3(m_vMin.x, m_vMin.y, m_vMax.z);
	pVertex[7].vTexUV = pVertex[7].vPos;
	
	m_pVB->Unlock();
	
	INDEX16*		pIndex = nullptr;
	
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	
	// x+
	pIndex[0]._0 = 1;
	pIndex[0]._1 = 5;
	pIndex[0]._2 = 6;
	
	pIndex[1]._0 = 1;
	pIndex[1]._1 = 6;
	pIndex[1]._2 = 2;
	
	// x-
	pIndex[2]._0 = 4;
	pIndex[2]._1 = 0;
	pIndex[2]._2 = 3;
	
	pIndex[3]._0 = 4;
	pIndex[3]._1 = 3;
	pIndex[3]._2 = 7;
	
	// y+
	pIndex[4]._0 = 4;
	pIndex[4]._1 = 5;
	pIndex[4]._2 = 1;
	
	pIndex[5]._0 = 4;
	pIndex[5]._1 = 1;
	pIndex[5]._2 = 0;
	
	// y-
	pIndex[6]._0 = 3;
	pIndex[6]._1 = 2;
	pIndex[6]._2 = 6;
	
	pIndex[7]._0 = 3;
	pIndex[7]._1 = 6;
	pIndex[7]._2 = 7;
	
	// z+
	pIndex[8]._0 = 7;
	pIndex[8]._1 = 6;
	pIndex[8]._2 = 5;
	
	pIndex[9]._0 = 7;
	pIndex[9]._1 = 5;
	pIndex[9]._2 = 4;
	
	// z-
	pIndex[10]._0 = 0;
	pIndex[10]._1 = 1;
	pIndex[10]._2 = 2;
	
	pIndex[11]._0 = 0;
	pIndex[11]._1 = 2;
	pIndex[11]._2 = 3;
	
	m_pIB->Unlock();

	for (_uint i = 0; i < COL_END; ++i)
	{
		m_pGraphicDev->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture[i], NULL);

		D3DLOCKED_RECT		LockRect;
		ZeroMemory(&LockRect, sizeof(D3DLOCKED_RECT));

		m_pTexture[i]->LockRect(0, &LockRect, NULL, 0);

		*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * (1.f - i), 0.f, 1.f);

		m_pTexture[i]->UnlockRect(0);
	}
	
	m_bColliderObject = false;
	m_fSphereRadius = 30;
	D3DXCreateSphere(m_pGraphicDev, m_fSphereRadius, 10, 10, &m_pBox, NULL);

	return S_OK;
}

HRESULT CCollider::Ready_Collider()
{
	for (_uint i = 0; i < COL_END; ++i)
	{
		m_pGraphicDev->CreateTexture(1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture[i], NULL);

		D3DLOCKED_RECT		LockRect;
		ZeroMemory(&LockRect, sizeof(D3DLOCKED_RECT));

		m_pTexture[i]->LockRect(0, &LockRect, NULL, 0);

		*((_ulong*)LockRect.pBits) = D3DXCOLOR(1.f * i, 1.f * (1.f - i), 0.f, 1.f);

		m_pTexture[i]->UnlockRect(0);
	}
	m_bColliderObject = true;
	m_fSphereRadius = 30;
	D3DXCreateSphere(m_pGraphicDev, m_fSphereRadius, 10, 10, &m_pBox, NULL);
	return S_OK;
}

Engine::CCollider* Engine::CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pVtxPos, const _ulong& dwNumVtxCnt, const _ulong& dwStride)
{
	CCollider*	pInstance = new CCollider(pGraphicDev);

	if (pVtxPos == nullptr)
	{
		if (FAILED(pInstance->Ready_Collider()))
			Safe_Release(pInstance);
	}
	else
	{
		if (FAILED(pInstance->Ready_Collider(pVtxPos, dwNumVtxCnt, dwStride)))
			Safe_Release(pInstance);
	}
	return pInstance;
}

void Engine::CCollider::Render_Collider(COLLTYPE eType, const _matrix* pColliderMatrix)
{
	m_matCollMatrix = *pColliderMatrix;

#ifdef _DEBUG
	m_pGraphicDev->SetTransform(D3DTS_WORLD, pColliderMatrix);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphicDev->SetTexture(0, m_pTexture[eType]);

		//m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXCUBE));
		//m_pGraphicDev->SetFVF(FVF_CUBE);
		//m_pGraphicDev->SetIndices(m_pIB);
		//m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		//D3DXCreateSphere(m_pGraphicDev, m_fSphereRadius, 10, 10, &m_pSphere, NULL);

	m_pBox->DrawSubset(0);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif


}

void CCollider::Set_BoxParameter(_float BoxWidth, _float BoxHeight, _float BoxDepth)
{
	m_fBoxWidth = BoxWidth;
	m_fBoxHeight = BoxHeight;
	m_fBoxDepth = BoxDepth;
	Engine::Safe_Release(m_pBox);
	D3DXCreateBox(m_pGraphicDev, m_fBoxWidth, m_fBoxHeight, m_fBoxDepth, &m_pBox, NULL);
}

CComponent * CCollider::Clone(void)
{
	return new CCollider(*this);
}

void Engine::CCollider::Free(void)
{
	_uint a;
	for (_uint i = 0; i < COL_END; ++i)
		a = Safe_Release(m_pTexture[i]);
	if (!m_bColliderObject)
	{
		a = Safe_Release(m_pVB);
		a = Safe_Release(m_pIB);
	}
	a = Safe_Release(m_pBox);
	a = Safe_Release(m_pGraphicDev);
}

