using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using CADContainerManager;

using Dist.IntegrateFramework.Core.Services;
using Dist.IntegrateFramework.MessageBusService;

namespace Test
{
    public partial class Form2 : Form
    {
        CADMessageManager CMM = new CADMessageManager();
        public Form2()
        {
            InitializeComponent();
            ServiceManager.Services.AddService(new MessageService());
            ServiceManager.Services.AddService(new MessageBusService());
        }

        private void button1_Click(object sender, EventArgs e)
        {
            CMM.Send();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            CMM.Receive();
        }
    }
}