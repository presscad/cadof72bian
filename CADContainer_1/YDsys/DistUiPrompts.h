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

// 文件说明：输入提示交互类（包括各种类型数据的输入，和信息提示的显示）

//使用说明：
//CDistUiPrIntDef temp("\n选择类型 (1)类型A (2)类型B (3)类型C",NULL,CDistUiPrInt::kRange,1); 
//temp.SetRange(1,3);
//if(prIntType.Go()!=CDistUiPrBase::kOk) return 0;
//int4 nType = prIntType.GetValue();


#ifndef __DIST__UIPROMPTS__H__
#define __DIST__UIPROMPTS__H__

class CDistUiPrBase;
class CDistUiPrAngle;
class CDistUiPrAngleDef;
class CDistUiPrCorner;
class CDistUiPrDist;
class CDistUiPrDistDef;
class CDistUiPrDouble;
class CDistUiPrDoubleDef;
class CDistUiPrEntity;
class CDistUiPrInt;
class CDistUiPrIntDef;
class CDistUiPrKeyWord;
class CDistUiPrKeyWordDef;
class CDistUiPrPoint;
class CDistUiPrString;
class CDistUiPrStringDef;


////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrBase 
{

public:

	//执行返回状态
    enum ExecuteStatus 
	{
        kCancel    = 0,
        kNone,          // not returned unless explicitly enabled
        kKeyWord,
        kOk
    };

	//接口函数
    virtual LPCTSTR				GetKeyWords() const;
    virtual Acad::ErrorStatus   SetKeyWords(LPCTSTR keyWordList);

    virtual LPCTSTR		        GetMessage() const;
    virtual Acad::ErrorStatus   SetMessage(LPCTSTR message);

    virtual LPCTSTR             GetKeyWordPicked() const;
    virtual bool		        IsKeyWordPicked(LPCTSTR matchKeyWord);

    virtual ExecuteStatus       Go() = 0;   // issue the prompt

protected:
	//构造和析构
                     CDistUiPrBase(LPCTSTR msg, LPCTSTR keyWordList);
    virtual         ~CDistUiPrBase();    // must be public to call delete on base class pointer

    //成员数据
    CString		m_keyWords;      //
    CString		m_message;
    CString		m_keyWordPicked;

private:
   //禁用函数
                    CDistUiPrBase(const CDistUiPrBase&);
    CDistUiPrBase&	operator=(const CDistUiPrBase&);
};


//////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrAngle : public CDistUiPrBase {

public:

    enum AngleType 
	{
        kAny    = 0,
        kNoZero,
        kRange
    };

                    CDistUiPrAngle(LPCTSTR msg, LPCTSTR keyWordList, AngleType type);
                    CDistUiPrAngle(LPCTSTR msg, LPCTSTR keyWordList, AngleType type, const AcGePoint3d& basePt);
    virtual         ~CDistUiPrAngle();

    virtual ExecuteStatus  Go();       // issue the prompt
    double          GetValue();

    void            SetAngleType(AngleType newType);
    void            SetRange(double minVal, double maxVal);
    void            SetBasePt(const AcGePoint3d& basePt);
    void            SetUseBasePt(bool useIt);
    void            SetUseDashedLine(bool useIt);
    void            SetUnit(int unit);
    void            SetPrecision(int precision);

    void            SetAllowNone(bool allowIt);

protected:
        // helper functions
    bool		    IsInRange();

        // data memebers
    bool			m_allowNone;
    double          m_value;
    double          m_minVal;
    double          m_maxVal;
    AngleType       m_angType;
    AcGePoint3d     m_basePt;
    bool			m_useBasePt;
    bool			m_useDashedLine;
    int             m_unit;
    int             m_precision;

private:
        // helper functions
    void            Init();

        // outlawed functions
						CDistUiPrAngle(const CDistUiPrAngle&);
    CDistUiPrAngle&	operator=(const CDistUiPrAngle&);
};

////////////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrAngleDef : public CDistUiPrAngle 
{

public:
                    CDistUiPrAngleDef(LPCTSTR msg, LPCTSTR keyWordList, AngleType type, double def);
                    CDistUiPrAngleDef(LPCTSTR msg, LPCTSTR keyWordList, AngleType type,
                            const AcGePoint3d& basePt, double def);
    virtual        ~CDistUiPrAngleDef();

    virtual ExecuteStatus  Go();       // issue the prompt

private:
        // data members
    double          m_default;

        // outlawed functions
                        CDistUiPrAngleDef(const CDistUiPrAngleDef&);
    CDistUiPrAngleDef&	operator=(const CDistUiPrAngleDef&);
};


//////////////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrCorner : public CDistUiPrBase {

public:
                        CDistUiPrCorner(LPCTSTR msg, LPCTSTR keyWordList, const AcGePoint3d& basePt);
    virtual             ~CDistUiPrCorner();

    virtual ExecuteStatus      Go();             // issue the prompt
    const AcGePoint3d&  GetValue();

    void                SetBasePt(const AcGePoint3d& basePt);
    void                SetUseDashedLine(bool useIt);
    void                SetNoLimitsCheck(bool noCheck);

protected:
        // data members
    AcGePoint3d         m_value;
    AcGePoint3d         m_basePt;
    bool			    m_useDashedLine;
    bool			    m_noLimCheck;

private:
        // outlawed functions
                        CDistUiPrCorner(const CDistUiPrCorner&);
    CDistUiPrCorner&	operator=(const CDistUiPrCorner&);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrDist : public CDistUiPrBase {

public:
    enum DistType {
        kAny    = 0,
        kNoNeg,
        kNoZero,
        kNoNegNoZero,
        kRange
    };

                CDistUiPrDist(LPCTSTR msg, LPCTSTR keyWordList, DistType type);
                CDistUiPrDist(LPCTSTR msg, LPCTSTR keyWordList, DistType type, const AcGePoint3d& basePt);
    virtual     ~CDistUiPrDist();

    virtual ExecuteStatus  Go();       // issue the prompt
    double          GetValue();

    void            SetDistType(DistType newType);
    void            SetRange(double minVal, double maxVal);
    void            SetBasePt(const AcGePoint3d& basePt);
    void            SetUseBasePt(bool useIt);
    void            SetUseDashedLine(bool useIt);
    void            Set2dOnly(bool only2d);
    void            SetUnit(int unit);
    void            SetPrecision(int precision);

    void            SetAllowNone(bool allowIt);

protected:
        // data memebers
    bool		    m_allowNone;
    double          m_value;
    double          m_minVal;
    double          m_maxVal;
    DistType        m_distType;
    AcGePoint3d     m_basePt;
    bool		    m_useBasePt;
    bool		    m_useDashedLine;
    bool			m_2dOnly;
    int             m_unit;
    int             m_precision;

        // helper functions
    bool		    IsInRange();

private:
        // helper functions
    void        Init();

        // outlawed functions
                    CDistUiPrDist(const CDistUiPrDist&);
    CDistUiPrDist&	operator=(const CDistUiPrDist&);
};


////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrDistDef : public CDistUiPrDist {

public:
                CDistUiPrDistDef(LPCTSTR msg, LPCTSTR keyWordList, DistType type, double def);
                CDistUiPrDistDef(LPCTSTR msg, LPCTSTR keyWordList, DistType type,
                        const AcGePoint3d& basePt, double def);
    virtual     ~CDistUiPrDistDef();

    virtual ExecuteStatus Go();       // issue the prompt

private:
        // data members
    double      m_default;

        // outlawed functions
                        CDistUiPrDistDef(const CDistUiPrDistDef&);
    CDistUiPrDistDef&	operator=(const CDistUiPrDistDef&);
};


/////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrDouble : public CDistUiPrBase {

public:

    enum DoubleType {
        kAny    = 0,
        kNoNeg,
        kNoZero,
        kNoNegNoZero,
        kRange
    };

                    CDistUiPrDouble(LPCTSTR msg, LPCTSTR keyWordList, DoubleType type);
    virtual         ~CDistUiPrDouble();

    virtual ExecuteStatus  Go();             // issue the prompt
    double          GetValue();

    void            SetDoubleType(DoubleType newType);
    void            SetRange(double minVal, double maxVal);

    void            SetUnit(int unit);
    void            SetPrecision(int precision);

protected:
        // data members
    double          m_value;
    double          m_minVal;
    double          m_maxVal;
    DoubleType      m_doubleType;
    int             m_unit;
    int             m_precision;

        // helper functions
    bool		    IsInRange();

private:
        // outlawed functions
						CDistUiPrDouble(const CDistUiPrDouble&);
    CDistUiPrDouble&	operator=(const CDistUiPrDouble&);
};

//////////////////////////////////////////////////////////////////////////////////
class CDistUiPrDoubleDef : public CDistUiPrDouble {

public:
                    CDistUiPrDoubleDef(LPCTSTR msg, LPCTSTR keyWordList, DoubleType type, double def);
    virtual         ~CDistUiPrDoubleDef();

    virtual ExecuteStatus  Go();             // issue the prompt

private:
        // data members
    double          m_default;

        // outlawed functions
							CDistUiPrDoubleDef(const CDistUiPrDoubleDef&);
    CDistUiPrDoubleDef&	operator=(const CDistUiPrDoubleDef&);
};



/////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrEntity : public CDistUiPrBase {

public:
                        CDistUiPrEntity(LPCTSTR msg, LPCTSTR keyWordList);
    virtual             ~CDistUiPrEntity();

    virtual ExecuteStatus      Go();       // issue the prompt

    Acad::ErrorStatus   AddAllowedClass(AcRxClass* classType, bool doIsATest = false);

    bool		        FilterLockedLayers() const;
    Acad::ErrorStatus   SetFilterLockedLayers(bool filterThem);

    AcDbObjectId        GetObjectId() const;
    AcGePoint3d         GetPickPoint() const;

    void                SetAllowNone(bool allowIt);

protected:
        // data members
    bool			    m_filterLockedLayers;
    AcDbVoidPtrArray    m_allowedClassTypes;
    AcDbIntArray        m_doIsATest;
    AcDbObjectId        m_objId;
    bool			    m_allowNone;
    AcGePoint3d         m_pickPt;

        // helper functions
    bool		        CorrectClass(AcDbEntity* ent);

private:
        // outlawed functions
                        CDistUiPrEntity(const CDistUiPrEntity&);
    CDistUiPrEntity&	operator=(const CDistUiPrEntity&);
};

///////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrInt : public CDistUiPrBase {

public:
        // type restricts input to an appropriate range
    enum IntType {
        kAny    = 0,
        kNoNeg,
        kNoZero,
        kNoNegNoZero,
        kRange
    };

                    CDistUiPrInt(LPCTSTR msg, LPCTSTR keyWordList, IntType type);
    virtual         ~CDistUiPrInt();

    virtual ExecuteStatus  Go();             // issue the prompt
    int             GetValue();

    virtual void    SetRange(int minVal, int maxVal);
    virtual void    SetType(IntType type);

protected:
        // data members
    IntType        m_intType;
    int            m_value;
    int            m_minVal;
    int            m_maxVal;

        // helper functions
    bool		   IsInRange();

private:
        // outlawed functions
					CDistUiPrInt(const CDistUiPrInt&);
    CDistUiPrInt&	operator=(const CDistUiPrInt&);
};


////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrIntDef : public CDistUiPrInt {

public:
                    CDistUiPrIntDef(LPCTSTR msg, LPCTSTR keyWordList, IntType type, int def);
    virtual         ~CDistUiPrIntDef();

    virtual ExecuteStatus  Go();             // issue the prompt

private:
        // data members
    int             m_default;

        // outlawed functions
						CDistUiPrIntDef(const CDistUiPrIntDef&);
    CDistUiPrIntDef&	operator=(const CDistUiPrIntDef&);
};



///////////////////////////////////////////////////////////////////////////////////
class CDistUiPrKeyWord : public CDistUiPrBase {

public:
                    CDistUiPrKeyWord(LPCTSTR msg, LPCTSTR keyWordList);
    virtual         ~CDistUiPrKeyWord();

    virtual ExecuteStatus  Go();                           // issue the prompt
    LPCTSTR         GetValue();                        // for consistency

    void            SetAllowNone(bool allowIt);

protected:
    bool			m_allowNone;

private:
        // outlawed functions
                        CDistUiPrKeyWord(const CDistUiPrKeyWord&);
    CDistUiPrKeyWord&	operator=(const CDistUiPrKeyWord&);
};


//////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrKeyWordDef : public CDistUiPrKeyWord {

public:
                    CDistUiPrKeyWordDef(LPCTSTR msg, LPCTSTR keyWordList, LPCTSTR def);
    virtual         ~CDistUiPrKeyWordDef();

    virtual ExecuteStatus  Go();             // issue the prompt

private:
    CString    m_default;

    bool	   IsKeyWordMatch();

        // outlawed functions
							CDistUiPrKeyWordDef(const CDistUiPrKeyWordDef&);
    CDistUiPrKeyWordDef&	operator=(const CDistUiPrKeyWordDef&);
};


/////////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrPoint : public CDistUiPrBase {

public:
                        CDistUiPrPoint(LPCTSTR msg, LPCTSTR keyWordList);
                        CDistUiPrPoint(LPCTSTR msg, LPCTSTR keyWordList, const AcGePoint3d& basePt);
    virtual             ~CDistUiPrPoint();

    virtual ExecuteStatus      Go();             // issue the prompt
    const AcGePoint3d&  GetValue()    const;

    const AcGePoint3d&  GetBasePt() const;
    void                SetBasePt(const AcGePoint3d& basePt);
    void                SetUseBasePt(bool useIt);
    void                SetUseDashedLine(bool useIt);
    void                SetNoLimitsCheck(bool noCheck);
    void                SetAllowNone(bool allowIt);

protected:
    AcGePoint3d         m_value;
    AcGePoint3d         m_basePt;
    bool		        m_useBasePt;
    bool		        m_useDashedLine;
    bool			    m_noLimCheck;
    bool			    m_allowNone;

private:
        // helper functions
    void                Init();

        // outlawed functions
                        CDistUiPrPoint(const CDistUiPrPoint&);
    CDistUiPrPoint&	operator=(const CDistUiPrPoint&);
};


/////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrString : public CDistUiPrBase {

public:
                    CDistUiPrString(LPCTSTR msg, bool allowSpaces = true);
    virtual         ~CDistUiPrString();

    virtual ExecuteStatus  Go();       // issue the prompt
    LPCTSTR         GetValue();

        // overridden because this class does not support keywords
    virtual LPCTSTR				GetKeyWords() const;
    virtual Acad::ErrorStatus   SetKeyWords(LPCTSTR keyWordList);

    virtual LPCTSTR             GetKeyWordPicked();
    virtual bool		        IsKeyWordPicked(LPCTSTR matchKeyWord);

protected:
    CString		m_value;
    bool		m_allowSpaces;

private:
        // outlawed functions
						CDistUiPrString(const CDistUiPrString&);
    CDistUiPrString&	operator=(const CDistUiPrString&);
};

/////////////////////////////////////////////////////////////////////////////////////
class CDistUiPrStringDef : public CDistUiPrString {

public:
                    CDistUiPrStringDef(LPCTSTR msg, LPCTSTR def, bool allowSpaces = true);
    virtual         ~CDistUiPrStringDef();

    virtual ExecuteStatus  Go();    // issue the prompt

private:
        // data members
    CString		m_default;

        // outlawed functions
							CDistUiPrStringDef(const CDistUiPrStringDef&);
    CDistUiPrStringDef&	operator=(const CDistUiPrStringDef&);
};


#endif //  __DIST__UIPROMPTS__H__



