#include "attrdataclass.h"

CAttrDataClass::CAttrDataClass(void)
{
	m_fid = -1;
	m_lCadId = -1;

	m_strCADLayer = "";
	m_strOldCADLayer = "";

	m_strSDELayer = "";
	m_strOldSDELayer = "";

	m_lShapeEditNum = 0;
	m_lLayerMoveNum = 0;

	aAttr.RemoveAll();

	aOldAttr.RemoveAll();
}

CAttrDataClass::~CAttrDataClass(void)
{
}


CAttrDataClass &CAttrDataClass::operator =(const CAttrDataClass &AttrDataClass)
{
	m_fid = AttrDataClass.m_fid;
	m_lCadId = AttrDataClass.m_lCadId;

	m_strCADLayer = AttrDataClass.m_strCADLayer;
	m_strOldCADLayer = AttrDataClass.m_strOldCADLayer;

	m_strSDELayer = AttrDataClass.m_strSDELayer;
	m_strOldSDELayer = AttrDataClass.m_strOldSDELayer;

	m_lShapeEditNum = AttrDataClass.m_lShapeEditNum;
	m_lLayerMoveNum = AttrDataClass.m_lLayerMoveNum;

	aAttr.RemoveAll();
	aAttr.Append(AttrDataClass.aAttr);

	aOldAttr.RemoveAll();
	aOldAttr.Append(AttrDataClass.aOldAttr);

	return *this;
}