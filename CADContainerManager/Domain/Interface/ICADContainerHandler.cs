using System;
using System.Collections.Generic;
using System.Text;

namespace Dist.CAD.CADContainerManager
{
    public interface ICADContainerHandler
    {
        // 处理消息
        //void ProcessCommandFromCAD(string strData);
        void ProcessCommand(string command, Dictionary<string, string> dicParams);

        // 获取权限信息
        //void GetItemPower(int RoleId, int ProcessId,... Dictionary<string, string> dicParams);

        // 获取项目基本信息
        void GetProjectBaseInfo(int ProjectId, Dictionary<string, string> dicParams);

        //// 获取业务（流程）信息
        //void GetProcessInfo(int ProcessId, Dictionary<string, string> dicParams);

        // 获取用户详细信息
        void GetUserDetailInfo(int UserId, Dictionary<string, string> dicParams);
    }
}
