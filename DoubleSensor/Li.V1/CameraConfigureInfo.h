#pragma once

class CameraConfigureInfo
{
public:
	CameraConfigureInfo(void);
	~CameraConfigureInfo(void);

	CString SensorName;  // 芯片名字，自定义的

	int MCLKSpeed;   //MCLK 时钟
	int I2CSlaveID; //摄像头i2c通信地址，十进制的，也可以是16进制  
    int I2CMode;       //i2c 模式 0: 8Addr,8Data;   3: 16Addr,8Data;    4: 16 Addr,16Data
	int PreviewWidth;  //预览 长度
	int PreviewHeight;  //预览 宽度
	int PreviewType;   //输出数据格式 1:Raw ;  2:YUV422;
	int OutFormat;     //数据格式顺序 0:YCbYCr/RG_GB; 1:YCrYCb/GR_BG; 2:CbYCrY/GB_RG; 3:CrYCbY/BG_GR
	int ResetPin;     //rest 高低 控制；
	int PwdnPin;      //PWDN 高低 控制
    int AFTYPE;        //AF 模式控制 ，你们可以不需要设置 
	int I2CFlagReg;      
	int I2CFlagValue;
	int I2CFlagMask;
	int I2CFlagReg1;
	int I2CFlagValue1;
	int I2CFlagMask1;
	int AVDD;
	int DOVDD;
	int DVDD;


	char DefaultBuf[1024*16];
	//CStringArray  m_DefaultStringArray;
};

