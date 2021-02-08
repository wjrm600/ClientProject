#ifndef BossStage_h__
#define BossStage_h__

#include "Scene.h"
#include "Define.h"

#include "SkyBox.h"
#include "Terrain.h"
#include "Player.h"
#include "BossStagePlayer.h"
#include "DynamicCamera.h"
#include "StaticCamera.h"
#include "Monster.h"
#include "Boss.h"

#include "Stone.h"
#include "Intro010.h"
#include "RuinBox.h"
#include "ColliderMesh.h"
#include "Sword.h"
#include "Tree.h"
#include "Effect.h"
#include "UI.h"
#include "SoundMgr.h"

BEGIN(Client)

class CBossStage : public Engine::CScene
{
private:
	explicit CBossStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossStage(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
		
private:
	HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT		Ready_LightInfo(void);

	multimap<const _tchar*, Engine::CGameObject*> m_vecColliderMesh;
	_vec3		m_vEye;
	_vec3		m_vAt;
	_vec3		m_vUp;
	_bool		m_bStageFirstCall = false;
	_bool		m_bMeetBoss = false;
public:
	HRESULT		Ready_LoadObjectFile(void);
	HRESULT		Ready_LoadNaviMeshFile(void);

	Engine::CLayer*			pGameLogicLayer;
	Engine::CLayer*			pEnvironmentLayer;
	CStaticCamera*		pStaticCamera;
public:
	static CBossStage*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
};

END
#endif // Stage_h__
