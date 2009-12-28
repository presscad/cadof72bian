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

// 将实体添加到数据库中
AcDbObjectId AddEntToDWGDatabase(AcDbEntity *pEnt);

// 简要描述 : 获取当前图层
BOOL GetCurrentLayer(CString& strCADLayer);

// 激活当前视图
void activeCurrentCadView();

// 简要描述 : 设置某一个块表记录中每一个对象的颜色为统一的颜色
void SetEntityColorInBlock(AcDbBlockTableRecord* pBlkTblRec, int nColor);

BOOL CheckTheDWGFileHasInsert(CString strDWGFileName);

// 简要描述 : 查看是否已存在同名的块
BOOL CheckTheBlockInCurrentDB(LPCTSTR szBlockName, AcDbObjectId& BlkRecId);

// 简要描述 : 设置数据库中每一个对象的颜色为统一的颜色
BOOL SetEntityColorInDb(AcDbDatabase *pDb, int nColor);

// 查找图形文件并将其作为块对象插入当前图面
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
简要描述 : 查找删格图并插入当前图面
输入参数 : strFilePath[I]----图形文件存放的路径
strFileName[I]----图形文件的名称，带.tif后缀
geInsertPt[I]----插入点
geScaleFactors[I]----拉伸比例
dRotation[I]----旋转角度
nColor[I]----使用同一颜色,负数则采用原有颜色
返 回 值 : 无/true-成功;false-失败/TRUE-成功;FALSE-失败
日    期 : 2007-1-10,Shanghai.
作    者 : 吴晶<wujing@dist.com.cn>
修改日志 : 
****************************************************/
AcDbObjectId FindTifAndInsertToCurrent(CString strFilePath,
									   CString strFileName,
									   AcGePoint3d geInsertPt,
									   AcGeScale3d geScaleFactors,
									   double dRotation,
									   int nColor,
									   AcGePoint2dArray ShpCornerPts);

void setView(AcGePoint3d Pt1, AcGePoint3d Pt2, double ex_ratio);

// 保存并通过向CADContainer窗体发消息实现上传文件
void SaveCurDWGtoFtp();

// 功能：创建范围线边界(要求闭合)
// 参数：strLyName     图层名称
//       strLineName   范围线名称
// 返回：无
BOOL gCmdCreateBound(CString strCadLyName, CString strLineName, AcDbObjectId& objId);

// 功能：创建多义线
// 参数：strLyName     图层名称
//       strLineName   范围线名称
// 返回：无
BOOL gCmdCreatePolyLine(CString strCadLyName, CString strLineName, AcDbObjectIdArray& objIds, BOOL bFilter = TRUE, BOOL bMustClose = FALSE);

// 保存项目范围线
BOOL SaveXMFWX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectId& objId);

// 保存项目红线
BOOL SaveXMHX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectIdArray& objIds);

//判断是否自相交
BOOL CheckPolylineSelfCross(AcDbPolyline * pPline);

#endif // _COMMONFUNC_H_