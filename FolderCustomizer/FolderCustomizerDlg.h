
// FolderCustomizerDlg.h: 头文件
//

#pragma once


// CFolderCustomizerDlg 对话框
class CFolderCustomizerDlg : public CDialogEx
{
// 构造
public:
	CFolderCustomizerDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CFolderCustomizerDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOLDERCUSTOMIZER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	// 初始化UI
	void InitDriversCombo();
	void UpdateTreeCtrl();
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
	CString GetLastErrorAsString();
public:
	CMFCShellTreeCtrlEx m_treeCtrl;
	CMFCPropertyGridCtrl m_wndPropList;
	afx_msg void OnCbnSelchangeDriverscombo();
	afx_msg void OnTvnSelchangedMfcshelltree(NMHDR* pNMHDR, LRESULT* pResult);
};
