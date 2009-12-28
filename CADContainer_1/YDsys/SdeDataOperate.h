#ifndef  __SDEDATA__OPERATE__H__
#define  __SDEDATA__OPERATE__H__


#include "..\SdeData\DistExchangeInterface.h"

class CSdeDataOperate
{
public:
		     CSdeDataOperate(void);
	virtual ~CSdeDataOperate(void);

public:
		
	//���ܣ���SDE���ݵ�CAD��
	long ReadAllSdeLayer(AcDbObjectIdArray& arrId,              //���ش��� CAD ʵ��� ID
		                 char* strSdeLyName,                    //ָ�� SDE ͼ������
						 DIST_STRUCT_SELECTWAY* pFilter = NULL, //ָ���ռ��������
						 char* strFieldSql = NULL,		        //ָ��SQL��ѯ����
						 int nPower=1);                         //ͼ���дȨ�ޣ�1ֻ�� 2ֻд 3��д ��



	//���ܣ�����CAD���ݵ�SDE��
	long SaveAllSdeLayer(const char* strSdeLyName,              //SDEͼ������(�����ͨ���)
		                 DIST_STRUCT_SELECTWAY* pFilter = NULL, //�ռ��������
						 bool bAfterSaveToDelete = true);       //������Ƿ��CADͼ����ɾ����Ӧ��ʵ��

	// ��ȡ��Ŀ��ŵĵ����飬�洢��CDefineFile::g_XMBHpts
	BOOL GetSdeOneShapePoints(AcGePoint3dArray& ptArray,const char* strSdeLyName,const char* strFName,const char* strFValue);

	void GetCadLayerBySdeLayer(CStringArray& strCadLyNameArray,const CStringArray& strSdeLyNameArray);
	void GetSdeLayerByCadLayer(CStringArray& strSdeLyNameArray,const CStringArray& strCadLyNameArray);


private:
	//���ܣ���SDE���ݵ�CAD��
	//������
	//      strSdeLyName		SDEͼ������(�������ͨ���)
	//		pFilter				�ռ��ѯ��ʽ
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long ReadSdeLayer(AcDbObjectIdArray& arrId, const char* strSdeLyName, DIST_STRUCT_SELECTWAY* pFilter = NULL, 
		              const char* strFieldSql = NULL,CStringArray* pFldArray = NULL,int nPower=1);

	//���ܣ�����CAD���ݵ�SDE��
	//������
	//      strSdeLyName		SDEͼ������(�������ͨ���)
	//		pFilter				�ռ��ѯ��ʽ
	//      bAfterSaveToDelete  ������Ƿ��CADͼ����ɾ����Ӧ��ʵ��
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long SaveSdeLayer(const char* strSdeLyName, DIST_STRUCT_SELECTWAY* pFilter = NULL, bool bAfterSaveToDelete = true);

	BOOL CmpTwoString(CString str1, CString str2);

	BOOL SaveAttrToRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue);

	BOOL IsDKLayer(CString strSdeLyName);
	BOOL IsDKLayerOld(CString strSdeLyName);
	BOOL IsXMLayer(CString strSdeLyName);
	long ReadDKLayer(AcDbObjectIdArray& arrId,char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter = NULL,const char* strFieldSql = NULL);
	long ReadXMLayer(AcDbObjectIdArray& arrId,char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter = NULL,const char* strFieldSql = NULL);
	long ReadDKLayerOld(AcDbObjectIdArray& arrId,char* strSdeLyName,DIST_STRUCT_SELECTWAY* pFilter = NULL,const char* strFieldSql = NULL);
private:
	//IDistConnection*   m_pConnection;
	//IDistExchangeData* m_pExchangeData;
	char               m_strGdbOwner[255];
};

#endif// __SDEDATA__OPERATE__H__