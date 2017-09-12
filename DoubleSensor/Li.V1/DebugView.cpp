// DebugView.cpp : implementation file
//

#include "stdafx.h"
#include "DebugView.h"
#include "afxdialogex.h"

#define   WIDTHBYTES(bits)         (((bits)   +   31)   /   32   *   4)
#define RGB565_MASK_RED		0xF800 
#define RGB565_MASK_GREEN	0x07E0 
#define RGB565_MASK_BLUE	0x001F
#define clip(min, x, max) x=(x < min) ? min : (x > max) ? max : x 
// DebugView dialog

IMPLEMENT_DYNAMIC(DebugView, CDialog)

DebugView::DebugView(CWnd* pParent /*=NULL*/)
	: CDialog(DebugView::IDD, pParent)
{

}

DebugView::~DebugView()
{
}

void DebugView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAYER, m_Video);
}


BEGIN_MESSAGE_MAP(DebugView, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// DebugView message handlers

BOOL DebugView::AllocateBuffers(CameraConfigureInfo info)
{
	/*
    CString        str;

    if (info.PreviewWidth == 0 || info.PreviewHeight == 0)
    {
        MessageBox("WDS文件中分辨率设置错误", "错误", MB_OK | MB_ICONSTOP);
        return false;
    }

	int nPixelBytes = 2;
	switch(OutFormat)
	{
		case 0:
			nPixelBytes = 1;
			break;
		case 1:
		case 2:
			nPixelBytes = 2;
			break;
	}
  

    int nRawBufferSize = info.PreviewWidth * info.PreviewHeight * nPixelBytes;
    m_nBufferSize = USB_GetBufferSize(m_pUSBContext,m_nRawBufferSize);

    //set the window position and display position to the appropriate sensor data size
    SetWindowPos(NULL, 0, 0, BORDER+m_nWidth +BORDER, EDIT_BORDER+BORDER+m_nHeight+BORDER, SWP_NOZORDER | SWP_NOMOVE);
    GetDlgItem(IDC_DISPLAY)->SetWindowPos(NULL, BORDER, EDIT_BORDER, m_nWidth, m_nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    //initialize the directdraw surface
    m_pDD = new CDirectDrawSurface();
    if (m_pDD->Create(GetDlgItem(IDC_DISPLAY)->m_hWnd, m_nWidth, m_nHeight) != DD_OK)
    {
        MessageBox("Could not initialize the DirectDraw surface.", "Error", MB_OK | MB_ICONSTOP);
        return false;
    }
    //m_pDD->SetDirectDrawAccel(CDirectDrawSurface::USEDD_ACCELERATED or _STANDARD or _NONE);

    //initialize the needed buffers 
    m_pTripleBuffer   = (unsigned char *)malloc(m_nBufferSize);
    m_pCameraBuffer   = (unsigned char *)malloc(m_nBufferSize);
    m_pDisplayBuffer  = (unsigned char *)malloc(m_nBufferSize);
    m_hDisplayBmp     = CreateCompatibleBitmap(::GetDC(m_hWnd), m_nWidth, m_nHeight);
    if (m_pTripleBuffer==NULL  || 
        m_pCameraBuffer==NULL  || 
        m_pDisplayBuffer==NULL || 
        m_hDisplayBmp==NULL)
    {
        MessageBox("Failed to create nescecary buffers to display the image");
        return false;
    }

    //get the current display format (preferably 32bpp)
    m_imageBpp = GetDeviceCaps(::GetDC(m_hWnd), BITSPIXEL);
    if (!(m_imageBpp==24 || m_imageBpp==32 || m_imageBpp==16))
    {
        MessageBox("Unsuported display format, please set the display to 32bpp (preferred), 24bpp or 16bpp");
        return false;
    }
	*/
    return true;
}
void DebugView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
}


void DebugView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}
void DebugView::Display(unsigned char *pBuff,long length)
{
	int biWidth  = m_CameraConfigureInfo.PreviewWidth; // 图像的高度
	int biHeight = m_CameraConfigureInfo.PreviewHeight; // 图像的宽度

	////http://www.rosoo.net/a/201107/14730.html
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_DISPLAYER);
	pWnd->GetClientRect(&rect);
	pDC = pWnd->GetDC(); 

	pWnd->Invalidate();  //使整个客户区窗口无效，因此需要重绘，会调用OnPaint
	pWnd->UpdateWindow();	//窗户去立即重绘
	pDC->Rectangle(&rect);  //绘制矩形

	//内存绘图，减少闪烁
    CBitmap memBitmap;
	CBitmap* pOldBmp = NULL;
	memDC.CreateCompatibleDC(pDC); //pDC指向当前客户区,为memDC创建一块兼容pDC的内存
	memBitmap.CreateCompatibleBitmap(pDC,rect.right,rect.bottom);//以当前DC创建一块位图区域
	pOldBmp = memDC.SelectObject(&memBitmap); //保存此时的位图信息
	memDC.BitBlt(rect.left,rect.top,rect.right,rect.bottom,pDC,0,0,SRCCOPY);//复杂位图到DC指向的区域

	//------------------下面是关键 由数组 显示图片的-------------------

	FILE* fr=NULL;
	BYTE* pData1;
	pData1=(BYTE*)new char[biWidth*biHeight*3];//biWidth和biHeight为视频采集卡获取//的图像尺寸 
	
	ZeroMemory(pData1,biWidth*biHeight*3); //将这块内存清零
	UYVY2BMP(pData1, pBuff);

	BITMAPINFO *m_pBMI1; //
	m_pBMI1= new BITMAPINFO;//自定义的BMP文件信息结构，用于后面的图像显示
	m_pBMI1->bmiHeader.biBitCount=24;  //每像素的位数
	m_pBMI1->bmiHeader.biClrImportant=0; //
	m_pBMI1->bmiHeader.biClrUsed=0;
	m_pBMI1->bmiHeader.biCompression=0;
	m_pBMI1->bmiHeader.biHeight=biHeight;
	m_pBMI1->bmiHeader.biPlanes=1;
	m_pBMI1->bmiHeader.biSize=40;  //这个结构体所需要的字节数
	m_pBMI1->bmiHeader.biSizeImage=WIDTHBYTES(biWidth*8)*biHeight*3;  //不知道这个位所代表的含义
	m_pBMI1->bmiHeader.biWidth=biWidth;
	m_pBMI1->bmiHeader.biXPelsPerMeter=0;
	m_pBMI1->bmiHeader.biYPelsPerMeter=0;

	::StretchDIBits(memDC,0,0,biWidth,biHeight,0,0,biWidth,biHeight,pData1,m_pBMI1,DIB_RGB_COLORS,SRCCOPY);//讲源矩形数据复制到目标矩形，并且可以改变其大小
	pDC->BitBlt(rect.left,rect.top,rect.right,rect.bottom,&memDC,0,0,SRCCOPY);

	delete m_pBMI1;
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	memBitmap.DeleteObject();
	
}
void DebugView::SetCameraConfigureInfo(CameraConfigureInfo cameraConfigureInfo)
{
	this->m_CameraConfigureInfo =  cameraConfigureInfo;
	GetDlgItem(IDC_DISPLAYER)->SetWindowPos(NULL, 2, 2, this->m_CameraConfigureInfo.PreviewWidth,this->m_CameraConfigureInfo.PreviewHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
}
void DebugView::SetCameraSize(int w ,int h)
{
	GetDlgItem(IDC_DISPLAYER)->SetWindowPos(NULL, 2, 2, w,h, SWP_NOZORDER | SWP_SHOWWINDOW);
}
void DebugView::UpdateDisplayer()
{
	//GetDlgItem(IDC_DISPLAYER)->Invalidate(true);
	if(m_NeedUpdateDisplayer==true)
	{
		Invalidate(true);
		m_NeedUpdateDisplayer = false;
	}
}
BOOL DebugView::UYVY2BMP(BYTE* pDst, BYTE *pSrc) 
{ 
	int biWidth  = m_CameraConfigureInfo.PreviewWidth; // 图像的高度
	int biHeight = m_CameraConfigureInfo.PreviewHeight; // 图像的宽度

	long m = 0; 
	long k = 0; 
	int n=biWidth/2; 
	int dec=biWidth*4; 
 
	int tmpR0 = 0; 
	int tmpG0 = 0; 
	int tmpB0 = 0; 
	int tmpR1 = 0; 
	int tmpG1 = 0; 
	int tmpB1 = 0; 
	MakeConversionTable(); 
	k=(biHeight-1)*biWidth<<1;         //k指向最后一行 
	for( int i=biHeight-1;i>-1;i--) 
	{ 
	   for(int j=0;j<n;j++) 
	   { 
			tmpR0 = (table_UYVY2rgb.YtoR[pSrc[k + 1]]+ table_UYVY2rgb.VtoR[pSrc[k + 2]] - 22906) >> 7; 
			tmpG0 = (table_UYVY2rgb.YtoG[pSrc[k + 1]]- table_UYVY2rgb.VtoG[pSrc[k + 2]]- table_UYVY2rgb.UtoG[pSrc[k + 0]] + 17264) >> 7; 
			tmpB0 = (table_UYVY2rgb.YtoB[pSrc[k + 1]] + table_UYVY2rgb.UtoB[pSrc[k + 0]] - 28928) >> 7; 
    
			tmpR1 = (table_UYVY2rgb.YtoR[pSrc[k + 3]]+ table_UYVY2rgb.VtoR[pSrc[k + 2]] - 22906) >> 7; 
			tmpG1 = (table_UYVY2rgb.YtoG[pSrc[k + 3]]- table_UYVY2rgb.VtoG[pSrc[k + 2]] - table_UYVY2rgb.UtoG[pSrc[k + 0]] + 17264) >> 7; 
			tmpB1 = (table_UYVY2rgb.YtoB[pSrc[k + 3]] + table_UYVY2rgb.UtoB[pSrc[k + 0]] - 28928) >> 7; 
    
			clip(0, tmpR0, 255); 
			clip(0, tmpG0, 255); 
			clip(0, tmpB0, 255); 
			clip(0, tmpR1, 255); 
			clip(0, tmpG1, 255); 
			clip(0, tmpB1, 255); 
 
			pDst[m + 0] = tmpB0;   
			pDst[m + 1] = tmpG0;   
			pDst[m + 2] = tmpR0;   
			pDst[m + 3] = tmpB1;   
			pDst[m + 4] = tmpG1;   
			pDst[m + 5] = tmpR1;   
    
			k       +=       4;   
			m       +=       6;   
	   }   
	   k=k-dec; 
	} 
	return 1; 
} 
void DebugView::MakeConversionTable() 
{ 
	for (long j = 0; j < 256; ++j) 
	{ 
		table_UYVY2rgb.YtoR[j] = table_UYVY2rgb.YtoG[j] = table_UYVY2rgb.YtoB[j] = (unsigned short)(j << 7); 
		table_UYVY2rgb.VtoR[j] = j * 180;     //180=1.402*128 
		table_UYVY2rgb.VtoG[j] = j * 91; 
		table_UYVY2rgb.UtoG[j] = j * 44;      //0.3437 = 44/128 
		table_UYVY2rgb.UtoB[j] = j * 226;      //1.772 = 256/128 
	} 
} 
CDC* DebugView::GetDisplayDC()
{
	if(m_Video.m_hWnd!=NULL)
	{
		return m_Video.GetDC();
	}
	return NULL;
}


void DebugView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo; 
    GetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
    switch (nSBCode) 
    { 
    case SB_BOTTOM: 
        ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMax)*10); 
        scrollinfo.nPos = scrollinfo.nMax; 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        break; 
    case SB_TOP: 
        ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMin)*10); 
        scrollinfo.nPos = scrollinfo.nMin; 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        break; 
    case SB_LINEUP: 
        scrollinfo.nPos -= 1; 
        if (scrollinfo.nPos<scrollinfo.nMin) 
        { 
            scrollinfo.nPos = scrollinfo.nMin; 
            break; 
        } 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        ScrollWindow(0,10); 
        break; 
    case SB_LINEDOWN: 
        scrollinfo.nPos += 1; 
        if (scrollinfo.nPos>scrollinfo.nMax) 
        { 
            scrollinfo.nPos = scrollinfo.nMax; 
            break; 
        } 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        ScrollWindow(0,-10); 
        break; 
    case SB_PAGEUP: 
        scrollinfo.nPos -= 5; 
        if (scrollinfo.nPos<scrollinfo.nMin) 
        { 
            scrollinfo.nPos = scrollinfo.nMin; 
            break; 
        } 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        ScrollWindow(0,10*5); 
        break; 
    case SB_PAGEDOWN: 
        scrollinfo.nPos += 5; 
        if (scrollinfo.nPos>scrollinfo.nMax) 
        { 
            scrollinfo.nPos = scrollinfo.nMax; 
            break; 
        } 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        ScrollWindow(0,-10*5); 
        break; 
    case SB_ENDSCROLL: 
        // MessageBox("SB_ENDSCROLL"); 
        break; 
    case SB_THUMBPOSITION: 
        // ScrollWindow(0,(scrollinfo.nPos-nPos)*10); 
        // scrollinfo.nPos = nPos; 
        // SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        break; 
    case SB_THUMBTRACK: 
        ScrollWindow(0,(scrollinfo.nPos-nPos)*10); 
        scrollinfo.nPos = nPos; 
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
        break; 
    } 
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void DebugView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo; 
    GetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
    switch (nSBCode) 
    { 
    case SB_LEFT: 
        ScrollWindow((scrollinfo.nPos-scrollinfo.nMin)*10,0); 
        scrollinfo.nPos = scrollinfo.nMin; 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        break; 
    case SB_RIGHT: 
        ScrollWindow((scrollinfo.nPos-scrollinfo.nMax)*10,0); 
        scrollinfo.nPos = scrollinfo.nMax; 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        break; 
    case SB_LINELEFT: 
        scrollinfo.nPos -= 1; 
        if (scrollinfo.nPos<scrollinfo.nMin) 
        { 
            scrollinfo.nPos = scrollinfo.nMin; 
            break; 
        } 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        ScrollWindow(10,0); 
        break; 
    case SB_LINERIGHT: 
        scrollinfo.nPos += 1; 
        if (scrollinfo.nPos>scrollinfo.nMax) 
        { 
            scrollinfo.nPos = scrollinfo.nMax; 
            break; 
        } 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        ScrollWindow(-10,0); 
        break; 
    case SB_PAGELEFT: 
        scrollinfo.nPos -= 5; 
        if (scrollinfo.nPos<scrollinfo.nMin) 
        { 
            scrollinfo.nPos = scrollinfo.nMin; 
            break; 
        } 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        ScrollWindow(10*5,0); 
        break; 
    case SB_PAGERIGHT: 
        scrollinfo.nPos += 5; 
        if (scrollinfo.nPos>scrollinfo.nMax) 
        { 
            scrollinfo.nPos = scrollinfo.nMax; 
            break; 
        } 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        ScrollWindow(-10*5,0); 
        break; 
    case SB_THUMBPOSITION: 
        break; 
    case SB_THUMBTRACK: 
        ScrollWindow((scrollinfo.nPos-nPos)*10,0); 
        scrollinfo.nPos = nPos; 
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL); 
        break; 
    case SB_ENDSCROLL: 
        break; 
    } 

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
void DebugView::IsDisplay(bool f)
{
	if(f==true)
	{
		GetDlgItem(IDC_DISPLAYER)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_DISPLAYER)->ShowWindow(SW_HIDE);
	}
}


void DebugView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);	
	//SetCameraSize(cx-10 ,cy-10);
	CWnd* pWnd = GetDlgItem(IDC_DISPLAYER);
	if(pWnd->GetSafeHwnd())
	{
		pWnd->SetWindowPos(NULL, 0, 0, cx-0,cy-0, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	/*
	if(this->GetSafeHwnd())
	{
		this->SetWindowPos(NULL, 10, 10, 5000,3000, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	*/
}

int DebugView::ProcessData(byte *pIn,int size)
{
	CRect rcClient;
	CWnd *pWnd=this->GetDlgItem(IDC_DISPLAYER);
	pWnd->GetClientRect(&rcClient);
	CDC* pDC=pWnd->GetDC();

	CDC memDC;	
	memDC.CreateCompatibleDC(pDC); 

	return 1;
}
CRect DebugView::GetDisplayRect()
{
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_DISPLAYER);
	pWnd->GetClientRect(&rect);

	return rect;
}


BOOL DebugView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//this->SetScrollRange(0,10,5000,1);
	//this->SetScrollRange(1,10,3000,1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
