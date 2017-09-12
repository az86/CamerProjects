#pragma once

class CameraConfigureInfo
{
public:
	CameraConfigureInfo(void);
	~CameraConfigureInfo(void);

	CString SensorName;  // оƬ���֣��Զ����

	int MCLKSpeed;   //MCLK ʱ��
	int I2CSlaveID; //����ͷi2cͨ�ŵ�ַ��ʮ���Ƶģ�Ҳ������16����  
    int I2CMode;       //i2c ģʽ 0: 8Addr,8Data;   3: 16Addr,8Data;    4: 16 Addr,16Data
	int PreviewWidth;  //Ԥ�� ����
	int PreviewHeight;  //Ԥ�� ���
	int PreviewType;   //������ݸ�ʽ 1:Raw ;  2:YUV422;
	int OutFormat;     //���ݸ�ʽ˳�� 0:YCbYCr/RG_GB; 1:YCrYCb/GR_BG; 2:CbYCrY/GB_RG; 3:CrYCbY/BG_GR
	int ResetPin;     //rest �ߵ� ���ƣ�
	int PwdnPin;      //PWDN �ߵ� ����
    int AFTYPE;        //AF ģʽ���� �����ǿ��Բ���Ҫ���� 
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

