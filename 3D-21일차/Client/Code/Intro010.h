#ifndef Intro010_h__
#define Intro010_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class COptimization;
class CShader;

END

BEGIN(Client)
class CIntro010 : public Engine::CGameObject
{
private:
	explicit CIntro010(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CIntro010(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	virtual void Load_Object(_vec3 Position) override;
private:
	HRESULT		Add_Component(void);
	void		SetUp_OnTerrain(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
private:

	Engine::CStaticMesh*		m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::COptimization*		m_pOptimizationCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	_bool						m_bColl = false;
	_bool						m_bDraw = false;
public:
	_tchar						m_sObjectName[30];
	static CIntro010*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _tchar* pObjectName);

private:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
