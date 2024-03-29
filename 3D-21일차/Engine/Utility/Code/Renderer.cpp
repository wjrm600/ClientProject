#include "Export_Utility.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

Engine::CRenderer::CRenderer(void)
	: m_pVB(nullptr)
	, m_pIB(nullptr)
{

}

Engine::CRenderer::~CRenderer(void)
{
	Free();
}

void Engine::CRenderer::Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject)
{
	if (RENDER_END <= eGroup)
		return;

	m_RenderGroup[eGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);
}

void Engine::CRenderer::Render_GameObject(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	Render_Priority(pGraphicDev);

	Render_Deferred(pGraphicDev);
	Render_LightAcc(pGraphicDev);

	Render_Blend(pGraphicDev);

	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	//Render_DebugBuffer(L"MRT_Deferred");
	//Render_DebugBuffer(L"MRT_LightAcc");

	Clear_RenderGroup();
}

void Engine::CRenderer::Clear_RenderGroup(void)
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

HRESULT Engine::CRenderer::Ready_Renderer(LPDIRECT3DDEVICE9& pGraphicDev)
{
	FAILED_CHECK_RETURN(pGraphicDev->CreateVertexBuffer(sizeof(VTXSCREEN) * 4,
		0, // 정적버퍼로 할당하겠다는 옵션
		FVF_SCREEN,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL),
		E_FAIL);

	FAILED_CHECK_RETURN(pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2,
		0, // 정적버퍼로 할당하겠다는 옵션
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL),
		E_FAIL);

	D3DVIEWPORT9		ViewPort;
	pGraphicDev->GetViewport(&ViewPort);

	VTXSCREEN*		pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, NULL);

	// 오른쪽 위
	pVertex[0].vPos = _vec4(0.f - 0.5f, 0.f - 0.5f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPos = _vec4(ViewPort.Width - 0.5f, 0.f - 0.5f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPos = _vec4(ViewPort.Width - 0.5f, ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPos = _vec4(0.f - 0.5f, ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	INDEX16*		pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (auto& iter : m_RenderGroup[RENDER_PRIORITY])
		iter->Render_Object();

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_NONALPHA])
		iter->Render_Object();
}

_bool		Compare_ViewZ(CGameObject* pDest, CGameObject* pSrc)
{
	return pDest->Get_ViewZ() > pSrc->Get_ViewZ();
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_RenderGroup[RENDER_ALPHA].sort(Compare_ViewZ);

	for (auto& iter : m_RenderGroup[RENDER_ALPHA])
		iter->Render_Object();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	for (auto& iter : m_RenderGroup[RENDER_UI])
		iter->Render_Object();
}

void CRenderer::Render_Deferred(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	Begin_MRT(L"MRT_Deferred");

	Render_NonAlpha(pGraphicDev);

	End_MRT(L"MRT_Deferred");
}

void Engine::CRenderer::Render_LightAcc(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	Begin_MRT(L"MRT_LightAcc");

	CShader*	pShader = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Shade"));
	NULL_CHECK(pShader);

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();// 빛 출력을 위한 쉐이더 파일이 필요하다.
	Safe_AddRef(pEffect);

	Engine::Throw_RenderTargetTexture(pEffect, L"Target_Normal", "g_NormalTexture");
	Engine::Throw_RenderTargetTexture(pEffect, L"Target_Depth", "g_DepthTexture");

	pEffect->Begin(NULL, 0);

	Engine::Render_Light(pEffect);
	
	pEffect->End();

	End_MRT(L"MRT_LightAcc");

	Safe_Release(pEffect);
	Safe_Release(pShader);
}

void Engine::CRenderer::Render_Blend(LPDIRECT3DDEVICE9 & pGraphicDev)
{
	CShader*		pShader = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Blend"));
	NULL_CHECK(pShader);

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	Safe_AddRef(pEffect);

	Engine::Throw_RenderTargetTexture(pEffect, L"Target_Albedo", "g_AlbedoTexture");
	Engine::Throw_RenderTargetTexture(pEffect, L"Target_Shade", "g_ShadeTexture");
	Engine::Throw_RenderTargetTexture(pEffect, L"Target_Specular", "g_SpecularTexture");

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREEN));
	pGraphicDev->SetFVF(FVF_SCREEN);
	pGraphicDev->SetIndices(m_pIB);
	pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	Safe_Release(pShader);
}

void Engine::CRenderer::Free(void)
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	Clear_RenderGroup();
}

