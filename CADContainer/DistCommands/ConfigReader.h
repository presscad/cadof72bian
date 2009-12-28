#pragma once
#include <map>
#include "StdAfx.h"

using namespace std;
class CConfigReader :
	public CObject
{
protected:
	void * _pConfigObject;
	CString _configFileName;	
public:
	CConfigReader(void);
	virtual ~CConfigReader(void);

	virtual void * ReadConfig();
};


class CJn_CommandPower: public CObject	//��������Ȩ��
{	
public:
	std::map<CString,BOOL> powerList;
	std::map<CString,CString> cmdDesc;
	CJn_CommandPower()
	{
		powerList["NEW"] = 0;		cmdDesc["NEW"] = "�½�ͼ��";
		powerList["QNEW"] = 0;		cmdDesc["QNEW"] = "�����½�";
		powerList["OPEN"] = 0;		cmdDesc["OPEN"] = "��";	
		powerList["QSAVE"] = 0;		cmdDesc["QSAVE"] = "���ٱ���";
		powerList["SAVEAS"] = 0;	cmdDesc["SAVEAS"] = "���Ϊ";
		powerList["EXPORT"] = 0;	cmdDesc["EXPORT"] = "����";
		powerList["WBLOCK"] = 0;	cmdDesc["WBLOCK"] = "д��";
		powerList["COPY"] = 0;		cmdDesc["COPY"] = "����";
		powerList["COPYCLIP"] = 0;	cmdDesc["COPYCLIP"] = "���Ƶ�������";
		powerList["COPYBASE"] = 0;	cmdDesc["COPYBASE"] = "�����㸴��";
		powerList["CUTCLIP"] = 0;	cmdDesc["CUTCLIP"] = "���е�������";
		powerList["APPLOAD"] = 0;	cmdDesc["APPLOAD"] = "����Ӧ�ó���";
		powerList["ARX"] = 0;		cmdDesc["ARX"] = "����ObjectArxӦ�ó���";
		powerList["VBAIDE"] = 0;	cmdDesc["VBAIDE"] = "VBA���ɿ�������";
		powerList["VBARUN"] = 0;	cmdDesc["VBARUN"] = "VBAִ��";
		powerList["VBALOAD"] = 0;	cmdDesc["VBALOAD"] = "VBA����";
		powerList["VBAMAN"] = 0;	cmdDesc["VBAMAN"] = "VBA����";
		powerList["SAVE"] = 0;		cmdDesc["SAVE"] = "����";
		powerList["QUIT"] = 0;		cmdDesc["QUIT"] = "�˳�";

		// �Զ�������
		powerList["_SHOWDOCKTREEPANE"] = 0;		cmdDesc["_SHOWDOCKTREEPANE"] = "��ʾĿ¼��";
		powerList["_READSDELAYERS"] = 0;		cmdDesc["_READSDELAYERS"] = "�Զ���ȡSDEͼ������";
		powerList["_READSDELAYERSEX"] = 0;		cmdDesc["_READSDELAYERSEX"] = "�ֹ���ѯSDEͼ������";
		powerList["_DISTSELPROJECT"] = 0;		cmdDesc["_DISTSELPROJECT"] = "�����Ŀ��ѯ";
		powerList["_SHOWOBJECTATTR"] = 0;		cmdDesc["_SHOWOBJECTATTR"] = "�鿴�༭����";
		powerList["_DRAWXMFWX"] = 0;			cmdDesc["_DRAWXMFWX"] = "��Ŀ��Χ���";
		powerList["_DRAWXMHX"] = 0;				cmdDesc["_DRAWXMHX"] = "������Ŀ����";
		powerList["_SAVESDELAYERS"] = 0;		cmdDesc["_SAVESDELAYERS"] = "����SDEͼ������";
		powerList["_DRAWDLHX"] = 0;				cmdDesc["_DRAWDLHX"] = "��·����";
		powerList["_DRAWCSLX"] = 0;				cmdDesc["_DRAWCSLX"] = "��������";
		powerList["_DRAWCSHX"] = 0;				cmdDesc["_DRAWCSHX"] = "���л���";
		powerList["_DRAWCSLVX"] = 0;			cmdDesc["_DRAWCSLVX"] = "��������";
		powerList["_DRAWCSZX"] = 0;				cmdDesc["_DRAWCSZX"] = "��������";
		powerList["_DRAWCSCX"] = 0;				cmdDesc["_DRAWCSCX"] = "���г���";
	}
	//CJn_CommandPower(){}
	CJn_CommandPower(const  CJn_CommandPower & src);
	void operator= (const  CJn_CommandPower & src);
	virtual ~CJn_CommandPower(){}
	virtual void Serialize(CArchive& archive);
protected:
	DECLARE_SERIAL(CJn_CommandPower)
};
class CCmdPwrConfigReader : public CConfigReader
{
protected:
	virtual void * ReadConfig();
	CCmdPwrConfigReader(void);	
public:
	static CCmdPwrConfigReader& Instanse();
	CJn_CommandPower *	GetCmdPwrConfig();
	virtual ~CCmdPwrConfigReader(void);	
};