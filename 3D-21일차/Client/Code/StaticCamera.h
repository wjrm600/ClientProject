#ifndef StaticCamera_h__
#define StaticCamera_h__

#include "Camera.h"
#include "Define.h"

BEGIN(Client)
class CStaticCamera : public Engine::CCamera
{
private:
	explicit CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticCamera(void);

public:
	HRESULT Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,	
						 const _float& fFovY, const _float& fAspect, const _float& fNear,	const _float& fFar );

	virtual _int Update_Object(const _float& fTimeDelta) override;

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

private:
	_bool		m_bClick = false;
	_bool		m_bFix = true;

public:
	static CStaticCamera*	Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									const _vec3* pEye, 
									const _vec3* pAt, 
									const _vec3* pUp, 
									const _float& fFovY = D3DXToRadian(60.f),
									const _float& fAspect = _float(WINCX) / WINCY, 
									const _float& fNear = 0.1f, 
									const _float& fFar = 1000.f);
	HRESULT CameraPositionUpdate(const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp);
private:
	virtual void Free(void) override;

};

END
#endif // StaticCamera_h__
