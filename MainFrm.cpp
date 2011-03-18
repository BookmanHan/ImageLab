// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "ImageLab.h"
#include "ValueDialog.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_OPEN_IMAGE, &CMainFrame::OnOpenImage)
	ON_COMMAND(ID_GRAY, &CMainFrame::OnGray)
	ON_COMMAND(ID_PART, &CMainFrame::OnPart)
	ON_COMMAND(ID_LINAR_TRANS, &CMainFrame::OnLinarTrans)
	ON_COMMAND(ID_LOG_TRANS, &CMainFrame::OnLogTrans)
	ON_COMMAND(ID_LOG_STD_TRANS, &CMainFrame::OnLogStdTrans)
	ON_COMMAND(ID_EXP_TRANS, &CMainFrame::OnExpTrans)
	ON_COMMAND(ID_THRESHOLD, &CMainFrame::OnThreshold)
	ON_COMMAND(ID_Menu32797, &CMainFrame::OnMenu32797)
	ON_COMMAND(ID_STRETCH_NEAREST, &CMainFrame::OnStretchNearest)
	ON_COMMAND(ID_STRETCH_BILINAR, &CMainFrame::OnStretchBilinar)
	ON_COMMAND(ID_X_Y, &CMainFrame::OnXY)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetMenu(NULL);

	CMenu menuMain;
	menuMain.LoadMenu(IDR_MAINMENU);

	SetMenu(&menuMain);

	menuMain.Detach();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序




void CMainFrame::OnOpenImage()
{
	CFileDialog dlgFile(true,NULL,NULL,NULL,L"BMP 图像文件|*.bmp||");
	
	if (dlgFile.DoModal() == IDOK)
	{
		CImageBase ib;
		ibDoc.Load(dlgFile.GetPathName());
		//ibDoc.ModelFrom(&ib);

		GetActiveView()->Invalidate();
	}
}

void CMainFrame::OnGray()
{
	ibDoc.Gray();

	GetActiveView()->Invalidate();
}

void CMainFrame::OnPart()
{
	CRect rect;

	CValueDialog vd;
	vd.strShow = L"请输入 X 的位置：";
	if (vd.DoModal() == IDOK)
	{
		rect.left = vd.Value;
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入 Y 的位置：";
	if (vd.DoModal() == IDOK)
	{
		rect.top = vd.Value;
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入 Width 的位置：";
	if (vd.DoModal() == IDOK)
	{
		rect.right = rect.left + vd.Value;
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入 Height 的位置：";
	if (vd.DoModal() == IDOK)
	{
		rect.bottom = rect.top + vd.Value;
	}
	else
	{
		return;
	}

	ibDoc.Part(rect);

	GetActiveView()->Invalidate();
}

void CMainFrame::OnLinarTrans()
{
	Doit();

	CValueDialog vd;
	vd.strShow = L"请输入线性变换的系数值：";

	double dFa = 0.0;

	if (vd.DoModal() == IDOK)
	{
		dFa = vd.Value;
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入线性变换的节距：";

	double dFb = 0.0;

	if (vd.DoModal() == IDOK)
	{
		dFb = vd.Value;
	}
	else
	{
		return;
	}

	ibDoc.LinarTrans(dFa,dFb);

	GetActiveView()->Invalidate();
}

bool CMainFrame::Doit(void)
{
	return true;
}

void CMainFrame::OnLogTrans()
{
	Doit();

	CValueDialog vd;
	vd.strShow = L"请输入对数系数：";

	double dLog = 0.0;

	if (vd.DoModal() == IDOK)
	{
		dLog = vd.Value;
	}
	else
	{
		return;
	}

	ibDoc.LogTrans(dLog);

	GetActiveView()->Invalidate();
}

void CMainFrame::OnLogStdTrans()
{
	Doit();

	ibDoc.LogTransStd();

	GetActiveView()->Invalidate();
}

void CMainFrame::OnExpTrans()
{
	Doit();

	CValueDialog vd;
	vd.strShow = L"请输入指数变换的指数系数：";

	double dExp = 0.0;

	if (vd.DoModal() == IDOK)
	{
		dExp = vd.Value;
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入指数变换的加权系数：";

	double dR = 0.0;

	if (vd.DoModal() == IDOK)
	{
		dR = vd.Value;
	}
	else
	{
		return;
	}

	ibDoc.ExpTrans(dExp,dR);

	GetActiveView()->Invalidate();
}

void CMainFrame::OnThreshold()
{
	Doit();

	CValueDialog vd;
	vd.strShow = L"请输入阈值：";

	double dThre = 0.0;

	if (vd.DoModal() == IDOK)
	{
		dThre = vd.Value;
	}
	else
	{
		return;
	}

	ibDoc.Threshold(int(dThre));

	GetActiveView()->Invalidate();
}

void CMainFrame::OnMenu32797()
{
	Doit();

	ibDoc.HistEq();

	GetActiveView()->Invalidate();
}

void CMainFrame::OnStretchNearest()
{
	Doit();

	CRect rct = CRect(0,0,0,0);

	CValueDialog vd;
	
	vd.strShow = L"请输入原矩形的Left：";
	if (vd.DoModal() == IDOK)
	{
		rct.left = int(vd.Value);
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入原矩形的Right：";
	if (vd.DoModal() == IDOK)
	{
		rct.right = int(vd.Value);
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入原矩形的Top：";
	if (vd.DoModal() == IDOK)
	{
		rct.top = int(vd.Value);
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入原矩形的Bottom：";
	if (vd.DoModal() == IDOK)
	{
		rct.bottom = int(vd.Value);
	}
	else
	{
		return;
	}

	ibDoc.StretchNearest(rct,CRect(0,0,ibDoc.GetWidth(),ibDoc.GetHeight()));

	GetActiveView()->Invalidate();
}

void CMainFrame::OnStretchBilinar()
{
	Doit();

	CRect rct = CRect(0,0,0,0);

	CValueDialog vd;

	vd.strShow = L"请输入原矩形的Left：";
	if (vd.DoModal() == IDOK)
	{
		rct.left = int(vd.Value);
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入原矩形的Right：";
	if (vd.DoModal() == IDOK)
	{
		rct.right = int(vd.Value);
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入原矩形的Top：";
	if (vd.DoModal() == IDOK)
	{
		rct.top = int(vd.Value);
	}
	else
	{
		return;
	}

	vd.strShow = L"请输入原矩形的Bottom：";
	if (vd.DoModal() == IDOK)
	{
		rct.bottom = int(vd.Value);
	}
	else
	{
		return;
	}


	ibDoc.StretchBilinar(rct,CRect(0,0,ibDoc.GetWidth(),ibDoc.GetHeight()));

	GetActiveView()->Invalidate();
}

void CMainFrame::OnXY()
{
	ibDoc.Threshold(127);
	ibDoc.ReduceEdge(100);
	ibDoc.Convex(true);

	GetActiveView()->Invalidate();
}
