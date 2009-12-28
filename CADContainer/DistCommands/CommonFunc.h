#ifndef  _COMMONFUNC_H_
#define  _COMMONFUNC_H_

#include "StdAfx.h"
#include "..\SdeData\DistExchangeInterface.h"

class LockDocument
{
public:
	LockDocument()
	{
		pDoc=NULL;
		if(acDocManager!=NULL)
		{
			try
			{
				pDoc = acDocManager->curDocument();
				if(acDocManager->lockDocument(pDoc)!=Acad::eOk)
				{
					pDoc=NULL;
				}
			}
			catch(...)
			{}
		}
	}

	~LockDocument()
	{
		if(pDoc&&acDocManager)
		{
			try
			{
				acDocManager->unlockDocument(pDoc);	
			}
			catch(...)
			{}
		}
	}

private:
	AcApDocument* pDoc;
};

void OutputDebugStringX(LPCTSTR fmt, ...);

void GetArxPath(LPTSTR lpPath, long nSize);

void _activeCurrentCadView();

BOOL getViewExtent(AcGePoint3d &leftBottom , AcGePoint3d &rightTop);

void fZoomExtents();

void decodeCString(CString source, CStringArray& dest, char division);

BOOL InitEnvironment(LPCTSTR lpPath);

// Modify
//BOOL GetFormItemText();

//void SendDataToDapForm(IDistConnection * pConnection);

void SendCommandToAutoCAD(CString strCommand);

void SendCopyDataMessage(HWND hwnd, CString& strCommand, CStringArray& strParamArray);

void ShowCommandLineWindow(BOOL bShow /*= TRUE*/);

long CopyRecordsToHistory(IDistConnection* pConnection, CUIntArray& rowIds, LPCTSTR lpSrcLayer, LPCTSTR lpDestlayer, LPCTSTR lpUserName, int nEditRowId);

long ReadSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, LPCTSTR lpSdeLayer, DIST_STRUCT_SELECTWAY* pFilter, LPCTSTR lpWhere, BOOL bReadOnly, BOOL bDeleteExistEntity = TRUE);

long SaveSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
				  DIST_STRUCT_SELECTWAY* pFilter = NULL,
				  bool bAfterSaveToDelete = true);

long SaveSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
					  AcDbObjectIdArray& objIds,
					  bool bAfterSaveToDelete = true);

long SaveSdeLayerInsert(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
						AcDbObjectIdArray& objIds,
						bool bAfterSaveToDelete = true);

long SaveSdeLayerModify(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
						AcDbObjectIdArray& objIds,
						bool bAfterSaveToDelete = true);

long SaveSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
				  AcDbObjectIdArray& objIds, CString strXMID,
				  bool bAfterSaveToDelete = true,
				  bool bInsertOnly = false);

long DeleSdeData(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
				  AcDbObjectIdArray& objIds,
				  bool bAfterSaveToDelete = true);

void DeleteAllEntityInBlock(AcDbBlockTableRecord* pBlkTblRec);

// ��ʵ����ӵ����ݿ���
AcDbObjectId AddEntToDWGDatabase(AcDbEntity *pEnt);

// ��Ҫ���� : ��ȡ��ǰͼ��
BOOL GetCurrentLayer(CString& strCADLayer);

// ���ǰ��ͼ
void activeCurrentCadView();

// ��Ҫ���� : ����ĳһ������¼��ÿһ���������ɫΪͳһ����ɫ
void SetEntityColorInBlock(AcDbBlockTableRecord* pBlkTblRec, int nColor);

BOOL CheckTheDWGFileHasInsert(CString strDWGFileName);

// ��Ҫ���� : �鿴�Ƿ��Ѵ���ͬ���Ŀ�
BOOL CheckTheBlockInCurrentDB(LPCTSTR szBlockName, AcDbObjectId& BlkRecId);

// ��Ҫ���� : �������ݿ���ÿһ���������ɫΪͳһ����ɫ
BOOL SetEntityColorInDb(AcDbDatabase *pDb, int nColor);

// ����ͼ���ļ���������Ϊ�������뵱ǰͼ��
AcDbObjectId FindDWGAndInsertAsBlock(CString strFilePath,
									 CString strFileName,
									 AcGePoint3d geInsertPt,
									 AcGeScale3d geScaleFactors,
									 double dRotation,
									 int nColor,
									 CString strRefLayerName = "");

BOOL SearchLinetype(AcDbDatabase *pDbDatab,char cLtName[],AcDbObjectId &DbOIdLtId);

BOOL AddLinetype(char cLtName[], AcDbObjectId &DbOIdLtId, AcDbDatabase * pDbDatab = acdbHostApplicationServices()->workingDatabase());

AcDbObjectId CreateLayer(char *cLyrName, 
						 Adesk::UInt16 Aun = 7,
						 AcDb::LineWeight ALW = AcDb::kLnWtByLwDefault,
						 char *cLtName = "Continuous",
						 AcDbDatabase * pDbDatab = acdbHostApplicationServices()->workingDatabase());

BOOL ChangeEntityLayer(AcDbObjectId objId, char * pLayer);

/*****************************************************
��Ҫ���� : ����ɾ��ͼ�����뵱ǰͼ��
������� : strFilePath[I]----ͼ���ļ���ŵ�·��
strFileName[I]----ͼ���ļ������ƣ���.tif��׺
geInsertPt[I]----�����
geScaleFactors[I]----�������
dRotation[I]----��ת�Ƕ�
nColor[I]----ʹ��ͬһ��ɫ,���������ԭ����ɫ
�� �� ֵ : ��/true-�ɹ�;false-ʧ��/TRUE-�ɹ�;FALSE-ʧ��
��    �� : 2007-1-10,Shanghai.
��    �� : �⾧<wujing@dist.com.cn>
�޸���־ : 
****************************************************/
AcDbObjectId FindTifAndInsertToCurrent(CString strFilePath,
									   CString strFileName,
									   AcGePoint3d geInsertPt,
									   AcGeScale3d geScaleFactors,
									   double dRotation,
									   int nColor,
									   AcGePoint2dArray ShpCornerPts);

void setView(AcGePoint3d Pt1, AcGePoint3d Pt2, double ex_ratio);

// ���沢ͨ����CADContainer���巢��Ϣʵ���ϴ��ļ�
void SaveCurDWGtoFtp();

// ���ܣ�������Χ�߽߱�(Ҫ��պ�)
// ������strLyName     ͼ������
//       strLineName   ��Χ������
// ���أ���
BOOL gCmdCreateBound(CString strCadLyName, CString strLineName, AcDbObjectId& objId);

// ���ܣ�����������
// ������strLyName     ͼ������
//       strLineName   ��Χ������
// ���أ���
BOOL gCmdCreatePolyLine(CString strCadLyName, CString strLineName, AcDbObjectIdArray& objIds, BOOL bFilter = TRUE, BOOL bMustClose = FALSE);

// ������Ŀ��Χ��
BOOL SaveXMFWX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectId& objId);

// ������Ŀ����
BOOL SaveXMHX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectIdArray& objIds);

//�ж��Ƿ����ཻ
BOOL CheckPolylineSelfCross(AcDbPolyline * pPline);

#endif // _COMMONFUNC_H_