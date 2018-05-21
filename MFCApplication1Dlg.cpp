
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
				SetFileAttributes(strDirName, FILE_ATTRIBUTE_NORMAL); //ȥ���ļ���ϵͳ����������
				DeleteFile(strDirName);
			}
		}
	}

	tempFind.Close();
	if (!RemoveDirectory(PUBPATH))
	{
		return false;
	}
	//AfxMessageBox(_T("�ļ���ɾ���ɹ�..."));
	return true;
}

BOOL IsFileExist(const CString& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CMFCApplication1Dlg::WriteConfig() {
	//�ڵ�ǰ����Ŀ¼�´���ini�ļ�
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

	//д����
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

	//���������ReleaseBuffer()�������޷��ٺ�������ַ���������  
	projectPath.ReleaseBuffer();
	nativePath.ReleaseBuffer();
	servicePath.ReleaseBuffer();
	checkSumPath.ReleaseBuffer();
	TMCPatch.ReleaseBuffer();

	GetDlgItem(IDC_EDIT_PROJECT)->SetWindowText(projectPath);
	GetDlgItem(IDC_EDIT_NATIVE)->SetWindowText(nativePath);
	GetDlgItem(IDC_EDIT_SERVICE)->SetWindowText(servicePath);
	GetDlgItem(IDC_EDIT_CHECK_SUM)->SetWindowText(checkSumPath);
	GetDlgItem(IDC_EDIT_TMC)->SetWindowText(TMCPatch);
}

void CMFCApplication1Dlg::GetVersionInfo(CString projectMainPatch) {
	
	int index = projectMainPatch.Find(_T("ut"));
	/* //debug
	CString str;
	str.Format(_T("%d"), index);
	AfxMessageBox(str);
	*/
	mProjectMainPath = projectMainPatch.Left(index - 1);  //end have '\'
	CString buildInfoPath = mProjectMainPath + _T("build\\tools\\buildinfo.sh");
	//AfxMessageBox(buildInfoPath);//debug
	//exit(0);

	CStdioFile read;
	if (!read.Open(buildInfoPath, CFile::modeRead))
	{
		AfxMessageBox(_T("Open file error! ret:") + index);
		return;
	}

	CString line;
	while (read.ReadString(line))
	{
		int ret = -1;
		if ((ret = line.Find(_T("custintid"))) > 0) {

			/* //debug
			CString str;
			str.Format(_T("%d,%d"), ret, line.GetLength());
			AfxMessageBox(str);
			*/
			mVersion = line.Mid(ret + strlen("custintid") + 1, line.GetLength() - ret - strlen("custintid") - 2);
			int index__ = line.ReverseFind('_');
			int index_V = line.ReverseFind('V');
			mSimpleVersion = line.Mid(index_V, index__ - index_V);

			//AfxMessageBox(mSimpleVersion); //debug
			
		}

		if (ret = line.Find(_T("ro.build.fingerprint")) > 0 && line.Find(_T("#")) == -1) {

            int index1 = line.Find(_T("="));
            int index2 = line.ReverseFind('\"');
            mFingerprint = line.Mid(index1 + 1, index2 - index1 - 1);
			mFingerprint.Replace(_T("\/"), _T("~"));
            mFingerprint.Replace(_T(":"), _T("~"));
			//AfxMessageBox(mFingerprint);
		}
	}
}

char *CStringToChar(CString & str) {
    wchar_t *pWChar = str.GetBuffer(); //��ȡStr�Ŀ��ַ������鱣��  
    str.ReleaseBuffer();  
      
    int nLen = str.GetLength(); //��ȡStr���ַ���  
    char *pChar = new char[nLen * 2 + 1];   
    memset(pChar, 0, nLen * 2 + 1);  
    int rtnVal = (int)wcstombs(pChar, pWChar, nLen * 2 + 1);
    return pChar;
}

CString CMFCApplication1Dlg::findTargetFile(CString str, int dbType) {
	CString dbStr;
	CFileFind dd;

	BOOL nResult = dd.FindFile(str);
	while (nResult) {
		nResult = dd.FindNextFileW();

		if (!dd.IsDirectory() && !dd.IsDots())	//�ݹ����
		{
			dbStr = dd.GetFileName();
			//AfxMessageBox(dbStr);
			switch (dbType) {
				case 1:
					if (dbStr.Find(_T("APDB_"), 0) != -1 && (dbStr.Find(_T("ENUM"), 0) == -1 && dbStr.Find(_T("_"), 0) > 1)) {
						dd.Close();
						return dbStr;
					}
					dbStr = "";
					break;
				case 2:
					if (dbStr.Find(_T("MDDB_Info"), 0) != -1) {
						dd.Close();
						return dbStr;
					}
					dbStr = "";
					break;
			}
		}
	}
	dd.Close();

	return dbStr;
}

void CMFCApplication1Dlg::copyDataBase(CString srcPatch, CString targetPatch) {
	CFileFind cf;
	CString fileName;

	BOOL ret = cf.FindFile(srcPatch + _T("\\*.*"));
	while (ret) {
		ret = cf.FindNextFileW();

		if (!cf.IsDirectory() && !cf.IsDots()) {
			fileName = cf.GetFileName();
			//AfxMessageBox(fileName);
			CopyFile(srcPatch + _T("\\") + fileName, targetPatch + _T("\\") + fileName, true);
		}
	}
}

void CMFCApplication1Dlg::copyCMCBatToImgPath(CString srcPath, CString targetPath, CString fileName) {
	CStdioFile read, write;
	//AfxMessageBox(srcPath + _T("\\") + fileName);
	if (!read.Open(srcPath + _T("\\") + fileName, CFile::modeRead)) {
		AfxMessageBox(_T("open tmc.bat file error."));
		return;
	}

	if (!write.Open(targetPath + _T("\\") + fileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		AfxMessageBox(_T("create tmc.bat file error."));
		return;
	}

	CString line;
	int ret = 0;;
	while (read.ReadString(line)) {
		if (ret = line.Find(_T("set c345=")) != -1) {
			CString tempStr = line.Mid(ret + 8, 3);

			line.SetAt(ret + 9, mSimpleVersion.GetAt(1));
			line.SetAt(ret + 10, mSimpleVersion.GetAt(3));
			//AfxMessageBox(tempStr);
			//AfxMessageBox(line);
			
		}
		write.WriteString(line);
		write.WriteString(_T("\n"));
	}

	read.Close();
	write.Close();
	
}

void CMFCApplication1Dlg::OnBnClickedButtonStart()
{
    // TODO: Add your control notification handler code here
    // TODO: �ڴ���ӿؼ�֪ͨ����������
	
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
        AfxMessageBox(_T("����Ŀ¼�����ڣ����������룡"));
        return;
	}
	else if (!IsDirExist(nativeStr)) {
		AfxMessageBox(_T("����Ŀ¼�����ڣ����������룡"));
		return;
	}
	
	else if (!IsDirExist(serviceStr)) {
		AfxMessageBox(_T("���Ŀ¼�����ڣ����������룡"));
		return;
	}
	else if (!IsDirExist(checkSumStr)) {
		AfxMessageBox(_T("CheckSumĿ¼�����ڣ����������룡"));
		return;
	}


	if (!IsFileExist(TMCStr)) {
		AfxMessageBox(_T("TMC�ļ������ڣ����������룡"));
		return;
	}
	
	
	GetVersionInfo(projectStr);


	CString nativeImgPatch = nativeStr + _T("\\") + mVersion;
	if (!PathIsDirectory(nativeImgPatch)) {
		if (!CreateDirectory(nativeImgPatch, NULL)) {
			AfxMessageBox(_T("Create Directory error."));
			exit(0);
		}
	}

	CreateDirectory(nativeImgPatch + _T("\\Database"), NULL);

	//AfxMessageBox(nativeImgPatch);

	//Added the Database files
	CString dbStr = NULL;
	dbStr = findTargetFile(projectStr + _T("\\obj\\CGEN\\*.*"), 1);
	if (!dbStr.IsEmpty()) {
		CopyFile(projectStr + _T("\\obj\\CGEN\\") + dbStr, nativeImgPatch + _T("\\Database\\") + dbStr, true);
	}

	dbStr = findTargetFile(mProjectMainPath + _T("vendor\\mediatek\\proprietary\\modem\\aus6739_66_n1_lwctg\\*.*"), 2);
	if (!dbStr.IsEmpty()) {
		CopyFile(mProjectMainPath + _T("vendor\\mediatek\\proprietary\\modem\\aus6739_66_n1_lwctg\\") + dbStr, nativeImgPatch + _T("\\Database\\") + dbStr, true);
	}
	
	CopyFile(projectStr + _T("\\boot-verified.img"), nativeImgPatch + _T("\\boot-verified.img"), true);
	CopyFile(projectStr + _T("\\cache.img"), nativeImgPatch + _T("\\cache.img"), true);
	CopyFile(projectStr + _T("\\efuse.img"), nativeImgPatch + _T("\\efuse.img"), true);
	CopyFile(projectStr + _T("\\lk-verified.bin") , nativeImgPatch + _T("\\lk-verified.bin"), true);
	CopyFile(projectStr + _T("\\loader_ext-verified.img"), nativeImgPatch + _T("\\loader_ext-verified.img"), true);
	CopyFile(projectStr + _T("\\logo-verified.bin"), nativeImgPatch + _T("\\logo-verified.bin"), true);
	CopyFile(projectStr + _T("\\mcupmfw-verified.img"), nativeImgPatch + _T("\\mcupmfw-verified.img"), true);
	CopyFile(projectStr + _T("\\md1arm7-verified.img"), nativeImgPatch + _T("\\md1arm7-verified.img"), true);
	CopyFile(projectStr + _T("\\md1dsp-verified.img"), nativeImgPatch + _T("\\md1dsp-verified.img"), true);
	CopyFile(projectStr + _T("\\md1rom-verified.img"), nativeImgPatch + _T("\\md1rom-verified.img"), true);
	CopyFile(projectStr + _T("\\md3rom-verified.img"), nativeImgPatch + _T("\\md3rom-verified.img"), true);
	CopyFile(projectStr + _T("\\MT6739_Android_scatter.txt"), nativeImgPatch + _T("\\MT6739_Android_scatter.txt"), true);
	CopyFile(projectStr + _T("\\preloader_aus6739_66_n1.bin"), nativeImgPatch + _T("\\preloader_aus6739_66_n1.bin"), true);
	CopyFile(projectStr + _T("\\recovery-verified.img"), nativeImgPatch + _T("\\recovery-verified.img"), true);
	CopyFile(projectStr + _T("\\secro.img"), nativeImgPatch + _T("\\secro.img"), true);
	CopyFile(projectStr + _T("\\spmfw-verified.bin"), nativeImgPatch + _T("\\spmfw-verified.bin"), true);
	//CopyFile(projectStr + _T("\\system.img"), nativeImgPatch + _T("\\system.img"), true);
	CopyFile(projectStr + _T("\\trustzone-verified.bin"), nativeImgPatch + _T("\\trustzone-verified.bin"), true);
	CopyFile(projectStr + _T("\\userdata.img"), nativeImgPatch + _T("\\userdata.img"), true);

	CopyFile(checkSumStr + _T("\\CheckSum_Gen.exe"), nativeImgPatch + _T("\\CheckSum_Gen.exe"), true);
	CopyFile(checkSumStr + _T("\\CheckSum_Gen.ilk"), nativeImgPatch + _T("\\CheckSum_Gen.ilk"), true);
	CopyFile(checkSumStr + _T("\\CheckSum_Gen.pdb"), nativeImgPatch + _T("\\CheckSum_Gen.pdb"), true);
	CopyFile(checkSumStr + _T("\\FlashToolLib.dll"), nativeImgPatch + _T("\\FlashToolLib.dll"), true);
	CopyFile(checkSumStr + _T("\\FlashToolLib.v1.dll"), nativeImgPatch + _T("\\FlashToolLib.v1.dll"), true);
	CopyFile(checkSumStr + _T("\\FlashToolLibEx.dll"), nativeImgPatch + _T("\\FlashToolLibEx.dll"), true);

	HINSTANCE hinstance = ShellExecute(NULL, _T("open"), nativeImgPatch + _T("\\CheckSum_Gen.exe"), NULL, nativeImgPatch, SW_SHOWNORMAL);
    //WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	
    //AfxMessageBox(_T("Sleep 10 before"));
    Sleep(60000);

	HWND hWnd = ::FindWindow(NULL, nativeImgPatch + _T("\\CheckSum_Gen.exe"));
    if (hWnd != NULL) {
	
        // ��Ŀ�괰�ڷ���WM_CLOSE��Ϣ
        //AfxMessageBox(_T("CheckSum_Gen��"));
        ::SendMessage(hWnd, WM_CLOSE, 0, 0);
		Sleep(5000);
        
        //CString str, strTemp = _T("H:\\vs_project\\check_sum\\CheckSum_Generate_exe\\CheckSum_Gen.exe");
       // str.Format(_T("%s"), strTemp);
		
		BOOL ret = DeleteFile(nativeImgPatch + _T("\\CheckSum_Gen.exe"));
		DeleteFile(nativeImgPatch + _T("\\CheckSum_Gen.ilk"));
		DeleteFile(nativeImgPatch + _T("\\CheckSum_Gen.pdb"));
		DeleteFile(nativeImgPatch + _T("\\FlashToolLib.dll"));
		DeleteFile(nativeImgPatch + _T("\\FlashToolLib.v1.dll"));
		DeleteFile(nativeImgPatch + _T("\\FlashToolLibEx.dll"));

		DeleteDirectory(nativeImgPatch + _T("\\Log"));
		//RemoveDirectory(_T("\\CheckSum_Generate_exe"));
		//���img�ļ�
		Sleep(10000);
		//AfxMessageBox(nativeStr);
        ZipHelper zh;

		zh.AddDir(CStringToChar(nativeStr + _T("\\") + mVersion));
		zh.ToZip(CStringToChar(nativeStr + _T("\\") + mVersion + _T(".zip")));

		zh.ClearFile();

		int position = TMCStr.ReverseFind(('\\'));
		CString tmcFileName = TMCStr.Right(TMCStr.GetLength() - position);
		//AfxMessageBox(tmcFileName);

		CString mOtherFilePath = TMCStr.Left(position);
		//AfxMessageBox(mOtherFilePath);
		//exit(1);
		zh.AddFile(CStringToChar(mOtherFilePath + _T("\\flash_tool.exe")));
		zh.AddFile(CStringToChar(nativeStr + _T("\\") + mVersion + _T("\\") + _T("boot-verified.img")));
		zh.AddFile(CStringToChar(nativeStr + _T("\\") + mVersion + _T("\\") + _T("recovery-verified.img")));
		zh.AddFile(CStringToChar(nativeStr + _T("\\") + mVersion + _T("\\") + _T("system.img")));
		zh.AddFile(CStringToChar(nativeStr + _T("\\") + mVersion + _T("\\") + _T("userdata.img")));
		zh.ToZip(CStringToChar(nativeStr + _T("\\") + mFingerprint + _T(".zip")));

		zh.ClearFile();
		
		copyDataBase(nativeStr + _T("\\") + mVersion + _T("\\DataBase"), nativeStr + _T("\\") + mVersion);
		DeleteDirectory(nativeStr + _T("\\") + mVersion + _T("\\DataBase"));

		//CopyFile(_T("E:\\version\\TMC�汾����5058I_EFUSE.bat"), nativeImgPatch + _T("\\TMC�汾����5058I_EFUSE.bat"), true);
		
		copyCMCBatToImgPath(mOtherFilePath, nativeImgPatch, tmcFileName);
		
		HINSTANCE hinstance = ShellExecute(NULL, _T("open"), nativeImgPatch + _T("\\") + tmcFileName, NULL, nativeImgPatch, SW_SHOWNORMAL);
		Sleep(5000);

		HWND hWnd = ::FindWindow(NULL, nativeImgPatch + _T("\\") + tmcFileName);
		DeleteFile(nativeImgPatch + _T("\\") + tmcFileName);

		zh.ClearFile();
		zh.AddDir(CStringToChar(nativeImgPatch));
		zh.ToZip(CStringToChar(nativeStr + _T("\\") + mVersion + _T("_TMC.zip")));

		CString fileVersion = mVersion + _T(".zip");
		CString tmcFileVersion = mVersion + _T("_TMC.zip");
		CString tclFileVersion = mFingerprint + _T(".zip");

		CopyFile(nativeStr + _T("\\") + fileVersion, serviceStr + _T("\\") + fileVersion, true);
		CopyFile(nativeStr + _T("\\") + tmcFileVersion, serviceStr + _T("\\") + tmcFileVersion, true);
		CopyFile(nativeStr + _T("\\") + tclFileVersion, serviceStr + _T("\\") + tclFileVersion, true);
    }
    else {
        AfxMessageBox(_T("CheckSum is not Running��"));
    }
	
	
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
