#ifndef Scene_h__
#define Scene_h__

#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene();

public:
	CComponent*	   Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);

public:
	virtual HRESULT		Ready_Scene(void);
	virtual _int		Update_Scene(const _float& fTimeDelta);
	virtual void		Render_Scene(void);

protected:
	LPDIRECT3DDEVICE9					m_pGraphicDev;
	map<const _tchar*, CLayer*>			m_mapLayer;

public:
	virtual void		Free(void);
};

END
#endif // Scene_h__
