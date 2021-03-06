
// MFCApplication1Dlg.h : header file
//

#pragma once


// CMFCApplication1Dlg dialog
class CMFCApplication1Dlg : public CDialogEx
{
// Construction
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CString  strPath;	// exe patch
	CString mVersion;
	CString mSimpleVersion;		//V1.5
	CString mFingerprint;
	CString mProjectMainPath;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnEnChangeEditProject();
	void ReadConfig();
	void WriteConfig();
	void GetVersionInfo(CString projectMainPatch);
	CString findTargetFile(CString str, int dbType);
	void copyDataBase(CString srcPatch, CString targetPatch);
	void copyCMCBatToImgPath(CString srcPath, CString targetPath, CString fileName);
};
