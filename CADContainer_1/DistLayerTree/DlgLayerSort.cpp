// DlgLayerSort.cpp : 实现文件
//

#include "stdafx.h"
//ARX 2005
//#include "sorttab.h"
#include "DlgLayerSort.h"

const int kNameLength = 260;

//ARX 2005
//bool getOrCreateSortEntsDictionary(AcDbSortentsTable *&pSortTab)
//{
//	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
//	if(pDb == NULL) return false;
//
//	AcDbBlockTable *pBT	= NULL;
//	AcDbBlockTableRecord *pBTR	= NULL;
//	Acad::ErrorStatus es;
//
//	//获取块表指针
//	if (Acad::eOk != pDb->getBlockTable(pBT,AcDb::kForRead))
//	{
//		acutPrintf("\nCouldn't get the block table"); return false;
//	}
//
//	acDocManager->lockDocument(acDocManager->document(pDb));
//	//获取模型空间表记录指针
//	if (Acad::eOk != pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForRead))
//	{
//		acutPrintf("\nCouldn't get the MODEL_SPACE block record opened");
//		pBT->close();
//		return false;
//
//	}
//	pBT->close();
//
//	//获取图层排序表指针
//	if (Acad::eOk !=pBTR->getSortentsTable(pSortTab, AcDb::kForWrite, true))
//	{
//		acutPrintf("\nCouldn't get or create AcDbSortEntsTable of Model Space");
//		pBTR->close();
//		return false;
//	}
//	pBTR->close();
//
//	return true;
//}


bool getAllEntitiesOnLayer(char* layerName, AcDbObjectIdArray &EntObjIdArray)
{
	//设置过虑器
	struct resbuf filter; 
	char strLyNameBuf[260]={0}; 
	filter.restype = 8;
	strcpy(strLyNameBuf, layerName); 
	filter.resval.rstring = strLyNameBuf; 
	filter.rbnext = NULL;

	//查询指定图层所有实体
	ads_name ssName;
	if (RTNORM != acedSSGet("X",NULL,NULL,&filter,ssName)) 
		return false;

	long nNum=0;
	if(RTNORM != acedSSLength(ssName,&nNum))
	{
		acedSSFree(ssName); return false;
	}
	//acutPrintf("\n%s=%d",layerName,nNum);
	EntObjIdArray.setPhysicalLength(0);
	for(int i = 0; i < nNum; i++ ) 
	{
		ads_name eName;
		if(RTNORM != acedSSName(ssName, i, eName)) continue;
		AcDbObjectId tempId;
		if(Acad::eOk == acdbGetObjectId(tempId, eName))
			EntObjIdArray.append(tempId);
	}
	acedSSFree(ssName);

	return true;
}



bool getAllLayers(AcDbObjectIdArray &LyObjIdArray)
{	
	LyObjIdArray.setPhysicalLength(0);
	AcDbLayerTable *pLayerTable = NULL ;    
	Acad::ErrorStatus es ;   

	if (Acad::eOk !=  acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead))
	{       
		acutPrintf ("\nCouldn''t get the layer table"); return false;    
	}   

	AcDbLayerTableIterator *pLayerTableIterator = NULL ;   
	if (Acad::eOk != pLayerTable->newIterator (pLayerTableIterator))
	{       
		acutPrintf ("\nCouldn''t get a new layer table iterator") ;        
		pLayerTable->close () ;       
		return false ;    
	}    
	pLayerTable->close () ;  



	for (pLayerTableIterator->start();!pLayerTableIterator->done ();pLayerTableIterator->step()) 
	{     
		AcDbObjectId layerId ; 
		if (Acad::eOk == pLayerTableIterator->getRecordId(layerId))
		{
			LyObjIdArray.append(layerId);
		}
	}
	delete pLayerTableIterator;  pLayerTableIterator = NULL;

	return true;
}




//ARX 2005
// CDlgLayerSort 对话框

//IMPLEMENT_DYNAMIC(CDlgLayerSort, CDialog)
//CDlgLayerSort::CDlgLayerSort(CWnd* pParent /*=NULL*/)
//	: CDialog(CDlgLayerSort::IDD, pParent)
//{
//}

/*
CDlgLayerSort::~CDlgLayerSort()
{
}

void CDlgLayerSort::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LY, m_listly);
}


BEGIN_MESSAGE_MAP(CDlgLayerSort, CDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_OK, OnBnClickedBtnOk)
END_MESSAGE_MAP()


// CDlgLayerSort 消息处理程序

BOOL CDlgLayerSort::OnInitDialog( )
{
	m_listly.SubclassDlgItem(IDC_LIST_LY, this);
	//AcDbObjectIdArray arrAllLayers;
	//if(getAllLayers(arrAllLayers)) 
	//{
		//if(DisplayLayerOrder(arrAllLayers, NULL))
			//return TRUE;
	//}
	DisplayEntorder();

	return FALSE;
}


BOOL CDlgLayerSort::DisplayLayerOrder(AcDbObjectIdArray &LyIdArray, CUIntArray* pIRepeatArray)
{
	m_listly.ResetContent();

	if (pIRepeatArray == NULL)  //读当前图层显示次序
	{	
		for (int i = 0; i < LyIdArray.length(); i++)
		{
			//打开图层记录,读取其名称
			AcDbObject* pLayer=NULL;
			if (Acad::eOk == acdbOpenObject(pLayer, LyIdArray.at(i), AcDb::kForRead))
			{
				const char* name=NULL;
				AcDbLayerTableRecord::cast(pLayer)->getName(name);
				char newName[260]={0};
				strcpy(newName, " ");
				strcat(newName, name);
				strcat(newName, " ");
				//m_listly.AddString(newName);	
				m_listly.InsertString(0,newName);
				pLayer->close();
			}
		}
		m_listly.AddString("");//最后添加一个空行

		return TRUE;
	}
	else 
	{
		for (int i = 0; i < LyIdArray.length(); i++)
		{
			AcDbObject* pLayer = NULL;
			if (Acad::eOk == acdbOpenObject(pLayer, LyIdArray.at(i), AcDb::kForRead))
			{
				char* name = NULL;
				AcDbLayerTableRecord::cast(pLayer)->getName(name);
				if (pIRepeatArray->GetAt(i) > 0)//有多个显示次序时,用*号来表示
				{
					char newName[260]={0};
					strcpy(newName, "*");
					strcat(newName, name);
					strcat(newName, "*");
					//m_listly.AddString(newName);
					m_listly.InsertString(0,newName);
				}else 
				{
					char newName[260]={0};
					strcpy(newName, " ");
					strcat(newName, name);
					strcat(newName, " ");
					//m_listly.AddString(newName);
					m_listly.InsertString(0,newName);
				}
				pLayer->close();
			}
		}
		m_listly.AddString("");

		return TRUE;
	}
}



bool CDlgLayerSort::RefreshList(void)
{
	CStringArray arrItems;
	for(int i=0;i<m_listly.GetCount();i++)
	{
		CString sItem;
		m_listly.GetText(i,sItem);
		if(sItem[0]=='*')
			sItem.Replace('*',' ');
		arrItems.Add(sItem);
	}
	m_listly.ResetContent();

	for(int i=0;i<arrItems.GetCount();i++)
	{
		m_listly.AddString(arrItems.GetAt(i));
	}

	return true;
}


void CDlgLayerSort::ApplyLayerOrder(void)
{
	Acad::ErrorStatus es ;
	AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase();
	if(pDb == NULL) return;

	AcDbObjectIdArray EntObjIdArray;
	EntObjIdArray.setPhysicalLength(0);

	for (int i =m_listly.GetCount()-2;i>=0; i--)
	{
		AcDbObjectIdArray EntOnLayerObjIdArray;
		EntOnLayerObjIdArray.setPhysicalLength(0);
		CString strLyName;
		m_listly.GetText(i, strLyName);
		strLyName.Replace('*',' ');
		strLyName.Trim();
		
		if(strLyName.CompareNoCase("0")==0) continue;
		if(getAllEntitiesOnLayer(strLyName.GetBuffer(0), EntOnLayerObjIdArray))
			EntObjIdArray.append(EntOnLayerObjIdArray);
		else
			acutPrintf("\n Read Ly[%s] failed!",strLyName);
		strLyName.ReleaseBuffer();
	}


	AcDbSortentsTable *pSortTab = NULL;
	if(getOrCreateSortEntsDictionary(pSortTab))
	{
		es = pSortTab->setRelativeDrawOrder(EntObjIdArray);
	}	
	pSortTab->close();
	pSortTab = NULL;

	//Regen to update the screen.
	ads_regen();

}


void CDlgLayerSort::OnBnClickedBtnApply()
{
	ApplyLayerOrder();

	RefreshList();
}

void CDlgLayerSort::OnBnClickedBtnOk()
{
	ApplyLayerOrder();
	RefreshList();

	OnOK();
}


void CDlgLayerSort::DisplayEntorder()
{
	Acad::ErrorStatus es ; 
	CUIntArray IRepeatArray;  //记录每一图层的实体个数(如果对应图层中没有实体,对应的值就为零)
	AcDbObjectIdArray EntObjIdArray;
	AcDbObjectIdArray lyObjIdArray;
	AcDbSortentsTable *pSortTab = NULL;

	//获取次序排列表指针
	if(getOrCreateSortEntsDictionary(pSortTab))  
	{
		es = pSortTab->getFullDrawOrder(EntObjIdArray);
		pSortTab->close(); pSortTab = NULL;
		if(es == Acad::eOk)
		{
			if(!EntObjIdArray.isEmpty())
			{		
				for(int i = 0; i < EntObjIdArray.length(); i++) 
				{
					AcDbEntity* pEnt=NULL;
					if(Acad::eOk != acdbOpenObject(pEnt,EntObjIdArray.at(i),AcDb::kForRead)) continue;
					if(lyObjIdArray.isEmpty())//第一次,直接插入
					{
						lyObjIdArray.append(pEnt->layerId());
						IRepeatArray.Add(0);
					}
					else 
					{
						//同一图层实体
						if( pEnt->layerId() == lyObjIdArray.last()) //连续同层实体,次序一致( 值 0 )
						{
							pEnt->close(); continue;
						}

						int index = 0;
						if(lyObjIdArray.find( pEnt->layerId(), index))
						{
							IRepeatArray.SetAt(index, 1);	//同一图层中的实体有不同的显示次序( 值 1 )
						}else 
						{
							lyObjIdArray.append(pEnt->layerId()); IRepeatArray.Add(0);
						}
					}
					pEnt->close();	
				}
			}
		}
		DisplayLayerOrder(lyObjIdArray, &IRepeatArray);
	}
}
*/
