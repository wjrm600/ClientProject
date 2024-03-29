#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"
#include "HierarchyLoader.h"
#include "AniCtrl.h"

BEGIN(Engine)

class ENGINE_DLL	CDynamicMesh : public CMesh 
{
private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);

public:
	HRESULT			Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void			Render_Meshes(void);
	void			Render_Meshes(LPD3DXEFFECT& pEffect);
	D3DXFRAME_DERIVED*		Get_FrameByName(const char* pFrameName);
	_bool							Is_AnimationSetEnd(void);
public:
	void			Set_AnimationSet(const _uint& iIndex);
	void			Play_Animation(const _float& fTimeDelta);
	const LPD3DXMESH	Get_Mesh(void);
	D3DXFRAME*		Get_RootFrame() { return m_pRootFrame; }
	const _ulong&		Get_NumVtx(void) { return m_dwNumVtx; }
	const _ulong&		Get_Stride(void) { return m_dwStride; }
	const _vec3*		Get_VtxPos(void) { return m_pVtxPos; }
public:

	// 모든 뼈들을 순회하면서 뼈들이 갖고 있는 TransformationMatrix와 pParentMatrix를 결합하여 CombinedTransformationMatrix를 만들어주는 함수
	void			Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);

	// CombinedTransformationMatrix의 주소값을 얻어와서 채워주는 함수
	void			SetUp_FrameMatrixPointer(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*				m_pRootFrame;
	CHierarchyLoader*		m_pLoader;
	CAniCtrl*				m_pAniCtrl;
	list<D3DXMESHCONTAINER_DERIVED*>			m_MeshContainerList;
	_ulong				m_dwStride;
	_ulong				m_dwNumVtx;
	_vec3*				m_pVtxPos;
public:
	static	CDynamicMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CResources*			Clone(void);
	virtual void				Free(void);
};

END
#endif // DynamicMesh_h__


//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;						// 뼈의 이름을 저장하기 위한 문자열(아스키코드)
//	D3DXMATRIX              TransformationMatrix;		// 뼈의 최초 로드 시점의 상태행렬
//
//	LPD3DXMESHCONTAINER     pMeshContainer;
//
//	struct _D3DXFRAME       *pFrameSibling;				// 형제 뼈의 주소를 저장하는 포인터
//	struct _D3DXFRAME       *pFrameFirstChild;			// 자식 뼈의 주소를 저장하는 포인터
//		
//} D3DXFRAME, *LPD3DXFRAME;
//
//
//typedef struct _D3DXMESHCONTAINER
//{
//	LPSTR                   Name;					// 메쉬 컨테이너의 이름을 저장하기 위한 문자열(아스키코드 : 근데 대부분 없거나 신경쓰지 않는다)
//	D3DXMESHDATA            MeshData;				// 메쉬의 종류를 결정하기 위한 열거체와, 컴객체를 소유하고 있는 구조체
//	LPD3DXMATERIAL          pMaterials;				// 재질 정보를 보관하는 포인터
//	LPD3DXEFFECTINSTANCE    pEffects;				// 메쉬가 지닌 특수한 이펙트(우리는 사용하지 못한다)
//	DWORD                   NumMaterials;			// 재질의 개수
//	DWORD                  *pAdjacency;				// 인접한 폴리곤의 첫 번째 주소값을 보관하는 포인터
//	LPD3DXSKININFO          pSkinInfo;				// 스키닝 애니메이션시 필요한 각종 함수를 제공하는 컴객체
//
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;	// 다음 연결된 메쉬 컨테이너의 주소(사실상 사용하지 않음)
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;