#include "distmap.h"

CDistMap::CDistMap(void)
{
	m_lpCADDatabase = NULL;
}

CDistMap::~CDistMap(void)
{
}

//功能：创建图层
//参数：strLyName   图层名
//      nClrIndex   颜色号
//      strLnType   线型
//      dLineWeigth 线宽
//返回：resultId    返回创建实体的ID 成功返回新建Layer ID ，失败返回NULL
AcDbObjectId CDistMap::CAD_CreateLayer(const char *strLyName, int  nClrIndex,const char *strLnType, AcDb::LineWeight dLineWeigth)										  
{
	AcDbObjectId			resultId;
	AcDbLayerTable			*pLyTable=NULL;
	AcDbLayerTableRecord	*pLyRcd=NULL;
	AcDbObjectId			lnTypeId;
	AcCmColor				TempColor;
	Acad::ErrorStatus		es;

	resultId.setNull();
	lnTypeId.setNull();

	if ((lnTypeId = CAD_GetLineTypeId(strLnType)) == NULL)
	{
		lnTypeId = CAD_LoadLineType(strLnType);
	}

	m_lpCADDatabase->getSymbolTable(pLyTable, AcDb::kForRead);
	if (!pLyTable->has(strLyName))
	{
		es = pLyTable->upgradeOpen();
		pLyRcd=new AcDbLayerTableRecord;
		if(NULL == pLyRcd) 
			return NULL;
		es = pLyRcd->setName(strLyName);
		es = pLyRcd->setIsFrozen(0);
		if(es != Acad::eOk) 
		{	
			pLyTable->close(); 
			delete pLyRcd; 
			return NULL;
		}
		pLyRcd->setIsLocked(0);
		pLyRcd->setIsOff(0);
		pLyRcd->setVPDFLT(0);

		TempColor.setColorIndex(nClrIndex);
		pLyRcd->setColor(TempColor);
		es = pLyRcd->setLineWeight(dLineWeigth);

		
		if(!lnTypeId.isNull())
			pLyRcd->setLinetypeObjectId(lnTypeId);
		es = pLyTable->add(resultId, pLyRcd);
		es = pLyRcd->close();
		es = pLyTable->close();
	}
	else
	{
		es = pLyTable->getAt(strLyName, resultId);
		es = pLyTable->getAt(strLyName, pLyRcd, AcDb::kForWrite);
		pLyTable->close();
		if(es != Acad::eOk) 
			return NULL;
		es = TempColor.setColorIndex(nClrIndex);
		pLyRcd->setColor(TempColor);
		es = pLyRcd->setLineWeight(dLineWeigth);
		if(!lnTypeId.isNull())
			pLyRcd->setLinetypeObjectId(lnTypeId);
		es = pLyRcd->close();
	}

	//系统变量
	/*struct resbuf *Value = acutBuildList(RTSTR,strLyName,0);

	if(acedSetVar("clayer", Value) != RTNORM)
	{
		acutRelRb(Value);
		return FALSE;
	}

	acutRelRb(Value);*/

	return resultId;

}

// 查找CAD实体线型
AcDbObjectId CDistMap::CAD_GetLineTypeId(const char* strLnTypeName)
{
	AcDbObjectId			resultId;
	resultId.setNull();

	AcDbLinetypeTable		*pDbLtT;
	m_lpCADDatabase->getSymbolTable(pDbLtT, AcDb::kForRead);

	if (!pDbLtT->has(strLnTypeName))
	{
		pDbLtT->close();
		return NULL;
	}
	else
	{
		pDbLtT->getAt(strLnTypeName, resultId);
		pDbLtT->close();
		return resultId;
	}
}

// 加载CAD实体线型
AcDbObjectId CDistMap::CAD_LoadLineType(const char* strLnTypeName)
{
	AcDbObjectId			resultId;

	AcDbLinetypeTable		* pDbLtT;
	Acad::ErrorStatus		es;

	resultId.setNull();

	es = m_lpCADDatabase->loadLineTypeFile(strLnTypeName, ".\\acadiso.lin");
	if( es != Acad::eOk)
	{
		char charBuff[1024]={0};
		sprintf(charBuff, "\n数慧提示:没有发现 %s 线型",strLnTypeName);
		//acutPrintf(charBuff);
		return NULL;
	}
	else
	{
		es = m_lpCADDatabase->getLinetypeTable(pDbLtT, AcDb::kForRead);
		if ( es == Acad::eOk)
		{
			pDbLtT->getAt(strLnTypeName, resultId);
			pDbLtT->close();
			return resultId;
		}
		else
		{
			pDbLtT->close();
			char charBuff[1024]={0};
			sprintf(charBuff, "\n数慧提示:加载线型 %s 失败!",strLnTypeName);
			//acutPrintf(charBuff);
			return NULL;
		}
	}

}

// 功能：创建SDE图层(存放CAD实体对象)
// 参数：strTableName      SDE关联的属性数据表名称
//       strSpacialFldName 图层与属性表关联的空间字段名(图层创建后会在属性表中增加该字段)
//       dScale            图层比例尺
//       ox,oy             坐标系原点
//       dGridSize         网格大小
//       lTypeMask         图层类型
//       strKeyword        关键字
// 返回：成功返回 1，失败返回 0，其他参考定义
long CDistMap::SDE_CreateLayer(const char* strTableName,
							   const char* strSpacialFldName,
							   double dScale,
							   double ox,double oy,
							   double dGridSize,
							   long   lTypeMask,
							   const char* strKeyword)
{
	//判断数据库连接对象指针是否存在
	if(!m_lpDataConnection->CheckConnected())
	{
		char strInfo[SE_MAX_MESSAGE_LENGTH] = {0};
		sprintf(strInfo,"\n \nDatabase not connected!\n");
		OutputDebugString(strInfo);
		return -1;
	}

	long rc = 0;
	switch (m_lpDataConnection->m_enType)
	{
	case C_SDE: //SDE
		SE_COORDREF  coordref;   //坐标系引用
		SE_LAYERINFO layerInfo;  //层信息
		// 创建层的坐标系参考
		rc = SE_coordref_create (&coordref);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_coordref_create");

		//设置假定坐标源点与比例尺
		rc = SE_coordref_set_xy (coordref,ox,oy,dScale);//新原点x,y和比例尺
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_coordref_set_xy");

		//分配并初始化层信息结构layerInfo
		rc = SE_layerinfo_create (coordref, &layerInfo);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_create");

		//设置绘图网格尺寸
		rc = SE_layerinfo_set_grid_sizes (layerInfo,dGridSize,0,0);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_grid_sizes");

		//设置图层类型
		rc = SE_layerinfo_set_shape_types(layerInfo,lTypeMask);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_shape_types");

		//设置关键字
		rc = SE_layerinfo_set_creation_keyword (layerInfo, strKeyword);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_creation_keyword");

		//属性表与空间表通过指定字段建立关联
		rc = SE_layerinfo_set_spatial_column (layerInfo, strTableName, strSpacialFldName); 
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_spatial_column");

		//创建层
		rc = SE_layer_create (((CDistConnectSDE*)m_lpDataConnection)->GetConnectObj(), layerInfo, 0,0);
		CDistConnectSDE::check_error(rc, "CreateLayer->SE_layer_create");

		SE_coordref_free(coordref);
		SE_layerinfo_free(layerInfo);

		if(rc == SE_SUCCESS)
			return 1;
		else
			return rc;
		break;
	case C_MIF:
		return rc;
		break;
	case C_SHAPE:
		return rc;
		break;
	}
}