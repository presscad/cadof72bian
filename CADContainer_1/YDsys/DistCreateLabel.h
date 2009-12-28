#pragma once
//#define USER_TextStyle "属性标注字体" 


//////////////////////////////////////////////////////////////////////////
// 坐标标注
class CSpcoordLabel
{
public:
	         CSpcoordLabel(void);
	virtual ~CSpcoordLabel(void);

	double FontHeight;

private:
	struct resbuf *m_OldRbf;

public:
	BOOL CreateSpcoordLabel(CString strCadLyName,ads_point SetPt = NULL, BOOL bInPut = FALSE);
	BOOL CreateSpcoordLabelX(ads_point SetPt = NULL, BOOL bInPut = FALSE);
protected:
	void CoverAngelP2(ads_real &dAngle,ads_real &dLabelAngle);
	void CoverAngelP3(ads_real &dAngle,ads_real &dLabelAngle);
	double GetTextWidth(LPCTSTR sText, double dHigh);
	BOOL AddBigFontToCurDwg(CString BigFontFileName, CString TextStyleName);
	AcDbObjectId GetTextStyleObjID(CString TextStyleName);
};


//////////////////////////////////////////////////////////////////////////
// 圆弧标注
class CCircleLabel :
	public CSpcoordLabel
{
public:
	CCircleLabel(void);
	virtual ~CCircleLabel(void);

public:
	BOOL CreatCircleLabel(CString strCadLyName,ads_point StartPt, ads_point EndPt, double Bulge);
	BOOL CreatCircleLabel(CString strCadLyName,AcGePoint3d ptCen,double dR,double dMAngle);
};



