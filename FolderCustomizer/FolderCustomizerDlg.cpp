
// FolderCustomizerDlg.cpp: 实现文件
//
#pragma once
#include "pch.h"
#include "framework.h"
#include "FolderCustomizer.h"
#include "FolderCustomizerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <cstring>

// CFolderCustomizerDlg 对话框



CFolderCustomizerDlg::CFolderCustomizerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FOLDERCUSTOMIZER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//注册COM组件,鬼大爷知道为啥要注册
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (!SUCCEEDED(hr)) {
		MessageBox(L"COM组件注册成功。");
	}
}

CFolderCustomizerDlg::~CFolderCustomizerDlg()
{
	//卸载COM组件,鬼大爷知道为何要卸载
	CoUninitialize();
}

void CFolderCustomizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCSHELLTREE, m_treeCtrl);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID,m_wndPropList);
}

BEGIN_MESSAGE_MAP(CFolderCustomizerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_CBN_SELCHANGE(IDC_DRIVERSCOMBO, &CFolderCustomizerDlg::OnCbnSelchangeDriverscombo)
ON_NOTIFY(TVN_SELCHANGED, IDC_MFCSHELLTREE, &CFolderCustomizerDlg::OnTvnSelchangedMfcshelltree)
END_MESSAGE_MAP()


// CFolderCustomizerDlg 消息处理程序

void CFolderCustomizerDlg::InitDriversCombo()
{
	CComboBox* pCombox;
	pCombox = (CComboBox*)GetDlgItem(IDC_DRIVERSCOMBO);
	DWORD drivers = GetLogicalDrives();
	WCHAR c = 'A';
	for (size_t i = 0; i < 26; i++)
	{
		int r = (drivers >> i) & 1;
		if (r) {
			WCHAR driver[] = {c,':','\0'};
			pCombox->AddString(driver);
			
		}
		c += 1;
	}
	pCombox->SetCurSel(0);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFolderCustomizerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFolderCustomizerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFolderCustomizerDlg::UpdateTreeCtrl()
{
	CComboBox* pCombox;
	pCombox = (CComboBox*)GetDlgItem(IDC_DRIVERSCOMBO);
	int index = pCombox->GetCurSel();
	if (index == CB_ERR) {
		return;
	}
	WCHAR driver[4] = {0};
	pCombox->GetLBText(index, driver);
	driver[2] = '\\';
	driver[3] = '\0';
	m_treeCtrl.SetRootFolder(driver);
}

BOOL CFolderCustomizerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码
	InitDriversCombo();
	UpdateTreeCtrl();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFolderCustomizerDlg::OnCbnSelchangeDriverscombo()
{
	UpdateTreeCtrl();
}


void CFolderCustomizerDlg::OnTvnSelchangedMfcshelltree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	//更新文件夹Combox
	CComboBox* pCombox;
	pCombox = (CComboBox*)GetDlgItem(IDC_FOLDERCOMBO);
	TVITEM itemNew= pNMTreeView->itemNew;
	HTREEITEM hItem= itemNew.hItem;
	CString strPath;
	BOOL ret= m_treeCtrl.GetItemPath(strPath, hItem);
	if (ret!=FALSE) {
		int iRet= pCombox->FindStringExact(0,strPath);
		if (iRet == CB_ERR) {
		pCombox->AddString(strPath);
		}
		pCombox->SelectString(0,strPath);
	}
	//更新属性列表框

	*pResult = 0;
}
