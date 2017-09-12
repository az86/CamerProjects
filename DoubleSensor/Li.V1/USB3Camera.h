#pragma once
#include "CyAPI.h"
#include "CameraConfigureInfo.h"
#include <vector>
using namespace std;

#define _VECTORLOCK()  \
		WaitForSingleObject( m_hAccessSem, INFINITE)
#define _UNVECTORLOCK()    \
		ReleaseSemaphore( m_hAccessSem, 1, NULL )

#define WM_USB_DISPLAY				WM_USER+7
#define WM_USB_STATE				WM_USER+8
#define WM_USB_COUNT				WM_USER+9
#define WM_USB_FAILURECOUNT				WM_USER+10

typedef struct BufDataStruct
{
	unsigned char *pBuf;
	int      size;
}BufData;
class USB3Camera
{
public:
	USB3Camera(CWnd* owner);
	~USB3Camera(void);

	void InitDevice();
	void GetBulkNames(CString deviceName);
	void GetDeviceNames();
	void CloseDevice();

	void SetAVDD(int i);
	void SetDOVDD(int i);
	void SetDVDD(int i);
	void SetClock(int i);
	void SetDefineClock(int clockHzValue);
	void SetDiaoDian(int i);
	void SetReset(int i);
	void InitClockHzValue();
	bool LoadWDSFile(CString filePath);
	void LoadImgFile(CString filePath);
	void SetDACAInit();
	int  GetDuanCountIndex(unsigned char h1,unsigned char h2);
	void TurnoffPower();
	void TurnonPower();

	BOOL GetFrame(unsigned char * pBuffer,long bytesRequested, long* bytesReturned);

	void StartPlay();
	void StopPlay();

	void StartGetData();
	void StopGetData();
	
	void ReadControl(int requestCode,UCHAR *data,long readLength);
	void WriteControl(int requestCode,int value,int index,UCHAR *data,long writeLength);
	void ReadControl(int requestCode,int value,int index,UCHAR *data,long writeLength);

	void BulkReadService();
	void StartBulkRead();
	void StopBulkRead();
	void ClearInBuffers();
	

	void WriteBulk(byte data);
	void BulkWriteService( LPVOID params );

	CString	m_DeviceName;
	CString	m_OutEndptName;
	CString	m_InEndptName;

	CStringArray m_DeviceNames;
	CStringArray m_OutEndptNames;
	CStringArray m_InEndptNames;

	static bool m_IsPlaying;

	BOOL	m_IsNeedLoadWdsFile;

	CameraConfigureInfo m_CameraConfigureInfo;
	int GetI2CV(int id,int reg);

	long GetDuanCountIndex1(unsigned char h1,unsigned char h2,unsigned char h3,unsigned char h4);

	vector<BufData>	m_pImageBuffers;
	HANDLE  m_hImageAccessSem;
protected:
	static UINT BulkRead( LPVOID params );
	UINT ReadInEndpt();
	void InitBulkOutAndInEndPoind();
	void GetDeviceName();
	

	void AbortXferLoop(int pending,int pktCount,int queueSize,int timeOut, PUCHAR *buffers, PUCHAR *contexts, OVERLAPPED* inOvLap);

	static void Camera_Thread(void *param);
	static void Camera_Thread2(void *param);
	static void Camera_Thread3(void *param);
	static void Camera_Thread4(void *param);
	static void Camera_Thread5(void *param);
	static void Display_Thread(void *param);
	static void Write_Thread(void *param);
	static void Image_Thread(void *param);
	static void Image_Thread1(void *param);
	static void GetData_Thread(void *param);

	static void OutData_Thread(void *param);
	static void ReCreateOutData_Thread(void *param);
private:
	CCyUSBDevice   *m_USBDevice;
	CCyFX3Device   *m_CCyFX3Device;

	CCyUSBEndPoint *m_OutEndpt;
    CCyUSBEndPoint *m_InEndpt;

	CWnd*			m_pOwner;

	vector<BufData>	m_pBuffers;

	HANDLE  m_hAccessSem;

	

};

