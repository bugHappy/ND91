#include "stdafx.h"
#include <ShellAPI.h>
#include "BaseOperation.h"
#include "FolderBrowser.h"
App_CC_BEGIN

bool BaseOperation::appFunctionInfo = false;
std::wstring BaseOperation::FileDialog( HWND pParent, LPCTSTR strFileType, DWORD flags)
{INTERFUN;
    OPENFILENAME ofn;
    wchar_t strFile[MAX_PATH];
    memset(&ofn,0,sizeof(OPENFILENAME));
    memset(strFile,0,sizeof(wchar_t)*MAX_PATH);

    ofn.hwndOwner = pParent;
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.lpstrFilter= strFileType;
    ofn.lpstrFile=strFile;
    ofn.nMaxFile=MAX_PATH;
    ofn.Flags=flags;

    if(GetOpenFileName(&ofn))//strFile得到用户所选择文件的路径和文件名 
        return strFile;
    return L"";
}

#define MaxMultiFileCount 200
vector<wstring> BaseOperation::MultiFileDialog( HWND pParent /*= NULL*/, LPCTSTR strFileType/*=L"*.*\0*.*\0"*/, DWORD flags/*=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER*/ , wstring wsTitle)
{INTERFUN;
    vector<wstring> files;

    TCHAR szFile[MaxMultiFileCount * MAX_PATH];
    TCHAR szPath[MAX_PATH];
    TCHAR szCopy[MAX_PATH];
    memset(szFile, 0, MaxMultiFileCount * MAX_PATH);
    memset(szPath, 0, MAX_PATH);
    memset(szCopy, 0, MAX_PATH);

    OPENFILENAME ofn;
    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.hwndOwner = pParent;
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.lpstrFilter= strFileType;
    ofn.lpstrFile=szFile;
    ofn.nMaxFile=MAX_PATH;
    ofn.Flags=flags;
    ofn.nMaxFile = MaxMultiFileCount * MAX_PATH;
	if (!wsTitle.empty())
		ofn.lpstrTitle=wsTitle.c_str();
    if(GetOpenFileName(&ofn))//strFile得到用户所选择文件的路径和文件名 
    {
        lstrcpyn(szPath, szFile, ofn.nFileOffset);
        szPath[ofn.nFileOffset] = TEXT('\0');

        if(szPath[_tcslen(szPath) - 1] != TEXT('\\'))     // 必须加上TEXT('\\'),因为是文件目录
        {
            _tcscat_s(szPath, MAX_PATH, TEXT("\\"));
        }
        TCHAR* pFilePath = szFile + ofn.nFileOffset;
        while( *pFilePath )
        {
            ZeroMemory(szCopy, sizeof(szCopy));
            _tcscpy_s(szCopy, szPath);                // 文件路径
            _tcscat_s(szCopy, pFilePath);             // 文件名
            files.push_back(szCopy);
            pFilePath += _tcslen(pFilePath) + 1;      // 指向下一个文件名
        }
        //return files;
    }
	wstring tmp = CFileOperation::GetCurrWorkingDir();
	SetCurrentDirectory(tmp.c_str());//防止选中后文件夹被占用
    return files;
}


wstring BaseOperation::DirDialog(HWND pParent, wstring strDefaultPath)
{INTERFUN;
    if (CFolderBrowser::SelectFolder(pParent, strDefaultPath))
        return strDefaultPath;
    else
        return L"";
}

std::wstring BaseOperation::ParseCountry( wstring str )
{INTERFUN;
    wstring strCountry = str;
    if (str == L"CN")
        strCountry = CommonUIFunc::LoadString(_T("CN"));
    else if (str == L"US")
        strCountry = CommonUIFunc::LoadString(_T("US"));
    else if (str == L"HK")
        strCountry =CommonUIFunc::LoadString(_T("HK"));
    else if (str == L"JP")
        strCountry = CommonUIFunc::LoadString(_T("JP"));
    else if (str == L"SG")
        strCountry = CommonUIFunc::LoadString(_T("SG"));    
    else if (str == L"KR")
        strCountry = CommonUIFunc::LoadString(_T("KR"));
    return strCountry;
}

HANDLE BaseOperation::Execute( wstring strPath, wstring strCmdLine )
{INTERFUN;
    STARTUPINFO si;
    PROCESS_INFORMATION pi; 
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si); 
    si.wShowWindow = SW_NORMAL;//  sw_showminimized;//SW_NORMAL;//SW_NORMAL;//SW_HIDE;		
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    wchar_t cmdLine[256] = {0};
    swprintf(cmdLine, 256, L"%s%s%s", strPath.c_str(), strCmdLine.length()>0?L" ":L"", strCmdLine.c_str());
    if (!CreateProcess(NULL,cmdLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
        return 0;
    return pi.hProcess;;
}

HANDLE BaseOperation::ExecuteAsUser( wstring strExe, wstring strWorkPath, wstring strCmdLine, WORD wShowWindow)
{INTERFUN;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    BOOL b; 
    HANDLE hToken; 
    HANDLE hNewToken; 
    if (wShowWindow > 0)
        si.wShowWindow = wShowWindow;
    b = OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED,   &hToken); 
    b = DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,  SecurityImpersonation, TokenPrimary, &hNewToken); 
    wchar_t pExe[512];
    if (strCmdLine.length() > 0)
        strExe += L" " + strCmdLine;
    wcscpy(pExe, strExe.c_str());
    b = CreateProcessAsUser(hNewToken, NULL, pExe, NULL, NULL,  FALSE, 0, NULL, strWorkPath.c_str(), &si, &pi);
    CloseHandle(hToken);
    CloseHandle(hNewToken);
    return pi.hProcess;
}

HANDLE BaseOperation::ExecuteDir( wstring strDir )
{INTERFUN;
    return BaseOperation::Execute(L"Explorer.exe", strDir);
}

std::wstring BaseOperation::GetPrompt( MESSAGE_CODE msgCode )
{INTERFUN;
    wstring strMsg;
    switch (msgCode)
    {
	case MESSAGE_EXPORT:
		strMsg = CommonUIFunc::LoadString(_T("Export_text"));
		break;
	case MESSAGE_IMPORT:
		strMsg = CommonUIFunc::LoadString(_T("Import_text"));
		break;
    case MESSAGE_PREPARE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Prepare_text"));
        break;
    case MESSAGE_INSTALL_PREPARE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_prepare_text"));
        break;
    case MESSAGE_INSTALL_PXLTOIPA:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_pxltoipa_text"));
        break;
    case MESSAGE_INSTALL_UPLOAD:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_upload_text"));
        break;
    case MESSAGE_INSTALL_INSTALL:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_install_text"));
        break;
    case MESSAGE_INSTALL_UNINSTALL:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_uninstall_text"));
        break;
    case MESSAGE_INSTALL_COPYDATA:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_copydata_text"));
        break;
    case MESSAGE_INSTALL_NOTIFYSB:
        strMsg = CommonUIFunc::LoadString(_T("Message_Install_notifysb_text"));
        break;
    case MESSAGE_ARCHIVE_DOWNLOAD:
        strMsg = CommonUIFunc::LoadString(_T("Message_Archive_download_text"));
        break;
    case MESSAGE_ARCHIVE_ARRANGE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Archive_arrange_text"));
        break;
    case MESSAGE_ARCHIVE_PACKAGE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Archive_package_text"));
        break;
    case MESSAGE_ARCHIVE_REMOVE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Archive_remove_text"));
        break;
    case MESSAGE_ARCHIVE_APP_ONPHONE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Archive_app_onphone_text"));
        break;
    case MESSAGE_UPLOAD:
        strMsg = CommonUIFunc::LoadString(_T("Message_Upload_text"));
        break;
    case MESSAGE_DOWNLOAD:
        strMsg = CommonUIFunc::LoadString(_T("Message_Download_text"));
        break;
    case MESSAGE_BACKUP:
        strMsg = CommonUIFunc::LoadString(_T("Message_Backup_text"));
        break;
    case MESSAGE_RESTORE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Restore_text"));
        break;
    case MESSAGE_PARSE:
        strMsg = CommonUIFunc::LoadString(_T("Message_Parse_text"));
        break;
    }
    return strMsg;
}

HANDLE BaseOperation::ExecuteUrl( wstring url )
{INTERFUN;
    TCHAR tszWindows[256];
    GetSystemDirectory(tszWindows, 256);
    TCHAR strExe[256];
    wsprintf(strExe, _T("%c:\\Program Files\\Internet Explorer\\IEXPLORE.EXE"), tszWindows[0]);
    return BaseOperation::Execute(strExe, url);
}

int BaseOperation::GetPer( size_t nPer, size_t nRange )
{INTERFUN;
    return GetPer((int64_t)nPer, (int64_t)nRange);
}

int BaseOperation::GetPer( int64_t nPer, int64_t nRange )
{   INTERFUN;
    if (nRange == 0)
        return 0;
    while ((uint64_t)nPer > 10240 && (uint64_t)nRange > 10240)
    {
        nPer = nPer >> 1;
        nRange = nRange >> 1;
    }    
    return (int)((nPer*100)/nRange);
}


wstring BaseOperation::GetIosDeviceRegion(wstring ios_deviceRegionInfo)
{INTERFUN;
    wstring strRegion = CStrOperation::toUpperW(ios_deviceRegionInfo);
    strRegion = CStrOperation::ReplaceW(strRegion, L"/A", L"");
    if (strRegion == L"AB" )
        return L"中东地区";
    if (strRegion == L"B" )
        return L"英国/爱尔兰";
    if (strRegion == L"C" )
        return L"加拿大";
    if (strRegion == L"CH" )
        return L"中国";
    if (strRegion == L"CZ" )
        return L"捷克";
    if (strRegion == L"DN" )
        return L"德国";
    if (strRegion == L"HN" )
        return L"印度";
    if (strRegion == L"J" )
        return L"日本";
    if (strRegion == L"KH" )
        return L"韩国";
    if (strRegion == L"LL" )
        return L"美国";
    if (strRegion == L"RS" )
        return L"俄罗斯";
    if (strRegion == L"T" )
        return L"意大利";
    if (strRegion == L"TA" )
        return L"台湾";
    if (strRegion == L"X" )
        return L"澳大利亚/新西兰";
    if (strRegion == L"Y" )
        return L"西班牙";
    if (strRegion == L"ZA" )
        return L"新加坡";
    if (strRegion == L"ZP" )
        return L"香港/澳门";
    return L"未知";
}

bool BaseOperation::CopyToClipboard( HWND hWnd, wstring str )
{INTERFUN;
    if( OpenClipboard(hWnd) )
    {
        HGLOBAL clipbuffer;
        wchar_t * buffer;
        EmptyClipboard();
        clipbuffer = GlobalAlloc(GMEM_DDESHARE, (str.length()+1)*sizeof(wchar_t));
        buffer = (wchar_t*)GlobalLock(clipbuffer);
        _tcscpy(buffer, str.c_str());
        GlobalUnlock(clipbuffer);
        SetClipboardData(CF_UNICODETEXT,clipbuffer);
        CloseClipboard();
        return true;
    }
    return false;
}

HANDLE BaseOperation::Open( wstring strPath, wstring strCmdLine/*=L""*/ )
{INTERFUN;
    return ShellExecute(NULL, _T("open"), strPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

wchar_t* BaseOperation::AllocateBuffer( wstring sn )
{    INTERFUN;
    wchar_t* pBuffer = new wchar_t[sn.size() + 1];
    wcscpy_s(pBuffer, sn.size()+1, sn.c_str());
    pBuffer[sn.size()] = 0;
    return pBuffer;
}

std::wstring BaseOperation::GetDateTimeString()
{INTERFUN;
    SYSTEMTIME   s; 
    GetLocalTime(&s); 
    WCHAR date[1024] = L"";
    wsprintf(date,_T("%04d-%02d-%02d  %02d:%02d:%02d"),s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);
    return wstring(date);
}

time_t BaseOperation::ParstTimeFormat( tstring sTime, tstring sFormat )
{INTERFUN;
    struct tm tm1;
    time_t time1;

    _stscanf(sTime.c_str(), sFormat.c_str(),     
        &tm1.tm_year, 
        &tm1.tm_mon, 
        &tm1.tm_mday, 
        &tm1.tm_hour, 
        &tm1.tm_min,
        &tm1.tm_sec);

    tm1.tm_year -= 1900;
    tm1.tm_mon --;

    tm1.tm_isdst=-1;

    time1 = mktime(&tm1);
    return time1;
}

void BaseOperation::FindTarget(LPCTSTR    str) 
{INTERFUN;
	HRESULT hres;   
	IShellLink* psl;   
	ITEMIDLIST* pidl; 
	IPersistFile* ppf;   
	CoInitialize(NULL); 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl);   
	if(SUCCEEDED(hres)) 
	{
		psl-> SetPath(str); 
		psl-> GetIDList(&pidl); 
		hres = psl-> QueryInterface(IID_IPersistFile, (void * *) &ppf);   
		if(SUCCEEDED(hres)) 
		{
			WCHAR wsz[MAX_PATH];
			wcscpy(wsz, str); 
			hres = ppf-> Load(wsz, STGM_READ);   
			if(SUCCEEDED(hres)) 
			{
				psl-> GetIDList(&pidl);
			} 
			ppf-> Release();
		} 
		XZSHOpenFolderAndSelectItems(pidl); 

		psl-> Release();
	} 
	CoUninitialize();
}

HRESULT BaseOperation::GetShellFolderViewDual(ITEMIDLIST* pidl, IShellFolderViewDual** ppIShellFolderViewDual) 
{INTERFUN;
	IWebBrowserApp* pIWebBrowserApp; 
	IDispatch* pDoc; 
	HWND hWnd; 
	HRESULT hr; 
	HINSTANCE ghSHDOCVW; 

	HRESULT(WINAPI * gpfSHGetIDispatchForFolder)(ITEMIDLIST * pidl, IWebBrowserApp * *ppIWebBrowserApp); 
	*ppIShellFolderViewDual = NULL; 
	ghSHDOCVW = LoadLibrary(_T("SHDOCVW.DLL")); 
	if(ghSHDOCVW == NULL)
		return   FALSE; 
	pIWebBrowserApp = NULL; 
	gpfSHGetIDispatchForFolder = (HRESULT(WINAPI *)(ITEMIDLIST *, IWebBrowserApp * *)) GetProcAddress(ghSHDOCVW, "SHGetIDispatchForFolder "); 
	if(gpfSHGetIDispatchForFolder == NULL)
		return   FALSE; 
	if(SUCCEEDED(gpfSHGetIDispatchForFolder(pidl, &pIWebBrowserApp))) 
	{
		if(SUCCEEDED(pIWebBrowserApp-> get_HWND((long *) &hWnd)))
		{
			SetForegroundWindow(hWnd); 
			ShowWindow(hWnd, SW_SHOWNORMAL);
		} 
		if(SUCCEEDED(hr = pIWebBrowserApp-> get_Document(&pDoc))) 
		{
			pDoc-> QueryInterface(IID_IShellFolderViewDual, (void * *) ppIShellFolderViewDual); 
			pDoc-> Release();
		} 
		pIWebBrowserApp-> Release();
	} 
	FreeLibrary(ghSHDOCVW); 
	return   TRUE;
} 

BOOL BaseOperation::XZSHOpenFolderAndSelectItems(ITEMIDLIST* pidlFolder) 
{INTERFUN;
	HRESULT(WINAPI * gpfSHOpenFolderAndSelectItems)(LPCITEMIDLIST * pidlFolder, UINT   cidl, LPCITEMIDLIST * apidl, DWORD   dwFlags); 
	HINSTANCE ghShell32; 
	ghShell32 = LoadLibrary(_T("Shell32.DLL")); 
	if(ghShell32 == NULL) 
	{
		return   FALSE; 
	}
	gpfSHOpenFolderAndSelectItems = (HRESULT(WINAPI *)(LPCITEMIDLIST *, UINT, LPCITEMIDLIST *, DWORD)) 
		GetProcAddress(ghShell32, "SHOpenFolderAndSelectItems"); 
	if(gpfSHOpenFolderAndSelectItems != NULL) 
	{
		if(SUCCEEDED(gpfSHOpenFolderAndSelectItems((LPCITEMIDLIST *) pidlFolder, 0, (LPCITEMIDLIST *) NULL, 0))) 
		{
			FreeLibrary(ghShell32); 
			return   TRUE;
		} 
		FreeLibrary(ghShell32); 
		return   FALSE;
	} 
	FreeLibrary(ghShell32); 
	return   FALSE;
} 

void BaseOperation::InsertFunctionInfo( LPCWSTR filename, int nLine, LPCWSTR functionname )
/*这里不能再调用INTERFUN;*/{

    if (!appFunctionInfo) return;

    wchar_t info[512]; 
    swprintf_s(info, 256, L"@@@ FunctionInfo : %s(%d) %s", filename, nLine, functionname);
    LOG->WriteDebug(info);
}

DeviceData* BaseOperation::FindDevice( wstring id )
{
	return DEVICE_MANAGER->FindDevice(id);
}

App_CC_END