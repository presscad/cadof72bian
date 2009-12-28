///////////////////////////////////////////////////////////////////////////////
//上海数慧系统技术有限公司拥有本代码之完全版权，未经上海数慧系统技术有限公司 //
//的明确书面许可，不得复制、取用、分发此源代码。                             //
//任何单位和个人在未获得许可情况下，不得以任何形式复制、修改和发布本软件的任 //
//何部分，否则将视为非法侵害，我公司将保留依法追究其责任的权利。在未获得许可 //
//情况下，任何组织或个人发布的产品都不能使用Dist的标志和Logo。               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//作者 : 王晖                                                                //
//                                                                           //
// 版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                          //
///////////////////////////////////////////////////////////////////////////////

#ifndef __DIST_SELSET__H__
#define __DIST_SELSET__H__


// 名称：选择集合类
// 内容：该类封装了一些常用的选择集合的方法，如point,cross,window,polygon等方法
// 使用：
// 1.CDistSelSet temp;
// 2.temp.选择方法(参数)
// 3.通过AsArray,AsAdsName,At等函数，直接使用，选择后的集合内容
// 4.注意其中标记位不同的组合，实现不同功能

//选择集合类
class CDistSelSet 
{

public:

	//选择状态值
    enum SelSetStatus 
	{
        kDistSelected=1,    //选择
        kDistNone=2,        //空
        kDistCanceled=3,    //取消
		kDistRejected=4     //不合格
    };

	//构造和析构
                     CDistSelSet();
    virtual         ~CDistSelSet();


	//创建一个空选择集合，并返回选择状态
    SelSetStatus    CreateEmptySet();  

	//用户自定义选择
    SelSetStatus    UserSelect(const resbuf* filter = NULL);
    SelSetStatus    UserSelect(LPCTSTR selectPrompt, LPCTSTR removePrompt, const resbuf* filter = NULL);

	//隐含选择
    SelSetStatus    ImpliedSelect(const resbuf* filter = NULL); 

	//交叉选择
    SelSetStatus    CrossingSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter = NULL);
    SelSetStatus    CrossingPolygonSelect(const AcGePoint3dArray& ptArray, const resbuf* filter = NULL);

    //圈选择
    SelSetStatus    FenceSelect(const AcGePoint3dArray& ptArray, const resbuf* filter = NULL);

	//上一次选择
    SelSetStatus    PreviousSelect(const resbuf* filter = NULL);
	//最后一次选择
    SelSetStatus    LastSelect(const resbuf* filter = NULL);

	//点选择
    SelSetStatus    PointSelect(const AcGePoint3d& pt1, const resbuf* filter = NULL);
    //窗口选择
    SelSetStatus    WindowSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter = NULL);

	//面选择
    SelSetStatus    WindowPolygonSelect(const AcGePoint3dArray& ptArray, const resbuf* filter = NULL);


    SelSetStatus    FilterOnlySelect(const resbuf* filter = NULL);
	SelSetStatus	AllSelect(const resbuf* filter = NULL);
	SelSetStatus	BoxSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter = NULL);

    //询查
    SelSetStatus    LastStatus() const;

    //选项设置
	void			SetFilterLockedLayers(bool filterThem);
    void            SetAllowDuplicates(bool dups);
    void            SetAllAtPickBox(bool pickBox);
    void            SetAllowSingleOnly(bool single, bool allowLast);
	void			SetRejectNonCurrentSpace(bool rejectIt);
	void			SetRejectPaperSpaceViewport(bool rejectId);
    void            SetKeywordCallback(LPCTSTR extraKwords, struct resbuf* (*pFunc) (const char*));
    void            SetOtherCallback(struct resbuf* (*pFunc) (const char*));

    //集合操作 (backward compatibility with ADS)
    int             Length();//取选择集合长度
    void            Add(ads_name ent);//向选择集中添加一个实体对象
    void            Add(const AcDbObjectId& objId);//向选择集中添加一个实体ID
    void            Remove(ads_name ent);//在选择集中删除一个实体对象
    void            Remove(const AcDbObjectId& objId);//在选择集中删除一个实体ID
    bool			IsMember(ads_name ent);//判断一个实体对象是否在选择集中
    bool			IsMember(const AcDbObjectId& objId);//判断一个实体ID是否在选择集中
	bool            At(int nPos,ads_name& ent);


	//确定选择集合中那些实体被选中
    bool			SubEntMarkerAt(int index, AcDbObjectId& objId, int& gsMarker);

    //转换为： AcDbObjectIdArray, ads_name
    void            AsArray(AcDbObjectIdArray& objIds);
    void            AsAdsName(ads_name ss);

private:
    // 成员变量
    ads_name        m_ss;                            //选择集合
    SelSetStatus    m_lastStatus;                    //当前状态

	//当前各个标记变量
    bool			m_allowDuplicates;
    bool			m_allAtPickBox;
    bool			m_singleOnly;
    bool			m_allowLast;
	bool			m_filterLockedLayers;
	bool			m_rejectNonCurrentSpace;
	bool			m_rejectPaperSpaceViewport;


    CString			m_flags;
    CString			m_extraKwords;

	//回调函数指针
    struct resbuf* (*m_kwordFuncPtr)(const char*);
    struct resbuf* (*m_otherFuncPtr)(const char*);

    //辅助函数
    bool			  IsInitialized() const;       // has the set been allocated by AutoCAD?
    void              Clear();                     // free up ss and reinitialize
    SelSetStatus      HandleResult(int result);    // common ads_ssget() return action
    void              SetFlags(bool hasPrompts);
	void              CopyAdsPt(ads_point pt1, ads_point pt2);
	Acad::ErrorStatus ObjIdToEname(const AcDbObjectId& objId, ads_name& ent);
	Acad::ErrorStatus EnameToObjId(ads_name ent, AcDbObjectId& objId);
	resbuf*           PtArrayToResbuf(const AcGePoint3dArray& ptArray);

    //禁用操作
                    CDistSelSet(const CDistSelSet&);
    CDistSelSet&	operator=(const CDistSelSet&);
};


#endif    // __DIST_SELSET__H__

//过滤器
//resbuf* filter = acutBuildList(-4, "<or",
//                               -4, "<and", RTDXF0, "CIRCLE", 40, 1.0,  -4, "and>",  
//                               -4, "<and", RTDXF0,"LINE", 8, "LyName", -4, "and>", 
//                               -4, "or>", 0); 


//××××××××××××××× ×××选择集合×××××××××××××××××××××//
	//int acedSSGet(const int1 * str,const void * pt1,const void * pt2,const struct resbuf * filter,ads_name ss);
	//函数参数 str 含义如下：

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
	//":D"  Duplicates(复制) OK 
	//":E"  Everything in aperature ( 感知)
	//":K"  Keyword callbacks 
	//":N"  Nested (嵌套)
	//":S"  Force single object selection only 
	//"."   User pick (挑选)

	///////////////////////// Keyword Filter Mode Options //////////////////////////
	//"#"   Nongeometric (all, last, previous) 非几何选择
	//"A"   All 
	//"B"   BOX 
	//"M"   Multiple 
    //××××××××××××××××××××××××××××××××××××××××××××//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

