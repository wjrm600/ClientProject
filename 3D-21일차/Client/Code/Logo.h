#ifndef Logo_h__
#define Logo_h__

#include "Scene.h"
#include "Define.h"

#include "BackGround.h"
#include "TestPlayer.h"
#include "TestMonster.h"
#include "Loading.h"
#include "SoundMgr.h"

BEGIN(Client)

class CLogo : public Engine::CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
		
private:
	HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT		Ready_Resource(Engine::RESOURCETYPE eType);

private:
	CLoading*	m_pLoading = nullptr;

public:
	static CLogo*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
};

END
#endif // Logo_h__
