#ifndef Boss_h__
#define Boss_h__

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
class CBoss : public Engine::CGameObject
{
private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss(void);

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
	_float						m_fSpeed = 1.5f;
	Engine::BOSSMOVESTATE		m_eBossMoveState = Engine::BMS_END;
	Engine::BOSSPATTERN			m_eBossPattern = Engine::BP_ONE;
	_bool						m_bIntroEvent = false;
	_bool						m_bIntro = true;
	_bool						m_bIntroRotate = false;
	bool						m_bHeightCollider = false;
	_bool						m_bDamaged = false;
	_bool						m_bFire = false;
	_bool						m_bOneAttack = false;

	_bool						m_bColliderHigher = false;
	_bool						m_bColliderFrontJump = false;
	_bool						m_bOnRuinBox = false;
	_bool						m_bNextStage = false;
	_float						m_fIntroTime = 0;
	_float						m_fAddHeight = 0;
	std::vector<std::vector<char>> m_vColliderName;
	map<char*, Engine::CCollider*> m_mapCollider;
	_uint							m_iColliderCount = 0;
	_matrix							m_FrameMatrix;
	_bool							m_bResearchCell = false;
	_bool							m_bHanging = false;
	_bool							m_bTurnOn = false;
	_bool							m_bTargetAniMoveOn = false;
	_bool							m_bAttackOn = false;
	_float							m_fPatternDelay = 0;
	_bool							m_bPatterning = false;
	_bool							m_bDead = false;
public:
	static CBoss*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void			Set_ColliderFrame(char* FrameName);
	bool			SphereCollision_ToObject(CGameObject* GameObject);
	bool			BoxCollision_ToObject(CGameObject* GameObject, const _tchar* ObjectKinds);
	bool			CellResearch();
	void			PatternCall(const _float& fTimeDelta);
private:
	virtual void Free(void) override;

};

END
#endif // Boss_h__
