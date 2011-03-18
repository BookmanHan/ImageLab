// MainFrm.h : CMainFrame 类的接口
//

#include "ImageBase.h"

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	CImageBase ibDoc;
	afx_msg void OnOpenImage();
	afx_msg void OnGray();
	afx_msg void OnPart();
	afx_msg void OnLinarTrans();
	bool Doit(void);
	afx_msg void OnLogTrans();
	afx_msg void OnLogStdTrans();
	afx_msg void OnExpTrans();
	afx_msg void OnThreshold();
	afx_msg void OnMenu32797();
	afx_msg void OnStretchNearest();
	afx_msg void OnStretchBilinar();
	afx_msg void OnXY();
};


