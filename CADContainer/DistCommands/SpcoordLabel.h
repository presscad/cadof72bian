#pragma once
#define USER_TextStyle "属性标注字体" 


void CreatLabelPlineAllPt();

// 创建红线标注
void CreateHXBZ(CString strLineLayer, CString strTextLayer);


//////////////////////////////////////////////////////////////////////////
// 坐标标注
class CSpcoordLabel
{
public:
	CSpcoordLabel(void);
	virtual ~CSpcoordLabel(void);

	static double FontHeight;

private:
	struct resbuf *m_OldRbf;

public:
	BOOL CreateSpcoordLabel(ads_point SetPt = NULL, BOOL bInPut = FALSE);

	//  参数: strLineLayer::标注线所在CAD图层, strTextLayer::标注文字所在CAD图层, startPt::指定标注起始点
	BOOL CreateSpcoordLabel(CString strLineLayer, CString strTextLayer, ads_point startPt = NULL);

	// 添加文字标注
	BOOL CreateDimText(CString strTextLayer, ads_point pt = NULL);

	BOOL AddProjectAtrr(AcDbObjectId& objectId, CString strLayerName, CString& strSdeLayerName);

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
	BOOL CreatCircleLabel(ads_point StartPt, ads_point EndPt, double Bulge);

	//  参数: strLineLayer::标注线所在CAD图层, strTextLayer::标注文字所在CAD图层, startPt::指定标注起始点
	BOOL CreatCircleLabel(CString strLineLayer, CString strTextLayer, ads_point StartPt, ads_point EndPt, double Bulge);
};
