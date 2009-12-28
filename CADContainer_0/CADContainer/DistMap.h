#pragma once
#include "stdafx.h"
#include "DistConnection.h"

//SDEͼ������
enum DIST_ENUM_SHAPETYPE     
{
	kShapeNil        = 1L,
	kShapePoint      = (1L<<1),
	kShapeLine       = (1L<<2),
	kShapeSimpleLine = (1L<<3),
	kShapeArea       = (1L<<4),
	kShapeMultiPart  = (1L<<18)
};

class CDistMap
{
public:
	CDistMap(void);
	~CDistMap(void);

public:
	//��ǰCADģ����AcDbDatabase
	AcDbDatabase * m_lpCADDatabase;
	CDistConnection * m_lpDataConnection;

public:

	//CAD����

	// ����CADʵ������
	//������strLnTypeName   ������
	//���أ�AcDbObjectId    ��������ID �ɹ����ز��ҵ���������Ӧ���͵�ID ��ʧ�ܷ���NULL
	AcDbObjectId CAD_GetLineTypeId(const char* strLnTypeName);

	// ����CADʵ������
	//������strLnTypeName   ������
	//���أ�AcDbObjectId    ��������ID �ɹ������¼�����������Ӧ���͵�ID ��ʧ�ܷ���NULL
	AcDbObjectId CAD_LoadLineType(const char* strLnTypeName);

	//���ܣ�����ͼ��
	//������strLyName   ͼ����
	//      nClrIndex   ��ɫ��
	//      strLnType   ����
	//      dLineWeigth �߿�
	//���أ�AcDbObjectId    ���ش���ʵ���ID �ɹ������½�Layer ID ��ʧ�ܷ���NULL
	AcDbObjectId CAD_CreateLayer(const char *strLyName, int  nClrIndex,const char *strLnType, AcDb::LineWeight dLineWeigth);



	//SDE����

	// ���ܣ�����SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ�㴴����������Ա������Ӹ��ֶ�)
	//       dScale            ͼ�������
	//       ox,oy             ����ϵԭ��
	//       dGridSize         �����С
	//       lTypeMask         ͼ������
	//       strKeyword        �ؼ���
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long SDE_CreateLayer(const char* strTableName,
		                 const char* strSpacialFldName,
						 double dScale,
						 double ox = 0,
						 double oy = 0,
						 double dGridSize = 1000,
						 long   lTypeMask = kShapeNil | kShapePoint |
						                    kShapeLine | kShapeSimpleLine|
						                    kShapeArea | kShapeMultiPart,
						 const char* strKeyword="DEFAULT");
};
