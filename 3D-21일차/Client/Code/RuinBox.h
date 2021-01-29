#ifndef RuinBox_h__
#define RuinBox_h__

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
class CRuinBox : public Engine::CGameObject
{
private:
	explicit CRuinBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRuinBox(void);

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
	void		Key_Input(const _float& fTimeDelta);
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
	Engine::PICKING_RAY			m_stPickRuinBox;
	_bool						m_bPickDrag = false;
	_float						m_fMoveGage = 10;
	_vec3						m_vOriPosition;
public:
	_tchar						m_sObjectName[30];
	static CRuinBox*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _tchar* pObjectName);

private:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
