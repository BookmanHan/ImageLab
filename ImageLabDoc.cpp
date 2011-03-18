// ImageLabDoc.cpp : CImageLabDoc 类的实现
//

#include "stdafx.h"
#include "ImageLab.h"

#include "ImageLabDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageLabDoc

IMPLEMENT_DYNCREATE(CImageLabDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageLabDoc, CDocument)
END_MESSAGE_MAP()


// CImageLabDoc 构造/析构

CImageLabDoc::CImageLabDoc()
{
	// TODO: 在此添加一次性构造代码

}

CImageLabDoc::~CImageLabDoc()
{
}

BOOL CImageLabDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CImageLabDoc 序列化

void CImageLabDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CImageLabDoc 诊断

#ifdef _DEBUG
void CImageLabDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageLabDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageLabDoc 命令
