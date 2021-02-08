#include "NaviMesh.h"
USING(Engine)

Engine::CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_dwIndex(0)
{
	
}

Engine::CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CMesh(rhs)
	, m_vecCell(rhs.m_vecCell)
	, m_dwIndex(rhs.m_dwIndex)
{
	for (auto& iter : m_vecCell)
		Safe_AddRef(iter);
}

Engine::CNaviMesh::~CNaviMesh(void)
{

}

HRESULT CNaviMesh::SetAddCell(_vec3 * vertex1, _vec3 * vertex2, _vec3 * vertex3)
{
	CCell*		pCell = nullptr;

	pCell = CCell::Create(m_pGraphicDev, m_vecCell.size(), vertex1, vertex2, vertex3);
	NULL_CHECK_RETURN(pCell, E_FAIL);
	m_vecCell.push_back(pCell);

	//AddSaveNaviMesh(vertex1, vertex2, vertex3);
	FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);
}

HRESULT Engine::CNaviMesh::Ready_NaviMeshes(void)
{

	m_vecCell.reserve(100);

	return S_OK;
}

void CNaviMesh::Render_NaviMeshes(void)
{
	for (auto& iter : m_vecCell)
		iter->Render_Cell();
}

_vec3 CNaviMesh::Move_OnNaviMesh(const _vec3 * pTargetPos, const _vec3 * pTargetDir, const _float& fTimeDelta)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;
	_vec3		GoalVec;
	if (CCell::MOVE == m_vecCell[m_dwIndex]->CompareCell(&vEndPos, &m_dwIndex))
		return vEndPos;

	else if (CCell::STOP == m_vecCell[m_dwIndex]->CompareCell(&vEndPos, &m_dwIndex))
	{
		_bool bCellGet = false;
		for (_ulong i = 0; i < GetVecCell()->size(); ++i)
		{
			bCellGet = (*(GetVecCell()))[i]->Compare_SearchCell(&((*pTargetPos) + ((*pTargetDir)*2.5f)), &(m_dwIndex));
		}
		if (bCellGet)
		{
			return vEndPos;
		}
		else
		{
			_vec3 PointA_Dis = *m_vecCell[m_dwIndex]->Get_Point(Engine::CCell::POINT_A) - *pTargetPos;
			_vec3 PointB_Dis = *m_vecCell[m_dwIndex]->Get_Point(Engine::CCell::POINT_B) - *pTargetPos;
			_vec3 PointC_Dis = *m_vecCell[m_dwIndex]->Get_Point(Engine::CCell::POINT_C) - *pTargetPos;
			_float ADistnaceLength = D3DXVec3Length(&PointA_Dis);
			_float BDistnaceLength = D3DXVec3Length(&PointB_Dis);
			_float CDistnaceLength = D3DXVec3Length(&PointC_Dis);

			PointDistanceOrderSort(ADistnaceLength, BDistnaceLength, CDistnaceLength);

			_vec3 ComparedVec = *m_vecCell[m_dwIndex]->Get_Point(DistanceOrder[0]) - *m_vecCell[m_dwIndex]->Get_Point(DistanceOrder[1]);
			_vec3 DirectionVec = *pTargetDir;

			D3DXVec3Normalize(&ComparedVec, &ComparedVec);
			D3DXVec3Normalize(&DirectionVec, &DirectionVec);

			_float ResultNor = D3DXVec3Dot(&ComparedVec, &DirectionVec);
			if (ResultNor > 0)
			{
				GoalVec = *m_vecCell[m_dwIndex]->Get_Point(DistanceOrder[0]) - *pTargetPos;
				D3DXVec3Normalize(&GoalVec, &GoalVec);
				GoalVec *= fTimeDelta;
				GoalVec += *pTargetPos;
			}
			else if (ResultNor < 0)
			{
				GoalVec = *m_vecCell[m_dwIndex]->Get_Point(DistanceOrder[1]) - *pTargetPos;
				D3DXVec3Normalize(&GoalVec, &GoalVec);
				GoalVec *= fTimeDelta;
				GoalVec += *pTargetPos;
			}
			else
			{
				GoalVec = *pTargetPos;
			}
			return GoalVec;
		}
	}
}

_vec3 CNaviMesh::Search_OnNaviMesh(const _vec3 * pTargetPos)
{
	auto CellIter = m_vecCell.begin();
	for (; CellIter != m_vecCell.end(); CellIter++)
	{
		(*CellIter)->CompareCell(pTargetPos, &m_dwIndex);
	}

	return *pTargetPos;
}

HRESULT Engine::CNaviMesh::Link_Cell(void)
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if(i == j)
				continue;

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]->Get_Point(CCell::POINT_B),	
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
													m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}

		}
	}
	
	return S_OK;
}

void CNaviMesh::PointDistanceOrderSort(_float DistanceA, _float DistanceB, _float DistanceC)
{
	if (DistanceA < DistanceB)
	{
		if (DistanceA < DistanceC)
		{
			DistanceOrder[0] = CCell::POINT_A;
			if (DistanceB < DistanceC)
			{
				DistanceOrder[1] = CCell::POINT_B;
				DistanceOrder[2] = CCell::POINT_C;
			}
			else
			{
				DistanceOrder[1] = CCell::POINT_C;
				DistanceOrder[2] = CCell::POINT_B;
			}
		}
		else
		{
			DistanceOrder[0] = CCell::POINT_C;
			DistanceOrder[1] = CCell::POINT_B;
			DistanceOrder[2] = CCell::POINT_A;
		}
	}
	else
	{
		if (DistanceB < DistanceC)
		{
			DistanceOrder[0] = CCell::POINT_B;
			if (DistanceA < DistanceC)
			{
				DistanceOrder[1] = CCell::POINT_A;
				DistanceOrder[2] = CCell::POINT_C;
			}
			else
			{
				DistanceOrder[1] = CCell::POINT_C;
				DistanceOrder[2] = CCell::POINT_A;
			}
		}
		else
		{
			DistanceOrder[0] = CCell::POINT_C;
			DistanceOrder[1] = CCell::POINT_B;
			DistanceOrder[2] = CCell::POINT_A;

		}
	}
}

Engine::CNaviMesh* Engine::CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviMesh*		pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMeshes()))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CComponent* Engine::CNaviMesh::Clone(void)
{
	return new CNaviMesh(*this);
}

void Engine::CNaviMesh::Free(void)
{

	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();


	CMesh::Free();
}

