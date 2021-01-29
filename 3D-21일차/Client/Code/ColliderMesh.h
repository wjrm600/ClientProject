#ifndef ColliderMesh_h__
#define ColliderMesh_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CRenderer;
class CCalculator;
class CCollider;

END

BEGIN(Client)
class CColliderMesh : public Engine::CGameObject
{
private:
	explicit CColliderMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CColliderMesh(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual Engine::_int Update_Object(const Engine::_float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	virtual void Load_Object(_vec3 Position) override;
private:
	HRESULT		Add_Component(void);

public:
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;

	bool						m_bHeightCollider = false;
	Engine::COLLIDERHEIGHT		m_eColliderHeight = Engine::CH_END;
public:
	static CColliderMesh*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void) override;

};

END
#endif