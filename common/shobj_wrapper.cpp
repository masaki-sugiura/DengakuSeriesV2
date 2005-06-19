//	$Id: shobj_wrapper.cpp,v 1.3 2005-06-19 15:33:53 sugiura Exp $
/*
 *	shobj_wrapper.cpp
 *	Shell オブジェクトをラップするクラスの実装
 */

#include "shobj_wrapper.h"
#include "resource.h"

SHOBJ_Allocator g_SHOBJ_Allocator;

SHOBJ_Allocator::SHOBJ_Allocator()
{
	::SHGetMalloc(&m_lpMAlloc);
}

SHOBJ_Allocator::~SHOBJ_Allocator()
{
	m_lpMAlloc->Release();
}

void*
SHOBJ_Allocator::allocate(DWORD size)
{
	return m_lpMAlloc->Alloc(size);
}

void
SHOBJ_Allocator::free(void* ptr)
{
	m_lpMAlloc->Free(ptr);
}

//	terminator の長さは除いた ITEMIDLIST 配列の長さを返す
DWORD
LPIDL_Wrapper::getListSize() const
{
	if (m_lpShObj == NULL) return 0;

	DWORD cb = 0;
	for (LPITEMIDLIST pidOne = m_lpShObj;
		pidOne->mkid.cb != 0;
		pidOne = getNextListPtr(pidOne)) {
		cb += pidOne->mkid.cb;
	}

	return cb;
}

LPIDL_Wrapper
LPIDL_Wrapper::getNextItem() const
{
	if (m_pEnumHeader == NULL) return NULL;
	int cb = m_pEnumHeader->mkid.cb;
	LPITEMIDLIST
		newIDL = (LPITEMIDLIST)g_SHOBJ_Allocator.allocate(cb + sizeof(USHORT));
	if (newIDL == NULL) return NULL;
	::CopyMemory(newIDL, m_pEnumHeader, cb);
	*((USHORT*)(((LPBYTE)newIDL) + cb)) = 0;
	m_pEnumHeader = getNextListPtr(m_pEnumHeader);
	if (m_pEnumHeader->mkid.cb == 0) m_pEnumHeader = NULL;
	return newIDL;
}

LPSF_Wrapper
LPSF_Wrapper::getDesktopFolder()
{
	LPSHELLFOLDER lpsfDesktop = NULL;

	::SHGetDesktopFolder(&lpsfDesktop);

	return lpsfDesktop;
}

StringBuffer
LPSF_Wrapper::getDisplayNameOf(const LPIDL_Wrapper& lpidlFolder, DWORD flag) const
{
	STRRET strRet;
	m_lpShObj->GetDisplayNameOf(lpidlFolder, flag, &strRet);

	switch (strRet.uType) {
	case STRRET_OFFSET:
		return (LPCSTR)(LPCITEMIDLIST)lpidlFolder + strRet.uOffset;

	case STRRET_WSTR:
		{
			TCHAR buf[MAX_PATH];
			buf[0] = '\0';
			::WideCharToMultiByte(CP_THREAD_ACP,
								WC_COMPOSITECHECK,
								strRet.pOleStr,-1,
								buf,MAX_PATH,
								0,NULL);
			g_SHOBJ_Allocator.free(strRet.pOleStr);
			return buf;
		}

	case STRRET_CSTR:
		return strRet.cStr;

	default:
		return nullStr;
	}
}

struct NETACCOUNTPARAM {
	LPCSTR uncpath;
	LPSTR username;
	LPSTR passwd;
	int username_size;
	int passwd_size;
};

static UINT
NetAccountDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NETACCOUNTPARAM* pnap;
	HWND hCtrl;

	switch (uMsg) {
	case WM_INITDIALOG:
		::SetWindowLong(hDlg, DWL_USER, (LONG)lParam);
		pnap = (NETACCOUNTPARAM*)lParam;
		hCtrl = ::GetDlgItem(hDlg, IDC_UNC_PATH);
		::SetWindowText(hCtrl, pnap->uncpath);
		::SetFocus(::GetDlgItem(hDlg, IDC_USERNAME));
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			pnap = (NETACCOUNTPARAM*)::GetWindowLong(hDlg, DWL_USER);
			hCtrl = ::GetDlgItem(hDlg, IDC_USERNAME);
			::GetWindowText(hCtrl, pnap->username, pnap->username_size - 1);
			hCtrl = ::GetDlgItem(hDlg, IDC_PASSWD);
			::GetWindowText(hCtrl, pnap->passwd, pnap->passwd_size - 1);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		default:
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static BOOL
GetNetAccountInfo(HWND hwndOwner,
				  const StringBuffer& UNC_Path,
				  NETACCOUNTPARAM* pNAP)
{
	HINSTANCE hInst = (HINSTANCE)::GetWindowLong(hwndOwner, GWL_HINSTANCE);
	return ::DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_NETACCOUNT),
							hwndOwner, (DLGPROC)NetAccountDlgProc, (LPARAM)pNAP);
}

void
LPSF_Wrapper::initSequentialGet(DWORD flag, HWND hwndOwner) const
{
	m_LPEIDL_Wrapper = NULL; // release first
	LPENUMIDLIST lpeidList = NULL;
	for (;;) {
		HRESULT ret = m_lpShObj->EnumObjects(NULL, flag, &lpeidList);
		if (ret == NOERROR) {
			m_LPEIDL_Wrapper = lpeidList;
		} else if (ret == 0x800704c7) {
			// Network share directory access
			LPITEMIDLIST
				lpidl = (LPITEMIDLIST)g_SHOBJ_Allocator.allocate(sizeof(USHORT));
			*(USHORT*)lpidl = 0;
			StringBuffer UNC_Path(this->getDisplayNameOf(lpidl,
												SHGDN_NORMAL|SHGDN_FORPARSING));
			TCHAR username[80], passwd[128];
			NETACCOUNTPARAM nap;
			nap.uncpath = (LPCSTR)UNC_Path;
			nap.username = username;
			nap.passwd = passwd;
			nap.username_size = 80;
			nap.passwd_size = 128;
			if (!GetNetAccountInfo(hwndOwner, UNC_Path, &nap)) return;

			NETRESOURCE nr;
			::ZeroMemory(&nr, sizeof(nr));
			nr.dwType = RESOURCETYPE_ANY;
			nr.lpRemoteName = UNC_Path.getBufPtr();

			::WNetAddConnection3(hwndOwner, &nr, passwd, username, 0);
			continue;
		}
#ifdef _DEBUG
		else {
			::OutputDebugString("enum object failed");
		}
#endif
		break;
	}
}

