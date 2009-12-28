//using System;
//using System.Collections.Generic;
//using System.Text;

//using Dist.IntegrateFramework.Core.AddIns.Codons;
//using Dist.IntegrateFramework.Base.Gui;
//using Dist.IntegrateFramework.Core.Services;
//using Dist.Messaging;
//using Dist.Messaging.Integration;

//namespace CADContainerManager
//{
//    public partial class CADMessageManager
//    {
//        public void Send()
//        {
//            MessageBusService ms = (MessageBusService)ServiceManager.Services.GetService(typeof(MessageBusService));
//            //IChannelFactory channelFactory = ms.GetChannelFactory();
//            IChannelFactory channelFactory = ms.GetChannelFactoryByXml(@"F:\dap\trunk\src_3_1\基础集成框架\Dist.IntegrateFramework.Messaging\resource\Messaging.xml");
//            Sender sender = channelFactory.GetSender("CAD");

//            Dictionary<string, string> dic = new Dictionary<string, string>();
//            dic.Add("area", "10.01");
//            dic.Add("length", "5.5");
//            dic.Add("value", "1.0");
//            Dist.Messaging.Envelope env = new Envelope("CAD", "CAD返回值", dic);

//            sender.Send(env);
//        }

//        public void Receive()
//        {
//            MessageBusService ms = (MessageBusService)ServiceManager.Services.GetService(typeof(MessageBusService));
//            IChannelFactory channelFactory = ms.GetChannelFactoryByXml(@"F:\dap\trunk\src_3_1\基础集成框架\Dist.IntegrateFramework.Messaging\resource\Messaging.xml");
//            Receiver receiver = channelFactory.GetReceiver("CAD");
//            receiver.OnReceiveCompleted += new CompletedEventHandler(receiver_OnReceiveCompleted);
//            receiver.BeginReceive();
//        }

//        void receiver_OnReceiveCompleted(object sender, CompletedEventArgs e)
//        {
//            System.IO.TextWriter write = System.IO.File.AppendText(@"c:\a.txt");
//            write.WriteLine(e.Envelope.Serialize());
//            write.Close();
//        }
//    }
//}
