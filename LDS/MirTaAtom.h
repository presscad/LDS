#ifndef __MIR_TA_ATOM_H_
#define __MIR_TA_ATOM_H_
void ConvertAttachHandle(CLDSNode* pOriginNode, CLDSNode* pDestNode, MIRMSG mirmsg);
BOOL MirTaAtom(CLDSDbObject* pOriginObj, MIRMSG mirmsg,BOOL bIncAttachInfo=TRUE,char *sError=NULL);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL MirTubeTransectFace(CLDSLineTube *pSrcTube, MIRMSG mirmsg, BOOL bStart, BOOL bEnd);
#endif
//设置(基准构件关联对象)列表中的对象彼此关联
void SetRelativeObj(CLDSDbObject *pObj);
#endif