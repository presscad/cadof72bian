#include "StdAfx.h"
//#include "CDistUtils.h"
#include "DistUiPrompts.h"
#include <ol_errno.h>


bool IsOnLockedLayer(AcDbEntity* ent, bool printMsg)
{
	AcDbObject* obj;
	AcDbLayerTableRecord* layer;
	bool isLocked = false;
	Acad::ErrorStatus es;

	es = acdbOpenAcDbObject(obj, ent->layerId(), AcDb::kForRead);
	if (es == Acad::eOk) {
		layer = AcDbLayerTableRecord::cast(obj);
		if (layer)
			isLocked = layer->isLocked();
		else {
			ASSERT(0);
		}
		obj->close();
	}
	else {
		ASSERT(0);
	}

	if (isLocked && printMsg) {
		acutPrintf(_T("\nSelected entity is on a locked layer."));
	}

	return isLocked;
}

Acad::ErrorStatus GetSysVar(LPCTSTR varName, int& val)
{
	resbuf rb;
	if (acedGetVar(varName, &rb)== RTNORM) {
		ASSERT(rb.restype == RTSHORT);
		val = rb.resval.rint;
		return(Acad::eOk);
	}
	else
		return(Acad::eInvalidInput);
}


Acad::ErrorStatus SetSysVar(LPCTSTR varName, const AcGePoint3d& val)
{
	ASSERT(varName != NULL);

	resbuf rb;
	rb.restype = RT3DPOINT;
	rb.resval.rpoint[0] = val.x;
	rb.resval.rpoint[1] = val.y;
	rb.resval.rpoint[2] = val.z;

	if (acedSetVar(varName, &rb) != RTNORM) {
		return Acad::eInvalidInput;
	}
	else
		return Acad::eOk;
}



////////////////////////// CDistUiPrBase///////////////////////////////////
CDistUiPrBase::CDistUiPrBase(LPCTSTR msg, LPCTSTR keyWordList)
:	m_keyWords(keyWordList),m_message(msg)
{
    ASSERT(msg != NULL);
}

CDistUiPrBase::~CDistUiPrBase()
{
}

LPCTSTR CDistUiPrBase::GetKeyWords() const
{
   return m_keyWords;
}

Acad::ErrorStatus
CDistUiPrBase::SetKeyWords(LPCTSTR keyWordList)
{
    ASSERT(keyWordList != NULL);

	m_keyWords = keyWordList;

    return Acad::eOk;
}

LPCTSTR
CDistUiPrBase::GetMessage() const
{
   return m_message;
}

Acad::ErrorStatus
CDistUiPrBase::SetMessage(LPCTSTR msg)
{
    m_message = msg;
    return Acad::eOk;
}

LPCTSTR
CDistUiPrBase::GetKeyWordPicked() const
{
    ASSERT(m_keyWordPicked.IsEmpty() == false);
    return m_keyWordPicked;
}

bool
CDistUiPrBase::IsKeyWordPicked(LPCTSTR keyWordMatch)
{
    ASSERT(m_keyWordPicked.IsEmpty() == false);
    ASSERT(keyWordMatch != NULL);

    if (m_keyWordPicked == keyWordMatch)
        return true;
    else
        return false;
}

CDistUiPrBase::ExecuteStatus CDistUiPrAngle::Go()
{
    CString prompt;
    int initFlag = 0;

    // set up prompt
    prompt.Format(_T("\n%s: "), GetMessage());

    // set up init flag
    if (m_allowNone == false)
        initFlag += RSG_NONULL;

    if (m_angType == kNoZero)
        initFlag += RSG_NOZERO;

    int result;
    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        if (m_useBasePt)
            result = acedGetOrient(asDblArray(m_basePt), prompt, &m_value);
        else
            result = acedGetOrient(NULL, prompt, &m_value);

        if (result == RTNORM) {
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}


//////////////////////////////////CDistUiPrAngle///////////////////////////
void
CDistUiPrAngle::Init()
{
    m_value = 0.0;
    m_minVal = 0.0;
    m_maxVal = 0.0;
    m_useDashedLine = false;
    m_unit = -1;		// use current setting
    m_precision = -1;	// use current setting
    m_allowNone = false;
}

CDistUiPrAngle::CDistUiPrAngle(LPCTSTR msg, LPCTSTR keyWordList, AngleType type)
:   CDistUiPrBase(msg, keyWordList),
    m_angType(type),
    m_basePt(AcGePoint3d::kOrigin),
    m_useBasePt(false)
{
    Init();
}

CDistUiPrAngle::CDistUiPrAngle(LPCTSTR msg, LPCTSTR keyWordList, AngleType type, const AcGePoint3d& basePt)
:   CDistUiPrBase(msg, keyWordList),
    m_angType(type),
    m_basePt(basePt),
    m_useBasePt(true)
{
    Init();
}

CDistUiPrAngle::~CDistUiPrAngle()
{
}



void
CDistUiPrAngle::SetRange(double minVal, double maxVal)
{
    ASSERT(maxVal >= minVal);

    m_minVal = minVal;
    m_maxVal = maxVal;
}

bool
CDistUiPrAngle::IsInRange()
{
    if (m_angType == CDistUiPrAngle::kRange) {
        ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
        if ((m_value >= m_minVal)&&(m_value <= m_maxVal))
            return true;
        else {
            char str1[512], str2[512];
            acdbAngToS(m_minVal, m_unit, m_precision, str1);
            acdbAngToS(m_maxVal, m_unit, m_precision, str2);
            acutPrintf(_T("\n输入值必须在 %s 到 %s 范围内."), str1, str2);
            return false;
        }
    }
    else
        return true;
}

double
CDistUiPrAngle::GetValue()
{
    return m_value;
}

void
CDistUiPrAngle::SetAngleType(AngleType newType)
{
    m_angType = newType;
}

void
CDistUiPrAngle::SetBasePt(const AcGePoint3d& basePt)
{
    m_basePt = basePt;
    m_useBasePt = true;
}

void
CDistUiPrAngle::SetUseBasePt(bool useIt)
{
    m_useBasePt = useIt;
}

void
CDistUiPrAngle::SetUseDashedLine(bool useIt)
{
    m_useDashedLine = useIt;
}

void
CDistUiPrAngle::SetUnit(int unit)
{
    m_unit = unit;
}

void
CDistUiPrAngle::SetPrecision(int precision)
{
    m_precision = precision;
}

void
CDistUiPrAngle::SetAllowNone(bool allowIt)
{
    m_allowNone = allowIt;
}



////////////////////////// CDistUiPrAngleDef /////////////////////////////////
CDistUiPrAngleDef::CDistUiPrAngleDef(LPCTSTR msg, LPCTSTR keyWordList, AngleType type, double def)
:   CDistUiPrAngle(msg, keyWordList, type)
{
    m_default = def;
}

CDistUiPrAngleDef::CDistUiPrAngleDef(LPCTSTR msg, LPCTSTR keyWordList, AngleType type,
            const AcGePoint3d& basePt, double def)
:   CDistUiPrAngle(msg, keyWordList, type, basePt)
{
    m_default = def;
}

CDistUiPrAngleDef::~CDistUiPrAngleDef()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrAngleDef::Go()
{
    CString prompt;
    char defStr[512];
    int initFlag;

        // set up prompt
    acdbAngToS(m_default, m_unit, m_precision, defStr);
    prompt.Format(_T("\n%s<%s>: "), GetMessage(), defStr);

        // set up init flag
    if (m_angType == kNoZero)
        initFlag = RSG_NOZERO;
    else
        initFlag = 0;

    int result;
    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        if (m_useBasePt)
            result = acedGetOrient(asDblArray(m_basePt), prompt, &m_value);
        else
            result = acedGetOrient(NULL, prompt, &m_value);

        if (result == RTNORM) {
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if(result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else if (result == RTNONE) {
            if (m_angType == CDistUiPrAngle::kRange) {
                ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
                ASSERT((m_default >= m_minVal) && (m_default <= m_maxVal));
            }
            m_value = m_default;
            return CDistUiPrBase::kOk;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}


///////////////////////////////// CDistUiPrCorner /////////////////////////////
CDistUiPrCorner::CDistUiPrCorner(LPCTSTR msg, LPCTSTR keyWordList, const AcGePoint3d& basePt)
:   CDistUiPrBase(msg, keyWordList),
    m_basePt(basePt),
    m_value(AcGePoint3d::kOrigin),
	m_useDashedLine(false),
	m_noLimCheck(false)
{
}

CDistUiPrCorner::~CDistUiPrCorner()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrCorner::Go()
{
    CString prompt;
    int result;
    ads_point adsPt;
    int initFlag = RSG_NONULL;

    if (m_noLimCheck == true)
        initFlag += RSG_NOLIM;
    if (m_useDashedLine == true)
        initFlag += RSG_DASH;

    prompt.Format(_T("\n%s: "), GetMessage());

    acedInitGet(initFlag, GetKeyWords());
    result = acedGetCorner(asDblArray(m_basePt), prompt, adsPt);

    if (result == RTNORM) {
        m_value = asPnt3d(adsPt);
        return CDistUiPrBase::kOk;
    }
    else if (result == RTKWORD) {
        acedGetInput(m_keyWordPicked.GetBuffer(512));
        m_keyWordPicked.ReleaseBuffer();
        return CDistUiPrBase::kKeyWord;
    }
    else
        return CDistUiPrBase::kCancel;
}

void
CDistUiPrCorner::SetBasePt(const AcGePoint3d& basePt)
{
    m_basePt = basePt;
}

void
CDistUiPrCorner::SetUseDashedLine(bool useIt)
{
    m_useDashedLine = useIt;
}

void
CDistUiPrCorner::SetNoLimitsCheck(bool noCheck)
{
    m_noLimCheck = noCheck;
}


const AcGePoint3d&
CDistUiPrCorner::GetValue()
{
    return m_value;
}



////////////////////////////////// CDistUiPrDist //////////////////////////////////
void
CDistUiPrDist::Init()
{
    m_value = 0.0;
    m_minVal = 0.0;
    m_maxVal = 0.0;
    m_useDashedLine = false;
    m_2dOnly = false;
    m_unit = -1;
    m_precision = -1;
    m_allowNone = false;
}

CDistUiPrDist::CDistUiPrDist(LPCTSTR msg, LPCTSTR keyWordList, DistType type)
:   CDistUiPrBase(msg, keyWordList),
    m_distType(type),
    m_basePt(AcGePoint3d::kOrigin),
    m_useBasePt(false)
{
    Init();
}

CDistUiPrDist::CDistUiPrDist(LPCTSTR msg, LPCTSTR keyWordList, DistType type, const AcGePoint3d& basePt)
:   CDistUiPrBase(msg, keyWordList),
    m_distType(type),
    m_basePt(basePt),
    m_useBasePt(true)
{
    Init();
}

CDistUiPrDist::~CDistUiPrDist()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrDist::Go()
{
    CString prompt;
    int initFlag = 0;

        // set up prompt
    prompt.Format(_T("\n%s: "), GetMessage());

        // set up init flag
    if (m_allowNone == false)
        initFlag += RSG_NONULL;

    if (m_distType == kNoZero)
        initFlag += RSG_NOZERO;
    else if (m_distType == kNoNeg)
        initFlag += RSG_NONEG;
    else if (m_distType == kNoNegNoZero)
        initFlag += (RSG_NOZERO+RSG_NONEG);

    int result;
    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        if (m_useBasePt)
            result = acedGetDist(asDblArray(m_basePt), prompt, &m_value);
        else
            result = acedGetDist(NULL, prompt, &m_value);

        if (result == RTNORM) {
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}

void
CDistUiPrDist::SetRange(double minVal, double maxVal)
{
    ASSERT(maxVal >= minVal);

    m_minVal = minVal;
    m_maxVal = maxVal;
}

bool
CDistUiPrDist::IsInRange()
{
    if (m_distType == CDistUiPrDist::kRange) {
        ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
        if ((m_value >= m_minVal) && (m_value <= m_maxVal))
            return true;
        else {
            char str1[512], str2[512];
            acdbRToS(m_minVal, m_unit, m_precision, str1);
            acdbRToS(m_maxVal, m_unit, m_precision, str2);
            acutPrintf(_T("\n输入值必须在 %s 到 %s 范围内."), str1, str2);
            return false;
        }
    }
    else
        return true;
}

double
CDistUiPrDist::GetValue()
{
    return m_value;
}

void
CDistUiPrDist::SetDistType(DistType newType)
{
    m_distType = newType;
}

void
CDistUiPrDist::SetBasePt(const AcGePoint3d& basePt)
{
    m_basePt = basePt;
    m_useBasePt = Adesk::kTrue;
}

void
CDistUiPrDist::SetUseBasePt(bool useIt)
{
    m_useBasePt = useIt;
}

void
CDistUiPrDist::SetUseDashedLine(bool useIt)
{
    m_useDashedLine = useIt;
}

void
CDistUiPrDist::Set2dOnly(bool only2d)
{
    m_2dOnly = only2d;
}

void
CDistUiPrDist::SetUnit(int unit)
{
    m_unit = unit;
}

void
CDistUiPrDist::SetPrecision(int precision)
{
    m_precision = precision;
}

void
CDistUiPrDist::SetAllowNone(bool allowIt)
{
    m_allowNone = allowIt;
}



////////////////////////////////CDistUiPrDistDef////////////////////////////////
CDistUiPrDistDef::CDistUiPrDistDef(LPCTSTR msg, LPCTSTR keyWordList, DistType type, double def)
:   CDistUiPrDist(msg, keyWordList, type)
{
    m_default = def;
}

CDistUiPrDistDef::CDistUiPrDistDef(LPCTSTR msg, LPCTSTR keyWordList, DistType type,
            const AcGePoint3d& basePt, double def)
:   CDistUiPrDist(msg, keyWordList, type, basePt)
{
    m_default = def;
}

CDistUiPrDistDef::~CDistUiPrDistDef()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrDistDef::Go()
{
    CString prompt;
    char defStr[512];
    int initFlag;

        // set up prompt
    acdbRToS(m_default, m_unit, m_precision, defStr);
    prompt.Format(_T("\n%s<%s>: "), GetMessage(), defStr);

        // set up init flag
    if (m_distType == kNoZero)
        initFlag = RSG_NOZERO;
    else if (m_distType == kNoNeg)
        initFlag = RSG_NONEG;
    else if (m_distType == kNoNegNoZero)
        initFlag = RSG_NOZERO+RSG_NONEG;
    else
        initFlag = 0;

    int result;
    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        if (m_useBasePt)
            result = acedGetDist(asDblArray(m_basePt), prompt, &m_value);
        else
            result = acedGetDist(NULL, prompt, &m_value);

        if (result == RTNORM){
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else if (result == RTNONE) {
            if (m_distType == CDistUiPrDist::kRange) {
                ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
                ASSERT((m_default >= m_minVal) && (m_default <= m_maxVal));
            }
            m_value = m_default;
            return CDistUiPrBase::kOk;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}



//////////////////////////////////// CDistUiPrDouble ///////////////////////////
CDistUiPrDouble::CDistUiPrDouble(LPCTSTR msg, LPCTSTR keyWordList, DoubleType type)
:   CDistUiPrBase(msg, keyWordList),
    m_doubleType(type),
    m_value(0.0),
    m_minVal(0.0),
    m_maxVal(0.0),
    m_unit(-1),
    m_precision(-1)
{
}

CDistUiPrDouble::~CDistUiPrDouble()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrDouble::Go()
{
    CString prompt;
    int initFlag;

        // set up prompt
    prompt.Format(_T("\n%s: "), GetMessage());

        // set up init flag
    if (m_doubleType == kNoZero)
        initFlag = RSG_NONULL+RSG_NOZERO;
    else if (m_doubleType == kNoNeg)
        initFlag = RSG_NONULL+RSG_NONEG;
    else if (m_doubleType == kNoNegNoZero)
        initFlag = RSG_NONULL+RSG_NOZERO+RSG_NONEG;
    else
        initFlag = RSG_NONULL;

    int result;
    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        result = acedGetReal(prompt, &m_value);

        if (result == RTNORM) {
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}

void
CDistUiPrDouble::SetRange(double minVal, double maxVal)
{
    ASSERT(maxVal >= minVal);

    m_minVal = minVal;
    m_maxVal = maxVal;
}

bool
CDistUiPrDouble::IsInRange()
{
    if (m_doubleType == CDistUiPrDouble::kRange) {
        ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
        if ((m_value >= m_minVal)&&(m_value <= m_maxVal))
            return true;
        else {
            char str1[512], str2[512];
            acdbRToS(m_minVal, m_unit, m_precision, str1);
            acdbRToS(m_maxVal, m_unit, m_precision, str2);
            acutPrintf("\n输入值必须在 %s 到 %s 范围内.", str1, str2);
            return false;
        }
    }
    else
        return true;
}

double
CDistUiPrDouble::GetValue()
{
    return m_value;
}

void
CDistUiPrDouble::SetDoubleType(DoubleType newType)
{
    m_doubleType = newType;
}

void
CDistUiPrDouble::SetUnit(int unit)
{
    m_unit = unit;
}

void
CDistUiPrDouble::SetPrecision(int precision)
{
    m_precision = precision;
}



///////////////////////////////////// CDistUiPrDoubleDef /////////////////////////////
CDistUiPrDoubleDef::CDistUiPrDoubleDef(LPCTSTR msg, LPCTSTR keyWordList, DoubleType type, double def)
:   CDistUiPrDouble(msg, keyWordList, type)
{
    m_default = def;
}

CDistUiPrDoubleDef::~CDistUiPrDoubleDef()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrDoubleDef::Go()
{
    CString prompt;
    char defStr[512];
    int initFlag;

        // set up prompt
    acdbRToS(m_default, m_unit, m_precision, defStr);
    prompt.Format(_T("\n%s<%s>: "), GetMessage(), defStr);

        // set up init flag
    if (m_doubleType == kNoZero)
        initFlag = RSG_NOZERO;
    else if (m_doubleType == kNoNeg)
        initFlag = RSG_NONEG;
    else if (m_doubleType == kNoNegNoZero)
        initFlag = RSG_NOZERO+RSG_NONEG;
    else
        initFlag = 0;

    int result;
    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        result = acedGetReal(prompt, &m_value);

        if (result == RTNORM){
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else if (result == RTNONE) {
            if (m_doubleType == CDistUiPrDouble::kRange) {
                ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
                ASSERT((m_default >= m_minVal) && (m_default <= m_maxVal));
            }
            m_value = m_default;
            return CDistUiPrBase::kOk;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}



////////////////////////////// CDistUiPrEntity ////////////////////////////////
CDistUiPrEntity::CDistUiPrEntity(LPCTSTR msg, LPCTSTR keyWordList)
:   CDistUiPrBase(msg, keyWordList),
    m_allowNone(false),
    m_objId(AcDbObjectId::kNull),
    m_filterLockedLayers(false)
{
}


CDistUiPrEntity::~CDistUiPrEntity()
{
}

Acad::ErrorStatus
CDistUiPrEntity::AddAllowedClass(AcRxClass* classType, bool doIsATest)
{
    ASSERT(classType != NULL);

    if (m_allowedClassTypes.contains(classType))
        return Acad::eDuplicateKey;

    m_allowedClassTypes.append(classType);
    m_doIsATest.append(static_cast<int>(doIsATest));

    return Acad::eOk;
}

bool
CDistUiPrEntity::FilterLockedLayers() const
{
    return m_filterLockedLayers;
}

Acad::ErrorStatus
CDistUiPrEntity::SetFilterLockedLayers(bool filterThem)
{
    m_filterLockedLayers = filterThem;

    return Acad::eOk;
}

CDistUiPrBase::ExecuteStatus
CDistUiPrEntity::Go()
{
    CString prompt;
    int result;
    int errNum;
    ads_point adsPt;
    ads_name ent;
    AcDbObjectId tmpId;
    AcDbEntity* tmpEnt;
    Acad::ErrorStatus es;

    prompt.Format(_T("\n%s: "), GetMessage());

    while (1) {
        acedInitGet(0, GetKeyWords());
        result = acedEntSel(prompt, ent, adsPt);

        if (result == RTNORM) {
     
			es =acdbGetObjectId(tmpId, ent);
			ASSERT(es == Acad::eOk);
            es = acdbOpenAcDbEntity(tmpEnt, tmpId, AcDb::kForRead);
            if (es == Acad::eOk) {
                    // if its correct class and we are not filtering locked layers its ok,
                    // or if we are filtering locked layers and this one isn't on a locked layer
                if (CorrectClass(tmpEnt)) {     // correctClass() will print error msg
                    if ((!m_filterLockedLayers) ||
                        (IsOnLockedLayer(tmpEnt, true) == false)) {    // isOnLockedLayer() will print error msg
                        tmpEnt->close();
                        m_pickPt = asPnt3d(adsPt);
                        m_objId = tmpId;
                        return CDistUiPrBase::kOk;
                    }
                }
                tmpEnt->close();    // close and loop again until they get it right!
            }
            else {
                ASSERT(0);
                return CDistUiPrBase::kCancel;
            }
        }
        else if (result == RTERROR) {
            GetSysVar("errno", errNum);
            if (errNum == OL_ENTSELPICK)            // picked but didn't get anything
                acutPrintf(_T("\n没有选择."));
            else if (errNum == OL_ENTSELNULL) {     // hit RETURN or SPACE
                if (m_allowNone)
                    return CDistUiPrBase::kNone;      // prompt specifically wants to know about None
                else
                    return CDistUiPrBase::kCancel;    // prompt wants to bail on None
            }
            else
                acutPrintf(_T("\n没有选择."));
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}

bool
CDistUiPrEntity::CorrectClass(AcDbEntity* ent)
{
    if (m_allowedClassTypes.isEmpty())
        return Adesk::kTrue;

    AcRxClass* rxClass;
    int len = m_allowedClassTypes.length();
    bool isOk = false;
    for (int i=0; i<len; i++) {
        rxClass = static_cast<AcRxClass*>(m_allowedClassTypes[i]);
        if (m_doIsATest[i]) {
            if (ent->isA() == rxClass)
                isOk = true;
        }
        else {
            if (ent->isKindOf(rxClass))
                isOk = true;
        }
    }

    if (isOk)
        return true;

        // print out error message with allowed types
    CString types;
    CString str;
    for (i=0; i<len;i++) {
        if (i > 0)
            types += _T(", ");

        rxClass = static_cast<AcRxClass*>(m_allowedClassTypes[i]);
        types += rxClass->name();
    }

    acutPrintf(_T("\n选择实体类型必须为: %s"), types);

    return false;
}

AcDbObjectId
CDistUiPrEntity::GetObjectId() const
{
    return m_objId;
}

AcGePoint3d
CDistUiPrEntity::GetPickPoint() const
{
    return m_pickPt;
}

void
CDistUiPrEntity::SetAllowNone(bool allowIt)
{
    m_allowNone = allowIt;
}



/////////////////////////////// CDistUiPrInt //////////////////////////////////
CDistUiPrInt::CDistUiPrInt(LPCTSTR msg, LPCTSTR keyWordList, IntType type)
:   CDistUiPrBase(msg, keyWordList),m_intType(type),m_value(0),m_minVal(0),m_maxVal(0)
{
}

CDistUiPrInt::~CDistUiPrInt()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrInt::Go()
{
    CString prompt;
    int initFlag;

        // set up prompt
    prompt.Format(_T("\n%s: "), GetMessage());

        // set up init flag
    if (m_intType == kNoZero)
        initFlag = RSG_NONULL+RSG_NOZERO;
    else if (m_intType == kNoNeg)
        initFlag = RSG_NONULL+RSG_NONEG;
    else if (m_intType == kNoNegNoZero)
        initFlag = RSG_NONULL+RSG_NOZERO+RSG_NONEG;
    else
        initFlag = RSG_NONULL;

    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        int result = acedGetInt(prompt, &m_value);

        if (result == RTNORM) {
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD){
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}

void
CDistUiPrInt::SetRange(int minVal, int maxVal)
{
    ASSERT(maxVal >= minVal);

    m_minVal = minVal;
    m_maxVal = maxVal;
}

bool
CDistUiPrInt::IsInRange()
{
    if (m_intType == CDistUiPrInt::kRange) {
        if ((m_value >= m_minVal) && (m_value <= m_maxVal))
            return true;
        else {
		    acutPrintf(_T("\n输入值必须在 %d 到 %d 范围内"), m_minVal, m_maxVal);
            return false;
        }
    }
    else
        return true;
}

int
CDistUiPrInt::GetValue()
{
    return m_value;
}

void
CDistUiPrInt::SetType(IntType type)
{
    m_intType = type;
}



//////////////////////////////// CDistUiPrIntDef //////////////////////////////////
CDistUiPrIntDef::CDistUiPrIntDef(LPCTSTR msg, LPCTSTR keyWordList, IntType type, int def)
:   CDistUiPrInt(msg, keyWordList, type),
    m_default(def)
{
}

CDistUiPrIntDef::~CDistUiPrIntDef()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrIntDef::Go()
{
    CString prompt;
    int initFlag;

        // set up prompt
    prompt.Format(_T("\n%s<%d>: "), GetMessage(), m_default);

        // set up init flag
    if (m_intType == kNoZero)
        initFlag = RSG_NOZERO;
    else if (m_intType == kNoNeg)
        initFlag = RSG_NONEG;
    else if (m_intType == kNoNegNoZero)
        initFlag = RSG_NOZERO+RSG_NONEG;
    else
        initFlag = 0;

    while (1) {
        acedInitGet(initFlag, GetKeyWords());
        int result = acedGetInt(prompt, &m_value);

        if (result == RTNORM) {
            if (IsInRange())
                return CDistUiPrBase::kOk;
        }
        else if (result == RTKWORD) {
            acedGetInput(m_keyWordPicked.GetBuffer(512));
            m_keyWordPicked.ReleaseBuffer();
            return CDistUiPrBase::kKeyWord;
        }
        else if (result == RTNONE) {
            if (m_intType == CDistUiPrInt::kRange) {
                ASSERT(m_minVal != m_maxVal);    // make sure they set ranges!
                ASSERT((m_default >= m_minVal) && (m_default <= m_maxVal));
            }
            m_value = m_default;
            return CDistUiPrBase::kOk;
        }
        else
            return CDistUiPrBase::kCancel;
    }
}



//////////////////////////////////CDistUiPrKeyWord/////////////////////////////
CDistUiPrKeyWord::CDistUiPrKeyWord(LPCTSTR msg, LPCTSTR keyWordList)
:   CDistUiPrBase(msg, keyWordList),
    m_allowNone(false)
{
}

CDistUiPrKeyWord::~CDistUiPrKeyWord()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrKeyWord::Go()
{
    CString prompt;

    prompt.Format(_T("\n%s: "), GetMessage());

    int initFlag = 0;
    if (m_allowNone == false)
        initFlag += RSG_NONULL;

    acedInitGet(initFlag, GetKeyWords());
    int result = acedGetKword(prompt, m_keyWordPicked.GetBuffer(512));
    m_keyWordPicked.ReleaseBuffer();

    if (result == RTNORM)
        return CDistUiPrBase::kOk;
    else if (result == RTNONE) {
        ASSERT(m_allowNone == Adesk::kTrue);
        return CDistUiPrBase::kNone;
    }
    else
        return CDistUiPrBase::kCancel;
}

LPCTSTR
CDistUiPrKeyWord::GetValue()
{
    return CDistUiPrBase::GetKeyWordPicked();
}


void
CDistUiPrKeyWord::SetAllowNone(bool allowIt)
{
    m_allowNone = allowIt;
}



//////////////////////////////// CDistUiPrKeyWordDef ////////////////////////
CDistUiPrKeyWordDef::CDistUiPrKeyWordDef(LPCTSTR msg, LPCTSTR keyWordList, LPCTSTR def)
:   CDistUiPrKeyWord(msg, keyWordList),
    m_default(def)
{
    ASSERT(def != NULL);
}

CDistUiPrKeyWordDef::~CDistUiPrKeyWordDef()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrKeyWordDef::Go()
{
    CString prompt;

    prompt.Format(_T("\n%s<%s>: "), GetMessage(), m_default);

    acedInitGet(0, GetKeyWords());
    int result = acedGetKword(prompt, m_keyWordPicked.GetBuffer(512));
    m_keyWordPicked.ReleaseBuffer();

    if (result == RTNORM) {
        return CDistUiPrBase::kOk;
    }
    else if (result == RTNONE) {
        if (IsKeyWordMatch() == Adesk::kTrue)
            return CDistUiPrBase::kOk;
        else
            return CDistUiPrBase::kCancel;
    }
    else
        return CDistUiPrBase::kCancel;
}

bool
CDistUiPrKeyWordDef::IsKeyWordMatch()
{
    CString kword;
    CString keyWordList = GetKeyWords();
    int defLen = m_default.GetLength();
    int kwordLen = keyWordList.GetLength();
    int i = 0;
    while (1) {
            // parse out an individual keyword
        kword.Empty();
        while ((i < kwordLen) && (keyWordList[i] != _T(' ')))
            kword += keyWordList[i++];

            // see if it matches the default
        if (!_tcsncmp(kword, m_default, defLen)) {
            m_keyWordPicked = kword;
            return true;
        }
        if (i >= kwordLen) {
            ASSERT(0);    // should never happen
            return false;
        }
        else {
            while (keyWordList[i] == ' ') // chew any whitespace between words
                i++;
        }
    }
}



///////////////////////////////// CDistUiPrPoint //////////////////////////////
void
CDistUiPrPoint::Init()
{
    m_value = AcGePoint3d::kOrigin;
    m_useDashedLine = false;
    m_noLimCheck = false;
    m_allowNone = false;
}

CDistUiPrPoint::CDistUiPrPoint(LPCTSTR msg, LPCTSTR keyWordList)
:   CDistUiPrBase(msg, keyWordList),
    m_basePt(AcGePoint3d::kOrigin),
    m_useBasePt(false)
{
    Init();
}


CDistUiPrPoint::CDistUiPrPoint(LPCTSTR msg, LPCTSTR keyWordList, const AcGePoint3d& basePt)
:   CDistUiPrBase(msg, keyWordList),
    m_basePt(basePt),
    m_useBasePt(true)
{
    Init();
}

CDistUiPrPoint::~CDistUiPrPoint()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrPoint::Go()
{
    CString prompt;
    int result;
    ads_point adsPt;
    int initFlag = 0;

    if (m_allowNone == Adesk::kFalse)
        initFlag += RSG_NONULL;
    if (m_noLimCheck == Adesk::kTrue)
        initFlag += RSG_NOLIM;
    if (m_useDashedLine == Adesk::kTrue)
        initFlag += RSG_DASH;

    prompt.Format(_T("\n%s: "), GetMessage());

    acedInitGet(initFlag, GetKeyWords());
    if (m_useBasePt)
        result = acedGetPoint(asDblArray(m_basePt), prompt, adsPt);
    else
        result = acedGetPoint(NULL, prompt, adsPt);

    if (result == RTNORM){
        m_value = asPnt3d(adsPt);
        SetSysVar(_T("LASTPOINT"), m_value);
        return CDistUiPrBase::kOk;
    }
    else if (result == RTNONE){
        ASSERT(m_allowNone == Adesk::kTrue);
        return CDistUiPrBase::kNone;
    }
    else if (result == RTKWORD){
        acedGetInput(m_keyWordPicked.GetBuffer(512));
        m_keyWordPicked.ReleaseBuffer();
        return CDistUiPrBase::kKeyWord;
    }
    else
        return CDistUiPrBase::kCancel;
}

const AcGePoint3d&
CDistUiPrPoint::GetValue() const
{
    return m_value;
}

const AcGePoint3d&
CDistUiPrPoint::GetBasePt() const
{
    return m_basePt;
}

void
CDistUiPrPoint::SetBasePt(const AcGePoint3d& basePt)
{
    m_basePt = basePt;
    m_useBasePt = true;
}

void
CDistUiPrPoint::SetUseBasePt(bool useIt)
{
    m_useBasePt = useIt;
}

void
CDistUiPrPoint::SetUseDashedLine(bool useIt)
{
    m_useDashedLine = useIt;
}

void
CDistUiPrPoint::SetNoLimitsCheck(bool noCheck)
{
    m_noLimCheck = noCheck;
}

void
CDistUiPrPoint::SetAllowNone(bool allowIt)
{
    m_allowNone = allowIt;
}


//////////////////////////// CDistUiPrString /////////////////////////////////////
CDistUiPrString::CDistUiPrString(LPCTSTR msg, bool allowSpaces)
:   CDistUiPrBase(msg, NULL),
	m_allowSpaces(allowSpaces)
{
}

CDistUiPrString::~CDistUiPrString()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrString::Go()
{
    CString prompt;
    char val[512];
    int result;

    prompt.Format(_T("\n%s: "), GetMessage());

    if (m_allowSpaces == true)
        result = acedGetString(1, prompt, val);
    else
        result = acedGetString(0, prompt, val);

    if (result == RTNORM) {
        m_value = val;
        return CDistUiPrBase::kOk;
    }
    else
        return CDistUiPrBase::kCancel;
}

LPCTSTR
CDistUiPrString::GetValue()
{
    return m_value;
}

LPCTSTR
CDistUiPrString::GetKeyWords() const
{
    return _T("");
}

Acad::ErrorStatus
CDistUiPrString::SetKeyWords(LPCTSTR keyWordList)
{
    return Acad::eNotApplicable;
}

LPCTSTR
CDistUiPrString::GetKeyWordPicked()
{
    return NULL;
}

bool
CDistUiPrString::IsKeyWordPicked(LPCTSTR matchKeyWord)
{
    return false;
}


///////////////////////////// CDistUiPrStringDef /////////////////////////////////
CDistUiPrStringDef::CDistUiPrStringDef(LPCTSTR msg, LPCTSTR def, bool allowSpaces)
:   CDistUiPrString(msg, allowSpaces),
    m_default(def)
{
    ASSERT(def != NULL);
}

CDistUiPrStringDef::~CDistUiPrStringDef()
{
}

CDistUiPrBase::ExecuteStatus
CDistUiPrStringDef::Go()
{
    CString prompt;
    char val[512];
    int result;

    prompt.Format(_T("\n%s<%s>: "), GetMessage(), m_default);

    if (m_allowSpaces == Adesk::kTrue)
        result = acedGetString(1, prompt, val);
    else
        result = acedGetString(0, prompt, val);

    if (result == RTNORM) {
        if (val[0] == '\0')
            m_value = m_default;
        else
            m_value = val;
        return CDistUiPrBase::kOk;
    }
    else
        return CDistUiPrBase::kCancel;
}
