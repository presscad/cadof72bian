using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Xml;

using Dist.CAD.CADContainerManager;

namespace CADContainerManager
{
    public partial class CADContainerCtrl : UserControl
    {
        private static CADContainerCtrl _cadContainerCtrl = null;
        public static CADContainerCtrl Instance
        {
            get
            {
                if (_cadContainerCtrl == null)
                {
                    _cadContainerCtrl = new CADContainerCtrl();
                }

                return _cadContainerCtrl;
            }
        }

        public static void UnloadCADContainerCtrl()
        {
            if (_cadContainerCtrl != null)
            {
                _cadContainerCtrl.UnLoad();
            }
        }


        [DllImport("User32.dll")]
        public static extern bool ShowWindowAsync(IntPtr hWnd, int cmdShow);
        [DllImport("User32.dll")]
        public static extern bool IsWindow(IntPtr hWnd);
        //WinAPI��long������32λ��,��C#��long��64λ��,��͵��¶�ջ���Գ�,��������.�����Ҫ��API�е�long��ΪC#��int
        [DllImport("user32.dll", EntryPoint = "SetWindowLongA", SetLastError = true)]
        public static extern int SetWindowLong(IntPtr hwnd, int nIndex, int dwNewLong);
        [DllImport("user32.dll", SetLastError = true)]
        public static extern long SetParent(IntPtr hWndChild, IntPtr hWndNewParent);
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool MoveWindow(IntPtr hwnd, int x, int y, int cx, int cy, bool repaint);

        //
        [DllImport("user32.dll", EntryPoint = "SendMessageW", CharSet = CharSet.Auto, ExactSpelling = true,
        CallingConvention = CallingConvention.StdCall)]
        public static extern int SendMessage(
                                             int hWnd, // handle to destination window
                                             int Msg, // message
                                             int wParam, // first message parameter
                                             ref COPYDATASTRUCT lParam // second message parameter
                                             );

        public const int SW_HIDE = 0;
        public const int SW_SHOWMAXIMIZED = 3;
        public const int SW_SHOWNORMAL = 1;
        public const int GWL_STYLE = (-16);
        public const int WS_VISIBLE = 0x10000000;  // 0x16010000  ��ʾ�ޱ߿�

        private IntPtr _autoCADHandle = new IntPtr();
        int nOldWindowStyle = 0;
        private Process _autoCADProcess = null;

        //CADMessageManager MessageManager = new CADMessageManager();

        // ��Ϣ����
        public const int WM_COPYDATA = 0x004A;

        public struct COPYDATASTRUCT
        {
            public IntPtr dwData;
            public int cbData;
            [MarshalAs(UnmanagedType.LPStr)]
            public string lpData;
        }
        
        public CADContainerCtrl()
        {
            InitializeComponent();

            LoadPlug();
        }

        private void LoadPlug(/*System.Collections.ArrayList paramArr*/)
        {
            if (!System.IO.File.Exists(AppDomain.CurrentDomain.BaseDirectory + @"CADContainer\LaunchCAD.exe"))
            {
                MessageBox.Show("ϵͳ�Ҳ���CADģ��");
                return;
            }
            this.SizeChanged += new EventHandler(CADContainerCtrl_SizeChanged);
            Cursor.Current = Cursors.WaitCursor;

            //// ��Ϣ����Ҫ���͸�CAD
            //string message = "GIVEDATA:dap|USER_ID:" + "123" + "|" + "PROJECT_ID:" + "123" + "|" + "WEB_HTTP:" + "http" + "XMWebService.asmx?WSDL" + "|" + "ROLE_ID:" + "123" + "|" + "PROCESS_ID:" + "123" + "|" + "RECT:" + "123" + "|" + "PRE_PROJECT_ID:" + "123" + "|" + "CADTYPE:" + "ѡַ" + "|zzz:zzzz|";

            this.Dock = DockStyle.Fill;

            System.Diagnostics.Process[] CadProcesses_Exist;
            CadProcesses_Exist = System.Diagnostics.Process.GetProcessesByName("acad");

            //IntPtr handle = new IntPtr();

            //
            if ((_autoCADHandle.ToInt32() == 0) || (!IsWindow(_autoCADHandle)))
            {
                Thread thread = new Thread(new ThreadStart(OpenCAD));
                thread.Start();
                Thread.Sleep(3000);
            }
            else
            {
                ShowWindowAsync(_autoCADHandle, SW_SHOWNORMAL);
                //handle = _autoCADHandle;
            }

            //
            ////while (Int32.Parse(Convert.ToString((int)handle, 10)) == 0)
            while (Int32.Parse(Convert.ToString((int)_autoCADHandle, 10)) == 0)
            {
                System.Diagnostics.Process[] CadProcesses;
                CadProcesses = System.Diagnostics.Process.GetProcessesByName("acad");

                ////foreach (Process process in System.Diagnostics.Process.GetProcesses())
                //DateTime ProcessStartTime = DateTime.MinValue;
                foreach (Process process in CadProcesses)
                {
                    ////if (process.ProcessName.ToUpper().Equals("ACAD") && process.MainWindowTitle.Contains("�õ�������ͼϵͳ"))
                    bool IsNewOpen = true;
                    foreach (Process process_exist in CadProcesses_Exist)
                    {
                        if (process.Id == process_exist.Id)
                        {
                            IsNewOpen = false;
                            break;
                        }
                    }
                    if (IsNewOpen && process.MainWindowTitle.Contains("CadContainer"))
                    {
                        //if (_autoCADProcess == null)
                        //{
                        //    ProcessStartTime = process.StartTime;
                            _autoCADProcess = process;
                            ////handle = process.MainWindowHandle;
                            ////ShowWindowAsync(handle, SW_HIDE);
                            _autoCADHandle = process.MainWindowHandle;
                            break;
                        //}
                        //else
                        //{
                        //    if (ProcessStartTime < process.StartTime)
                        //    {
                        //        ProcessStartTime = process.StartTime;
                        //        _autoCADProcess = process;
                        //        _autoCADHandle = process.MainWindowHandle;
                        //    }
                        //}
                    }
                }
                Thread.Sleep(500);
            }

            ////_autoCADHandle = handle;
            /**//// ShowWindowAsync(_autoCADHandle, SW_HIDE);

            //WinAPIs.ShowWindow(p, WinAPIs.SW_SHOWMAXIMIZED);     

            /**//// nOldWindowStyle = SetWindowLong(_autoCADHandle, GWL_STYLE, WS_VISIBLE);      //��������ı߿�

            //WinAPIs.MoveWindow(p, 0, 0, this.Width, this.Height, true);

            /**//// SetParent(_autoCADHandle, this.Handle);

            ShowWindowAsync(_autoCADHandle, SW_SHOWMAXIMIZED);
            Cursor.Current = Cursors.Default;
        }

        private void CheckCadProcess()
        {
            if ((_autoCADHandle.ToInt32() != 0) && (IsWindow(_autoCADHandle)))
                return;

            _autoCADProcess = null;
            _autoCADHandle = new IntPtr();

            if (!System.IO.File.Exists(AppDomain.CurrentDomain.BaseDirectory + @"CADContainer\LaunchCAD.exe"))
            {
                MessageBox.Show("ϵͳ�Ҳ���CADģ��");
                return;
            }

            System.Diagnostics.Process[] CadProcesses_Exist;
            CadProcesses_Exist = System.Diagnostics.Process.GetProcessesByName("acad");

            //IntPtr handle = new IntPtr();

            //
            if ((_autoCADHandle.ToInt32() == 0) || (!IsWindow(_autoCADHandle)))
            {
                Thread thread = new Thread(new ThreadStart(OpenCAD));
                thread.Start();
                Thread.Sleep(3000);
            }
            else
            {
                ShowWindowAsync(_autoCADHandle, SW_SHOWNORMAL);
                //handle = _autoCADHandle;
            }

            //
            ////while (Int32.Parse(Convert.ToString((int)handle, 10)) == 0)
            while (Int32.Parse(Convert.ToString((int)_autoCADHandle, 10)) == 0)
            {
                System.Diagnostics.Process[] CadProcesses;
                CadProcesses = System.Diagnostics.Process.GetProcessesByName("acad");

                ////foreach (Process process in System.Diagnostics.Process.GetProcesses())
                //DateTime ProcessStartTime = DateTime.MinValue;
                foreach (Process process in CadProcesses)
                {
                    ////if (process.ProcessName.ToUpper().Equals("ACAD") && process.MainWindowTitle.Contains("�õ�������ͼϵͳ"))
                    bool IsNewOpen = true;
                    foreach (Process process_exist in CadProcesses_Exist)
                    {
                        if (process.Id == process_exist.Id)
                        {
                            IsNewOpen = false;
                            break;
                        }
                    }
                    if (IsNewOpen && process.MainWindowTitle.Contains("CadContainer"))
                    {
                        //if (_autoCADProcess == null)
                        //{
                        //    ProcessStartTime = process.StartTime;
                        _autoCADProcess = process;
                        ////handle = process.MainWindowHandle;
                        ////ShowWindowAsync(handle, SW_HIDE);
                        _autoCADHandle = process.MainWindowHandle;
                        break;
                        //}
                        //else
                        //{
                        //    if (ProcessStartTime < process.StartTime)
                        //    {
                        //        ProcessStartTime = process.StartTime;
                        //        _autoCADProcess = process;
                        //        _autoCADHandle = process.MainWindowHandle;
                        //    }
                        //}
                    }
                }
                Thread.Sleep(500);
            }

            ////_autoCADHandle = handle;
            /**/
            /// ShowWindowAsync(_autoCADHandle, SW_HIDE);

            //WinAPIs.ShowWindow(p, WinAPIs.SW_SHOWMAXIMIZED);     

            /**/
            /// nOldWindowStyle = SetWindowLong(_autoCADHandle, GWL_STYLE, WS_VISIBLE);      //��������ı߿�

            //WinAPIs.MoveWindow(p, 0, 0, this.Width, this.Height, true);

            /**/
            /// SetParent(_autoCADHandle, this.Handle);

            ShowWindowAsync(_autoCADHandle, SW_SHOWMAXIMIZED);
            Cursor.Current = Cursors.Default;
        }

        private void CADContainerCtrl_SizeChanged(object sender, EventArgs e)
        {
            /**//// MoveWindow(_autoCADHandle, 0, 0, this.Width, this.Height, true);
        }

        private void OpenCAD()
        {
            Process.Start(AppDomain.CurrentDomain.BaseDirectory + @"CADContainer\LaunchCAD.exe");
            //Process.Start(@"D:\Program Files\AutoCAD 2005\acad.exe");
        }

        // ������Ϣ
        private void SendMessage(string message)
        {
            //string message = "GIVEDATA:dap|USER_ID:" + "123" + "|" + "PROJECT_ID:" + "123" + "|" + "WEB_HTTP:" + "http" + "XMWebService.asmx?WSDL" + "|" + "ROLE_ID:" + "123" + "|" + "PROCESS_ID:" + "123" + "|" + "RECT:" + "123" + "|" + "PRE_PROJECT_ID:" + "123" + "|" + "CADTYPE:" + "ѡַ" + "|zzz:zzzz|";

            //��β�ᱻ�ص�һ���֣���Ҫ��Ӷ�����Ϣ
            message += "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";

            // ������Ϣ��CAD
            int len = message.Length;
            COPYDATASTRUCT cds;
            cds.dwData = (IntPtr)100;
            cds.lpData = message;
            cds.cbData = len * 2;

            SendMessage(_autoCADHandle.ToInt32(), WM_COPYDATA, this.Handle.ToInt32(), ref cds);
        }

        private void SendMessage(string command, Dictionary<string, string> dicParams)
        {
            XmlDocument xmldocument = new XmlDocument();

            xmldocument.AppendChild(xmldocument.CreateProcessingInstruction("xml", "version='1.0' encoding='GB2312'"));

            XmlElement rootnode = xmldocument.CreateElement("root");

            XmlElement commandnode = xmldocument.CreateElement("command");

            commandnode.SetAttribute("name", command);

            if (dicParams != null)
            {
                foreach (KeyValuePair<string, string> kvp in dicParams)
                {
                    XmlElement paramnode = xmldocument.CreateElement("param");
                    paramnode.SetAttribute("name", kvp.Key);
                    paramnode.SetAttribute("value", kvp.Value);
                    commandnode.AppendChild(paramnode);
                }
            }

            rootnode.AppendChild(commandnode);

            xmldocument.AppendChild(rootnode);

            SendMessage(xmldocument.InnerXml);
        }
        
        /// <summary>
        /// ���ͳ�ʼ����Ϣ
        /// </summary>
        /// <param name="UserId">�û�ID</param>
        /// <param name="ProjectId">��ĿId</param>
        /// <param name="FullFileName">��Ҫ���ļ��ı���·��</param>
        /// <param name="SDEConnectionXML">SDE����</param>
        /// <param name="WebServicesURL">WebServices��ַ</param>
        /// <param name="RoleId">��ɫID</param>
        /// <param name="ProcessId">����ID</param>
        /// <param name="PreProcessId">�Ͻ׶�����ID(ҵ��ϵͳҪ�����)</param>
        /// <param name="CADType">��Ŀ���ͣ�"ѡַ","�õ�","����","Ԥѡ","����"��</param>
        //public void SendInitialMessage(int UserId, int ProjectId, string FullFileName, string SDEConnectionXML, string WebServicesURL, int RoleId, int ProcessId, int PreProcessId, string CADType)
        public void SendInitialMessage(string SDEConnectionXML, int ProjectId, int UserId, int RoleId, int ProcessId, Dictionary<string, string> property)
        {
            //string message = "GIVEDATA:dap|USER_ID:" + UserId.ToString() + "|" + "PROJECT_ID:" + ProjectId.ToString() + "|" + "FILE_FULLNAME:" + FullFileName + "|" + "SDE_CONN:" + SDEConnectionXML + "|" + "WEB_HTTP:" + WebServicesURL + "?WSDL" + "|" + "ROLE_ID:" + RoleId.ToString() + "|" + "PROCESS_ID:" + ProcessId.ToString() + "|" + "PRE_PROJECT_ID:" + PreProcessId.ToString() + "|" + "CADTYPE:" + CADType + "|zzz:zzzz|";
            //SendMessage(message);
            CheckCadProcess();

            string command = "";
            Dictionary<string, string> dicParams = new Dictionary<string, string>();

            SendSDEConnMessage(SDEConnectionXML);

            SendMessage("ShowCommandLine", null);

            // ����ͼ��Ȩ�� //��ʱû�� //ͨ���ص�����ʵ��,�޷���ֵ��Ϊ��ֵ
            command = "SetItemPower";
            dicParams.Clear();
            //(this.Parent as ICADContainerHandler).GetItemPower(RoleId, ProcessId,... dicParams);
            dicParams.Add("ID:88888888", "POWER:1,PRIOR:2"); // ����CADͼ�� Ȩ�ޣ�1.ֻ����2.��д�� ���ȼ���Ȩ�أ���2
            // CADϵͳ����
            dicParams.Add("ID:80000001", "POWER:1,PRIOR:1"); //"80000001",	"new",		"new",
            dicParams.Add("ID:80000002", "POWER:1,PRIOR:1"); //"80000002",	"qnew",		"qnew",
            dicParams.Add("ID:80000003", "POWER:1,PRIOR:1"); //"80000003",	"open",		"open",
            dicParams.Add("ID:80000004", "POWER:1,PRIOR:1"); //"80000004",	"qsave",	"qsave",
            dicParams.Add("ID:80000005", "POWER:1,PRIOR:1"); //"80000005",	"saveas",	"saveas",
            dicParams.Add("ID:80000006", "POWER:1,PRIOR:1"); //"80000006",	"export",	"export",
            dicParams.Add("ID:80000007", "POWER:1,PRIOR:1"); //"80000007",	"WBLOCK",	"WBLOCK",
            dicParams.Add("ID:80000008", "POWER:1,PRIOR:1"); //"80000008",	"COPY",		"COPY",
            dicParams.Add("ID:80000009", "POWER:1,PRIOR:1"); //"80000009",	"COPYCLIP",	"COPYCLIP",
            dicParams.Add("ID:80000010", "POWER:1,PRIOR:1"); //"80000010",	"copybase",	"copybase",
            dicParams.Add("ID:80000011", "POWER:1,PRIOR:1"); //"80000011",	"CUTCLIP",	"CUTCLIP",
            dicParams.Add("ID:80000012", "POWER:1,PRIOR:1"); //"80000012",	"APPLOAD",	"APPLOAD",
            dicParams.Add("ID:80000013", "POWER:1,PRIOR:1"); //"80000013",	"ARX",		"ARX",
            dicParams.Add("ID:80000014", "POWER:1,PRIOR:1"); //"80000014",	"VBAIDE",	"VBAIDE",
            dicParams.Add("ID:80000015", "POWER:1,PRIOR:1"); //"80000015",	"VBARUN",	"VBARUN",
            dicParams.Add("ID:80000016", "POWER:1,PRIOR:1"); //"80000016",	"VBALOAD",	"VBALOAD",
            dicParams.Add("ID:80000017", "POWER:1,PRIOR:1"); //"80000017",	"VBAMAN",	"VBAMAN",
            dicParams.Add("ID:80000018", "POWER:1,PRIOR:1"); //"80000018",	"save",		"save",
            SendMessage(command, dicParams);

            // ����ҵ������ //��ʱû�� //ͨ���ص�����ʵ��,�޷���ֵ��Ϊ��ֵ
            command = "bizzdata";
            dicParams.Clear();
            dicParams.Add("projectid", ProjectId.ToString()); //��ĿId
            // ҵ��ʵ��
            //dicParams.Add("bi_code", ""); //��Ŀ���
            //dicParams.Add("bi_same", ""); //�������
            //dicParams.Add("bi_name", ""); //��Ŀ����
            //dicParams.Add("bi_owner", ""); // ���赥λ
            //dicParams.Add("bi_tmOver", ""); // ����ʱ��
            // ҵ��
            //dicParams.Add("bi_BzId", ""); // ҵ������
            //dicParams.Add("bi_BzName", ""); // ҵ������
            //(this.Parent as ICADContainerHandler).GetProcessInfo(ProcessId, dicParams);
            (this.Parent as ICADContainerHandler).GetProjectBaseInfo(ProjectId, dicParams);
            // ��ǰ�û�
            //dicParams.Add("lg_UserName", ""); // ��¼�û���
            (this.Parent as ICADContainerHandler).GetUserDetailInfo(UserId, dicParams);
            // ��������ǰʱ�� //YY-MM-DD
            dicParams.Add("cur_time", ""); // ��������ǰʱ��
            //(this.Parent as ICADContainerHandler).GetCurTime(dicParams);
            SendMessage(command, dicParams);

            // ����Ŀ¼��
            command = "LoadLayerTree";
            dicParams.Clear();
            // ��ȡ��Ŀͼ������ѡ״̬ //��������δ����������Ҫ���ֱ�����SDE���ݿ���
            // ��ȡҵ��ͼ������ѡ״̬ //��Ȩ�ޱ�����ҵ����� //ͨ���ص�����ʵ��
            //dicParams.Add("88888888", "1");
            //(this.Parent as ICADContainerHandler).GetItemPower(RoleId, ProcessId,... dicParams);
            SendMessage(command, dicParams);

            // ��Ŀ����
            command = "bizzdata_ex";
            SendMessage(command, property);

            // ��ʾ������
            SendMessage("ShowCommandLine", null);

            //// ��ȡsde����
            //SendMessage("ReadSdeData", null);
        }

        public void SendSDEConnMessage(string SDEConnectionXML)
        {
            string strxml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><root>"
                            + SDEConnectionXML
                            + "</root>";
            XmlDocument SDEConnxmldoc = new XmlDocument();
            SDEConnxmldoc.LoadXml(strxml);

            string command = "sdedata";
            Dictionary<string, string> dicParams = new Dictionary<string, string>();
            dicParams.Clear();

            // SDE������Ϣ����
            if (SDEConnxmldoc.DocumentElement != null)
            {
                dicParams.Add("server", SDEConnxmldoc.DocumentElement["SERVICE_NAME"].InnerText);

                dicParams.Add("service", SDEConnxmldoc.DocumentElement["SERVER_PORT"].InnerText);

                dicParams.Add("database", SDEConnxmldoc.DocumentElement["DATABASE_NAME"].InnerText);

                dicParams.Add("username", SDEConnxmldoc.DocumentElement["LOGIN_USER"].InnerText);

                dicParams.Add("password", SDEConnxmldoc.DocumentElement["LOGIN_PASSWORD"].InnerText);
            }

            // FTP������Ϣ //��ʱ����
            //dicParams.Add("ftpServer", "");
            //dicParams.Add("ftpUser", "");
            //dicParams.Add("ftpPassword", "");
            //dicParams.Add("ftpPort", "");

            // ͼ��������Ϣ //��ʱ����
            dicParams.Add("����ͼ", "");
            dicParams.Add("����ͼ�����ֶ�", "");
            dicParams.Add("��ע�߲�", "");
            dicParams.Add("��ע���ֲ�", "");
            dicParams.Add("��Χͼ��", "");
            dicParams.Add("��Χͼ���ֶ�", "");

            dicParams.Add("parent_hwnd", this.Handle.ToString());

            SendMessage(command, dicParams);

            //CProgressDlg::SetProgressInfo("��������Sde���ݿ�");
            //CProgressDlg::SetPercent(25);
        }

        public void SendInsertFileMessage(string strFileName, string strFileId)
        {
            string strcommand = "LoadAffixFile";
            Dictionary<string, string> dicParams = new Dictionary<string, string>();
            dicParams.Clear();

            dicParams.Add("filename", strFileName);
            dicParams.Add("fileid", strFileId);

            SendMessage(strcommand, dicParams);
        }

        public void SendOpenFileMessage(string strFileName, string strFileId)
        {
            string strcommand = "OpenDwgFile";
            Dictionary<string, string> dicParams = new Dictionary<string, string>();
            dicParams.Clear();

            dicParams.Add("filename", strFileName);
            dicParams.Add("fileid", strFileId);

            SendMessage(strcommand, dicParams);
        }

        //public void SendPropCloseMessage()
        //{
        //    string message = "GIVECOMMAND:propertiesclose";

        //    SendMessage(message);
        //}

        public void SendHideMessage()
        {
            //string message = "GIVECOMMAND:CADCONTAINER_HIDE";

            //SendMessage(message);

            string command = "DeleteAllData";

            SendMessage(command, null);
        }

        //public void CloseCAD()
        //{
        //    string message = "GIVECOMMAND:APPLICATION_EXIT";
        //    int len = message.Length;
        //    COPYDATASTRUCT cds;
        //    cds.dwData = (IntPtr)100;
        //    cds.lpData = message;
        //    cds.cbData = len + 1;
        //    SendMessage(_autoCADHandle.ToInt32(), WM_COPYDATA, this.Handle.ToInt32(), ref cds);
        //}
        
        // ������Ϣ
        protected override void DefWndProc(ref System.Windows.Forms.Message m)
        {
            switch (m.Msg)
            {
                case WM_COPYDATA:
                    COPYDATASTRUCT mystr = new COPYDATASTRUCT();
                    Type mytype = mystr.GetType();
                    mystr = (COPYDATASTRUCT)m.GetLParam(mytype);

                    string strXML = mystr.lpData;
                    string command = "";
                    Dictionary<string, string> dicParams = new Dictionary<string,string>();
                    if (ParseParameters(strXML, ref command, dicParams))
                    {
                        if (this.Parent != null)
                        {
                            (this.Parent as ICADContainerHandler).ProcessCommand(command, dicParams);
                        }
                    }

                    //if (mystr.lpData.Substring(0, "GIVECOMMAND:".Length).CompareTo("GIVECOMMAND:") == 0)
                    //{
                    //    if (this.Parent != null)
                    //    {
                    //        (this.Parent as ICADContainerHandler).ProcessCommandFromCAD(mystr.lpData);
                    //    }
                    //}
                    //else
                    //{
                    //    //// lpData�ĸ�ʽ��  SUM_AREA:198823.45332|HOUSE_AREA:180.33|
                    //    //// ���ƣ�ֵ|���ƣ�ֵ|���ƣ�ֵ|���ƣ�ֵ|���ƣ�ֵ|    ----��"|"��β

                    //    ////  lpData��������ĸ�ʽ��
                    //    ////  BDE_DATA=value;BDE_DATA=value;BDE_DATA=value;
                    //    ////  �ԷֺŽ�β,
                    //    ////  ����  TBSELECT_ADDR_HDL/SUM_LAND_AREA_SUM=56899.23569;/TBSELECT_ADDR_HDL/BUILD_LAND_AREA_SUM=7799.33;
                    //    ////FillDataToWeb(mystr.lpData);           
                    //    ////FillDataToWeb("/TBSELECT_ADDR_HDL/SUM_LAND_AREA_SUM=56899.23569;/TBSELECT_ADDR_HDL/BUILD_LAND_AREA_SUM=7799.33;");           

                    //    //string temp = mystr.lpData;

                    //    ////$$PROJECT_CODE=D2007D0289;PROJECT_NAME=��ɽ������ͧ���ֲ�;BUILD_UNIT=��ɽ������ͧ���ֲ������ţ����޹�˾;BUILD_ADDRESS=701097.854;PROJECT_TYPE=�õ�;CREATE_TIME=2007-9-18## PROJECT_CODE=D2008D0066;PROJECT_NAME=��˹��Ӫ������;BUILD_UNIT=�������վ���Ͷ�ʹ������޹�˾;BUILD_ADDRESS=5680.951;PROJECT_TYPE=�õ�;CREATE_TIME=2008-4-10## 
                    //    ////�����������

                    //    //if ((temp).IndexOf("$$") == -1)//��$$����ʾ����Ϣ��ɾ��ͼ��ר�õ���Ϣ
                    //    //{
                    //    //    FillDataToWeb(temp);
                    //    //    //�����ݱ�����web���ݿ�
                    //    //    UpdateClass updateClass = new UpdateClass(temp);
                    //    //    Thread thread = new Thread(new ThreadStart(updateClass.UpdateData));
                    //    //    thread.Start();
                    //    //}
                    //    //else
                    //    //{
                    //    //    //���ý�ͼ��ɾ�����ݱ�����web���ݿ�
                    //    //    InsertClass insertClass = new InsertClass(temp);
                    //    //    Thread thread_map = new Thread(new ThreadStart(insertClass.InsertData));
                    //    //    thread_map.Start();
                    //    //}
                    //    ////UpdateToData(temp);
                    //}
                    break;
                default:
                    base.DefWndProc(ref m);
                    break;
            }

        }

        private bool ParseParameters(string strXML,ref string command, Dictionary<string, string> dicParams)
        {
            XmlDocument xmldocument = new XmlDocument();
            // װ��XML�ַ���
            xmldocument.LoadXml(strXML);
            if (xmldocument.InnerXml == "")
            {
                return false;
            }

            // ����Command����
            XmlNode commandnode = xmldocument.DocumentElement.SelectSingleNode("command");
            if (commandnode == null)
            {
                return false;
            }

            XmlNode commandnodename = commandnode.Attributes.GetNamedItem("name");
            if (commandnodename == null)
            {
                return false;
            }
            command = commandnodename.Value;
            if (command == "")
            {
                return false;
            }

            // ���������б�
            foreach (XmlNode childnode in commandnode.ChildNodes)
            {
                XmlNode childnodename = childnode.Attributes.GetNamedItem("name");
                if (childnodename == null)
                {
                    continue;
                }
                string paramname = childnodename.Value;
                if (paramname == "")
                {
                    continue;
                }

                XmlNode childnodevalue = childnode.Attributes.GetNamedItem("value");
                if (childnodevalue == null)
                {
                    continue;
                }
                string paramvalue = childnodevalue.Value;

                dicParams.Add(paramname, paramvalue);
            }

            return true;
        }

        // �ر�
        public void UnLoad()
        {
            this.UnLoadPlug(true);

            this.Dispose();
        }

        private void UnLoadPlug(bool completeRemove)
        {
            return;
            if (completeRemove)
            {
                if (_autoCADHandle.ToInt32() != 0)
                {
                    /**//// SetWindowLong(_autoCADHandle, GWL_STYLE, nOldWindowStyle);      //�ָ�CAD����߿�

                    /**//// SetParent(_autoCADHandle, IntPtr.Zero);

                    //Thread thread = new Thread(new ThreadStart(KillCAD));
                    //thread.Start();
                    //Thread.Sleep(3000);
                    //while (!IsCADKilled())
                    //{
                    //    Thread.Sleep(5000);
                    //}

                    //_autoCADProcess = null;

                    foreach (Process process in System.Diagnostics.Process.GetProcesses())
                    {
                        //// Ϊʲô���Ʋ����ã�������رյ����е�cad���ܷ����Ҫ��cad������� ����ͨ��findwindow�Ƚ�
                        //if (process.ProcessName.ToUpper().Equals("ACAD"))// && process.MainWindowTitle.Contains("�õ�������ͼϵͳ"))
                        //{
                        //        process.Kill();
                        //}

                        //if (process.Id == _autoCADHandle.ToInt32())
                        if (process.Id == _autoCADProcess.Id)
                        {
                            process.Kill();
                            _autoCADProcess = null;
                            _autoCADHandle = new IntPtr();
                            break;
                        }
                    }
                }
            }
            else
            {
                /**//// ShowWindowAsync(_autoCADHandle, SW_HIDE);
                //UnLoadPluginMenuAndToolBar(this._app);
            }
        }

        //private void KillCAD()
        //{
        //    foreach (Process process in System.Diagnostics.Process.GetProcesses())
        //    {
        //        if (process.Id == _autoCADProcess.Id)
        //        {
        //            process.Kill();
        //            break;
        //        }
        //    }
        //}

        //private bool IsCADKilled()
        //{
        //    foreach (Process process in System.Diagnostics.Process.GetProcesses())
        //    {
        //        if (process.Id == _autoCADProcess.Id)
        //        {
        //            return false;
        //            break;
        //        }
        //    }
        //    return true;
        //}

        protected override void Dispose(bool disposing)
        {
            //if (disposing)
            //{
            //    if (_autoCADProcess != null)
            //    {
            //        //_autoCADProcess.Close();
            //        //_autoCADProcess.Dispose();

            //        //
            //        //string message = "GIVECOMMAND:APPLICATION_EXIT";
            //        //int len = message.Length;
            //        //COPYDATASTRUCT cds;
            //        //cds.dwData = (IntPtr)100;
            //        //cds.lpData = message;
            //        //cds.cbData = len + 1;
            //        //SendMessage(_autoCADHandle.ToInt32(), WM_COPYDATA, this.Handle.ToInt32(), ref cds);
            //    }
            //}
            //if (disposing && (components != null))
            //{
            //    components.Dispose();
            //}
            //base.Dispose(disposing);

            return;
            base.Dispose(disposing);
            if (disposing)
            {
                if (_autoCADProcess != null)
                {
                    _autoCADProcess.Close();
                    _autoCADProcess.Dispose();
                }
            }
        }
    }
}
