using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CADContainerManager;

namespace Test
{
    public partial class Form1 : Form
    {
        public CADContainerManager.CADContainerCtrl CADCC;

        public Form1()
        {
            InitializeComponent();
            CADCC = new CADContainerCtrl();
            //CADCC.Dock = DockStyle.Fill;
            CADCC.LoadPlug();

            //CADCC.SendInitialMessage(130, 2144, @"E:\projects\济南规划\src\Dist.JiNanPlanning.Engine\bin\CADContainer\Work\草图.dwg", "http://192.168.1.66/Dist.JiNanPlanning.WebService.AutoCAD/Service.asmx", 0, 0, 0, "选址");

            this.Controls.Add(CADCC);
        }
        protected override void Dispose(bool disposing)
        {
            //CADCC.CloseAutoCAD();

            CADCC.UnLoadPlug(true);
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void Form1_Activated(object sender, EventArgs e)
        {
            //CADCC.SendInitialMessage(130, 2144, @"E:\projects\济南规划\src\Dist.JiNanPlanning.Engine\bin\CADContainer\Work\草图.dwg", "http://192.168.1.66/Dist.JiNanPlanning.WebService.AutoCAD/Service.asmx", 0, 0, 0, "选址");
        }
    }
}