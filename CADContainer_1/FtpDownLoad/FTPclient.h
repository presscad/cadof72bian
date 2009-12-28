#ifndef  __DIST__FTP__CLIENT__H__
#define  __DIST__FTP__CLIENT__H__


class CFTPclient
{
public:
	         CFTPclient();
	virtual ~CFTPclient();

public:

	BOOL MoveFile(CString remotefile, CString localfile,BOOL pasv,BOOL get);
	BOOL LogOnToServer(CString hostname,int hostport,CString username, CString password, CString acct, CString fwhost,CString fwusername, CString fwpassword,int fwport,int logontype);
	void LogOffServer();

	BOOL FTPcommand(CString command);
	BOOL ReadStr();
	BOOL WriteStr(CString outputstring);

private:
	BOOL ReadStr2();
	BOOL OpenControlChannel(CString serverhost,int serverport);
	void CloseControlChannel();

public:
	CString m_retmsg;

private:
	CArchive*    m_pCtrlRxarch;
	CArchive*    m_pCtrlTxarch;
	CSocketFile* m_pCtrlsokfile;
	CSocket*     m_Ctrlsok;
	int          m_fc;
};


#endif //__DIST__FTP__CLIENT__H__
