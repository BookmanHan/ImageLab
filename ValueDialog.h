#pragma once
#include "afxwin.h"


// CValueDialog 对话框

class CValueDialog : public CDialog
{
	DECLARE_DYNAMIC(CValueDialog)
public:
	double Value;
	CString strShow;

public:
	CValueDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CValueDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_edtValue;
	CEdit m_edtShow;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk2();
};
