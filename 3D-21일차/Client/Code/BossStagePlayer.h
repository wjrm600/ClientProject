#ifndef BossStagePlayer_h__
#define BossStagePlayer_h__

#include "GameObject.h"
#include "Define.h"
#include "SoundMgr.h"

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
class CBossStagePlayer : public Engine::CGameObject
{
private:
	explicit CBossStagePlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossStagePlayer(void);

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
	Engine::PLAYERMOVESTATE		m_ePlayerMoveState;
	_bool						m_bColliderHigher = false;
	_bool						m_bColliderFrontJump = false;
	_bool						m_bOnRuinBox = false;
	_bool						m_bNextStage = false;

	std::vector<std::vector<char>> m_vColliderName;
	map<char*, Engine::CCollider*> m_mapCollider;
	_uint							m_iColliderCount = 0;
	_matrix							m_FrameMatrix;
	_bool							m_bResearchCell = false;
	_bool							m_bHanging = false;
	_bool							m_bTurnOn = false;
	_uint							m_uiComboStack = 0;
public:
	static CBossStagePlayer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void			Set_ColliderFrame(char* FrameName);
	bool			BossSphereCollision_ToObject(CGameObject* GameObject);
	bool			BoxCollision_ToObject(CGameObject* GameObject, const _tchar* ObjectKinds);
	bool			CellResearch();
private:
	virtual void Free(void) override;

};

END
#endif // BossStagePlayer_h__
