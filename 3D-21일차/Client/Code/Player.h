#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CNaviMesh;
class CShader;

END

BEGIN(Client)
class CPlayer : public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	virtual void Load_Object(_vec3 Position) override;
private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		SetUp_OnTerrain(void);
	void		Key_Input(const _float& fTimeDelta);
	_vec3		PickUp_OnTerrain(void);
	Engine::PLAYERMOVESTATE		m_ePlayerMoveState;
public:
	Engine::CDynamicMesh*		m_pMeshCom = nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom = nullptr; 
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	_vec3						m_vDir;
	_float						m_fSpeed = 2.f;

	bool						m_bHeightCollider = false;
	Engine::PLAYERHEIGHTCOLLISIONSTATE	m_ePlayerHeightCollisionState = Engine::PHCS_END;
	Engine::PLAYERWALKDIRECTION			m_ePlayerWalkDirection = Engine::PWD_END;
	_bool						m_bColliderHigher = false;

	std::vector<std::vector<char>> m_vColliderName;
	map<char*, Engine::CCollider*> m_mapCollider;
	_uint							m_iColliderCount = 0;
	_matrix							m_FrameMatrix;
	_bool							m_bResearchCell = false;
	_bool							m_bHanging = false;
	_bool							m_bTurnOn = false;
public:
	static CPlayer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void			Set_ColliderFrame(char* FrameName);
	bool			SphereCollision_ToObject(CGameObject* GameObject);
	bool			BoxCollision_ToObject(CGameObject* GameObject);
	bool			CellResearch();
private:
	virtual void Free(void) override;

};

END
#endif // Player_h__
