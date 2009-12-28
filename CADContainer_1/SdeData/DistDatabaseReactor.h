///////////////////////////////////////////////////////////////////////////////
//�Ϻ�����ϵͳ�������޹�˾ӵ�б�����֮��ȫ��Ȩ��δ���Ϻ�����ϵͳ�������޹�˾ //
//����ȷ������ɣ����ø��ơ�ȡ�á��ַ���Դ���롣                             //
//�κε�λ�͸�����δ����������£��������κ���ʽ���ơ��޸ĺͷ������������ //
//�β��֣�������Ϊ�Ƿ��ֺ����ҹ�˾����������׷�������ε�Ȩ������δ������ //
//����£��κ���֯����˷����Ĳ�Ʒ������ʹ��Dist�ı�־��Logo��               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//���� : ����                                                                //
//                                                                           //
// ��Ȩ���� (C) 2007��2010 Dist Inc. ��������Ȩ����                          //
///////////////////////////////////////////////////////////////////////////////

#ifndef __DISTDBREACTOR__H__
#define __DISTDBREACTOR__H__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//������   �����ݿⷴӦ��
//���˼· ��ͨ��DIST_STRUCT_CHANGEROWID�ṹ��������¼��ͬ�ĵ��Ĳ�ͬͼ���У�ʵ��༭���
//           ���Խṹ�����а�����strFileName,strCadLyName����Ա�������ֲ�ͬ�ñ༭λ�ã���
//           ���ڼ̳�ʵ��AcDbDatabaseReactor������ϣ������˲����ṹ����ļ�����������ʵ��
//           �Խṹ��RowId������ɾ���ģ���ȹ���
//
//ʹ��˵�� ����Ϊ�����ݿⷴӦ�������Ը��������CAD��ֻ��Ҫ����Ψһʵ��������Ϊȫ�ֶ���
//           ���ڲ�ͬ�ĵ�֮��ĵ���
//           ע�� ->  CDistDatabaseReactor::rxInit();
//           ���� ->  CDistDatabaseReactor* g_pInstance = CDistDatabaseReactor::GetInstance();
//           �ͷ�->   CDistDatabaseReactor::Release();
//�޸�˵���� DIST_STRUCT_CHANGEROWID�ṹ������strSdeLyName�ֶΣ�����Ӧ��CADͼ���Ӧ���SDEͼ��


#define  PCURDB acdbHostApplicationServices()->workingDatabase()

//����޸ļ�¼���ṹ����
struct DIST_STRUCT_CHANGEROWID
{
	int              nID;                //AcDbDatabase �����ID(��Ҫ���ֶ��ĵ�)
	char             strCadLyName[255];  //CADͼ����
	char             strSdeLyName[255];  //SDEͼ����
	CArray<int,int>  RowIdArray;         //RowId����

	DIST_STRUCT_CHANGEROWID* pNext;

	DIST_STRUCT_CHANGEROWID()//��ʼ������
	{
		nID = -1;
		memset(strCadLyName,0,sizeof(char)*255);
		memset(strSdeLyName,0,sizeof(char)*255);
		RowIdArray.RemoveAll();
		pNext = NULL;
	}

};


//ֻ��ͼ���б�ṹ
struct DIST_STRUCT_READONLYLAYER
{
	int nID;
	char strSdeLyName[255];
	DIST_STRUCT_READONLYLAYER* pNext;

	DIST_STRUCT_READONLYLAYER()
	{
		{
			nID = -1;
			memset(strSdeLyName,0,sizeof(char)*255);
			pNext = NULL;
		}
	}

};

class CDistDatabaseReactor : public AcDbDatabaseReactor 
{
public:
	ACRX_DECLARE_MEMBERS(CDistDatabaseReactor);

    // ���ܣ���������
	// ��������
	// ���أ���
    virtual  ~CDistDatabaseReactor();

	// ���ܣ���ȡ��Ψһʵ������ָ��
	// ��������
	// ���أ�ʵ������ָ��
	static CDistDatabaseReactor* GetInstance();

	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	static void Release();

	// ���ܣ����ò���(SDEͼ����RowId���ƣ���չ�������ƣ�Ĭ��ΪRowId_Objs)
	// ������strRowId      SDEͼ����RowId����
	//       strXDataName  ��չ��������
	// ���أ���
	void SetParam(const char* strRowIdName,const char* strXDataName);

	// ���ܣ��ж�ָ������ʵ���Ƿ����
	// ������strCadLyName     CADͼ������
	//       strSdeLyName     SDEͼ������
	//       nRowId           ʵ��RowId
	//       pDB              ���ݿ�ָ��(��Ҫ���ֶ��ĵ�)
	// ���أ����ڷ��� 1 �������ڷ��� 0 �������ο�����
	long  IsRowIdExists(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB=PCURDB);
	
	// ���ܣ�����ָ������������ѯ��Ӧʵ���RowID
	// ������nRowId        ����ʵ��RowId
	//       strCadLyName  CADͼ������
	//       strSdeLyName  SDEͼ������
	//       nIndex        �������е����
	//       pDB           ���ݿ�ָ��(��Ҫ���ֶ��ĵ�)
	// ���أ����ڷ��� 1 �������ڷ��� 0 �������ο�����
	long  SelectRowId(int& nRowId,const char* strCadLyName,const char *strSdeLyName,int nIndex,AcDbDatabase* pDB= PCURDB);
	
	// ���ܣ��������в���һ��ʵ����Ϣ
	// ������strCadLyName     CADͼ������
	//       strSdeLyName     SDEͼ������
	//       nRowId           ʵ��RowId
	//       pDB              ���ݿ�ָ��(��Ҫ���ֶ��ĵ�)
	// ���أ��ɹ����� 1 ��ʧ�ܷ��� 0 �������ο�����
	void InsertRowId(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB=PCURDB);
	
	// ���ܣ�ɾ��������ָ��ʵ����Ϣ
	// ������strCadLyName     CADͼ������
	//       strSdeLyName     SDEͼ������
	//       nRowId           ʵ��RowId
	//       pDB              ���ݿ�ָ��(��Ҫ���ֶ��ĵ�)
	// ���أ���
	void DeleteRowId(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB = PCURDB);
	
	// ���ܣ�ɾ��������ָSDEͼ�����CADͼ�������ʵ��
	// ������strCadLyName     CADͼ������
	//       strSdeLyName     SDEͼ������
	//       pDB              ���ݿ�ָ��(��Ҫ���ֶ��ĵ�)
	// ���أ���
	void DeleteLayerRowIdArray(const char* strCadLyName,const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);
	
	// ���ܣ���ȡ������ָSDEͼ�����CADͼ���ʵ������
	// ������strCadLyName     CADͼ������
	//       strSdeLyName     SDEͼ������
	//       pDB              ���ݿ�ָ��(��Ҫ���ֶ��ĵ�)
	// ���أ�ʵ������
	long GetRowIdCount(const char* strCadLyName,const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);

	long IsReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);
	void InsertReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);
	void DeleteReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);


private:

	// ���ܣ�д��Ϣ��ָ��ʵ�����չ����
	// ������pObj      ָ��ʵ��ָ��
	//       pDB       ���ݿ�ָ��
	// ���أ���
	void WriteInfoToXRecord(const AcDbObject* pObj,const AcDbDatabase* pDB);

private:

    // ���ú���
							CDistDatabaseReactor();
							CDistDatabaseReactor(const& CDistDatabaseReactor);
    CDistDatabaseReactor&  operator=(const& CDistDatabaseReactor);



public:
	//======================== ʵ�ֻ����麯�� ==========================//
    virtual void    objectAppended(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectUnAppended(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectReAppended(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectOpenedForModify(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectModified(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectErased(const AcDbDatabase* dwg, const AcDbObject* obj,Adesk::Boolean pErased = Adesk::kTrue);
    virtual void    headerSysVarWillChange(const AcDbDatabase* dwg, const char* name);
    virtual void    headerSysVarChanged(const AcDbDatabase* dwg, const char* name,Adesk::Boolean bSuccess);
    virtual void	proxyResurrectionCompleted(const AcDbDatabase* dwg,const char* appname, AcDbObjectIdArray& objects);
    virtual void	goodbye(const AcDbDatabase* dwg);

private:
	static CDistDatabaseReactor*  m_pInstance;         // Ψһʵ��
	char                          m_strRowIdName[60];  // RowID��SDE���ݿ��е��ֶ�����
	char                          m_strXDataName[160]; // ��չ����XDataע������
	DIST_STRUCT_CHANGEROWID*      m_pRowIdData;        // RowID����ṹ����
	DIST_STRUCT_READONLYLAYER*    m_pReadOnlyLayer;    // ֻ��ͼ��ṹ����
};


#endif    // __DISTDBREACTOR__H__
