// ImageLabView.cpp : CImageLabView 类的实现
//

#include "stdafx.h"
#include "ImageLab.h"

#include "ImageLabDoc.h"
#include "ImageLabView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageLabView

IMPLEMENT_DYNCREATE(CImageLabView, CView)

BEGIN_MESSAGE_MAP(CImageLabView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CImageLabView 构造/析构

CImageLabView::CImageLabView()
{
	// TODO: 在此处添加构造代码

}

CImageLabView::~CImageLabView()
{
}

BOOL CImageLabView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CImageLabView 绘制

void CImageLabView::OnDraw(CDC* pDC)
{
	CImageLabDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CMainFrame* mf = (CMainFrame*)AfxGetMainWnd();

	mf->ibDoc.Display(pDC);

	// TODO: 在此处为本机数据添加绘制代码
}


// CImageLabView 打印

BOOL CImageLabView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CImageLabView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CImageLabView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CImageLabView 诊断

#ifdef _DEBUG
void CImageLabView::AssertValid() const
{
	CView::AssertValid();
}

void CImageLabView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageLabDoc* CImageLabView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageLabDoc)));
	return (CImageLabDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageLabView 消息处理程序
