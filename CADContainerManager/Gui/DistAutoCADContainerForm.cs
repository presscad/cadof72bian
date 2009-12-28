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

        // 处理消息
        //public virtual void ProcessCommandFromCAD(string strData)
        public virtual void ProcessCommand(string command, Dictionary<string, string> dicParams)
        {
            return;
        }

        // 获取权限信息
        //public virtual void GetItemPower(int RoleId, int ProcessId,... Dictionary<string, string> dicParams)
        //{
        //}

        // 获取项目基本信息
        public virtual void GetProjectBaseInfo(int ProjectId, Dictionary<string, string> dicParams)
        {
            return;
        }

        //// 获取业务（流程）信息
        //public virtual void GetProcessInfo(int ProcessId, Dictionary<string, string> dicParams)
        //{
        //    return;
        //}

        // 获取用户详细信息
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
                dicParams.Add("lg_UserId", UserId.ToString()); // 登录用户名ID
                dicParams.Add("lg_UserName", user.Name != null ? user.Name : ""); // 登录用户名
            }
            return;
        }
    }
}