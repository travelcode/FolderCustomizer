
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
#include <memory>
#include <system_error>
#include <filesystem>
using namespace std;
using namespace std::filesystem;
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
	DDX_Control(pDX, IDC_MFCPROPERTYGRID, m_wndPropList);
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
			WCHAR driver[] = { c,':','\0' };
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
	CComboBox* pCombox = nullptr;
	pCombox = (CComboBox*)GetDlgItem(IDC_DRIVERSCOMBO);
	int index = pCombox->GetCurSel();
	if (index == CB_ERR) {
		return;
	}
	WCHAR driver[4] = { 0 };
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
	CStatic* errorText = (CStatic*)GetDlgItem(IDC_ERROR);
	errorText->SetWindowTextW(L"");
	//更新文件夹Combox
	CComboBox* pCombox = nullptr;
	pCombox = (CComboBox*)GetDlgItem(IDC_FOLDERCOMBO);
	TVITEM itemNew = pNMTreeView->itemNew;
	HTREEITEM hItem = itemNew.hItem;
	CString strPath;
	BOOL ret = m_treeCtrl.GetItemPath(strPath, hItem);
	ret = pCombox->FindString(0, strPath);
	if (ret == -1) {
		pCombox->AddString(strPath);
	}
	pCombox->SelectString(0, strPath);

	//更新属性列表框
	CString temp;
	CString strProp((LPCTSTR)IDS_CTRLHEADERPROP);
	CString strValue((LPCTSTR)IDS_CTRLHEADERVALUE);
	//希望你删除了所有的对象
	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(TRUE, strProp, strValue);
	CString strFolderBase((LPCTSTR)IDS_FOLDERBASEINFO);

	DWORD FileAttributes = GetFileAttributes(strPath);
	if (FileAttributes == INVALID_FILE_ATTRIBUTES) {
		errorText->SetWindowTextW(GetLastErrorAsString());
		return;
	}
	//========================基本信息=============================
	CString yes = CString((LPCTSTR)IDS_YES);
	CString no = CString((LPCTSTR)IDS_NO);
	CString none = CString((LPCSTR)IDS_NONE);
	CMFCPropertyGridProperty* props = new CMFCPropertyGridProperty(strFolderBase);
	//######文件名#########
	path folderPath(strPath.GetBuffer());
	CString fileName = CString(folderPath.filename().c_str());
	CMFCPropertyGridProperty* item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_FILENAME), fileName, CString((LPCTSTR)IDS_FILENAMEDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	//######添加路径###########
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_FOLDERPATH), strPath, CString((LPCTSTR)IDS_FOLDERPATHDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	//######目录#########
	CString value = (FileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? yes : no;
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_DIRECTORY), value, CString((LPCTSTR)IDS_DIRECTORYDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	//######只读#########
	value = (FileAttributes & FILE_ATTRIBUTE_READONLY) ? yes : no;
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_READONLY), value, CString((LPCTSTR)IDS_READONLYDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	//######隐藏#########
	value = (FileAttributes & FILE_ATTRIBUTE_HIDDEN) ? yes : no;
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_HIDDEN), value, CString((LPCTSTR)IDS_HIDDENDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	//######系统#########
	value = (FileAttributes & FILE_ATTRIBUTE_SYSTEM) ? yes : no;
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_SYSTEM), value, CString((LPCTSTR)IDS_SYSTEMDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	//######普通#########
	value = (FileAttributes & FILE_ATTRIBUTE_NORMAL) ? yes : no;
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_NORMAL), value, CString((LPCTSTR)IDS_NORMALDES));
	item->AllowEdit(FALSE);
	props->AddSubItem(item);
	m_wndPropList.AddProperty(props);
	//######可定制信息###########
	LPCTSTR name = folderPath.append(L"Desktop.ini").c_str();
	CString localizeResourceName = none;
	CString iconResource = none;
	BOOL r = PathFileExists(name);
	WCHAR lpReturnedString[MAX_PATH] = { 0 };
	if (r) {
		DWORD  dret = GetPrivateProfileString(
			L".ShellClassInfo",
			L"LocalizedResourceName",
			NULL,
			lpReturnedString,
			MAX_PATH,
			name
		);
		if (dret > 0) {
			localizeResourceName = CString(lpReturnedString);
		}
		dret = GetPrivateProfileString(
			L".ShellClassInfo",
			L"IconResource",
			NULL,
			lpReturnedString,
			MAX_PATH,
			name
		);
		if (dret > 0) {
			iconResource = CString(lpReturnedString);
		}
	}
	props = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_Customization));
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_LocalizedResourceName), localizeResourceName, CString((LPCTSTR)IDS_LocalizedResourceNameDes));
	item->AddOption(CString((LPCTSTR)IDS_EDIT));
	item->AddOption(CString((LPCTSTR)IDS_BROWSEDLL));
	item->AddOption(CString((LPCTSTR)IDS_BROWSCOMPUTER));
	props->AddSubItem(item);
	// ###########获取完整路径#################################
	int index = localizeResourceName.Find(L",");
	if (index != -1) {
		CString dllPath = localizeResourceName.Left(index);
		CString envName;
		int leftIndex = dllPath.Find(L'%');
		int rightIndex = dllPath.ReverseFind(L'%');
		if (rightIndex > leftIndex) {
			envName = dllPath.Mid(leftIndex+1, rightIndex - leftIndex-1);
			WCHAR envPath[MAX_PATH];
			DWORD dret = GetEnvironmentVariable(envName, envPath, MAX_PATH);
			if (dret>0) {
				CString dllSubPath = dllPath.Right(dllPath.GetLength()-rightIndex-1);
				dllPath = envPath + dllSubPath;
			}
		}
		CString resourceIndexString = localizeResourceName.Right(localizeResourceName.GetLength() - index - 1);
		int index = _wtoi(resourceIndexString);
		CString dll;
		CString dllDes;
		dll.LoadString(IDS_NATIVEDLLPATH);
		dllDes.LoadString(IDS_NATIVEDLLPATHDES);
		item = new CMFCPropertyGridProperty(dll, dllPath,dllDes);
		props->AddSubItem(item);
		CString indexStr;
		CString indexDes;
	    indexStr.LoadString(IDS_NATIVEDLLINDEX);
		indexDes.LoadString(IDS_NATIVEDLLINDEXDES);
		item = new CMFCPropertyGridProperty(indexStr, resourceIndexString, indexDes);
		props->AddSubItem(item);
	}

	//=======================图标=================================
	item = new CMFCPropertyGridProperty(CString((LPCTSTR)IDS_IconResource), iconResource, CString((LPCTSTR)IDS_IconResourceDes));
	item->AddOption(CString((LPCTSTR)IDS_EDIT));
	item->AddOption(CString((LPCTSTR)IDS_BROWSEDLL));
	item->AddOption(CString((LPCTSTR)IDS_BROWSCOMPUTER));
	props->AddSubItem(item);
	m_wndPropList.AddProperty(props);
}

CString CFolderCustomizerDlg::GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return CString(); //No error message has been recorded

	LPWSTR messageBuffer = nullptr;
	size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

	std::wstring message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return CString(message.c_str());
}


