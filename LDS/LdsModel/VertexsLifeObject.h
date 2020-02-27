#ifndef _VERTEXS_LIFE_OBJECT_H
#define _VERTEXS_LIFE_OBJECT_H
#include "lds_part.h"
class CVertexsLifeObject
{
private:
	bool m_TerminalBackup;
	ATOM_LIST<PROFILE_VERTEX>  m_vertexList;
	ATOM_LIST<PROFILE_VERTEX>* m_pSrcVertexList;
public:
	CVertexsLifeObject(){m_pSrcVertexList=NULL;m_TerminalBackup=false;}
	CVertexsLifeObject(ATOM_LIST<PROFILE_VERTEX>& vertex_list){BackupVertexList(vertex_list);}
	~CVertexsLifeObject();
	void BackupVertexList(ATOM_LIST<PROFILE_VERTEX>& vertex_list);
	void DisableBackup(){m_TerminalBackup=false;}
	BOOL RestoreVertexList(ATOM_LIST<PROFILE_VERTEX>& vertex_list);
};
#endif
