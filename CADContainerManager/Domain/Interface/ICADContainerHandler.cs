using System;
using System.Collections.Generic;
using System.Text;

namespace Dist.CAD.CADContainerManager
{
    public interface ICADContainerHandler
    {
        // ������Ϣ
        //void ProcessCommandFromCAD(string strData);
        void ProcessCommand(string command, Dictionary<string, string> dicParams);

        // ��ȡȨ����Ϣ
        //void GetItemPower(int RoleId, int ProcessId,... Dictionary<string, string> dicParams);

        // ��ȡ��Ŀ������Ϣ
        void GetProjectBaseInfo(int ProjectId, Dictionary<string, string> dicParams);

        //// ��ȡҵ�����̣���Ϣ
        //void GetProcessInfo(int ProcessId, Dictionary<string, string> dicParams);

        // ��ȡ�û���ϸ��Ϣ
        void GetUserDetailInfo(int UserId, Dictionary<string, string> dicParams);
    }
}
