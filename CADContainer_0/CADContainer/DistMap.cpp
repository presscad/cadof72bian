#include "distmap.h"

CDistMap::CDistMap(void)
{
	m_lpCADDatabase = NULL;
}

CDistMap::~CDistMap(void)
{
}

//���ܣ�����ͼ��
//������strLyName   ͼ����
//      nClrIndex   ��ɫ��
//      strLnType   ����
//      dLineWeigth �߿�
//���أ�resultId    ���ش���ʵ���ID �ɹ������½�Layer ID ��ʧ�ܷ���NULL
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

	//ϵͳ����
	/*struct resbuf *Value = acutBuildList(RTSTR,strLyName,0);

	if(acedSetVar("clayer", Value) != RTNORM)
	{
		acutRelRb(Value);
		return FALSE;
	}

	acutRelRb(Value);*/

	return resultId;

}

// ����CADʵ������
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

// ����CADʵ������
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
		sprintf(charBuff, "\n������ʾ:û�з��� %s ����",strLnTypeName);
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
			sprintf(charBuff, "\n������ʾ:�������� %s ʧ��!",strLnTypeName);
			//acutPrintf(charBuff);
			return NULL;
		}
	}

}

// ���ܣ�����SDEͼ��(���CADʵ�����)
// ������strTableName      SDE�������������ݱ�����
//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ�㴴����������Ա������Ӹ��ֶ�)
//       dScale            ͼ�������
//       ox,oy             ����ϵԭ��
//       dGridSize         �����С
//       lTypeMask         ͼ������
//       strKeyword        �ؼ���
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CDistMap::SDE_CreateLayer(const char* strTableName,
							   const char* strSpacialFldName,
							   double dScale,
							   double ox,double oy,
							   double dGridSize,
							   long   lTypeMask,
							   const char* strKeyword)
{
	//�ж����ݿ����Ӷ���ָ���Ƿ����
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
		SE_COORDREF  coordref;   //����ϵ����
		SE_LAYERINFO layerInfo;  //����Ϣ
		// �����������ϵ�ο�
		rc = SE_coordref_create (&coordref);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_coordref_create");

		//���üٶ�����Դ���������
		rc = SE_coordref_set_xy (coordref,ox,oy,dScale);//��ԭ��x,y�ͱ�����
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_coordref_set_xy");

		//���䲢��ʼ������Ϣ�ṹlayerInfo
		rc = SE_layerinfo_create (coordref, &layerInfo);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_create");

		//���û�ͼ����ߴ�
		rc = SE_layerinfo_set_grid_sizes (layerInfo,dGridSize,0,0);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_grid_sizes");

		//����ͼ������
		rc = SE_layerinfo_set_shape_types(layerInfo,lTypeMask);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_shape_types");

		//���ùؼ���
		rc = SE_layerinfo_set_creation_keyword (layerInfo, strKeyword);
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_creation_keyword");

		//���Ա���ռ��ͨ��ָ���ֶν�������
		rc = SE_layerinfo_set_spatial_column (layerInfo, strTableName, strSpacialFldName); 
		CDistConnectSDE::check_error (rc, "CreateLayer->SE_layerinfo_set_spatial_column");

		//������
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