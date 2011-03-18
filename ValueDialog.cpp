// ValueDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageLab.h"
#include "ValueDialog.h"
#include <cmath>

// CValueDialog 对话框

IMPLEMENT_DYNAMIC(CValueDialog, CDialog)

CValueDialog::CValueDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CValueDialog::IDD, pParent)
{

}

CValueDialog::~CValueDialog()
{
}

void CValueDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_edtValue);
	DDX_Control(pDX, IDC_EDIT1, m_edtShow);
}


BEGIN_MESSAGE_MAP(CValueDialog, CDialog)
	ON_BN_CLICKED(ID_OK, &CValueDialog::OnBnClickedOk)
	ON_BN_CLICKED(ID_OK2, &CValueDialog::OnBnClickedOk2)
END_MESSAGE_MAP()


// CValueDialog 消息处理程序

double CStringToDouble(CString str)
{
	double d = 0;
	int nDot = 0;

	for(int i=0;i<str.GetLength();i++)
	{
		if (str[i] == '.')
		{
			nDot++;
			continue;
		}

		if (nDot == 0)
		{
			d = d*10 + str[i] - '0';
		}
		else
		{
			d = d + double(str[i] - '0') / pow(10.0,nDot*1.0);

			nDot ++;
		}
	}

	return d;
}

void CValueDialog::OnBnClickedOk()
{
	CString strValue;

	m_edtValue.GetWindowText(strValue);

	Value = CStringToDouble(strValue);

	OnOK();
}

BOOL CValueDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edtShow.SetWindowText(strShow);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CValueDialog::OnBnClickedOk2()
{
	OnCancel();
}
