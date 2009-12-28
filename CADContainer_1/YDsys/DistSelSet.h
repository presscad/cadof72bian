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

#ifndef __DIST_SELSET__H__
#define __DIST_SELSET__H__


// ���ƣ�ѡ�񼯺���
// ���ݣ������װ��һЩ���õ�ѡ�񼯺ϵķ�������point,cross,window,polygon�ȷ���
// ʹ�ã�
// 1.CDistSelSet temp;
// 2.temp.ѡ�񷽷�(����)
// 3.ͨ��AsArray,AsAdsName,At�Ⱥ�����ֱ��ʹ�ã�ѡ���ļ�������
// 4.ע�����б��λ��ͬ����ϣ�ʵ�ֲ�ͬ����

//ѡ�񼯺���
class CDistSelSet 
{

public:

	//ѡ��״ֵ̬
    enum SelSetStatus 
	{
        kDistSelected=1,    //ѡ��
        kDistNone=2,        //��
        kDistCanceled=3,    //ȡ��
		kDistRejected=4     //���ϸ�
    };

	//���������
                     CDistSelSet();
    virtual         ~CDistSelSet();


	//����һ����ѡ�񼯺ϣ�������ѡ��״̬
    SelSetStatus    CreateEmptySet();  

	//�û��Զ���ѡ��
    SelSetStatus    UserSelect(const resbuf* filter = NULL);
    SelSetStatus    UserSelect(LPCTSTR selectPrompt, LPCTSTR removePrompt, const resbuf* filter = NULL);

	//����ѡ��
    SelSetStatus    ImpliedSelect(const resbuf* filter = NULL); 

	//����ѡ��
    SelSetStatus    CrossingSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter = NULL);
    SelSetStatus    CrossingPolygonSelect(const AcGePoint3dArray& ptArray, const resbuf* filter = NULL);

    //Ȧѡ��
    SelSetStatus    FenceSelect(const AcGePoint3dArray& ptArray, const resbuf* filter = NULL);

	//��һ��ѡ��
    SelSetStatus    PreviousSelect(const resbuf* filter = NULL);
	//���һ��ѡ��
    SelSetStatus    LastSelect(const resbuf* filter = NULL);

	//��ѡ��
    SelSetStatus    PointSelect(const AcGePoint3d& pt1, const resbuf* filter = NULL);
    //����ѡ��
    SelSetStatus    WindowSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter = NULL);

	//��ѡ��
    SelSetStatus    WindowPolygonSelect(const AcGePoint3dArray& ptArray, const resbuf* filter = NULL);


    SelSetStatus    FilterOnlySelect(const resbuf* filter = NULL);
	SelSetStatus	AllSelect(const resbuf* filter = NULL);
	SelSetStatus	BoxSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter = NULL);

    //ѯ��
    SelSetStatus    LastStatus() const;

    //ѡ������
	void			SetFilterLockedLayers(bool filterThem);
    void            SetAllowDuplicates(bool dups);
    void            SetAllAtPickBox(bool pickBox);
    void            SetAllowSingleOnly(bool single, bool allowLast);
	void			SetRejectNonCurrentSpace(bool rejectIt);
	void			SetRejectPaperSpaceViewport(bool rejectId);
    void            SetKeywordCallback(LPCTSTR extraKwords, struct resbuf* (*pFunc) (const char*));
    void            SetOtherCallback(struct resbuf* (*pFunc) (const char*));

    //���ϲ��� (backward compatibility with ADS)
    int             Length();//ȡѡ�񼯺ϳ���
    void            Add(ads_name ent);//��ѡ�������һ��ʵ�����
    void            Add(const AcDbObjectId& objId);//��ѡ�������һ��ʵ��ID
    void            Remove(ads_name ent);//��ѡ����ɾ��һ��ʵ�����
    void            Remove(const AcDbObjectId& objId);//��ѡ����ɾ��һ��ʵ��ID
    bool			IsMember(ads_name ent);//�ж�һ��ʵ������Ƿ���ѡ����
    bool			IsMember(const AcDbObjectId& objId);//�ж�һ��ʵ��ID�Ƿ���ѡ����
	bool            At(int nPos,ads_name& ent);


	//ȷ��ѡ�񼯺�����Щʵ�屻ѡ��
    bool			SubEntMarkerAt(int index, AcDbObjectId& objId, int& gsMarker);

    //ת��Ϊ�� AcDbObjectIdArray, ads_name
    void            AsArray(AcDbObjectIdArray& objIds);
    void            AsAdsName(ads_name ss);

private:
    // ��Ա����
    ads_name        m_ss;                            //ѡ�񼯺�
    SelSetStatus    m_lastStatus;                    //��ǰ״̬

	//��ǰ������Ǳ���
    bool			m_allowDuplicates;
    bool			m_allAtPickBox;
    bool			m_singleOnly;
    bool			m_allowLast;
	bool			m_filterLockedLayers;
	bool			m_rejectNonCurrentSpace;
	bool			m_rejectPaperSpaceViewport;


    CString			m_flags;
    CString			m_extraKwords;

	//�ص�����ָ��
    struct resbuf* (*m_kwordFuncPtr)(const char*);
    struct resbuf* (*m_otherFuncPtr)(const char*);

    //��������
    bool			  IsInitialized() const;       // has the set been allocated by AutoCAD?
    void              Clear();                     // free up ss and reinitialize
    SelSetStatus      HandleResult(int result);    // common ads_ssget() return action
    void              SetFlags(bool hasPrompts);
	void              CopyAdsPt(ads_point pt1, ads_point pt2);
	Acad::ErrorStatus ObjIdToEname(const AcDbObjectId& objId, ads_name& ent);
	Acad::ErrorStatus EnameToObjId(ads_name ent, AcDbObjectId& objId);
	resbuf*           PtArrayToResbuf(const AcGePoint3dArray& ptArray);

    //���ò���
                    CDistSelSet(const CDistSelSet&);
    CDistSelSet&	operator=(const CDistSelSet&);
};


#endif    // __DIST_SELSET__H__

//������
//resbuf* filter = acutBuildList(-4, "<or",
//                               -4, "<and", RTDXF0, "CIRCLE", 40, 1.0,  -4, "and>",  
//                               -4, "<and", RTDXF0,"LINE", 8, "LyName", -4, "and>", 
//                               -4, "or>", 0); 


//������������������������������ ������ѡ�񼯺ϡ�����������������������������������������//
	//int acedSSGet(const int1 * str,const void * pt1,const void * pt2,const struct resbuf * filter,ads_name ss);
	//�������� str �������£�

	//////////////////////////// Selection Method Mode Options //////////////////////////////////////
    //NULL  Single-point selection (if pt1 is specified)or user selection (if pt1 is also NULL)
	//"I"   The PICKFIRST set 
	//"C"   Crossing selection 
	//"CP"  Crossing polygon selection 
	//"F"   Fence (open polygon) selection 
	//"L"   Last created entity 
	//"P"   Previous selection set 
	//"W"   Window selection 
	//"WP"  Window polygon selection 
	//"X"   Filter selection only  
	//"G"   Groups 

	///////////////////////  Colon Mode Options ////////////////////
	//":$"  Prompts supplied 
	//":?" "Other" callbacks 
	//":D"  Duplicates(����) OK 
	//":E"  Everything in aperature ( ��֪)
	//":K"  Keyword callbacks 
	//":N"  Nested (Ƕ��)
	//":S"  Force single object selection only 
	//"."   User pick (��ѡ)

	///////////////////////// Keyword Filter Mode Options //////////////////////////
	//"#"   Nongeometric (all, last, previous) �Ǽ���ѡ��
	//"A"   All 
	//"B"   BOX 
	//"M"   Multiple 
    //����������������������������������������������������������������������������������������//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

