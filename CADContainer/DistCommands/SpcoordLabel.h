#pragma once
#define USER_TextStyle "���Ա�ע����" 


void CreatLabelPlineAllPt();

// �������߱�ע
void CreateHXBZ(CString strLineLayer, CString strTextLayer);


//////////////////////////////////////////////////////////////////////////
// �����ע
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

	//  ����: strLineLayer::��ע������CADͼ��, strTextLayer::��ע��������CADͼ��, startPt::ָ����ע��ʼ��
	BOOL CreateSpcoordLabel(CString strLineLayer, CString strTextLayer, ads_point startPt = NULL);

	// ������ֱ�ע
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
// Բ����ע
class CCircleLabel :
	public CSpcoordLabel
{
public:
	CCircleLabel(void);
	virtual ~CCircleLabel(void);

public:
	BOOL CreatCircleLabel(ads_point StartPt, ads_point EndPt, double Bulge);

	//  ����: strLineLayer::��ע������CADͼ��, strTextLayer::��ע��������CADͼ��, startPt::ָ����ע��ʼ��
	BOOL CreatCircleLabel(CString strLineLayer, CString strTextLayer, ads_point StartPt, ads_point EndPt, double Bulge);
};
