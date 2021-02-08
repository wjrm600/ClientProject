#include "stdafx.h"
#include "StaticCamera.h"
#include "Export_Function.h"

CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)	
{

}

CStaticCamera::~CStaticCamera(void)
{

}

HRESULT Client::CStaticCamera::Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;
	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(Engine::CCamera::Ready_Object(), E_FAIL);
	
	return S_OK;
}

Client::CStaticCamera* Client::CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, 
	const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CStaticCamera*	pInstance = new CStaticCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		Client::Safe_Release(pInstance);

	return pInstance;
}

HRESULT CStaticCamera::CameraPositionUpdate(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	FAILED_CHECK_RETURN(Engine::CCamera::Ready_Object(), E_FAIL);
}

void Client::CStaticCamera::Key_Input(const _float& fTimeDelta)
{
	//_matrix			matCamWorld;
	//D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);
	//
	//if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	//{
	//	_vec3		vLook;
	//	memcpy(vLook, &matCamWorld.m[2][0], sizeof(_vec3));
	//
	//	_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;
	//
	//	m_vEye += vLength;
	//	m_vAt += vLength;
	//}
	//
	//if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	//{
	//	_vec3		vLook;
	//	memcpy(vLook, &matCamWorld.m[2][0], sizeof(_vec3));
	//
	//	_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;
	//
	//	m_vEye -= vLength;
	//	m_vAt -= vLength;
	//}
	//
	//if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	//{
	//	_vec3		vRight;
	//	memcpy(vRight, &matCamWorld.m[0][0], sizeof(_vec3));
	//
	//	_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;
	//
	//	m_vEye -= vLength;
	//	m_vAt -= vLength;
	//}
	//
	//if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	//{
	//	_vec3		vRight;
	//	memcpy(vRight, &matCamWorld.m[0][0], sizeof(_vec3));
	//
	//	_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;
	//
	//	m_vEye += vLength;
	//	m_vAt += vLength;
	//}
	//
	if (Engine::Get_DIKeyState(DIK_LCONTROL) & 0x80)
	{
		if (true == m_bClick)
			return;

		m_bClick = true;

		if (true == m_bFix)
			m_bFix = false;

		else
			m_bFix = true;
	}
	else
		m_bClick = false;

	if (false == m_bFix)
		return;
}

void Client::CStaticCamera::Mouse_Move(void)
{
	_matrix			matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	_long	dwMouseMove = 0;
	// 마우스를 위아래로 움직일 때
	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::DIMS_Y))
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLook = m_vAt - m_vEye;
		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::DIMS_X))
	{
		_vec3		vUp = _vec3(0.f, 1.f, 0.f);
		
		_vec3	vLook = m_vAt - m_vEye;
		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	
}

void Client::CStaticCamera::Mouse_Fix(void)
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

}

void CStaticCamera::Free(void)
{
	Engine::CCamera::Free();
}

Client::_int Client::CStaticCamera::Update_Object(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	if (true == m_bFix)
	{
		//Mouse_Fix();
		//Mouse_Move();
	}
	
	_int iExit = Engine::CCamera::Update_Object(fTimeDelta);
	
	return iExit;
}
