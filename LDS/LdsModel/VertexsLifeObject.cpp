#include "stdafx.h"
#include "VertexsLifeObject.h"
///////////////////////////////////////////////////////////////////////////////////////////
//管理顶点集合类
CVertexsLifeObject::~CVertexsLifeObject()
{
	if(m_TerminalBackup&&m_pSrcVertexList)
		RestoreVertexList(*m_pSrcVertexList);
}
void CVertexsLifeObject::BackupVertexList(ATOM_LIST<PROFILE_VERTEX>& vertex_list)
{
	m_TerminalBackup=true;
	m_pSrcVertexList = &vertex_list;
	m_vertexList.Empty();
	for(PROFILE_VERTEX* vertex=vertex_list.GetFirst();vertex!=NULL;vertex=vertex_list.GetNext())
	{
 		if(NULL==m_vertexList.append(*vertex))
 			return;
	}
}
// 顶点集合的复原函数
BOOL CVertexsLifeObject::RestoreVertexList(ATOM_LIST<PROFILE_VERTEX>& vertex_list)
{
	for(PROFILE_VERTEX* vertex=m_vertexList.GetFirst();vertex!=NULL;vertex=m_vertexList.GetNext())
	{
 		if(NULL==vertex_list.append(*vertex))
 			return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
