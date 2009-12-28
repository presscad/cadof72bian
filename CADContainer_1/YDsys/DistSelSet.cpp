#include "StdAfx.h"
#include "DistSelSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDistSelSet::CDistSelSet()
:   m_allowDuplicates(false),
    m_allAtPickBox(false),
    m_singleOnly(false),
    m_kwordFuncPtr(NULL),
    m_otherFuncPtr(NULL),
    m_allowLast(true),
	m_filterLockedLayers(false),
	m_rejectNonCurrentSpace(false),
	m_rejectPaperSpaceViewport(false)
{
    m_ss[0] = 0L;
    m_ss[1] = 0L;

    m_lastStatus = kDistCanceled;
}



CDistSelSet::~CDistSelSet()
{
    Clear();
}


//判断集合是否已经被初始化
bool CDistSelSet::IsInitialized() const
{
    if ((m_ss[0] == 0L) && (m_ss[1] == 0L))
        return false;
    else
        return true;
}

//清空集合
void CDistSelSet::Clear()
{
    if (IsInitialized()) 
	{
        acedSSFree(m_ss); //释放（If it succeeds, acedSSFree() returns RTNORM; otherwise, it returns an error code.）
        m_ss[0] = m_ss[1] = 0L;
    }
}


//创建新集合，并返回当前状态
CDistSelSet::SelSetStatus CDistSelSet::CreateEmptySet()
{
    Clear();
    int result = acedSSAdd(NULL, NULL, m_ss);//acedSSAdd ：创建一个新集合或将一个实体添加到已知集合中

    if (result == RTNORM)
        m_lastStatus = CDistSelSet::kDistSelected;
    else
        m_lastStatus = CDistSelSet::kDistCanceled;

    return m_lastStatus;
}


//CAD状态到自定义状态的转换
CDistSelSet::SelSetStatus CDistSelSet::HandleResult(int result)
{
    if (result == RTNORM)
        m_lastStatus = CDistSelSet::kDistSelected;
    else if (result == RTCAN)
        m_lastStatus = CDistSelSet::kDistCanceled;
	else if (result == RTREJ) 
	{
		ASSERT(0);
		m_lastStatus = CDistSelSet::kDistRejected;
	}
    else {        // doesn't return RTNONE
        CreateEmptySet();
        m_lastStatus = CDistSelSet::kDistNone;
    }
    return m_lastStatus;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//用户自定义选择集合
CDistSelSet::SelSetStatus CDistSelSet::UserSelect(const resbuf* filter)
{
    Clear();

    SetFlags(false);

    // if caller wants callback function for keywords
    if (m_kwordFuncPtr) //(参数为函数指针)
        acedSSSetKwordCallbackPtr(m_kwordFuncPtr); //Provides a pointer to a keyword callback function.

    // if caller wants callback function for garbage input
    if (m_otherFuncPtr)
        acedSSSetOtherCallbackPtr(m_otherFuncPtr);

    int result;
    if (m_kwordFuncPtr) //选择集合
        result = acedSSGet(m_flags, //选择模式
		                   NULL,    //An optional point relevant to some selection modes; or a result-buffer list that contains multiple points for the polygon or fence selection options; or an array of two strings that are replacement prompts for a :$ mode option
						   static_cast<LPCTSTR>(m_extraKwords),//An optional point relevant(相关) to some selection modes
						   filter, //An optional result-buffer list that enables acedSSGet() to filter the drawing to select entities of certain types or that have certain properties
						   m_ss);  //The name of the selection set
    else
        result = acedSSGet(m_flags, NULL, NULL, filter, m_ss);

    return HandleResult(result);
}


//用户自定义选择集合
CDistSelSet::SelSetStatus
CDistSelSet::UserSelect(LPCTSTR selectPrompt, LPCTSTR removePrompt, const resbuf* filter)
{
	// if they wanted both to be NULL they should have used other signature (信号)!
    ASSERT(selectPrompt || removePrompt);  
    char* promptPtrs[2];

    // allow for one of the prompts to be NULL.  It is usually useful for the Remove prompt.
    if ((selectPrompt == NULL) || (selectPrompt[0] == _T('\0')))
        promptPtrs[0] = _T("\n选择对象： ");
    else
        promptPtrs[0] = const_cast<LPTSTR>(selectPrompt);

    if ((removePrompt == NULL) || (removePrompt[0] == _T('\0')))
        promptPtrs[1] =  _T("\n删除对象： ");
    else
        promptPtrs[1] = const_cast<LPTSTR>(removePrompt);

    // if caller wants callback function for keywords
    if (m_kwordFuncPtr)
        acedSSSetKwordCallbackPtr(m_kwordFuncPtr);

        // if caller wants callback function for garbage input
    if (m_otherFuncPtr)
        acedSSSetOtherCallbackPtr(m_otherFuncPtr);

	// set current state of flags
    SetFlags(true);                     

    int result;
    if (m_kwordFuncPtr)
        result = acedSSGet(m_flags, promptPtrs, static_cast<LPCTSTR>(m_extraKwords), filter, m_ss);
    else
        result = acedSSGet(m_flags, promptPtrs, NULL, filter, m_ss);

    return HandleResult(result);
}


//选择符号条件的第一个实体
CDistSelSet::SelSetStatus
CDistSelSet::ImpliedSelect(const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection
    int result = acedSSGet(_T("_I"), NULL, NULL, filter, m_ss); //The PICKFIRST set
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：交叉选择（与线段P1P2相交的实体集合）
// 入口参数：
//          pt1,pt2  线段P1P2
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////
CDistSelSet::SelSetStatus
CDistSelSet::CrossingSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    int result = acedSSGet(_T("_C"), asDblArray(pt1), asDblArray(pt2), filter, m_ss);
    return HandleResult(result);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：多边形选择
// 入口参数：
//          ptArray :多边形点集合
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////
CDistSelSet::SelSetStatus
CDistSelSet::CrossingPolygonSelect(const AcGePoint3dArray& ptArray, const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    resbuf* ptList = PtArrayToResbuf(ptArray);
    if (ptList == NULL) 
	{
        m_lastStatus = CDistSelSet::kDistCanceled;
        return m_lastStatus;
    }

    int result = acedSSGet(_T("_CP"), ptList, NULL, filter, m_ss);
    acutRelRb(ptList);//释放
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：多段线选择（不闭合）
// 入口参数：
//          ptArray :多边形点集合
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////
CDistSelSet::SelSetStatus
CDistSelSet::FenceSelect(const AcGePoint3dArray& ptArray, const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    resbuf* ptList = PtArrayToResbuf(ptArray);
    if (ptList == NULL) {
        m_lastStatus = CDistSelSet::kDistCanceled;
        return m_lastStatus;
    }

    int result = acedSSGet(_T("_F"), ptList, NULL, filter, m_ss);
    acutRelRb(ptList);//释放
    return HandleResult(result);
}

//在最后一次搜索的基础上，过滤选择集合
CDistSelSet::SelSetStatus CDistSelSet::LastSelect(const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    int result = acedSSGet(_T("_L"), NULL, NULL, filter, m_ss);
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：点选择
// 入口参数：
//          pt1      点
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////

CDistSelSet::SelSetStatus CDistSelSet::PointSelect(const AcGePoint3d& pt1, const resbuf* filter)
{
    Clear();
	SetFlags(false);

    int result = acedSSGet(m_flags, asDblArray(pt1), NULL, filter, m_ss);
    return HandleResult(result);
}

//在前一次搜索的基础上，过滤选择集合
CDistSelSet::SelSetStatus CDistSelSet::PreviousSelect(const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    int result = acedSSGet(_T("_P"), NULL, NULL, filter, m_ss);
    return HandleResult(result);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：窗口选择
// 入口参数：
//          pt1,pt2  窗口的左下和右上角点
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////
CDistSelSet::SelSetStatus
CDistSelSet::WindowSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    int result = acedSSGet(_T("_W"), asDblArray(pt1), asDblArray(pt2), filter, m_ss);
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：窗口多边形选择(包括边框)
// 入口参数：
//          ptArray  窗口多边形
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////
CDistSelSet::SelSetStatus
CDistSelSet::WindowPolygonSelect(const AcGePoint3dArray& ptArray, const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    resbuf* ptList = PtArrayToResbuf(ptArray);
    if (ptList == NULL) {
        m_lastStatus = CDistSelSet::kDistCanceled;
        return m_lastStatus;
    }

    int result = acedSSGet(_T("_WP"), ptList, NULL, filter, m_ss);
    acutRelRb(ptList);
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：过滤器单独选择
// 入口参数：
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////

CDistSelSet::SelSetStatus
CDistSelSet::FilterOnlySelect(const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    int result = acedSSGet(_T("_X"), NULL, NULL, filter, m_ss);
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：选择所有
// 入口参数：
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////
CDistSelSet::SelSetStatus CDistSelSet::AllSelect(const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection
	
    int result = acedSSGet(_T("_A"), NULL, NULL, filter, m_ss);
    return HandleResult(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数功能：框选择
// 入口参数：
//          pt1,pt2  框的左下和右上角点
//          filter   搜索过滤器
// 返回值：返回选择状态结果
////////////////////////////////////////////////////////////////////////////////////////////////////

CDistSelSet::SelSetStatus
CDistSelSet::BoxSelect(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const resbuf* filter)
{
    Clear();

	// NOTE: flags not allowed on this type of selection

    int result = acedSSGet(_T("_B"), asDblArray(pt1), asDblArray(pt2), filter, m_ss);
    return HandleResult(result);
}


//获取选择集合长度
int CDistSelSet::Length()
{
    ASSERT(IsInitialized());

    long sslen;
    if (acedSSLength(m_ss, &sslen) != RTNORM) {
        //acutPrintf(_T("\n无效选择集合！"));
        return 0L;
    }
    else
        return sslen;
}

//添加一个实体（ads_name）到选择集合
void CDistSelSet::Add(ads_name ent)
{
    ASSERT(IsInitialized());
    acedSSAdd(ent, m_ss, m_ss);
}

//添加一个实体（AcDbObjectId）到选择集合
void CDistSelSet::Add(const AcDbObjectId& objId)
{
    ads_name ent;
    if (ObjIdToEname(objId, ent) == Acad::eOk)
        Add(ent);
}

//从选择集合中删除一个实体（ads_name）
void CDistSelSet::Remove(ads_name ent)
{
    ASSERT(IsInitialized());
    acedSSDel(ent, m_ss);
}


//从选择集合中删除一个实体（AcDbObjectId）
void CDistSelSet::Remove(const AcDbObjectId& objId)
{
    ads_name ent;
    if (ObjIdToEname(objId, ent) == Acad::eOk)
        Remove(ent);
}


//判断当前实体是否是集合中的一个成员
bool CDistSelSet::IsMember(ads_name ent)
{
    ASSERT(IsInitialized());

    if (acedSSMemb(ent, m_ss) == RTNORM)
        return true;
    else
        return false;
}

//判断当前实体是否是集合中的一个成员
bool
CDistSelSet::IsMember(const AcDbObjectId& objId)
{
    ads_name ent;
    if (ObjIdToEname(objId, ent) == Acad::eOk)
        return IsMember(ent);
    else
        return false;
}


//取选择集合中指定位置的实体
bool CDistSelSet::At(int nPos,ads_name& ent)
{
	if(acedSSName(m_ss, nPos, ent)==RTNORM)
		return true;
	else
		return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDistSelSet::SubEntMarkerAt(int index, AcDbObjectId& objId, int& gsMarker)
{
    resbuf* rb;

    if (acedSSNameX(&rb, m_ss, index) != RTNORM)
        return false;

    // walk to third element to retrieve ename of selected entity
    int i = 1;
    resbuf* tmp = rb;
    while (tmp && (i < 3)) {
        i++;
        tmp = tmp->rbnext;
    }
        // get the objectId of the slected entity
    ASSERT((tmp != NULL) && (i == 3));
    if (tmp == NULL)
        return false;

    if (EnameToObjId(tmp->resval.rlname, objId) != Acad::eOk) {
        acutRelRb(rb);
        return false;
    }
        // get the gsMarker, which should be the 4th element
    tmp = tmp->rbnext;
    ASSERT(tmp != NULL);
    if (tmp == NULL) {
        acutRelRb(rb);
        return false;
    }

    gsMarker = tmp->resval.rint;
    acutRelRb(rb);
    return true;
}


///////////////////////////////////////////////////////////////////////////////////
//将选择集合中的实体的ObjectId表示
void CDistSelSet::AsArray(AcDbObjectIdArray& objIds)
{
    ASSERT(IsInitialized());
    int len = Length();
    int ret;
    AcDbObjectId objId;
    ads_name ent;

    for (int i=0L; i<len; i++) 
	{
        ret = acedSSName(m_ss, i, ent);
        ASSERT(ret == RTNORM);
        if (EnameToObjId(ent, objId) == Acad::eOk)
            objIds.append(objId);
		
    }
}


//最后一次选择状态
CDistSelSet::SelSetStatus CDistSelSet::LastStatus() const
{
    return m_lastStatus;
}

//复制实体指针
void CDistSelSet::AsAdsName(ads_name ss)
{
    ss[0] = m_ss[0];
    ss[1] = m_ss[1];
}

//
void CDistSelSet::SetFilterLockedLayers(bool filterThem)//锁定图层
{
    m_filterLockedLayers = filterThem;
}


void CDistSelSet::SetAllowDuplicates(bool dups)//允许复制
{
    m_allowDuplicates = dups;
}


void CDistSelSet::SetAllAtPickBox(bool pickBox) //所有在选择框内
{
    m_allAtPickBox = pickBox;
}


void CDistSelSet::SetAllowSingleOnly(bool single, bool allowLast) //允许单个选择
{
    m_singleOnly = single;
    m_allowLast = allowLast;
}


void CDistSelSet::SetRejectNonCurrentSpace(bool rejectIt) //拒绝当前空间为空
{
	m_rejectNonCurrentSpace = rejectIt;
}


void CDistSelSet::SetRejectPaperSpaceViewport(bool rejectIt)//拒绝图纸，视图空间
{
	m_rejectPaperSpaceViewport = rejectIt;
}


void CDistSelSet::SetKeywordCallback(LPCTSTR extraKwords, struct resbuf* (*pFunc) (const char*))
{
    ASSERT(pFunc != NULL);

    m_kwordFuncPtr = pFunc;
    m_extraKwords = extraKwords;
}


void CDistSelSet::SetOtherCallback(struct resbuf* (*pFunc) (const char*))
{
    ASSERT(pFunc != NULL);

    m_otherFuncPtr = pFunc;
}

//设置标记
void CDistSelSet::SetFlags(bool hasPrompt)
{
    m_flags = _T("_");

    if (m_singleOnly) {
        if (m_allowLast)
            m_flags += _T("+L");

        m_flags += _T("+.:S");    // put in single mode, plus allow only pick selection
    }

    if (hasPrompt)
        m_flags += _T(":$");

    if (m_allowDuplicates)
        m_flags += _T(":D");

    if (m_allAtPickBox)
        m_flags += _T(":E");

	if (m_filterLockedLayers)
		m_flags += _T(":L");

	if (m_rejectNonCurrentSpace)
		m_flags += _T(":C");

	if (m_rejectPaperSpaceViewport)
		m_flags += _T(":P");

    if (m_kwordFuncPtr != NULL)
        m_flags += _T(":K");

    if (m_otherFuncPtr != NULL)
        m_flags += _T(":?");
}


Acad::ErrorStatus CDistSelSet::EnameToObjId(ads_name ent, AcDbObjectId& objId)
{
	Acad::ErrorStatus es = acdbGetObjectId(objId, ent);
	ASSERT(es == Acad::eOk);
	if (es != Acad::eOk)
		acutPrintf("\n Tranas ads_name to objectId error!");

	return es;
}


Acad::ErrorStatus CDistSelSet::ObjIdToEname(const AcDbObjectId& objId, ads_name& ent)
{
	Acad::ErrorStatus es = acdbGetAdsName(ent, objId);
	ASSERT(es == Acad::eOk);
	if (es == Acad::eOk)
		acutPrintf("\n Tranas objectId to ads_name error!");

	return es;
}


resbuf* CDistSelSet::PtArrayToResbuf(const AcGePoint3dArray& ptArray)
{
	resbuf* ptList = NULL;        // overall list
	resbuf* lastRb = NULL;        // place holder to end of list
	resbuf* rb;
	int len = ptArray.length();
	for (int i=0;i<len;i++) {
		if ((rb = acutNewRb(RT3DPOINT)) == NULL) {
			acutRelRb(ptList);
			return NULL;
		}
		CopyAdsPt(rb->resval.rpoint,asDblArray(ptArray.at(i)));		

		if (ptList == NULL) {
			ptList = rb;
			lastRb = rb;
		}
		else {
			lastRb->rbnext = rb;
			lastRb = rb;
		}
	}
	return ptList;
}


void CDistSelSet::CopyAdsPt(ads_point pt1, ads_point pt2)
{
	pt1[0] = pt2[0];  //x,y,z
	pt1[1] = pt2[1];
	pt1[2] = pt2[2];
}
