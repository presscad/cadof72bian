using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using Dist.ORUP.BLL;

using Dist.IntegrateFramework.Base.Gui;
using Dist.CAD.CADContainerManager;

namespace CADContainerManager
{
    public partial class DistAutoCADContainerForm : AbstractWorkbenchWindow,ICADContainerHandler
    {
        public DistAutoCADContainerForm()
        {
            InitializeComponent();

            CADContainerCtrl.Instance.Dock = DockStyle.Fill;
            this.Controls.Add(CADContainerCtrl.Instance);
            //CADContainerCtrl.Instance.LoadPlug();
        }

        protected override void ClosingWorkbenchWindow(object sender, FormClosingEventArgs e)
        {
            this.Controls.Remove(CADContainerCtrl.Instance);
        }

        public override bool CloseWindow(bool force)
        {
            return true;
        }

        // ������Ϣ
        //public virtual void ProcessCommandFromCAD(string strData)
        public virtual void ProcessCommand(string command, Dictionary<string, string> dicParams)
        {
            return;
        }

        // ��ȡȨ����Ϣ
        //public virtual void GetItemPower(int RoleId, int ProcessId,... Dictionary<string, string> dicParams)
        //{
        //}

        // ��ȡ��Ŀ������Ϣ
        public virtual void GetProjectBaseInfo(int ProjectId, Dictionary<string, string> dicParams)
        {
            return;
        }

        //// ��ȡҵ�����̣���Ϣ
        //public virtual void GetProcessInfo(int ProcessId, Dictionary<string, string> dicParams)
        //{
        //    return;
        //}

        // ��ȡ�û���ϸ��Ϣ
        public virtual void GetUserDetailInfo(int UserId, Dictionary<string, string> dicParams)
        {
            IUser user = null;
            try
            {
                user = Dist.ORUP.BLL.User.GetUser(UserId);
            }
            catch (Exception e)
            {
                user = null;
            }
            if (user != null)
            {
                dicParams.Add("lg_UserId", UserId.ToString()); // ��¼�û���ID
                dicParams.Add("lg_UserName", user.Name != null ? user.Name : ""); // ��¼�û���
            }
            return;
        }
    }
}