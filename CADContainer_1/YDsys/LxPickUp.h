#pragma once

class CLxPickUp
{
	class CLxLine
	{
	public:
		CLxLine(void);
		~CLxLine(void);

		void GetLxLine();

	public:
		char * pch;
		AcDbObjectIdArray m_LineArr;

	private:
		BOOL GetAllLine(AcDbObjectIdArray& dbObjIdArr);

		void CopyAllLine(AcDbObjectIdArray& dbObjIdArr, AcDbObjectIdArray& arrObjIdOut);

		Acad::ErrorStatus EnterToDwg(AcDbObjectId& pOutputId, AcDbEntity* pEntity);

		void SetObjectHighLight(AcDbObjectIdArray& dbObjIdArr, BOOL bHighLight = TRUE);
	};

	class CPickUpBorder
	{
	public:
		CPickUpBorder(void);
		~CPickUpBorder(void);

		AcDbObjectId GetOutPline(AcDbObjectIdArray dbObjIdArr);

	public:
		char * pch;
		int m_Num;
		double m_dOffSetX, m_dOffSetY;

	private:
		void CopyAllLine(AcDbObjectIdArray& dbObjIdArr, AcDbObjectIdArray &arrObjIdOut);

		void CreateCrossLine(AcDbObjectIdArray&dbObjIdArrOut, AcDbObjectIdArray&dbObjIdArrOutNew);

		void RemoveEntity(AcDbObjectIdArray&dbObjIdArrOutNew);

		void Check2dPoint(AcDbObject *pDbO,AcDbDatabase *pDbDatab = acdbHostApplicationServices()->workingDatabase());
		//获得起始点及长度
		void getStartPointAndLength(AcGeCurve2d *pCurve2d, AcGePoint2d startPt, AcGePoint2d endPt, double &startParam,
			double &len_EllArc, AcGePoint2dArray &PartPtArray);
		// 获得点，并加入数组
		void addPt(AcGeCurve2d *pCurve2d, AcGePoint2d startPt, AcGePoint2d endPt, Adesk::Boolean posParamDir,
			double inc_ArcLength, AcGePoint2dArray &PartPtArray);
		//曲线分割的实行函数
		void PartExec(AcGeCurve2d *pCurve2d, AcGePoint2d pPoint, double inc_ArcLength, AcGePoint2dArray &PartPtArray);

		void MoveEntity(AcDbObjectId dbObjId ,double dOffsetX,double dOffsetY);

		BOOL GetArcStAndEdPt(AcDbArc*pdbArc,AcGePoint3d &geStPt,AcGePoint3d &geEdPt);
		//去除所有的端点
		void RemoveAllVertPoint(AcDbObject*pdbObj, AcGePoint3dArray&insertPoints);

		AcDbObjectId CreateDistLine(AcGePoint3d &gePoi3d,AcGePoint3d&gePoi3d1);

		void SortByDistance(AcGePoint3d &stPt, AcGePoint3dArray&gePoi3dArr);

		void CreateFromPoints(AcDbObject*pdbObj, AcDbObjectIdArray &dbIdArray, AcGePoint3dArray &gePoi3dArr, AcDbObjectIdArray &dbIdRemove);

		AcDbObjectId CreateDistArc(double dStAngle,double dEndAngle,double dRadius,AcGePoint3d &gePoiCenter);

		void Bubble(AcGePoint3d &stPt, AcGePoint3dArray &gePoi3dArr, int n);

		void BubbleByAngle(double dStAngle, AcArray<double> &AngArr, int n);

		void SortByAngle(double dStAngle,AcArray<double> &AngleArr);

		void CreateArcForPoints(AcDbObject*pdbObj, AcDbObjectIdArray &dbIdArray, AcGePoint3dArray &gePoi3dArr, AcDbObjectIdArray &dbIdRemove);

		void CreateTempLine(AcDbObject*pdbObj, AcDbObjectIdArray& entity, AcDbObjectIdArray &dbIdArray, AcDbObjectIdArray&dbIdRemove);
	};

public:
	CLxPickUp(void);
	~CLxPickUp(void);

	void mainFun();

	void RemoveEntity();

public:
	char * pch;
	AcDbObjectId m_OutId;

private:
	BOOL GetLine();

	BOOL GetOutPline();

};
