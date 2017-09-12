#include "StdAfx.h"
#include "USB3Camera.h"
#include "SetInfoHelper.h"
#include "SystemHelper.h"
#include "CameraConfigureInfoHelper.h"
#include "LogTool.h"

bool  USB3Camera::m_IsPlaying=false;
static const int				VENDOR_ID	= 0x04B4;
static const int				PRODUCT_ID	= 0x00F1; 
USB3Camera::USB3Camera(CWnd* owner)
{
	this->m_IsNeedLoadWdsFile = true;

	this->m_pOwner = owner;

	this->m_USBDevice = NULL;
	this->m_InEndpt = NULL;
	this->m_OutEndpt = NULL;

	this->m_CCyFX3Device = NULL;

	m_hAccessSem = CreateSemaphore(NULL, 1, 1, "Semaphore");
	m_hImageAccessSem = CreateSemaphore(NULL, 1, 1, "ImageSemaphore");
}


USB3Camera::~USB3Camera(void)
{

}

void USB3Camera::GetDeviceNames()
{
	this->m_IsNeedLoadWdsFile = true;

	USB3Camera::m_IsPlaying = false;

	CString info;
	this->m_DeviceNames.RemoveAll();
	if (this->m_USBDevice==NULL)
	{
		this->m_USBDevice = new CCyUSBDevice((HANDLE)m_pOwner->m_hWnd,CYUSBDRV_GUID,true);
	}

	if (this->m_USBDevice==NULL)
	{
		info = "设备创建失败！";
		::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)0);
	}
	else
	{
		if (this->m_USBDevice->DeviceCount()==0) 
		{
			info = "没有发现设备！";
			::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)0);
		}
		else
		{
			for (int i=0; i<this->m_USBDevice->DeviceCount(); i++)
			{
				/*
				if ((this->m_USBDevice->VendorID == VENDOR_ID) && (this->m_USBDevice->ProductID == PRODUCT_ID)) 
				{
					break;
				}
				*/
				this->m_USBDevice->Open(i);

				USB_CONFIGURATION_DESCRIPTOR ConfDesc;
				m_USBDevice->GetConfigDescriptor(&ConfDesc);
				if(ConfDesc.bNumInterfaces==1)
				{
					USB_INTERFACE_DESCRIPTOR IntfDesc;
					m_USBDevice->GetIntfcDescriptor(&IntfDesc);
					if(IntfDesc.bAlternateSetting==0)
					{
						CString str;
						str.Format("%s",m_USBDevice->DeviceName);
						this->m_DeviceNames.Add(str);
					}				
				}
			}

			if ((this->m_USBDevice->VendorID == VENDOR_ID) && (this->m_USBDevice->ProductID == PRODUCT_ID)) 
			{
				int interfaces = this->m_USBDevice->AltIntfcCount()+1;

                bool bHighSpeedDevice = this->m_USBDevice->bHighSpeed;
                bool bSuperSpeedDevice = this->m_USBDevice->bSuperSpeed;

                for (int i=0; i< interfaces; i++)
                {
                    this->m_USBDevice->SetAltIntfc(i);
				}
			}

			if(this->m_DeviceNames.GetSize()>0)
			{
				GetBulkNames(this->m_DeviceNames[0]);
				if(this->m_InEndptNames.GetSize()>0)
				{
					//this->m_InEndpt = this->m_USBDevice->EndPoints[1];
				}
				if(this->m_OutEndptNames.GetSize()>0)
				{
					//this->m_OutEndpt = this->m_USBDevice->EndPoints[0];
				}
			}
			info = "发现设备,加载WDS文件！";
			::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);
		}
	}
}
void USB3Camera::GetBulkNames(CString deviceName)
{
	this->m_InEndptNames.RemoveAll();
	this->m_OutEndptNames.RemoveAll();

	for (int i=0; i<this->m_DeviceNames.GetSize(); i++)
	{
		if(m_DeviceNames[i]==deviceName)
		{
			this->m_USBDevice->Open(i);

			CCyUSBEndPoint *endpt = NULL;
			for (i=1; i<this->m_USBDevice->EndPointCount(); i++)
			{
				endpt = this->m_USBDevice->EndPoints[i];
				if (endpt->Attributes == 2) 
				{
					CString str;
					//0x02是写入端口，0x08是读取端口
					if (endpt->Address & 0x80)
					{
						str.Format("%d,In:0x%02X",i,endpt->Address);
						this->m_InEndptNames.Add(str);
						this->m_InEndpt = this->m_USBDevice->EndPoints[i];
					}
					else
					{
						str.Format("%d,Out:0x%02X",i,endpt->Address);
						this->m_OutEndptNames.Add(str);
					}
				}
			}
		}
	}
}

void USB3Camera::GetDeviceName()
{
	if (this->m_USBDevice->DeviceCount())
    {
        for (int i = 0; i < this->m_USBDevice->DeviceCount(); i++)
        {
            this->m_USBDevice->Open(i);

            USB_CONFIGURATION_DESCRIPTOR ConfDesc;
            m_USBDevice->GetConfigDescriptor(&ConfDesc);

            if(ConfDesc.bNumInterfaces==1)
            {
                USB_INTERFACE_DESCRIPTOR IntfDesc;
                m_USBDevice->GetIntfcDescriptor(&IntfDesc);
                if(IntfDesc.bAlternateSetting==0)
                {
					this->m_DeviceName.Format("%s",m_USBDevice->DeviceName);
                }				
            }
        }
	}
}
void USB3Camera::InitDevice()
{
	GetDeviceName();
	InitBulkOutAndInEndPoind();
}
void USB3Camera::CloseDevice()
{
	/*
	if(this->m_InEndpt != NULL)
	{
		m_InEndpt->Abort();
		m_InEndpt->Reset();
	}

	if(this->m_OutEndpt != NULL)
	{
		m_OutEndpt->Abort();
		m_OutEndpt->Reset();
	}
	
	 m_USBDevice->Reset();	
	*/
	if (this->m_USBDevice!=NULL)
	{
		 if(!this->m_USBDevice->IsOpen())
		 {
			 this->m_USBDevice->Close(); 
		 }

		 delete this->m_USBDevice;
		 this->m_USBDevice = NULL;
	}
}


//控制传输读操作
void USB3Camera::ReadControl(int requestCode,UCHAR *data,long readLength)
{
	CCyControlEndPoint  *ept = this->m_USBDevice->ControlEndPt;
	ept->Target   = TGT_DEVICE;
	ept->ReqType  = REQ_VENDOR;
	ept->Direction = DIR_FROM_DEVICE;  
	ept->ReqCode  = requestCode;//控制码
	ept->Value    = 1;//控制参数
	ept->Index    = 0;

	ept->Read(data,readLength);

}
//控制传输读操作 (MGL)
void USB3Camera::ReadControl(int requestCode,int value,int index,UCHAR *data,long writeLength)
{
	if(m_USBDevice==NULL)
	{
		return;
	}
	if(this->m_USBDevice->DeviceCount()==0)
	{
		return;
	}

	CCyControlEndPoint  *ept = m_USBDevice->ControlEndPt;
	ept->Target   = TGT_DEVICE;
	ept->ReqType  = REQ_VENDOR;
	ept->Direction =  DIR_FROM_DEVICE  ;  
	ept->ReqCode  = requestCode;//控制码
	ept->Value    = value;
	ept->Index    = index;

	ept->XferData(data,writeLength);
}
//控制传输写操作 (MGL)
void USB3Camera::WriteControl(int requestCode,int value,int index,UCHAR *data,long writeLength)
{
	if(m_USBDevice==NULL)
	{
		return;
	}
	if(this->m_USBDevice->DeviceCount()==0)
	{
		return;
	}

	CCyControlEndPoint  *ept = m_USBDevice->ControlEndPt;
	ept->Target   = TGT_DEVICE;
	ept->ReqType  = REQ_VENDOR;
	ept->Direction = DIR_FROM_DEVICE;  
	ept->ReqCode  = requestCode;//控制码
	ept->Value    = value;
	ept->Index    = index;

	ept->XferData(data,writeLength);
}


//传输化块传输端点
void USB3Camera::InitBulkOutAndInEndPoind()
{
	for (int i=1; i<this->m_USBDevice->EndPointCount(); i++)
	{
		CCyUSBEndPoint *endpt = this->m_USBDevice->EndPoints[i];

		if (endpt->Attributes == 2)   
        {
            if (endpt->bIn)
            {
                this->m_InEndpt = this->m_USBDevice->EndPointOf(endpt->Address);
				this->m_InEndptName.Format("BULK IN 地址0x%2X,包:%dBytes",endpt->Address,endpt->MaxPktSize);	
            }
            else
            {
				this->m_OutEndpt = this->m_USBDevice->EndPointOf(endpt->Address);
				this->m_OutEndptName.Format("BULK OUT 地址0x%2X,包:%dBytes",endpt->Address,endpt->MaxPktSize);	
            }
        }
	}
}
//块写入缓存
void USB3Camera::WriteBulk(byte data)
{
	HANDLE HANDLE_WRITEBULK = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_WRITEBULK"));
	SetEvent(HANDLE_WRITEBULK);
}
//块写操作
void USB3Camera::BulkWriteService( LPVOID params )
{
	USB3Camera *usb3camera = (USB3Camera *) params;

	int  pktsize = usb3camera->m_OutEndpt->MaxPktSize;
	long length = pktsize * 64;//每包的数量512*64 = 32768
	usb3camera->m_OutEndpt->SetXferSize(length);

	int queuenumber = 0;//工作队列号
	int queuesize = 1;//发送队列
	int timeout = 1500;//时间溢出(毫秒)

	OVERLAPPED	outOverLap[64];
	PUCHAR	*buffers  = new PUCHAR[queuesize];
	PUCHAR	*contexts = new PUCHAR[queuesize];
	for (int i=0; i< queuesize; i++) 
    { 
        buffers[i] = new UCHAR[length];
		memset(buffers[i],0x00,length);
        outOverLap[i].hEvent = CreateEvent(NULL, false, false, "CYUSB_OUT");
    }
	
	HANDLE HANDLE_WRITEBULK = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_WRITEBULK"));

	long failurecount = 0;
	long successcount =0;
	queuenumber = 0;
	while (TRUE)
	{
		WaitForSingleObject(HANDLE_WRITEBULK, INFINITE );
		//清零操作
		memset(buffers[queuenumber],0x00,length);
		for(int c=0;c<length;c++)
		{
		}

		contexts[queuenumber] = usb3camera->m_OutEndpt->BeginDataXfer(buffers[queuenumber], length, &outOverLap[queuenumber]);
		
		if (!usb3camera->m_OutEndpt->WaitForXfer(&outOverLap[queuenumber], timeout))
		{
			usb3camera->m_OutEndpt->Abort();
			if (usb3camera->m_OutEndpt->LastError == ERROR_IO_PENDING)
			{
				WaitForSingleObject(outOverLap[queuenumber].hEvent,2000);
			}
		}

		if (usb3camera->m_OutEndpt->FinishDataXfer(buffers[queuenumber], length, &outOverLap[queuenumber], contexts[queuenumber])) 
		{			
			successcount++;
		} 
		else
		{
			failurecount++;
		}

		queuenumber++;

		if (queuenumber == queuesize)
		{
			queuenumber = 0;			
		}
	}
}
//块读操作
UINT USB3Camera::BulkRead( LPVOID params )
{
	USB3Camera *usb3camera = (USB3Camera *) params;

	HANDLE HANDLE_READBULK = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_READBULK"));

	while (TRUE)
	{
		WaitForSingleObject(HANDLE_READBULK, INFINITE );
		
		usb3camera->ReadInEndpt();
	}
}
UINT USB3Camera::ReadInEndpt()
{
	int timeout = 1500;//时间溢出(毫秒)
	this->m_InEndpt->TimeOut = timeout;

	OVERLAPPED	inOvLap;
	memset(&inOvLap, 0, sizeof(inOvLap));
    inOvLap.hEvent = CreateEvent(NULL, false, false, "CYUSB_IN");

	long bytesRequested = 1024;
	UCHAR * pBuffer = new UCHAR[bytesRequested];
	UCHAR * inContext = this->m_InEndpt->BeginDataXfer(pBuffer, bytesRequested, &inOvLap);
    if (!this->m_InEndpt->WaitForXfer(&inOvLap, 5000))
    {
        this->m_InEndpt->Abort();                            
        if (this->m_InEndpt->LastError == ERROR_IO_PENDING)   
		{
            WaitForSingleObject(inOvLap.hEvent, 2000); 
		}
    }

	long bytesReturned = 0;
    bool bResult = this->m_InEndpt->FinishDataXfer(pBuffer, bytesReturned, &inOvLap, inContext);

	/*
	CString logstr;
	CString str1;
	for(int i=0;i<bytesReturned;i++)
	{
		str1.Format("%2X ",pBuffer[i]);
		logstr = logstr + str1;
	}
	*/
    CloseHandle(inOvLap.hEvent); 
	delete pBuffer;
	

	return 1;
}
void USB3Camera::BulkReadService()
{
	AfxBeginThread(BulkRead, this);	
}
void USB3Camera::StartBulkRead()
{
	HANDLE HANDLE_READBULK = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_READBULK"));
	SetEvent(HANDLE_READBULK);
}
void USB3Camera::StopBulkRead()
{
	HANDLE HANDLE_READBULK = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_READBULK"));
	ResetEvent(HANDLE_READBULK);
}
void USB3Camera::SetAVDD(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x7243;break;
		case 1:ivalue = 0x7380;break;
		case 2:ivalue = 0x73D0;break;
	}

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	WriteControl(rcode,ivalue,0x0001,buf,bytesToSend);
}
void USB3Camera::SetDOVDD(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x7380;break;
		case 1:ivalue = 0x73D0;break;
		case 2:ivalue = 0x7550;break;
	}

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	WriteControl(rcode,ivalue,0x0002,buf,bytesToSend);
}
void USB3Camera::SetDVDD(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA控制");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x7500;break;
		case 1:ivalue = 0x7550;break;
		case 2:ivalue = 0x75C0;break;
	}

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	WriteControl(rcode,ivalue,0x0004,buf,bytesToSend);
}
//初始化时钟
void USB3Camera::InitClockHzValue()
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("可编程时钟频率");

	CStringArray strArray;
	strArray.RemoveAll();

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());
	SystemHelper::Split(info.wValues,strArray,',');
	for(int i=0;i<strArray.GetSize();i++)
	{
	   strArray[i].ReleaseBuffer();
	   int ivalue = SystemHelper::ChangeNum(strArray[i],strArray[i].GetLength());

	   WriteControl(rcode,ivalue,iindex,buf,bytesToSend);

	   //Sleep(100);
	}
}
void USB3Camera::SetClock(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("可编程时钟频率");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());

	int ivalue = 0x0000;

	switch( i)
	{
		case 0:
		case 12:
			ivalue=0x0C20;
			break;//12M
		case 1:
		case 24:
			ivalue=0x0C10;
			break;//24M
		case 2:
		case 48:
			ivalue=0x0C08;
			break;//48M
		case 3:
		case 96:
			ivalue=0x0C04;
			break;//96M
	}

	WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}
//DACA初始化
void USB3Camera::SetDACAInit()
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA初始化");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	WriteControl(rcode,0x0001,0x0000,buf,bytesToSend);//0x0001dakai 0x0000guanbi
}
//切断电压
void USB3Camera::TurnoffPower()
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA初始化");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	WriteControl(rcode,0x0000,0x0000,buf,bytesToSend);//0x0001dakai 0x0000guanbi
}
//恢复电压
void USB3Camera::TurnonPower()
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("DACA初始化");

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	WriteControl(rcode,0x0001,0x0000,buf,bytesToSend);//0x0001dakai 0x0000guanbi
}

void USB3Camera::SetDefineClock(int clockHzValue)
{
	if(clockHzValue==0) return;

	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("可编程时钟频率");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());

	int ivalue = 0x0000;

	int v = 384/clockHzValue;
	//v -=2 ;
	v += 0xC00;
	ivalue = v;

	WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}
void USB3Camera::SetDiaoDian(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("掉电控制");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	/*
	switch(i)
	{
		case 0:ivalue = 0x0000;break;
		case 1:ivalue = 0x0002;break;
	}
	*/
	ivalue = i;
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());

	WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}
void USB3Camera::SetReset(int i)
{
	SetInfoHelper helper;
	SetInfo info =  helper.GetSetInfo("复位控制");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	info.Reqcode.ReleaseBuffer();
	int rcode = SystemHelper::ChangeNum(info.Reqcode,info.Reqcode.GetLength());

	int ivalue = 0x0000;
	switch(i)
	{
		case 0:ivalue = 0x0000;break;
		case 1:ivalue = 0x0001;break;
	}
	info.wIndexs.ReleaseBuffer();
	int iindex = SystemHelper::ChangeNum(info.wIndexs,info.wIndexs.GetLength());

	WriteControl(rcode,ivalue,iindex,buf,bytesToSend);
}
bool USB3Camera::LoadWDSFile(CString filePath)
{
	CameraConfigureInfoHelper cameraConfigureInfoHelper;
	this->m_CameraConfigureInfo  = cameraConfigureInfoHelper.GetCameraConfigureInfo(filePath);

	//Sleep(3000);
	//SetAVDD(this->m_CameraConfigureInfo.AVDD);
	//SetDOVDD(this->m_CameraConfigureInfo.DOVDD);
	//SetDVDD(this->m_CameraConfigureInfo.DVDD);
	
	//SetDACAInit();

	SetDVDD(this->m_CameraConfigureInfo.DVDD);
	//Sleep(1000);
	SetAVDD(this->m_CameraConfigureInfo.AVDD);
	//Sleep(100);
	SetDOVDD(this->m_CameraConfigureInfo.DOVDD);
	//Sleep(1000);
	SetDACAInit();
	//shizhong
	InitClockHzValue();
	//SetClock(this->m_CameraConfigureInfo.MCLKSpeed);
	SetDefineClock(this->m_CameraConfigureInfo.MCLKSpeed);

	//设置掉电
	SetDiaoDian(this->m_CameraConfigureInfo.PwdnPin);
	//Sleep(100);
	//SetDiaoDian(!this->m_CameraConfigureInfo.PwdnPin);
	//Sleep(100);
	//SetDiaoDian(this->m_CameraConfigureInfo.PwdnPin);

	//复位设置
	SetReset(this->m_CameraConfigureInfo.ResetPin);
	Sleep(50);
	SetReset(!this->m_CameraConfigureInfo.ResetPin);
	Sleep(30);
	SetReset(this->m_CameraConfigureInfo.ResetPin);
	//I2CID------------------------------------------------
	/*
	CString str1;
	str1 = "目标摄像头和WDS文件不匹配";
	int v = GetI2CV(this->m_CameraConfigureInfo.I2CSlaveID,this->m_CameraConfigureInfo.I2CFlagReg);
	if( (v&this->m_CameraConfigureInfo.I2CFlagMask) != this->m_CameraConfigureInfo.I2CFlagValue)
	{
		
		::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)str1.AllocSysString(), (LPARAM)1);

		TurnoffPower();
		return false;
	}
	int v1 = GetI2CV(this->m_CameraConfigureInfo.I2CSlaveID,this->m_CameraConfigureInfo.I2CFlagReg1);
	if( (v1&this->m_CameraConfigureInfo.I2CFlagMask1) != this->m_CameraConfigureInfo.I2CFlagValue1)
	{
		::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)str1.AllocSysString(), (LPARAM)1);

		TurnoffPower();
		return false;
	}
	*/
	//------------------------------------------
	CStringArray data1;
	CStringArray data2;
	//CString ll;
	data1.RemoveAll();
	cameraConfigureInfoHelper.Split(this->m_CameraConfigureInfo.DefaultBuf,16*1024,data1);
	//ll.Format("data1.GetSize:%d",data1.GetSize());
	//AfxMessageBox(ll);

	data2.RemoveAll();
	for(int i=0;i<data1.GetSize();i++)
	{
		data2.Add(cameraConfigureInfoHelper.TrimString(data1[i],'/'));
	}
	//寻找地址值
	CStringArray strArray;
	strArray.RemoveAll();
	for(int i=0;i<data2.GetSize();i++)
	{
		CString tstr;
		tstr.Format("%s",data2[i]);;
		data1.RemoveAll();
		SystemHelper::Split(tstr,data1,',');
		if(data1.GetSize()==0)
		{
			continue;
		}

		bool flag = false;
		for(int j=0;j<strArray.GetSize();j++)
		{
			if(data1[0]==strArray[j])
			{
				flag = true;
				break;
			}
		}
		if(flag==false)
		{
			strArray.Add(tstr);
		}
	}

	SetInfoHelper setInfoHelper;
	SetInfo setInfo =  setInfoHelper.GetSetInfo("WDS");

	unsigned char  buf[1];
	ZeroMemory(buf,1);
	long bytesToSend = 1;

	//CStringArray strArray;
	//strArray.RemoveAll();
	//SystemHelper::Split(setInfo.wValues,strArray,',');

	for(int i=0;i<strArray.GetSize();i++)
	{
		strArray[i].ReleaseBuffer();
		int ivalue = SystemHelper::ChangeNum(strArray[i],strArray[i].GetLength());

		WriteControl(0xBF,ivalue,0x0000,buf,bytesToSend);//发送地址

		for(int i=0;i<data2.GetSize();i++)
		{
			CString str1;
			str1.Format("%s",data2[i]);

			data1.RemoveAll();
			SystemHelper::Split(str1,data1,',');

			if(data1.GetSize()<4)
			{
				continue;
			}
			
			int key = SystemHelper::ChangeNum(data1[0],data1[0].GetLength());
			if(ivalue==key)
			{

				int d1 = SystemHelper::ChangeNum(data1[1],data1[1].GetLength());
				int d2 = SystemHelper::ChangeNum(data1[2],data1[2].GetLength());
				int d3 = SystemHelper::ChangeNum(data1[3],data1[3].GetLength());
				//char arr[256];
				//sprintf(arr, "%s - %d",data1[3].GetBuffer(), data1[3].GetLength());
				//MessageBoxA(NULL, arr, "sdf", MB_OK);
				//处理延时
				if(d2==0xeeee)//liangziusb 20151015
				{
					Sleep(d3);
				}

				if(d2==0xffff)
				{
					int flag = (data1[3].GetLength() < 5 && d3 < 0x100) ? 0xB0 : 0xB1;
					
					WriteControl(flag,d1,d3,buf,bytesToSend);
				}
				else
				{
					int v = GetI2CV(ivalue,d1);
					int vv = 0;
					if(d3==1)
					{
						vv = v | d2;
					}
					if(d3==0)//取反，再与，
					{
						int vvv = ~d2;
						vv = v & vvv;
					}
					WriteControl(0xB1,d1,vv,buf,bytesToSend);
				}
				//WriteControl(0xB1,d1,d2,buf,bytesToSend);

				//CString logstr;
				//logstr.Format("d1:%04X d2:%04X bytesToSend:%d",d1,d2,bytesToSend);
				//LogTool::Writelog(logstr+"--\t\n","data");
				//Sleep(100);
			}
		}
	}
	CString info;
	info = "完成加载WDS文件";
	::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	this->m_IsNeedLoadWdsFile = false;

	return true;
}
int USB3Camera::GetI2CV(int id,int reg)
{
	unsigned char  buf[2];
	ZeroMemory(buf,2);
	long bytesToSend = 2;
	this->WriteControl(0xBF,id,0x0000,buf,bytesToSend);
	ZeroMemory(buf,2);
	this->ReadControl(0xB2,reg,0x0000,buf,bytesToSend);
	CString str1;
	CString str2 = "0x";
	for(int i=1;i>=0;i--)
	{
		str1.Format("%02X",buf[i]);
		str2 += str1;
	}

	return SystemHelper::ChangeNum(str2,str2.GetLength());
}
void USB3Camera::LoadImgFile(CString filePath)
{
	this->m_CCyFX3Device = new CCyFX3Device();
	FX3_FWDWNLOAD_ERROR_CODE dwld_status = FAILED;

	if(m_CCyFX3Device->Open(0))
	{
		bool status = m_CCyFX3Device->IsBootLoaderRunning();
		if(status)
		{
			dwld_status = m_CCyFX3Device->DownloadFw((LPSTR)(LPCTSTR)filePath,RAM);
		}
	}

	this->m_CCyFX3Device->Close();
	delete this->m_CCyFX3Device;
	this->m_CCyFX3Device = NULL;

	CString info;
	info = "完成加载img文件";
	::SendMessage(m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);
}

BOOL USB3Camera::GetFrame(unsigned char * pBuffer,long bytesRequested, long* bytesReturned)
{
	BOOL   bResult;
	long   dwBytes     = 0;
    OVERLAPPED          inOvLap;
    UCHAR *             inContext;

	//long len = this->m_InEndpt->MaxPktSize*2;
	//this->m_InEndpt->SetXferSize(bytesRequested);

	//memset(&inOvLap, 0, sizeof(inOvLap));
    inOvLap.hEvent = CreateEvent(NULL, false, false, "CYUSB_IN");
    this->m_InEndpt->TimeOut = 2000;

	//bool IsPkt = IsPacket.Checked ? true : false;
	//bool b = this->m_InEndpt->XferData(pBuffer, bytesRequested);

	inContext = this->m_InEndpt->BeginDataXfer(pBuffer, bytesRequested, &inOvLap);
	/*
	if (this->m_InEndpt->NtStatus || this->m_InEndpt->UsbdStatus) // BeginDataXfer failed
    {
        return false;
    }
	*/
	this->m_InEndpt->WaitForXfer(&inOvLap, 2000);
	/*
    if (!this->m_InEndpt->WaitForXfer(&inOvLap, 2000))
    {
        this->m_InEndpt->Abort();                             //  Transfer failed, clearing the buffer
        if (this->m_InEndpt->LastError == ERROR_IO_PENDING)   //  Error code from Abort()
            WaitForSingleObject(inOvLap.hEvent, 2000); //  For Abort() to complete
    }
	*/
    bResult = this->m_InEndpt->FinishDataXfer(pBuffer, dwBytes, &inOvLap, inContext);
	
    CloseHandle(inOvLap.hEvent); 
    *bytesReturned = dwBytes;

	/*
	CString log;
	for(int j=0;j<dwBytes;j++)
	{
		CString str;
		str.Format("%02X",pBuffer[j]);
		log += str;
	}
	*/
    return (BOOL)bResult;
	//return b;
}
void USB3Camera::Camera_Thread(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;

	//图像大小
	long pImageBufferSize = pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight*pUSB3Camera->m_CameraConfigureInfo.OutFormat;
	//短包标记
	int shortbagsize = 64;
	int maxpktsize = pUSB3Camera->m_InEndpt->MaxPktSize;
	long transferSize = maxpktsize*16;
	//图像缓存尺寸
	pImageBufferSize = transferSize;
	unsigned char * pImageBuffer =  new unsigned char[pImageBufferSize];
	
	USB3Camera::m_IsPlaying = true;
	while(USB3Camera::m_IsPlaying==true)
	{
		ZeroMemory(pImageBuffer,pImageBufferSize);

		long bytesRequestedCount = pImageBufferSize;
		long  nbytesSize = 0;
		long  offset = 0;
		bool IsSuccess=false;
		unsigned char *temp=pImageBuffer;
		long  rowindex = 0;
		while(USB3Camera::m_IsPlaying==true)
		{
			ZeroMemory(pImageBuffer,pImageBufferSize);
			long nbytescount =0;
			if(pUSB3Camera->GetFrame(temp,transferSize, &nbytescount)==true)
			{
				CString str;
				str.Format("transferSize=%d;nbytescount=%d;nbytescount:maxpktsize%d;bytesRequestedCount=%d",transferSize,nbytescount,nbytescount%maxpktsize,bytesRequestedCount);
				LogTool::WriteTimelog(str,"countsize");

				int n1 = nbytescount%maxpktsize;
				int n2 = nbytescount/maxpktsize;
				if(n1==0)
				{
					LogTool::Writelog(LogTool::FormatLog(temp,nbytescount,&rowindex),"data");
					/*
					bytesRequestedCount -= nbytescount;
					offset += nbytescount;
					temp = &(pImageBuffer[offset]);
					*/
				}
				else
				{
					LogTool::Writelog(LogTool::FormatLog(temp,(long)n2*maxpktsize,&rowindex),"data");
				    str.Format("短包大小=%d",n1);
					LogTool::Writelog(str+"------------------short begin------------------------------\t\n","data");
					LogTool::Writelog(LogTool::FormatLog( &(pImageBuffer[n2*maxpktsize]),n1),"data");
					LogTool::Writelog("------------------short end------------------------------\t\n","data");
					rowindex = 0;
					/*
					if(bytesRequestedCount>0)
					{
						LogTool::Writelog(LogTool::FormatLog(temp,nbytescount),"data");
						LogTool::Writelog("\t\n","data");
						break;
					}
					*/
				}
			}
			else
			{
				::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("图像传输发生错误"), (LPARAM)1);
				break;
			}
			
			if(bytesRequestedCount<=0)
			{
				//LogTool::Writelog(LogTool::FormatLog(pImageBuffer,pImageBufferSize),"data");
				//LogTool::Writelog("\t\n","data");

				//::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_DISPLAY, (WPARAM)pImageBuffer,(LPARAM)(pImageBufferSize));
			}
		}
	}

	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("图像传输停止"), (LPARAM)1);
	delete []pImageBuffer;
}
void USB3Camera::Camera_Thread2(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;
	
	CString logstr;
	int queueSize = 1;
	int pktCount = 1024;
	int timeOut = 1500;//时间溢出
	int i = 0;
	//图像大小
	long pImageBufferSize = pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight*pUSB3Camera->m_CameraConfigureInfo.OutFormat;
	//图像缓存尺寸
	unsigned char * pImageBuffer =  new unsigned char[pImageBufferSize];
	//包标记
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;
	long transferSize = maxPktsize*pktCount;
	pUSB3Camera->m_InEndpt->SetXferSize(transferSize);
	//序列
	OVERLAPPED	inOvLap[64];
	PUCHAR *buffers  = new PUCHAR[queueSize];
	PUCHAR *contexts = new PUCHAR[queueSize];
	for (i=0; i< queueSize; i++) 
	{ 
		buffers[i]        = new unsigned char[transferSize];
		inOvLap[i].hEvent = CreateEvent(NULL, false, false, "CYUSB_IN");
		memset(buffers[i],0,transferSize);
	}
	for (i=0; i< queueSize; i++)	
	{
		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) 
        {
			::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("图像传输停止"), (LPARAM)1);
			USB3Camera::m_IsPlaying = false;
        }
	}

	//运行
	i =0;
	long  rowindex = 0;
	while(USB3Camera::m_IsPlaying==true)
	{
		//ZeroMemory(pImageBuffer,pImageBufferSize);

		if (!pUSB3Camera->m_InEndpt->WaitForXfer(&inOvLap[i], timeOut))
		{
			pUSB3Camera->m_InEndpt->Abort();
			if (pUSB3Camera->m_InEndpt->LastError == ERROR_IO_PENDING)
			{
				WaitForSingleObject(inOvLap[i].hEvent,2000);
			}
		}

		long returnCount = 0;
		if (pUSB3Camera->m_InEndpt->FinishDataXfer(buffers[i], returnCount, &inOvLap[i], contexts[i])) 
		{
			int n1 = returnCount%maxPktsize;
			int n2 = returnCount/maxPktsize;

			logstr.Format("transferSize=%d;returnCount=%d;returnCount%maxpktsize=%d;returnCount/maxpktsize=%d",transferSize,returnCount,n1,n2);
			LogTool::WriteTimelog(logstr,"countsize");

			if(n1==0)
			{
				LogTool::Writelog(LogTool::FormatLog(buffers[i],(long)returnCount,&rowindex),"data");
			}
			else
			{
				LogTool::Writelog(LogTool::FormatLog(buffers[i],(long)n2*maxPktsize,&rowindex),"data");
				logstr.Format("短包大小=%d",n1);
				LogTool::Writelog(logstr+"------------------short begin------------------------------\t\n","data");
				LogTool::Writelog(LogTool::FormatLog( &(pImageBuffer[n2*maxPktsize]),n1),"data");
				LogTool::Writelog("------------------short end------------------------------\t\n","data");
				rowindex = 0;
			}

		}
		else
		{
			LogTool::WriteTimelog("-------------失败-------------","countsize");
		}

		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) // BeginDataXfer failed
		{
			break;
		}

		i++;

		if (i == queueSize)
		{
			i=0;			
		}
		
	}

	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("图像传输停止"), (LPARAM)1);

	for (i=0; i< queueSize; i++) 
	{ 
		CloseHandle(inOvLap[i].hEvent);
		delete [] buffers[i];
	}
	delete [] buffers;
	delete [] contexts;

	delete []pImageBuffer;
}
void USB3Camera::Camera_Thread3(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;
	
	CString logstr;
	int queueSize = 64;
	int pktCount = 1024;
	int timeOut = 2000;//时间溢出
	int i = 0;
	//信标
	HANDLE hHasHandle = CreateEvent(NULL, FALSE, FALSE, _T("HANDLE_HAVE_DATA"));
	//包标记
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;
	long transferSize = maxPktsize*pktCount;
	pUSB3Camera->m_InEndpt->SetXferSize(transferSize);
	//序列
	OVERLAPPED	inOvLap[64];
	PUCHAR *buffers  = new PUCHAR[queueSize];
	PUCHAR *contexts = new PUCHAR[queueSize];
	for (i=0; i< queueSize; i++) 
	{ 
		buffers[i]        = new unsigned char[transferSize];
		inOvLap[i].hEvent = CreateEvent(NULL, false, false, "CYUSB_IN");
		memset(buffers[i],0,transferSize);
	}
	for (i=0; i< queueSize; i++)	
	{
		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) 
        {
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("数据采集初始化异常"), (LPARAM)1);
        }
	}

	//运行
	HANDLE hDataHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_DATA"));
	i =0;
	while(USB3Camera::m_IsPlaying==true)
	{
		//WaitForSingleObject( hDataHandle, INFINITE );

		if (!pUSB3Camera->m_InEndpt->WaitForXfer(&inOvLap[i], timeOut))
		{
			pUSB3Camera->m_InEndpt->Abort();
			if (pUSB3Camera->m_InEndpt->LastError == ERROR_IO_PENDING)
			{
				WaitForSingleObject(inOvLap[i].hEvent,2000);
			}
		}

		long returnCount = 0;
		if (pUSB3Camera->m_InEndpt->FinishDataXfer(buffers[i], returnCount, &inOvLap[i], contexts[i])) 
		{
			unsigned char * pBuffer =  new unsigned char[returnCount];
			memset(pBuffer,0,returnCount);
			memcpy(pBuffer,buffers[i],returnCount); 

			BufData buf;
			buf.pBuf = pBuffer;
			buf.size = returnCount;
			WaitForSingleObject( pUSB3Camera->m_hAccessSem, INFINITE);
			pUSB3Camera->m_pBuffers.push_back(buf);
			ReleaseSemaphore( pUSB3Camera->m_hAccessSem, 1, NULL );

			SetEvent(hHasHandle);

			logstr.ReleaseBuffer();
			logstr.Format("transferSize:%d;returnCount:%d;returnCount求模maxpktsize:%d;returnCount除以maxpktsize:%d",transferSize,returnCount,returnCount%maxPktsize,returnCount/maxPktsize);
			LogTool::WriteTimelog(logstr,"countsize");
		}
		else
		{
			LogTool::WriteTimelog("-------------失败-------------","countsize");
		}

		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) // BeginDataXfer failed
		{
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("数据采集初始化异常"), (LPARAM)1);
			LogTool::WriteTimelog("------------数据采集初始化异常-----------","countsize");
		}

		i++;

		if (i == queueSize)
		{
			i=0;			
		}
	}

	for (i=0; i< queueSize; i++) 
	{ 
		CloseHandle(inOvLap[i].hEvent);
		delete [] buffers[i];
	}
	delete [] buffers;
	delete [] contexts;

	CloseHandle(hHasHandle);
	CloseHandle(hDataHandle);

	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)("数据采集退出"), (LPARAM)1);
}

void USB3Camera::Camera_Thread4(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;

	if(pUSB3Camera->m_InEndpt==NULL) return;
	
	CString info;
	CString logstr;
	int queueSize = 64;
	int pktCount = 1024;//1024
	int timeOut = 1000;//时间溢出
	int i = 0;

	bool bHighSpeed = pUSB3Camera->m_USBDevice->bHighSpeed;
	
	//序列
	OVERLAPPED	inOvLap[64];
	PUCHAR *buffers  = new PUCHAR[queueSize];
	//memset(buffers,0x00,queueSize);
	PUCHAR *contexts = new PUCHAR[queueSize];
	//memset(contexts,0x00,queueSize);

	//包标记
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;
	long transferSize = maxPktsize*pktCount;
	pUSB3Camera->m_InEndpt->SetXferSize(transferSize);

	for (i=0; i< queueSize; i++) 
	{ 
		buffers[i]        = new UCHAR[transferSize];
		memset(buffers[i],0x00,transferSize);
		inOvLap[i].hEvent = CreateEvent(NULL, false, false, NULL);

		memset(buffers[i],0xEF,transferSize);
	}
	for (i=0; i< queueSize; i++)	
	{
		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) 
        {
			logstr.Format("传输发生m_InEndpt->NtStatus:%08X",pUSB3Camera->m_InEndpt->NtStatus);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)2);
			pUSB3Camera->AbortXferLoop(i+1,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

			//切断电压
			pUSB3Camera->TurnoffPower();
			return;
        }
	}

	long size = pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight*pUSB3Camera->m_CameraConfigureInfo.OutFormat*2;

	i =0;
	unsigned char * orgData = NULL;
	long  revcount = -1;
	long  surpluscount = 0;
	long  successcount = 0;
	long  failcount = 0;

	CTime t1=CTime::GetCurrentTime();
	pUSB3Camera->m_InEndpt->TimeOut = 200;
	while(USB3Camera::m_IsPlaying==true)
	{
		long returnCount = transferSize;

		if (!pUSB3Camera->m_InEndpt->WaitForXfer(&inOvLap[i], timeOut))
		{
			//LogTool::Writelog("--------------WaitForXfer false-----------------------\n","countsize");
			pUSB3Camera->m_InEndpt->Abort();
			if (pUSB3Camera->m_InEndpt->LastError == ERROR_IO_PENDING)
			{
				//LogTool::Writelog("--------------WaitForSingleObject 2000-----------------------\n","countsize");
				//WaitForSingleObject(inOvLap[i].hEvent,2000);
			}
		}

		if (pUSB3Camera->m_InEndpt->FinishDataXfer(buffers[i], returnCount, &inOvLap[i], contexts[i])) 
		{
			unsigned char * pBuffer =  buffers[i];

			successcount += returnCount;

			logstr.Format("请求数量:%d;返回数量:%d;returnCount求模maxpktsize:%d;returnCount除以maxpktsize:%d",transferSize,returnCount,returnCount%maxPktsize,returnCount/maxPktsize);
			LogTool::WriteTimelog(logstr,"countsize");

			//LogTool::WriteTimelog(LogTool::FormatLog(pBuffer, returnCount),"countsize");

			int n1 = returnCount%maxPktsize;
			int n2 = returnCount/maxPktsize;
			if(n1!=0)
			{
				if(revcount>0)
				{
					if(returnCount<surpluscount)
					{
						memcpy(&orgData[revcount],pBuffer,returnCount);
						revcount += returnCount;
						surpluscount -= returnCount;
					}
					else
					{
						memcpy(&orgData[revcount],pBuffer,surpluscount);
						revcount += surpluscount;
						surpluscount -= surpluscount;
					}

					if(surpluscount==0 )
					{
						if(pUSB3Camera->m_pImageBuffers.size()<20)
						{
							BufData buf;
							buf.pBuf = orgData;
							buf.size = revcount;
							WaitForSingleObject( pUSB3Camera->m_hImageAccessSem, INFINITE);
							pUSB3Camera->m_pImageBuffers.push_back(buf);
							ReleaseSemaphore( pUSB3Camera->m_hImageAccessSem, 1, NULL );
						}
						else
						{
							delete [] orgData;
							orgData = NULL;
							//LogTool::Writelog("-----------------释放多余数据----------------------------\n","pdata");
						}
					}
					else
					{
						delete [] orgData;
						orgData = NULL;
						
						//LogTool::Writelog("-----------------释放残数据----------------------------\n","pdata");
					}
				}

				int duanindex = pUSB3Camera->GetDuanCountIndex(pBuffer[returnCount-1],pBuffer[returnCount-2]);
				logstr.Format("短包编号：%d ",duanindex);
				//LogTool::Writelog("-----------------"+logstr+"------------------------------\n","countsize");

				::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_COUNT, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

				orgData =  new unsigned char[size];
				memset(orgData,0x00,size);
				revcount = 0;
				surpluscount = size;
			}
			else
			{
				if(surpluscount!=0 && revcount>=0)
				{
					if(returnCount<surpluscount)
					{
						memcpy(&orgData[revcount],pBuffer,returnCount);
						revcount += returnCount;
						surpluscount -= returnCount;
					}
					else
					{
						memcpy(&orgData[revcount],pBuffer,surpluscount);
						revcount += surpluscount;
						surpluscount -= surpluscount;
					}
				}
			}
		}
		else
		{
			failcount++;

		}

		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) // BeginDataXfer failed
		{
			logstr.Format("传输发生m_InEndpt->NtStatus:%08X",pUSB3Camera->m_InEndpt->NtStatus);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)2);
			pUSB3Camera->AbortXferLoop(queueSize,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

			//切断电压
			pUSB3Camera->TurnoffPower();
			return;
		}

		i++;

		if (i == queueSize)
		{
			i=0;

			logstr.Format("失败包数:%d",failcount);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_FAILURECOUNT, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

			CTime t = CTime::GetCurrentTime();
			CTimeSpan span = t-t1;
			int totMS = span.GetTotalSeconds();
			if (totMS >0)
			{
				long XferRate = successcount / totMS;
				XferRate = XferRate / (1024*1024);

				logstr.Format("传输速度:%d MB/s",XferRate);
				::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

				t1=CTime::GetCurrentTime();
				successcount = 0;
			}
		}
	}
	info = "数据采集准备退出...";
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	pUSB3Camera->AbortXferLoop(queueSize,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

	info = "数据采集退出";
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	//切断电压
	pUSB3Camera->TurnoffPower();
}
void USB3Camera::Camera_Thread5(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;
	if(pUSB3Camera->m_InEndpt==NULL) return;
	
	CString info;
	CString logstr;
	int queueSize = 64;
	int pktCount = 1024;//1024
	int timeOut = 1000;//时间溢出
	int i = 0;

	
	long size = 0;
	switch(pUSB3Camera->m_CameraConfigureInfo.OutFormat)
	{
		case 0x02://YUV
			size = pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight * 2*2;
			break;
		case 0x03://RAW data
			//size = 15 * 1024 *1024 + 191*1024 + 854;
			size =  pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight;
			break;
		default:
			return;
	}

	LONG tmpLen = 32;
	auto pTmp = new unsigned char[tmpLen];

	while (USB3Camera::m_IsPlaying == true)
	{
		BufData buf;
		buf.pBuf = new unsigned char[size];
		buf.size = size;
		LONG val = 128;
		for (int i = 0; i < size; i += val)
		{
			bool k = pUSB3Camera->m_USBDevice->BulkInEndPt->XferData(buf.pBuf, val);
			if (!k)
			{
				OutputDebugStringA("1234");
			}
		}
		
 		WaitForSingleObject(pUSB3Camera->m_hImageAccessSem, INFINITE);
 		pUSB3Camera->m_pImageBuffers.push_back(buf);
 		ReleaseSemaphore(pUSB3Camera->m_hImageAccessSem, 1, NULL);

		
		pUSB3Camera->m_USBDevice->BulkInEndPt->XferData(pTmp, tmpLen);
	}


	info.Format("数据采集准备退出...");
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);
	info.ReleaseBuffer();
	info.Format("数据采集退出");
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	//切断电压
	pUSB3Camera->TurnoffPower();
}
int USB3Camera::GetDuanCountIndex(unsigned char h1,unsigned char h2)
{
	return h1*0x100+h2;
}
long USB3Camera::GetDuanCountIndex1(unsigned char h1,unsigned char h2,unsigned char h3,unsigned char h4)
{
	return h1*0x1000000+h2*0x10000+h3*0x100+h4;
}
void USB3Camera::AbortXferLoop(int pending,int pktCount,int queueSize,int timeOut, PUCHAR *buffers, PUCHAR *contexts, OVERLAPPED *inOvLap)
{      
    long len = m_InEndpt->MaxPktSize * pktCount;

    for (int j=0; j< queueSize; j++) 
    { 
        if (j<pending)
        {
            if (!m_InEndpt->WaitForXfer(&inOvLap[j], timeOut)) 
            {
                m_InEndpt->Abort();
                if (m_InEndpt->LastError == ERROR_IO_PENDING)
				{
                    //WaitForSingleObject(inOvLap[j].hEvent,2000);
				}	
            }

            m_InEndpt->FinishDataXfer(buffers[j], len, &inOvLap[j], contexts[j]);
        }
		

        CloseHandle(inOvLap[j].hEvent);
        delete [] buffers[j];
    }

    delete [] buffers;
    delete [] contexts;

	USB3Camera::m_IsPlaying = false;
}
void USB3Camera::GetData_Thread(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;

	if(pUSB3Camera->m_InEndpt==NULL) return;
	
	CString info;
	CString logstr;
	int queueSize = 64;
	int pktCount = 1024;//1024
	int timeOut = 1000;//时间溢出
	int i = 0;

	bool bHighSpeed = pUSB3Camera->m_USBDevice->bHighSpeed;
	
	//序列
	OVERLAPPED	inOvLap[64];
	PUCHAR *buffers  = new PUCHAR[queueSize];
	//memset(buffers,0x00,queueSize);
	PUCHAR *contexts = new PUCHAR[queueSize];
	//memset(contexts,0x00,queueSize);

	//包标记
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;
	long transferSize = maxPktsize*pktCount;
	pUSB3Camera->m_InEndpt->SetXferSize(transferSize);

	for (i=0; i< queueSize; i++) 
	{ 
		buffers[i]        = new UCHAR[transferSize];
		memset(buffers[i],0x00,transferSize);
		inOvLap[i].hEvent = CreateEvent(NULL, false, false, NULL);

		memset(buffers[i],0xEF,transferSize);
	}
	for (i=0; i< queueSize; i++)	
	{
		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) 
        {
			logstr.Format("传输发生m_InEndpt->NtStatus:%08X",pUSB3Camera->m_InEndpt->NtStatus);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)2);
			pUSB3Camera->AbortXferLoop(i+1,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

			//切断电压
			pUSB3Camera->TurnoffPower();
			return;
        }
	}

	i =0;
	//long  revcount = -1;
	//long  surpluscount = 0;
	long  successcount = 0;
	long  failcount = 0;

	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_WRITE_LOG"));
	CTime t1=CTime::GetCurrentTime();
	pUSB3Camera->m_InEndpt->TimeOut = 200;
	while(USB3Camera::m_IsPlaying==true)
	{
		long returnCount = transferSize;

		if (!pUSB3Camera->m_InEndpt->WaitForXfer(&inOvLap[i], timeOut))
		{
			//LogTool::Writelog("--------------WaitForXfer false-----------------------\n","countsize");
			pUSB3Camera->m_InEndpt->Abort();
			if (pUSB3Camera->m_InEndpt->LastError == ERROR_IO_PENDING)
			{
				//LogTool::Writelog("--------------WaitForSingleObject 2000-----------------------\n","countsize");
				WaitForSingleObject(inOvLap[i].hEvent,2000);
			}
		}

		if (pUSB3Camera->m_InEndpt->FinishDataXfer(buffers[i], returnCount, &inOvLap[i], contexts[i])) 
		{
			logstr.Format("请求数量:%d;返回数量:%d;returnCount求模maxpktsize:%d;returnCount除以maxpktsize:%d",transferSize,returnCount,returnCount%maxPktsize,returnCount/maxPktsize);
			LogTool::WriteTimelog(logstr,"GetDataThreadcountsize");

			logstr.Format("pUSB3Camera->m_pImageBuffers:%d",pUSB3Camera->m_pImageBuffers.size());
			LogTool::WriteTimelog(logstr,"GetDataThreadcountsize");
			successcount += returnCount;

			if(pUSB3Camera->m_pImageBuffers.size()<1000)
			{
				unsigned char * pBuffer =  new unsigned char[returnCount];
				memset(pBuffer,0x00,returnCount);
				memcpy(pBuffer,buffers[i],returnCount);

				BufData buf;
				buf.pBuf = pBuffer;
				buf.size = returnCount;
				WaitForSingleObject( pUSB3Camera->m_hImageAccessSem, INFINITE);
				pUSB3Camera->m_pImageBuffers.push_back(buf);
				ReleaseSemaphore( pUSB3Camera->m_hImageAccessSem, 1, NULL );
				SetEvent(hHandle);
			}
			
		}
		else
		{
			failcount++;
		}

		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) // BeginDataXfer failed
		{
			logstr.Format("传输发生m_InEndpt->NtStatus:%08X",pUSB3Camera->m_InEndpt->NtStatus);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)2);
			pUSB3Camera->AbortXferLoop(queueSize,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

			//切断电压
			pUSB3Camera->TurnoffPower();
			return;
		}

		i++;

		if (i == queueSize)
		{
			i=0;

			logstr.Format("失败包数:%d",failcount);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_FAILURECOUNT, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

			CTime t = CTime::GetCurrentTime();
			CTimeSpan span = t-t1;
			int totMS = span.GetTotalSeconds();
			if (totMS >0)
			{
				long XferRate = successcount / totMS;
				XferRate = XferRate / (1024*1024);

				logstr.Format("传输速度:%d MB/s",XferRate);
				::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

				t1=CTime::GetCurrentTime();
				successcount = 0;
			}
		}
	}
	info = "数据采集准备退出...";
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	LogTool::WriteTimelog(info+"\t\n","GetDataThreadcountsize");

	pUSB3Camera->AbortXferLoop(queueSize,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

	LogTool::WriteTimelog("AbortXferLoop....\t\n","GetDataThreadcountsize");

	info = "数据采集退出";
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	//切断电压
	pUSB3Camera->TurnoffPower();
}
void USB3Camera::Write_Thread(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;

	CString logstr;
	long  rowindex = 0;
	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_WRITE_LOG"));
	while ( true )
	{
		WaitForSingleObject( hHandle, INFINITE );
		if(!pUSB3Camera->m_pImageBuffers.empty())
		{
			BufData buf = *(pUSB3Camera->m_pImageBuffers.begin());

			WaitForSingleObject( pUSB3Camera->m_hAccessSem, INFINITE);
			pUSB3Camera->m_pImageBuffers.erase(pUSB3Camera->m_pImageBuffers.begin());
			ReleaseSemaphore( pUSB3Camera->m_hAccessSem, 1, NULL );

		    int n1 = buf.size%maxPktsize;
			int n2 = buf.size/maxPktsize;
			if(n1==0)
			{
				LogTool::Writelog(LogTool::FormatLog(buf.pBuf,buf.size,&rowindex),"data");
			}
			else
			{
				LogTool::Writelog(LogTool::FormatLog(buf.pBuf,n2*maxPktsize,&rowindex),"data");
				logstr.Format("短包大小%d",n1);
				LogTool::Writelog(logstr+"------------------short begin------------------------------\t\n","data");
				LogTool::Writelog(LogTool::FormatLog( &(buf.pBuf[n2*maxPktsize]),n1),"data");
				LogTool::Writelog("------------------short end------------------------------\t\n","data");
				rowindex = 0;
			}

			delete [] buf.pBuf;
		}
		else
		{
			if(USB3Camera::m_IsPlaying==false)
			{
				break;
			}
		}
	}

	CloseHandle(hHandle);
}
//对应马硬件版本
void USB3Camera::Image_Thread(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;

	long size = pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight*pUSB3Camera->m_CameraConfigureInfo.OutFormat*2;
	unsigned char * orgData = NULL;

	CString logstr;
	long  revcount = -1;
	long  surpluscount = 0;
	bool  Is12 = false;
	bool  Is24 = false;
	bool  IsHasM = false;
	int   offset12 = 2;
	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_HAVE_DATA"));
	while ( USB3Camera::m_IsPlaying==true )
	{
		WaitForSingleObject( hHandle, INFINITE );

		if(!pUSB3Camera->m_pBuffers.empty())
		{
			BufData buf = *(pUSB3Camera->m_pBuffers.begin());

			WaitForSingleObject( pUSB3Camera->m_hAccessSem, INFINITE);
			pUSB3Camera->m_pBuffers.erase(pUSB3Camera->m_pBuffers.begin());
			ReleaseSemaphore( pUSB3Camera->m_hAccessSem, 1, NULL );

			logstr.Format("出队数据BufData;buf.size求模maxpktsize:%d---buf.size除以maxpktsize:%d",buf.size%maxPktsize,buf.size/maxPktsize);
			LogTool::WriteTimelog(logstr,"pdata");
			logstr = LogTool::FormatLog(buf.pBuf,12);
			LogTool::Writelog(logstr,"pdata");

		    int n1 = buf.size%maxPktsize;//
			int n2 = buf.size/maxPktsize;
			if(n1==0)
			{
				if(surpluscount!=0 && revcount>=0)
				{
					if(buf.size<surpluscount)
					{
						memcpy(&orgData[revcount],buf.pBuf,buf.size);
						revcount += buf.size;
						surpluscount -= buf.size;
					}
					else
					{
						memcpy(&orgData[revcount],buf.pBuf,surpluscount);
						revcount += surpluscount;
						surpluscount -= surpluscount;
					}
				}
			}
			else
			{
				//发现短包
				//logstr.Format("短包buf.size:%d----------",buf.size);
				//LogTool::WriteTimelog(logstr,"duanbao");
				logstr = LogTool::FormatLog(&buf.pBuf[buf.size-24],24);
				LogTool::Writelog("短包末尾数据："+logstr+"\n","pdata");

				if(revcount>0)
				{
					logstr.Format("短包 ->已经接受:%d;还剩:%d;buf.size:%d",revcount,surpluscount,buf.size);
					LogTool::Writelog(logstr+"---------------\t\n","pdata");

					if(buf.size<surpluscount)
					{
						memcpy(&orgData[revcount],buf.pBuf,buf.size);
						revcount += buf.size;
						surpluscount -= buf.size;
					}
					else
					{
						memcpy(&orgData[revcount],buf.pBuf,surpluscount);
						revcount += surpluscount;
						surpluscount -= surpluscount;
					}

					if(surpluscount==0 )
					{
						if(pUSB3Camera->m_pImageBuffers.size()<15)
						{
							//logstr.Format("短包buf.size:%d----------",buf.size);
							//LogTool::WriteTimelog(logstr,"duanbao");
							//logstr = LogTool::FormatLog(buf.pBuf,buf.size);
							//LogTool::Writelog(logstr,"duanbao");
							BufData buf;
							buf.pBuf = orgData;
							buf.size = revcount;
							WaitForSingleObject( pUSB3Camera->m_hImageAccessSem, INFINITE);
							pUSB3Camera->m_pImageBuffers.push_back(buf);
							ReleaseSemaphore( pUSB3Camera->m_hImageAccessSem, 1, NULL );

							logstr = LogTool::FormatLog(orgData,12);
							LogTool::Writelog("图像数据orgData-->"+logstr+"\n","pdata");
						}
						else
						{
							delete [] orgData;
							orgData = NULL;

							LogTool::Writelog("-----------------释放多余数据----------------------------\n","pdata");
						}
					}
					else
					{
						delete [] orgData;
						orgData = NULL;
						LogTool::Writelog("-----------------释放残数据----------------------------\n","pdata");
					}
				}
			
				
				orgData =  new unsigned char[size];
				memset(orgData,0x00,size);
				revcount = 0;
				surpluscount = size;

				//logstr.Format("开图像内存:%d",size);
				LogTool::Writelog("----------------开图像内存-----------------------\n","pdata");
				
				Is24 = false;
				
				if(n1==24)
				{
					LogTool::Writelog("----------开图像内存-------Is24 = true-------------------------\n","pdata");
					Is24 = true;
				}
				if(n1==12)
				{
					LogTool::Writelog("-----------------短包数据12----------------------------\n","pdata");
					Is12 = true;
				}
				
			}

			delete [] buf.pBuf;
		}
		else
		{
			ResetEvent(hHandle);
		}
	}

	CloseHandle(hHandle);
}
//对于MDR硬件版本
void USB3Camera::Image_Thread1(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;

	long size = pUSB3Camera->m_CameraConfigureInfo.PreviewWidth*pUSB3Camera->m_CameraConfigureInfo.PreviewHeight*pUSB3Camera->m_CameraConfigureInfo.OutFormat*2;
	unsigned char * orgData = NULL;

	CString logstr;
	long  revcount = -1;
	long  surpluscount = 0;
	bool  Is12 = false;
	bool  Is24 = false;
	bool  IsHasM = false;
	int   offset12 = 2;
	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_WRITE_LOG"));
	while ( USB3Camera::m_IsPlaying==true )
	{
		WaitForSingleObject( hHandle, INFINITE );
		if(!pUSB3Camera->m_pBuffers.empty())
		{
			BufData buf = *(pUSB3Camera->m_pBuffers.begin());

			WaitForSingleObject( pUSB3Camera->m_hAccessSem, INFINITE);
			pUSB3Camera->m_pBuffers.erase(pUSB3Camera->m_pBuffers.begin());
			ReleaseSemaphore( pUSB3Camera->m_hAccessSem, 1, NULL );

			logstr.Format("出队数据BufData;buf.size求模maxpktsize:%d---buf.size除以maxpktsize:%d",buf.size%maxPktsize,buf.size/maxPktsize);
			LogTool::WriteTimelog(logstr,"pdata");
			logstr = LogTool::FormatLog(buf.pBuf,12);
			LogTool::Writelog(logstr,"pdata");


		    int n1 = buf.size%maxPktsize;//
			int n2 = buf.size/maxPktsize;
			if(n1==0)
			{
				if(surpluscount!=0 && revcount>=0)
				{
					if(buf.size<surpluscount)
					{
						memcpy(&orgData[revcount],buf.pBuf,buf.size);
						revcount += buf.size;
						surpluscount -= buf.size;
					}
					else
					{
						memcpy(&orgData[revcount],buf.pBuf,surpluscount);
						revcount += surpluscount;
						surpluscount -= surpluscount;
					}
				}
			}
			else
			{
				//发现短包
				//logstr.Format("短包buf.size:%d----------",buf.size);
				//LogTool::WriteTimelog(logstr,"duanbao");
				logstr = LogTool::FormatLog(&buf.pBuf[buf.size-24],24);
				LogTool::Writelog("短包末尾数据："+logstr+"\n","pdata");

				if(revcount>0)
				{
					logstr.Format("短包 ->已经接受:%d;还剩:%d;buf.size:%d",revcount,surpluscount,buf.size);
					LogTool::Writelog(logstr+"---------------\t\n","pdata");

					if(buf.size<surpluscount)
					{
						memcpy(&orgData[revcount],buf.pBuf,buf.size);
						revcount += buf.size;
						surpluscount -= buf.size;
					}
					else
					{
						memcpy(&orgData[revcount],buf.pBuf,surpluscount);
						revcount += surpluscount;
						surpluscount -= surpluscount;
					}

					if(surpluscount==0 )
					{
						if(pUSB3Camera->m_pImageBuffers.size()<15)
						{
							//logstr.Format("短包buf.size:%d----------",buf.size);
							//LogTool::WriteTimelog(logstr,"duanbao");
							//logstr = LogTool::FormatLog(buf.pBuf,buf.size);
							//LogTool::Writelog(logstr,"duanbao");
							BufData buf;
							buf.pBuf = orgData;
							buf.size = revcount;
							WaitForSingleObject( pUSB3Camera->m_hImageAccessSem, INFINITE);
							pUSB3Camera->m_pImageBuffers.push_back(buf);
							ReleaseSemaphore( pUSB3Camera->m_hImageAccessSem, 1, NULL );

							logstr = LogTool::FormatLog(orgData,12);
							LogTool::Writelog("图像数据orgData-->"+logstr+"\n","pdata");
						}
						else
						{
							delete [] orgData;
							orgData = NULL;

							LogTool::Writelog("-----------------释放多余数据----------------------------\n","pdata");
						}
					}
					else
					{
						delete [] orgData;
						orgData = NULL;
						LogTool::Writelog("-----------------释放残数据----------------------------\n","pdata");
					}
				}
			
				
				orgData =  new unsigned char[size];
				memset(orgData,0x00,size);
				revcount = 0;
				surpluscount = size;

				//logstr.Format("开图像内存:%d",size);
				LogTool::Writelog("----------------开图像内存-----------------------\n","pdata");
				
				Is24 = false;
				
				if(n1==24)
				{
					LogTool::Writelog("----------开图像内存-------Is24 = true-------------------------\n","pdata");
					Is24 = true;
				}
				if(n1==12)
				{
					LogTool::Writelog("-----------------短包数据12----------------------------\n","pdata");
					Is12 = true;
				}
				
			}

			delete [] buf.pBuf;
		}
		else
		{
			ResetEvent(hHandle);
		}
	}

	CloseHandle(hHandle);
}
void USB3Camera::StartPlay()
{
	ClearInBuffers();
	USB3Camera::m_IsPlaying = true;
	HANDLE hThread = AfxBeginThread((AFX_THREADPROC)Camera_Thread5,(void*)this,THREAD_PRIORITY_TIME_CRITICAL);
}
void USB3Camera::StopPlay()
{
	TurnoffPower();

	USB3Camera::m_IsPlaying = false;
}
void USB3Camera::StartGetData()
{
	ClearInBuffers();
	USB3Camera::m_IsPlaying = true;
	AfxBeginThread((AFX_THREADPROC)GetData_Thread,(void*)this,THREAD_PRIORITY_TIME_CRITICAL);
	AfxBeginThread((AFX_THREADPROC)Write_Thread,(void*)this);
}
void USB3Camera::StopGetData()
{
	USB3Camera::m_IsPlaying = false;
}

void USB3Camera::ClearInBuffers()
{
	WaitForSingleObject( this->m_hAccessSem, INFINITE);
	vector<BufData>::iterator it;
	for(it=this->m_pBuffers.begin();it!=this->m_pBuffers.end();it++)
	{
		delete [] it->pBuf;
	}
	this->m_pBuffers.clear();
	ReleaseSemaphore( this->m_hAccessSem, 1, NULL );
}

void USB3Camera::OutData_Thread(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;

	if(pUSB3Camera->m_InEndpt==NULL) return;
	
	CString info;
	CString logstr;
	int queueSize = 64;
	int pktCount = 1024;//1024
	int timeOut = 1000;//时间溢出
	int i = 0;

	bool bHighSpeed = pUSB3Camera->m_USBDevice->bHighSpeed;
	
	//序列
	OVERLAPPED	inOvLap[64];
	PUCHAR *buffers  = new PUCHAR[queueSize];
	//memset(buffers,0x00,queueSize);
	PUCHAR *contexts = new PUCHAR[queueSize];
	//memset(contexts,0x00,queueSize);

	//包标记
	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;
	long transferSize = maxPktsize*pktCount;
	pUSB3Camera->m_InEndpt->SetXferSize(transferSize);

	for (i=0; i< queueSize; i++) 
	{ 
		buffers[i]        = new UCHAR[transferSize];
		memset(buffers[i],0x00,transferSize);
		inOvLap[i].hEvent = CreateEvent(NULL, false, false, NULL);

		memset(buffers[i],0xEF,transferSize);
	}
	for (i=0; i< queueSize; i++)	
	{
		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) 
        {
			logstr.Format("传输发生m_InEndpt->NtStatus:%08X",pUSB3Camera->m_InEndpt->NtStatus);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)2);
			pUSB3Camera->AbortXferLoop(i+1,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

			//切断电压
			pUSB3Camera->TurnoffPower();
			return;
        }
	}

	i =0;
	long  successcount = 0;
	long  failcount = 0;

	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_ReCreateOutData"));
	CTime t1=CTime::GetCurrentTime();
	pUSB3Camera->m_InEndpt->TimeOut = 200;
	while(USB3Camera::m_IsPlaying==true)
	{
		long returnCount = transferSize;

		if (!pUSB3Camera->m_InEndpt->WaitForXfer(&inOvLap[i], timeOut))
		{
			//LogTool::Writelog("--------------WaitForXfer false-----------------------\n","countsize");
			pUSB3Camera->m_InEndpt->Abort();
			if (pUSB3Camera->m_InEndpt->LastError == ERROR_IO_PENDING)
			{
				//LogTool::Writelog("--------------WaitForSingleObject 2000-----------------------\n","countsize");
				WaitForSingleObject(inOvLap[i].hEvent,2000);
			}
		}

		if (pUSB3Camera->m_InEndpt->FinishDataXfer(buffers[i], returnCount, &inOvLap[i], contexts[i])) 
		{
			logstr.Format("请求数量:%d;返回数量:%d;returnCount求模maxpktsize:%d;returnCount除以maxpktsize:%d",transferSize,returnCount,returnCount%maxPktsize,returnCount/maxPktsize);
			LogTool::WriteTimelog(logstr,"GetDataThreadcountsize");

			logstr.Format("pUSB3Camera->m_pImageBuffers:%d",pUSB3Camera->m_pImageBuffers.size());
			LogTool::WriteTimelog(logstr,"GetDataThreadcountsize");
			successcount += returnCount;

			if(pUSB3Camera->m_pImageBuffers.size()<1000)
			{
				unsigned char * pBuffer =  new unsigned char[returnCount];
				memset(pBuffer,0x00,returnCount);
				memcpy(pBuffer,buffers[i],returnCount);

				BufData buf;
				buf.pBuf = pBuffer;
				buf.size = returnCount;
				WaitForSingleObject( pUSB3Camera->m_hImageAccessSem, INFINITE);
				pUSB3Camera->m_pImageBuffers.push_back(buf);
				ReleaseSemaphore( pUSB3Camera->m_hImageAccessSem, 1, NULL );
				SetEvent(hHandle);
			}
			
		}
		else
		{
			failcount++;
		}

		contexts[i] = pUSB3Camera->m_InEndpt->BeginDataXfer(buffers[i], transferSize, &inOvLap[i]);
		if (pUSB3Camera->m_InEndpt->NtStatus || pUSB3Camera->m_InEndpt->UsbdStatus) // BeginDataXfer failed
		{
			logstr.Format("传输发生m_InEndpt->NtStatus:%08X",pUSB3Camera->m_InEndpt->NtStatus);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)2);
			pUSB3Camera->AbortXferLoop(queueSize,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

			//切断电压
			pUSB3Camera->TurnoffPower();
			return;
		}

		i++;

		if (i == queueSize)
		{
			i=0;

			logstr.Format("失败包数:%d",failcount);
			::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_FAILURECOUNT, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

			CTime t = CTime::GetCurrentTime();
			CTimeSpan span = t-t1;
			int totMS = span.GetTotalSeconds();
			if (totMS >0)
			{
				long XferRate = successcount / totMS;
				XferRate = XferRate / (1024*1024);

				logstr.Format("传输速度:%d MB/s",XferRate);
				::PostMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)logstr.AllocSysString(), (LPARAM)1);

				t1=CTime::GetCurrentTime();
				successcount = 0;
			}
		}
	}
	info = "数据采集准备退出...";
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	LogTool::WriteTimelog(info+"\t\n","GetDataThreadcountsize");

	pUSB3Camera->AbortXferLoop(queueSize,pktCount,queueSize,timeOut, buffers, contexts, inOvLap);

	LogTool::WriteTimelog("AbortXferLoop....\t\n","GetDataThreadcountsize");

	info = "数据采集退出";
	::SendMessage(pUSB3Camera->m_pOwner->m_hWnd, WM_USB_STATE, (WPARAM)info.AllocSysString(), (LPARAM)2);

	//切断电压
	pUSB3Camera->TurnoffPower();
}
void USB3Camera::ReCreateOutData_Thread(void *param)
{
	USB3Camera *pUSB3Camera = (USB3Camera *)param;

	if(pUSB3Camera->m_InEndpt==NULL) return;

	int maxPktsize = pUSB3Camera->m_InEndpt->MaxPktSize;

	CString logstr;
	long  rowindex = 0;
	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_ReCreateOutData"));
	while ( USB3Camera::m_IsPlaying==true )
	{
		WaitForSingleObject( hHandle, INFINITE );
		if(!pUSB3Camera->m_pImageBuffers.empty())
		{
			BufData buf = *(pUSB3Camera->m_pImageBuffers.begin());

			WaitForSingleObject( pUSB3Camera->m_hAccessSem, INFINITE);
			pUSB3Camera->m_pImageBuffers.erase(pUSB3Camera->m_pImageBuffers.begin());
			ReleaseSemaphore( pUSB3Camera->m_hAccessSem, 1, NULL );

			delete [] buf.pBuf;
		}
	}

	CloseHandle(hHandle);
}