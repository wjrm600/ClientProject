#ifndef NaviMesh_h__
#define NaviMesh_h__

#include "Base.h"
#include "Mesh.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh : public CMesh
{
private:
	explicit CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh(void);

public:
	void		Set_NaviIndex(const _ulong& dwIndex) { m_dwIndex = dwIndex; }
	HRESULT		SetAddCell(_vec3* vertex1, _vec3* vertex2, _vec3* vertex3);
public:
	HRESULT		Ready_NaviMeshes(void);
	void		Render_NaviMeshes(void);

	_vec3			Move_OnNaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir, const _float& fTimeDelta);
	_vec3			Search_OnNaviMesh(const _vec3* pTargetPos);
	vector<CCell*>* GetVecCell() { return &m_vecCell; }
	_ulong					m_dwIndex;
private:
	HRESULT		Link_Cell(void);
private:
	vector<CCell*>			m_vecCell;
	void					PointDistanceOrderSort(_float DistanceA, _float DistanceB, _float DistanceC);
	CCell::POINT			DistanceOrder[3];
public:
	static CNaviMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

};

END
#endif // NaviMesh_h__
