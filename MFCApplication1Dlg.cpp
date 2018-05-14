
// MFCApplication1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include "zip_helper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

//void BOOL DeleteDirectory(CString directory_path);

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg dialog



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CMFCApplication1Dlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_START, &CMFCApplication1Dlg::OnBnClickedButtonStart)
	ON_EN_CHANGE(IDC_EDIT_PROJECT, &CMFCApplication1Dlg::OnEnChangeEditProject)
END_MESSAGE_MAP()


// CMFCApplication1Dlg message handlers

BOOL CMFCApplication1Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos;
	nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	strPath += "\\config.ini";

	ReadConfig();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication1Dlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


/*
void CMFCApplication1Dlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    //CDialogEx::OnOK();
    ZipHelper zh;
    zh.AddDir("D:\\vs_project\\MFCApplication1\\Release\\test");
    //zh.AddFile("zlibwapi.dll");
    zh.ToZip("1.zip");
}
*/

BOOL IsDirExist(const CString & csDir)
{
	DWORD dwAttrib = GetFileAttributes(csDir);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL DeleteDirectory(CString DirName)
{
	CString PUBPATH;
	PUBPATH = DirName;
	CFileFind tempFind;
	DirName += "\\*.*";
	BOOL IsFinded = (BOOL)tempFind.FindFile(DirName);
	while (IsFinded)
	{
		IsFinded = (BOOL)tempFind.FindNextFile();
		if (!tempFind.IsDots())
		{
			CString strDirName;
			strDirName += PUBPATH;
			strDirName += "\\";
			strDirName += tempFind.GetFileName();
			if (tempFind.IsDirectory())
			{
				//strDirName += PUBPATH;
				DeleteDirectory(strDirName);
			}
			else
			{
				SetFileAttributes(strDirName, FILE_ATTRIBUTE_NORMAL); //去掉文件的系统和隐藏属性
				DeleteFile(strDirName);
			}
		}
	}

	tempFind.Close();
	if (!RemoveDirectory(PUBPATH))
	{
		return false;
	}
	//AfxMessageBox(_T("文件夹删除成功..."));
	return true;
}

BOOL IsFileExist(const CString& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CMFCApplication1Dlg::WriteConfig() {
	//在当前程序目录下创建ini文件
	/*
	CString  strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos);
	strPath += "\\config.ini";
	*/

	CString projectPath, nativePath, servicePath, checkSumPath, TMCPatch;
	GetDlgItem(IDC_EDIT_PROJECT)->GetWindowText(projectPath);
	GetDlgItem(IDC_EDIT_NATIVE)->GetWindowText(nativePath);
	GetDlgItem(IDC_EDIT_SERVICE)->GetWindowText(servicePath);
	GetDlgItem(IDC_EDIT_CHECK_SUM)->GetWindowText(checkSumPath);
	GetDlgItem(IDC_EDIT_TMC)->GetWindowText(TMCPatch);

	//写内容
	WritePrivateProfileString(_T("project"), _T("projectPath"), projectPath, strPath);
	WritePrivateProfileString(_T("project"), _T("nativePath"), nativePath, strPath);
	WritePrivateProfileString(_T("project"), _T("servicePath"), servicePath, strPath);
	WritePrivateProfileString(_T("project"), _T("checkSumPath"), checkSumPath, strPath);
	WritePrivateProfileString(_T("project"), _T("TMCPatch"), TMCPatch, strPath);
}

void CMFCApplication1Dlg::ReadConfig() {
	CString projectPath;
	CString nativePath;
	CString servicePath;
	CString checkSumPath;
	CString TMCPatch;

	if (!IsFileExist(strPath)) {
		return;
	}

	GetPrivateProfileString(_T("project"), _T("projectPath"), 0, projectPath.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	GetPrivateProfileString(_T("project"), _T("nativePath"), 0, nativePath.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	GetPrivateProfileString(_T("project"), _T("servicePath"), 0, servicePath.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	GetPrivateProfileString(_T("project"), _T("checkSumPath"), 0, checkSumPath.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	GetPrivateProfileString(_T("project"), _T("TMCPatch"), 0, TMCPatch.GetBuffer(MAX_PATH), MAX_PATH, strPath);

	//在这里必须ReleaseBuffer()，否则无法再后面进行字符串的连接  
	projectPath.ReleaseBuffer();
	nativePath.ReleaseBuffer();
	servicePath.ReleaseBuffer();
	checkSumPath.ReleaseBuffer();
	TMCPatch.ReleaseBuffer();

	GetDlgItem(IDC_EDIT_PROJECT)->SetWindowTextW(projectPath);
	GetDlgItem(IDC_EDIT_NATIVE)->SetWindowTextW(nativePath);
	GetDlgItem(IDC_EDIT_SERVICE)->SetWindowTextW(servicePath);
	GetDlgItem(IDC_EDIT_CHECK_SUM)->SetWindowTextW(checkSumPath);
	GetDlgItem(IDC_EDIT_TMC)->SetWindowTextW(TMCPatch);
}

void CMFCApplication1Dlg::OnBnClickedButtonStart()
{
    // TODO: Add your control notification handler code here
    // TODO: 在此添加控件通知处理程序代码
	
    CString projectStr;
	GetDlgItem(IDC_EDIT_PROJECT)->GetWindowText(projectStr);

	CString nativeStr;
	GetDlgItem(IDC_EDIT_NATIVE)->GetWindowText(nativeStr);

	CString serviceStr;
	GetDlgItem(IDC_EDIT_SERVICE)->GetWindowText(serviceStr);

	CString checkSumStr;
	GetDlgItem(IDC_EDIT_CHECK_SUM)->GetWindowText(checkSumStr);

	CString TMCStr;
	GetDlgItem(IDC_EDIT_TMC)->GetWindowText(TMCStr);

	if (!IsDirExist(projectStr)) {
        AfxMessageBox(_T("工程目录不存在，请重新输入！"));
        return;
	}
	else if (!IsDirExist(nativeStr)) {
		AfxMessageBox(_T("本地目录不存在，请重新输入！"));
		return;
	}
	/*
	else if (!IsDirExist(serviceStr)) {
		AfxMessageBox(_T("存放目录不存在，请重新输入！"));
		return;
	}*/
	else if (!IsDirExist(checkSumStr)) {
		AfxMessageBox(_T("CheckSum目录不存在，请重新输入！"));
		return;
	}
	/*
	CopyFile(projectStr + _T("\\boot-verified.img"), nativeStr + _T("\\boot-verified.img"), true);
	CopyFile(projectStr + _T("\\cache.img"), nativeStr + _T("cache.img"), true);
	CopyFile(projectStr + _T("\\efuse.img"), nativeStr + _T("efuse.img"), true);
	CopyFile(projectStr + _T("\\lk-verified.img"), nativeStr + _T("lk-verified.img"), true);
	CopyFile(projectStr + _T("\\loader_ext-verified.img"), nativeStr + _T("\\loader_ext-verified.img"), true);
	CopyFile(projectStr + _T("\\logo-verified.img"), nativeStr + _T("\\logo-verified.img"), true);
	CopyFile(projectStr + _T("\\mcupmfw-verified.img"), nativeStr + _T("\\mcupmfw-verified.img"), true);
	CopyFile(projectStr + _T("\\md1arm7-verified.img"), nativeStr + _T("\\md1arm7-verified.img"), true);
	CopyFile(projectStr + _T("\\md1dsp-verified.img"), nativeStr + _T("\\md1dsp-verified.img"), true);
	CopyFile(projectStr + _T("\\md1rom-verified.img"), nativeStr + _T("\\md1rom-verified.img"), true);
	CopyFile(projectStr + _T("\\md3rom-verified.img"), nativeStr + _T("\\md3rom-verified.img"), true);
	CopyFile(projectStr + _T("\\MT6739_Android_scatter.txt"), nativeStr + _T("\\MT6739_Android_scatter.txt"), true);
	CopyFile(projectStr + _T("\\preloader_aus6739_66_n1.bin"), nativeStr + _T("\\preloader_aus6739_66_n1.bin"), true);
	CopyFile(projectStr + _T("\\recovery-verified.img"), nativeStr + _T("\\recovery-verified.img"), true);
	CopyFile(projectStr + _T("\\secro.img"), nativeStr + _T("\\secro.img"), true);
	CopyFile(projectStr + _T("\\spmfw-verified.img"), nativeStr + _T("\\spmfw-verified.img"), true);
	CopyFile(projectStr + _T("\\system.img"), nativeStr + _T("\\system.img"), true);
	CopyFile(projectStr + _T("\\trustzone-verified.img"), nativeStr + _T("\\trustzone-verified.img"), true);
	CopyFile(projectStr + _T("\\userdata.img"), nativeStr + _T("\\userdata.img"), true);

	CopyFile(checkSumStr + _T("\\CheckSum_Gen.exe"), nativeStr + _T("\\CheckSum_Gen.exe"), true);
	CopyFile(checkSumStr + _T("\\CheckSum_Gen.ilk"), nativeStr + _T("\\CheckSum_Gen.ilk"), true);
	CopyFile(checkSumStr + _T("\\CheckSum_Gen.pdb"), nativeStr + _T("\\CheckSum_Gen.pdb"), true);
	CopyFile(checkSumStr + _T("\\FlashToolLib.dll"), nativeStr + _T("\\FlashToolLib.dll"), true);
	CopyFile(checkSumStr + _T("\\FlashToolLib.v1.dll"), nativeStr + _T("\\FlashToolLib.v1.dll"), true);
	CopyFile(checkSumStr + _T("\\FlashToolLibEx.dll"), nativeStr + _T("\\FlashToolLibEx.dll"), true);

    //ShellExecute();
    //StartProcess(_T("\\\\192.168.1.16\\Files\\软件一部\\software version\\MTK6739\\L05A\\临时版本\\test11\\CheckSum_Gen.exe"),  "");
	//Sleep(5000);
	//AfxMessageBox(nativeStr + _T("\\CheckSum_Gen.exe"));

	HINSTANCE hinstance = ShellExecute(NULL, _T("open"), nativeStr + _T("\\CheckSum_Gen.exe"), NULL, nativeStr, SW_SHOWNORMAL);
    //WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	
    //AfxMessageBox(_T("Sleep 10 before"));
    Sleep(10000);

	HWND hWnd = ::FindWindow(NULL, nativeStr + _T("\\CheckSum_Gen.exe"));
    if (hWnd != NULL) {
	
        // 向目标窗口发送WM_CLOSE消息
        //AfxMessageBox(_T("CheckSum_Gen！"));
        ::SendMessage(hWnd, WM_CLOSE, 0, 0);
		Sleep(5000);
        
        //CString str, strTemp = _T("H:\\vs_project\\check_sum\\CheckSum_Generate_exe\\CheckSum_Gen.exe");
       // str.Format(_T("%s"), strTemp);
        
		BOOL ret = DeleteFile(nativeStr + _T("\\CheckSum_Gen.exe"));
		DeleteFile(nativeStr + _T("\\CheckSum_Gen.ilk"));
		DeleteFile(nativeStr + _T("\\CheckSum_Gen.pdb"));
		DeleteFile(nativeStr + _T("\\FlashToolLib.dll"));
		DeleteFile(nativeStr + _T("\\FlashToolLib.v1.dll"));
		DeleteFile(nativeStr + _T("\\FlashToolLibEx.dll"));

		DeleteDirectory(nativeStr + _T("\\Log"));
		//RemoveDirectory(_T("\\CheckSum_Generate_exe"));
		//打包img文件
		Sleep(3000);
		*/
        ZipHelper zh;
		//zh.AddDir((char*)nativeStr.GetBuffer(nativeStr.GetLength()));
		printf("zly --> %s", (char*)nativeStr.GetBuffer(nativeStr.GetLength()));
		//zh.AddDir(("D:\\liebao"));
		//zh.AddDir(("test"));
        //zh.AddFile("zlibwapi.dll");
		//zh.AddFile("test\\boot-verified.img");
		//zh.AddFile("test\\loader_ext-verified.img");
		//zh.AddFile("test\\mcupmfw-verified.img");
		//zh.AddFile("test\\md1arm7-verified.img");
		//zh.AddFile("test\\md1dsp-verified.img");
		//zh.AddFile("test\\md1rom-verified.img");
		//zh.AddFile("test\\md3rom-verified.img");
		zh.AddFile("test\\recovery-verified.img.img");
		zh.AddFile("test\\userdata.img");
        zh.ToZip("1.zip");
    /*}
    else {
        AfxMessageBox(_T("CheckSum is not Running！"));
    }
	*/
	WriteConfig();
}



void CMFCApplication1Dlg::OnEnChangeEditProject()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
