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
        //WinAPI的long类型是32位的,而C#的long是64位的,这就导致堆栈不对称,引发错误.因此需要把API中的long改为C#的int
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
        public const int WS_VISIBLE = 0x10000000;  // 0x16010000  表示无边框

        private IntPtr _autoCADHandle = new IntPtr();
        int nOldWindowStyle = 0;
        private Process _autoCADProcess = null;

        //CADMessageManager MessageManager = new CADMessageManager();

        // 消息接收
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
                MessageBox.Show("系统找不到CAD模块");
                return;
            }
            this.SizeChanged += new EventHandler(CADContainerCtrl_SizeChanged);
            Cursor.Current = Cursors.WaitCursor;

            //// 消息，需要发送给CAD
            //string message = "GIVEDATA:dap|USER_ID:" + "123" + "|" + "PROJECT_ID:" + "123" + "|" + "WEB_HTTP:" + "http" + "XMWebService.asmx?WSDL" + "|" + "ROLE_ID:" + "123" + "|" + "PROCESS_ID:" + "123" + "|" + "RECT:" + "123" + "|" + "PRE_PROJECT_ID:" + "123" + "|" + "CADTYPE:" + "选址" + "|zzz:zzzz|";

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
                    ////if (process.ProcessName.ToUpper().Equals("ACAD") && process.MainWindowTitle.Contains("用地审批制图系统"))
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

            /**//// nOldWindowStyle = SetWindowLong(_autoCADHandle, GWL_STYLE, WS_VISIBLE);      //隐藏上面的边框

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
                MessageBox.Show("系统找不到CAD模块");
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
                    ////if (process.ProcessName.ToUpper().Equals("ACAD") && process.MainWindowTitle.Contains("用地审批制图系统"))
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
            /// nOldWindowStyle = SetWindowLong(_autoCADHandle, GWL_STYLE, WS_VISIBLE);      //隐藏上面的边框

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

        // 发送消息
        private void SendMessage(string message)
        {
            //string message = "GIVEDATA:dap|USER_ID:" + "123" + "|" + "PROJECT_ID:" + "123" + "|" + "WEB_HTTP:" + "http" + "XMWebService.asmx?WSDL" + "|" + "ROLE_ID:" + "123" + "|" + "PROCESS_ID:" + "123" + "|" + "RECT:" + "123" + "|" + "PRE_PROJECT_ID:" + "123" + "|" + "CADTYPE:" + "选址" + "|zzz:zzzz|";

            //结尾会被截掉一部分，需要添加多余信息
            message += "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";

            // 发送消息到CAD
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
        /// 发送初始化消息
        /// </summary>
        /// <param name="UserId">用户ID</param>
        /// <param name="ProjectId">项目Id</param>
        /// <param name="FullFileName">需要打开文件的本地路径</param>
        /// <param name="SDEConnectionXML">SDE连接</param>
        /// <param name="WebServicesURL">WebServices地址</param>
        /// <param name="RoleId">角色ID</param>
        /// <param name="ProcessId">流程ID</param>
        /// <param name="PreProcessId">上阶段流程ID(业务系统要求添加)</param>
        /// <param name="CADType">项目类型（"选址","用地","红线","预选","储备"）</param>
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

            // 传输图层权限 //暂时没有 //通过回调函数实现,无返回值即为空值
            command = "SetItemPower";
            dicParams.Clear();
            //(this.Parent as ICADContainerHandler).GetItemPower(RoleId, ProcessId,... dicParams);
            dicParams.Add("ID:88888888", "POWER:1,PRIOR:2"); // 所有CAD图层 权限：1.只读（2.可写） 优先级（权重）：2
            // CAD系统命令
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

            // 传输业务数据 //暂时没有 //通过回调函数实现,无返回值即为空值
            command = "bizzdata";
            dicParams.Clear();
            dicParams.Add("projectid", ProjectId.ToString()); //项目Id
            // 业务实例
            //dicParams.Add("bi_code", ""); //项目编号
            //dicParams.Add("bi_same", ""); //报建编号
            //dicParams.Add("bi_name", ""); //项目名称
            //dicParams.Add("bi_owner", ""); // 建设单位
            //dicParams.Add("bi_tmOver", ""); // 结束时间
            // 业务
            //dicParams.Add("bi_BzId", ""); // 业务类型
            //dicParams.Add("bi_BzName", ""); // 业务名称
            //(this.Parent as ICADContainerHandler).GetProcessInfo(ProcessId, dicParams);
            (this.Parent as ICADContainerHandler).GetProjectBaseInfo(ProjectId, dicParams);
            // 当前用户
            //dicParams.Add("lg_UserName", ""); // 登录用户名
            (this.Parent as ICADContainerHandler).GetUserDetailInfo(UserId, dicParams);
            // 服务器当前时间 //YY-MM-DD
            dicParams.Add("cur_time", ""); // 服务器当前时间
            //(this.Parent as ICADContainerHandler).GetCurTime(dicParams);
            SendMessage(command, dicParams);

            // 加载目录树
            command = "LoadLayerTree";
            dicParams.Clear();
            // 获取项目图层数勾选状态 //？？？（未定）不再需要，现保存在SDE数据库中
            // 获取业务图层数勾选状态 //随权限保存在业务库中 //通过回调函数实现
            //dicParams.Add("88888888", "1");
            //(this.Parent as ICADContainerHandler).GetItemPower(RoleId, ProcessId,... dicParams);
            SendMessage(command, dicParams);

            // 项目属性
            command = "bizzdata_ex";
            SendMessage(command, property);

            // 显示命令行
            SendMessage("ShowCommandLine", null);

            //// 读取sde数据
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

            // SDE连接信息配置
            if (SDEConnxmldoc.DocumentElement != null)
            {
                dicParams.Add("server", SDEConnxmldoc.DocumentElement["SERVICE_NAME"].InnerText);

                dicParams.Add("service", SDEConnxmldoc.DocumentElement["SERVER_PORT"].InnerText);

                dicParams.Add("database", SDEConnxmldoc.DocumentElement["DATABASE_NAME"].InnerText);

                dicParams.Add("username", SDEConnxmldoc.DocumentElement["LOGIN_USER"].InnerText);

                dicParams.Add("password", SDEConnxmldoc.DocumentElement["LOGIN_PASSWORD"].InnerText);
            }

            // FTP连接信息 //暂时不用
            //dicParams.Add("ftpServer", "");
            //dicParams.Add("ftpUser", "");
            //dicParams.Add("ftpPassword", "");
            //dicParams.Add("ftpPort", "");

            // 图层配置信息 //暂时不用
            dicParams.Add("地形图", "");
            dicParams.Add("地形图索引字段", "");
            dicParams.Add("标注线层", "");
            dicParams.Add("标注文字层", "");
            dicParams.Add("范围图层", "");
            dicParams.Add("范围图层字段", "");

            dicParams.Add("parent_hwnd", this.Handle.ToString());

            SendMessage(command, dicParams);

            //CProgressDlg::SetProgressInfo("正在连接Sde数据库");
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
        
        // 接收消息
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
                    //    //// lpData的格式是  SUM_AREA:198823.45332|HOUSE_AREA:180.33|
                    //    //// 名称：值|名称：值|名称：值|名称：值|名称：值|    ----以"|"结尾

                    //    ////  lpData采用下面的格式：
                    //    ////  BDE_DATA=value;BDE_DATA=value;BDE_DATA=value;
                    //    ////  以分号结尾,
                    //    ////  例：  TBSELECT_ADDR_HDL/SUM_LAND_AREA_SUM=56899.23569;/TBSELECT_ADDR_HDL/BUILD_LAND_AREA_SUM=7799.33;
                    //    ////FillDataToWeb(mystr.lpData);           
                    //    ////FillDataToWeb("/TBSELECT_ADDR_HDL/SUM_LAND_AREA_SUM=56899.23569;/TBSELECT_ADDR_HDL/BUILD_LAND_AREA_SUM=7799.33;");           

                    //    //string temp = mystr.lpData;

                    //    ////$$PROJECT_CODE=D2007D0289;PROJECT_NAME=香山国际游艇俱乐部;BUILD_UNIT=香山国际游艇俱乐部（厦门）有限公司;BUILD_ADDRESS=701097.854;PROJECT_TYPE=用地;CREATE_TIME=2007-9-18## PROJECT_CODE=D2008D0066;PROJECT_NAME=恩斯凯营运中心;BUILD_UNIT=厦门新日精工投资管理有限公司;BUILD_ADDRESS=5680.951;PROJECT_TYPE=用地;CREATE_TIME=2008-4-10## 
                    //    ////填充数据至表单

                    //    //if ((temp).IndexOf("$$") == -1)//有$$符表示该信息是删除图层专用的信息
                    //    //{
                    //    //    FillDataToWeb(temp);
                    //    //    //将数据保存至web数据库
                    //    //    UpdateClass updateClass = new UpdateClass(temp);
                    //    //    Thread thread = new Thread(new ThreadStart(updateClass.UpdateData));
                    //    //    thread.Start();
                    //    //}
                    //    //else
                    //    //{
                    //    //    //留用将图层删除数据保存至web数据库
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
            // 装载XML字符串
            xmldocument.LoadXml(strXML);
            if (xmldocument.InnerXml == "")
            {
                return false;
            }

            // 解析Command名称
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

            // 解析参数列表
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

        // 关闭
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
                    /**//// SetWindowLong(_autoCADHandle, GWL_STYLE, nOldWindowStyle);      //恢复CAD的外边框

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
                        //// 为什么名称不能用，这样会关闭掉所有的cad，能否给需要的cad做个标记 或者通过findwindow比较
                        //if (process.ProcessName.ToUpper().Equals("ACAD"))// && process.MainWindowTitle.Contains("用地审批制图系统"))
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
