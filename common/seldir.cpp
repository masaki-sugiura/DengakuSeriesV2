//	$Id: seldir.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	seldir.cpp
 *	ディレクトリ選択ダイアログの実装
 */

#include "seldir.h"
#include "shicons.h"
#include "resource.h"
#include "session.h"

#define IDM_ADDFOLDER 9001
#define IDM_RENAME 9002
#define IDM_DELETE 9003

#define SELDIR_SHOWERROR 1

#ifdef SELDIR_SHOWERROR
#define ShowErrorMsg(msg) ::MessageBox(NULL,(msg),NULL,MB_OK);
#else
#define ShowErrorMsg(msg)
#endif

#ifdef __GNUC__

// should be in shlobj.h
#define SHGDN_INCLUDE_NONFILESYS (SHGNO)0x2000
#define SHGDN_FORADDRESSBAR      (SHGNO)0x4000
#define SFGAO_BROWSABLE          0x08000000L

typedef struct tagTVDISPINFOA {
	NMHDR hdr;
	TVITEMA item;
} NMTVDISPINFOA, FAR *LPNMTVDISPINFOA;

typedef struct tagTVDISPINFOW {
	NMHDR hdr;
	TVITEMW item;
} NMTVDISPINFOW, FAR *LPNMTVDISPINFOW;

#ifndef UNICODE
#define NMTVDISPINFO   NMTVDISPINFOA
#define LPNMTVDISPINFO LPNMTVDISPINFOA
#else
#define NMTVDISPINFO   NMTVDISPINFOW
#define LPNMTVDISPINFO LPNMTVDISPINFOW
#endif

#endif

SelectDirByDlg::SelectDirByDlg(HINSTANCE hInstance)
	:	m_hInstance(hInstance),
		m_Title(nullStr),
		m_IniDir(nullStr),
		m_SelectDir(nullStr)
{
	::InitCommonControls();

	m_pShellIcons = new ShellIcons();
	m_hImgList = ImageList_Create(m_pShellIcons->getIconSize(),
								m_pShellIcons->getIconSize(),
								ILC_COLOR16,	//	環境依存？
								0,20);
	::SHGetMalloc(&m_pMAlloc);
	::SHGetDesktopFolder(&m_psfDesktop);
	m_hPopupMenu = createTreePopupMenu();
	m_htiDesktop = NULL;
	m_hTreeView = NULL;
	m_bEditMode = FALSE;
}

SelectDirByDlg::~SelectDirByDlg()
{
	::DestroyMenu(m_hPopupMenu);
	m_psfDesktop->Release();
	m_pMAlloc->Release();
	ImageList_Destroy(m_hImgList);
	delete m_pShellIcons;
}

//	ポップアップメニューに関する関数群

//	ポップアップメニューを作成
HMENU
SelectDirByDlg::createTreePopupMenu()
{
	HMENU hMenu = ::CreatePopupMenu();
	::AppendMenu(hMenu,MF_STRING,IDM_ADDFOLDER,"サブフォルダの追加");
	::AppendMenu(hMenu,MF_STRING,IDM_RENAME,"名前の変更");
	::AppendMenu(hMenu,MF_STRING,IDM_DELETE,"削除");
	return hMenu;
}

//	メニューの状態を変更
//	attr :	IShellFolder::GetAttributesOf() の返り値
//			SFGAO_READONLY | SFGAO_CANRENAME | SFGAO_CANDELETE のどれか
void
SelectDirByDlg::setMenuState(DWORD attr)
{
	::ModifyMenu(m_hPopupMenu,
				0,
				MF_BYPOSITION|MF_STRING|
					((attr & SFGAO_READONLY) ? MF_GRAYED : 0),
				IDM_ADDFOLDER,
				"サブフォルダの追加");
	::ModifyMenu(m_hPopupMenu,
				1,
				MF_BYPOSITION|MF_STRING|
					((attr & SFGAO_CANRENAME) ? 0 : MF_GRAYED),
				IDM_RENAME,
				"名前の変更");
	::ModifyMenu(m_hPopupMenu,
				2,
				MF_BYPOSITION|MF_STRING|
					((attr & SFGAO_CANDELETE) ? 0 : MF_GRAYED),
				IDM_DELETE,
				"削除");
}

int
SelectDirByDlg::showPopupMenu(POINT& pt)
{
	return ::TrackPopupMenu(m_hPopupMenu,
						TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD,
						pt.x,pt.y,
						0,
						m_hTreeView,
						NULL);
}

//	アイコン・イメージリストの管理を行う関数群

//	イメージリストをツリービューコントロールに登録
void
SelectDirByDlg::setImgListToTreeView()
{
	COLORREF crBkColor = (COLORREF)0x00ffffff;
	HDC hDc = ::GetDC(m_hTreeView);
	if (hDc != NULL) {
		COLORREF newcolor = ::GetBkColor(hDc);
		if (newcolor != CLR_INVALID) crBkColor = newcolor;
		::ReleaseDC(m_hTreeView,hDc);
	}
	ImageList_SetBkColor(m_hImgList,crBkColor);
	TreeView_SetImageList(m_hTreeView,m_hImgList,TVSIL_NORMAL);
}

BOOL
SelectDirByDlg::getIconFromInterface(
	IExtractIcon* peIcon,
	int* pindex,
	int* pselindex)
{
	TCHAR buf[MAX_PATH];
	UINT ret;
	if (peIcon->GetIconLocation(GIL_OPENICON,
								buf,
								MAX_PATH,
								pselindex,
								&ret) != NOERROR) {
		ShowErrorMsg("ShellError: #150");
		return FALSE;
	}
	HICON hIcon, hSmIcon;
	int size = (m_pShellIcons->getIconSize()<<16) + 32;	//	環境依存？
	if (peIcon->Extract(buf,*pselindex,&hIcon,&hSmIcon,size) != NOERROR) {
		ShowErrorMsg("ShellError: #156");
		return FALSE;
	}
	*pselindex
		= m_pShellIcons->addIconEntry(m_hImgList,buf,*pselindex,hSmIcon);
	if (pindex != NULL) {
		if (peIcon->GetIconLocation(0,buf,MAX_PATH,pindex,(UINT*)&ret)
			!= NOERROR) {
			ShowErrorMsg("ShellError: #163");
			*pindex = *pselindex;
			return TRUE;
		}
		if (peIcon->Extract(buf,*pindex,&hIcon,&hSmIcon,size) != NOERROR) {
			ShowErrorMsg("ShellError: #167");
			*pindex = *pselindex;
			return TRUE;
		}
		*pindex = m_pShellIcons->addIconEntry(m_hImgList,buf,*pindex,hSmIcon);
	}
	return TRUE;
}

BOOL
SelectDirByDlg::getIconFromFileInfo(
	LPSHELLFOLDER psfParent,
	LPCITEMIDLIST pidlFolder,
	int* pindex,
	int* pselindex
	)
{
	//	このメソッドが呼び出される可能性は、今の所 Windows2000 上で
	//	ＨＤＤのアイコンを extract する時だけ、であることを仮定する。
	//	何故なら、SHGetFileInfo はフルパスのファイル名しか受け付けないから。

	HRESULT hRet;
	STRRET strRet;
	if ((hRet = psfParent->GetDisplayNameOf(pidlFolder,
											SHGDN_NORMAL|SHGDN_FORPARSING
											/* |SHGDN_FORADDRESSBAR */,
											&strRet))
		!= NOERROR) {
		StringBuffer buf;
		buf.append("ShellError: #140\nerror code = ").append((ULONG)hRet);
		ShowErrorMsg(buf);
		return FALSE;
	}
	TCHAR buf[MAX_PATH];
	if (!getDisplayNameToBuffer(strRet,pidlFolder,buf))
		return FALSE;

	//	この時点で buf には "C:\" 等のドライブ名が入っている(はず)

	SHFILEINFO shFileInfo;
	if (::SHGetFileInfo(buf,0,
						&shFileInfo,sizeof(SHFILEINFO),
						SHGFI_ICON|SHGFI_SMALLICON|SHGFI_OPENICON) == 0)
		return FALSE;
	*pselindex = m_pShellIcons->addIconEntry(m_hImgList,shFileInfo);

	if (pindex == NULL) return TRUE;

	if (::SHGetFileInfo(buf,0,
						&shFileInfo,sizeof(SHFILEINFO),
						SHGFI_ICON|SHGFI_SMALLICON/*|SHGFI_OPENICON*/) == 0) {
		*pindex = *pselindex;
		return TRUE;
	}
	*pindex = m_pShellIcons->addIconEntry(m_hImgList,shFileInfo);

	return TRUE;
}

//	フォルダに関連付けられたアイコンをイメージリストに登録し、
//	イメージリストのインデックスを返す
//	状態が１つしかないフォルダについては *pselindex に格納
BOOL
SelectDirByDlg::getIconIndex(
	LPSHELLFOLDER lpsfParent,	//	親フォルダのインターフェイス
	LPCITEMIDLIST lpilFolder,	//	登録対象のサブフォルダのＩＤ
	int* pindex,				//	インデックスを格納する int へのポインタ
	int* pselindex)				//	選択状態の　〃
{
	if (pselindex == NULL) return FALSE;
	IExtractIcon* lpeIcon;
	HRESULT hRet;
	if ((hRet = lpsfParent->GetUIObjectOf(NULL,
										1,
										&lpilFolder,
										IID_IExtractIcon,
										NULL,
										(void**)&lpeIcon))
		!= NOERROR) {
		return getIconFromFileInfo(lpsfParent,lpilFolder,pindex,pselindex);
	} else {
		int ret = getIconFromInterface(lpeIcon,pindex,pselindex);
		lpeIcon->Release();
		return ret;
	}
}

//	ＩＤリストの操作系関数群

//	ITEMIDLIST の配列の先頭の ITEMIDLIST "だけ"をコピーする関数
//	返されたリストは使用後に g_pMAlloc->Free() で開放すること！
LPITEMIDLIST
SelectDirByDlg::copyIDList(LPCITEMIDLIST lpilFolder)
{
	int cb = lpilFolder->mkid.cb;
	LPITEMIDLIST newIDL = (LPITEMIDLIST)m_pMAlloc->Alloc(cb + sizeof(USHORT));
	if (newIDL == NULL) return NULL;
	::CopyMemory(newIDL,lpilFolder,cb);
	*((USHORT*)(((LPBYTE)newIDL) + cb)) = 0;
	return newIDL;
}

//	ITEMIDLIST の配列中の次の ITEMIDLIST へのポインタを返す
LPCITEMIDLIST
SelectDirByDlg::getNextItemID(LPCITEMIDLIST pidl)
{ 
	int cb = pidl->mkid.cb;
	if (cb == 0) return NULL;
	pidl = (LPCITEMIDLIST)(((LPBYTE)pidl) + cb);
	return (pidl->mkid.cb == 0) ? NULL : pidl;
} 

//	terminator の長さは除いた ITEMIDLIST 配列の長さを返す
int
SelectDirByDlg::getIDListSize(LPCITEMIDLIST pidl)
{
	int cb = 0;
	for (LPCITEMIDLIST pidOne = pidl;
		pidOne != NULL;
		pidOne = getNextItemID(pidOne)) {
		cb += pidOne->mkid.cb;
	}
	return cb;
}

//	STRRET 構造体に格納されたファイル名を buf に抜き出す
//	（name.uType == STRRET_WSTR の場合、ついでにメモリを開放する）
BOOL
SelectDirByDlg::getDisplayNameToBuffer(
	const STRRET& name,	//	対象となる STRRET 構造体への参照
	LPCITEMIDLIST lpilFolder,	//	name が指すサブフォルダの ITEMIDLIST
								//	(相対値)
	LPSTR buf)	//	結果を格納する配列 (sizeof(buf) >= MAX_PATH)
{
	if (buf == NULL) return FALSE;
	switch (name.uType) {
	case STRRET_OFFSET:
		lstrcpy(buf,((LPCSTR)lpilFolder) + name.uOffset);
		break;
	case STRRET_WSTR:
		::WideCharToMultiByte(CP_ACP,
							WC_COMPOSITECHECK,
							name.pOleStr,-1,
							buf,MAX_PATH,
							0,NULL);
		m_pMAlloc->Free(name.pOleStr);
		break;
	case STRRET_CSTR:
		lstrcpy(buf,name.cStr);
		break;
	default:
		buf[0] = '\0';
		return FALSE;
	}
	return TRUE;
}

//	サブフォルダの ITEMIDLIST を得る
void
SelectDirByDlg::getSubFolderIDList(
	LPSHELLFOLDER lpsfParent,	//	親フォルダを表す IShellFolder
								//	インターフェイス
	LPCSTR fname,	//	サブフォルダ名
	LPITEMIDLIST& lpilFolder)	//	結果を格納するポインタへの参照
{
	LPENUMIDLIST lpelParent = NULL;
	lpsfParent->EnumObjects(NULL,
							SHCONTF_FOLDERS|SHCONTF_INCLUDEHIDDEN,
							&lpelParent);
	STRRET strRet;
	TCHAR buf[MAX_PATH];
	while (lpelParent->Next(1,&lpilFolder,NULL) == NOERROR) {
		lpsfParent->GetDisplayNameOf(lpilFolder,
									SHGDN_INFOLDER|SHGDN_INCLUDE_NONFILESYS,
									&strRet);
		buf[0] = '\0';
		getDisplayNameToBuffer(strRet,lpilFolder,buf);
		if (lstrcmpi(buf,fname) == 0) break;
		m_pMAlloc->Free(lpilFolder);
		lpilFolder = NULL;
	}
	if (lpilFolder == NULL) {
		ShowErrorMsg("ShellError: #273");
	}
	lpelParent->Release();
}

BOOL
SelectDirByDlg::getItemIDFromFSPath(
	const StringBuffer& iniDir,
	int len,
	LPITEMIDLIST& lpilFolder)
{
	WCHAR dirname[MAX_PATH];
	::ZeroMemory(dirname,sizeof(WCHAR)*MAX_PATH);
	iniDir.toUnicode(dirname);

	BOOL bRet = (m_psfDesktop->ParseDisplayName(NULL,NULL,
												dirname,(ULONG*)&len,
												&lpilFolder,NULL) == NOERROR);
	if (!bRet) {
		ShowErrorMsg("ShellError: #291");
	}
	return bRet;
}

class AutoPtr_ShellFolder {
public:
	AutoPtr_ShellFolder(LPSHELLFOLDER lpShellFolder = NULL)
		: m_lpShellFolder(lpShellFolder) {}
	~AutoPtr_ShellFolder()
	{
		if (m_lpShellFolder != NULL) m_lpShellFolder->Release();
	}
	AutoPtr_ShellFolder& operator=(LPSHELLFOLDER lpShellFolder)
	{
		if (m_lpShellFolder != NULL &&
			m_lpShellFolder != lpShellFolder) m_lpShellFolder->Release();
		m_lpShellFolder = lpShellFolder;
		return *this;
	}
	operator LPSHELLFOLDER&() { return m_lpShellFolder; }
	LPSHELLFOLDER operator->() { return m_lpShellFolder; }
	LPSHELLFOLDER* operator&() { return &m_lpShellFolder; }

private:
	LPSHELLFOLDER m_lpShellFolder;
};

class AutoPtr_ItemIdList {
public:
	AutoPtr_ItemIdList(LPMALLOC lpMAlloc, LPITEMIDLIST ptr = NULL)
		: m_lpMAlloc(lpMAlloc), m_ptr(ptr) {}
	~AutoPtr_ItemIdList() { m_lpMAlloc->Free(m_ptr); }
	AutoPtr_ItemIdList& operator=(LPITEMIDLIST ptr)
	{
		if (m_ptr != NULL && m_ptr != ptr) m_lpMAlloc->Free(m_ptr);
		m_ptr = ptr;
		return *this;
	}
	operator LPITEMIDLIST&() { return m_ptr; }
	LPITEMIDLIST operator->() { return m_ptr; }
	LPITEMIDLIST* operator&() { return &m_ptr; }

private:
	LPMALLOC m_lpMAlloc;
	LPITEMIDLIST m_ptr;
};

BOOL
SelectDirByDlg::getItemIDFromUNCPath(
	const StringBuffer& iniDir,
	int len,
	LPITEMIDLIST& lpilFolder)
{
	TCHAR NameBuf[MAX_PATH];
	LPCSTR ostr = ((LPCSTR)iniDir) + 2;
	LPSTR dstr = NameBuf;
	while (ostr != NULL && *ostr != '\\' && *ostr != '\0')
		*dstr++ = *ostr++;
	*dstr = '\0';
	ostr++;
	AutoPtr_ItemIdList lpilNetworkRoot(m_pMAlloc);
	::SHGetSpecialFolderLocation(NULL,CSIDL_NETWORK,&lpilNetworkRoot);
	AutoPtr_ShellFolder lpsfNetworkRoot;
	m_psfDesktop->BindToObject(lpilNetworkRoot,NULL,
							IID_IShellFolder,(void**)&lpsfNetworkRoot);
	AutoPtr_ItemIdList lpilServer(m_pMAlloc);
	getSubFolderIDList(lpsfNetworkRoot,NameBuf,(LPITEMIDLIST&)lpilServer);
	if ((LPITEMIDLIST&)lpilServer == NULL) return FALSE;
	AutoPtr_ShellFolder lpsfServer;
	lpsfNetworkRoot->BindToObject(lpilServer,NULL,
								IID_IShellFolder,(void**)&lpsfServer);
	dstr = NameBuf;
	while (ostr != NULL && *ostr != '\\' && *ostr != '\0')
		*dstr++ = *ostr++;
	*dstr = '\0';
	ostr++;
	AutoPtr_ItemIdList lpilShare(m_pMAlloc);
	getSubFolderIDList(lpsfServer,NameBuf,lpilShare);
	if ((LPITEMIDLIST&)lpilShare == NULL) return FALSE;
	AutoPtr_ItemIdList lpilDirectory(m_pMAlloc);
	if (*ostr != '\0') {
		AutoPtr_ShellFolder lpsfShare;
		lpsfServer->BindToObject(lpilShare,NULL,
								IID_IShellFolder,(void**)&lpsfShare);
		if ((LPSHELLFOLDER&)lpsfShare == NULL) return FALSE;
		WCHAR dirname[MAX_PATH];
		::ZeroMemory(dirname,sizeof(WCHAR)*MAX_PATH);
		StringBuffer(ostr,len - (ostr - (LPCSTR)iniDir)).toUnicode(dirname);
		ULONG dirlen;
		lpsfShare->ParseDisplayName(NULL,NULL,dirname,&dirlen,
									&lpilDirectory,NULL);
	}
	int nrlen = getIDListSize(lpilNetworkRoot),
		svlen = getIDListSize(lpilServer),
		srlen = getIDListSize(lpilShare),
		drlen = getIDListSize(lpilDirectory);
	lpilFolder = (LPITEMIDLIST)m_pMAlloc->Alloc(nrlen + svlen + srlen + drlen +
												sizeof(USHORT));
	::CopyMemory(lpilFolder,(LPITEMIDLIST&)lpilNetworkRoot,nrlen);
	::CopyMemory(((LPBYTE)lpilFolder)+nrlen,(LPITEMIDLIST&)lpilServer,svlen);
	::CopyMemory(((LPBYTE)lpilFolder)+nrlen+svlen,
				(LPITEMIDLIST&)lpilShare,srlen);
	::CopyMemory(((LPBYTE)lpilFolder)+nrlen+svlen+srlen,
				(LPITEMIDLIST&)lpilDirectory,drlen);
	*(USHORT*)(((LPBYTE)lpilFolder)+nrlen+svlen+srlen+drlen) = 0;
	return TRUE;
}

//	ツリービューコントロールと shell 操作をあわせた関数群

//	ツリービューに項目を追加
//	結果は追加された項目のハンドル(HTREEITEM)
HTREEITEM
SelectDirByDlg::insertTreeItem(
	HTREEITEM hParent,	//	追加対象の親となる項目のハンドル
	HTREEITEM hInsertAfter,	//	追加場所を指定するハンドル
	LPSHELLFOLDER lpsfParent,	//	追加対象の親フォルダを表す IShellFolder
								//	インターフェイス
	LPCITEMIDLIST lpilFolder)	//	追加対象のサブフォルダを表す
								//	ITEMIDLIST (相対値)
{
	ULONG mask = SFGAO_CAPABILITYMASK | SFGAO_CONTENTSMASK |
				 SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FOLDER;
	BOOL isDesktop = (hParent == TVI_ROOT);
	if (isDesktop)
		mask = SFGAO_HASSUBFOLDER;	//	desktop itself.
	else
		lpsfParent->GetAttributesOf(1,&lpilFolder,&mask);
	BOOL bTwoState = ((mask & SFGAO_FOLDER) != 0 &&
					  (mask & SFGAO_FILESYSTEM) != 0);

	int index = -1, selindex = -1;
	if (!getIconIndex(lpsfParent,
					lpilFolder,
					(bTwoState || isDesktop) ? (&index) : NULL,
					&selindex)) {
		index = selindex = 0;
	}

	LPTVITEMINFO lptviInfo = new TVITEMINFO;
	lptviInfo->m_inserted = (mask & SFGAO_HASSUBFOLDER) ?
							TVITEMINFO_HASSUBFOLDER : 0;
	::ZeroMemory(lptviInfo->m_strbuf,MAX_PATH);

	STRRET DisplayName;
	::ZeroMemory(&DisplayName,sizeof(STRRET));
	if (lpsfParent->GetDisplayNameOf(lpilFolder,
									SHGDN_INFOLDER|SHGDN_INCLUDE_NONFILESYS,
									&DisplayName)
		!= NOERROR) {
		return NULL;
	}
	getDisplayNameToBuffer(DisplayName,lpilFolder,lptviInfo->m_strbuf);

	//	まだ展開されていない項目の
	//	lptviInfo->m_pShellFolder は NULL。
	//	展開された時点で自分自身の IShellFolder インターフェイスに置き換わる。
	lptviInfo->m_pShellFolder = NULL;
	lptviInfo->m_pItemIDList = (LPITEMIDLIST)lpilFolder;

	TVINSERTSTRUCT is;
	is.hParent = hParent;
	is.hInsertAfter = hInsertAfter;
	is.item.mask = TVIF_CHILDREN|TVIF_IMAGE|TVIF_TEXT|TVIF_PARAM|
					TVIF_SELECTEDIMAGE;
	is.item.iImage = (bTwoState||isDesktop) ? index : selindex;
	is.item.iSelectedImage = selindex;
	is.item.cChildren = I_CHILDRENCALLBACK;
	is.item.pszText = lptviInfo->m_strbuf;
	is.item.cchTextMax = MAX_PATH;
	is.item.lParam = (LPARAM)lptviInfo;

	return TreeView_InsertItem(m_hTreeView,&is);
}

LPSHELLFOLDER
SelectDirByDlg::getParentFolder(HTREEITEM hItem)
{
	TVITEM tvItem;
	tvItem.hItem = TreeView_GetParent(m_hTreeView,hItem);
	if (tvItem.hItem == NULL) return NULL;
	else if (tvItem.hItem == TVI_ROOT) return m_psfDesktop;
	tvItem.mask = TVIF_PARAM;
	TreeView_GetItem(m_hTreeView,&tvItem);
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	if (lptviInfo == NULL) return NULL;
	return lptviInfo->m_pShellFolder;
}

int
SelectDirByDlg::addChildFolders(
	HTREEITEM htiParent,
	LPTVITEMINFO lptiParent)
{
	//	lptiParent->m_pShellFolder を自分自身に置き換える
	LPSHELLFOLDER& lpsfParent = lptiParent->m_pShellFolder;
	if (htiParent == m_htiDesktop)
		lpsfParent = m_psfDesktop;	//	desktop.
	else {
		LPSHELLFOLDER psfGrandParent = getParentFolder(htiParent);
		if (psfGrandParent == NULL) {
			return 0; // 有り得ないはず
		}
		psfGrandParent->BindToObject(lptiParent->m_pItemIDList,
									NULL,
									IID_IShellFolder,
									(void**)&lpsfParent);
		if (lpsfParent == NULL) {
			return 0; // これも有り得ないはず
		}
	}

	LPENUMIDLIST lpelChilds = NULL;
	lpsfParent->EnumObjects(m_hTreeView,
							SHCONTF_FOLDERS|SHCONTF_INCLUDEHIDDEN,
							&lpelChilds);
	int ret = 0;
	if (lpelChilds != NULL) {
		LPITEMIDLIST lpilChild = NULL;
		ULONG num;
		while (lpelChilds->Next(1,&lpilChild,&num) == NOERROR) {
			insertTreeItem(htiParent,TVI_LAST,lpsfParent,lpilChild);
			ret++;
		}
		lpelChilds->Release();
	}
	lptiParent->m_inserted = TRUE;
	if (ret > 0) lptiParent->m_inserted |= TVITEMINFO_HASSUBFOLDER;
	return ret;
}

BOOL
SelectDirByDlg::getFullPathFolderName(HTREEITEM hItem, LPSTR buf)
{
	if (!::IsWindow(m_hTreeView) || hItem == NULL || buf == NULL)
		return FALSE;
	buf[0] = '\0';
	TVITEM tvItem;
	tvItem.hItem = hItem;
	tvItem.mask = TVIF_PARAM;
	TreeView_GetItem(m_hTreeView,&tvItem);
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	LPSHELLFOLDER pShellFolder = getParentFolder(hItem);
	if (pShellFolder == NULL) {
		return FALSE; // hItem = desktop ?
	}
	STRRET strRet;
	pShellFolder->GetDisplayNameOf(lptviInfo->m_pItemIDList,
								SHGDN_NORMAL|SHGDN_FORPARSING
								/* |SHGDN_FORADDRESSBAR */,
								&strRet);
	return getDisplayNameToBuffer(strRet,lptviInfo->m_pItemIDList,buf);
}

void
SelectDirByDlg::setInitialFolder(const StringBuffer& iniDir)
{
	int len = iniDir.length();
	if (len <= 0) return;
	DWORD attr = ::GetFileAttributes(iniDir);
	if (attr == 0xFFFFFFFF) return;
	else if ((attr&FILE_ATTRIBUTE_DIRECTORY) == 0) {
		int sep = iniDir.rfind('\\');
		if (sep == -1) return;
		else if (sep == 2) sep++;
		len = sep;
	}
	LPITEMIDLIST lpilFolder = NULL;
	if (iniDir.compareTo("\\\\",FALSE,2) == 0) {
		if (!getItemIDFromUNCPath(iniDir,len,lpilFolder)) return;
	} else {
		if (!getItemIDFromFSPath(iniDir,len,lpilFolder)) return;
	}
	if (lpilFolder == NULL) return;

#if 0
	TCHAR buf[MAX_PATH];
	LPSHELLFOLDER pShellFolder = m_psfDesktop, pSubFolder = NULL;
	BOOL bDesktop = TRUE;
	for (LPCITEMIDLIST pidl = lpilFolder;
		 pidl != NULL;
		 pidl = getNextItemID(pidl)) {
		LPCITEMIDLIST pidlone = copyIDList(pidl);
		STRRET ret;
		pShellFolder->GetDisplayNameOf(pidlone,SHGDN_NORMAL,&ret);
		buf[0] = 0;
		getDisplayNameToBuffer(ret,pidlone,buf);
		::MessageBox(m_hTreeView,buf,NULL,MB_OK);
		pSubFolder = NULL;
		pShellFolder->BindToObject(pidlone,NULL,
								IID_IShellFolder,(void**)&pSubFolder);
		if (!bDesktop) pShellFolder->Release();
		pShellFolder = pSubFolder;
		m_pMAlloc->Free((void*)pidlone);
		bDesktop = FALSE;
	}
	if (pSubFolder != NULL) pSubFolder->Release();
#endif

	TVITEM tviFolder;
	HTREEITEM htiFolder = m_htiDesktop, htiChild = NULL;
	for (LPCITEMIDLIST lpilSubFolder = lpilFolder;
		 lpilSubFolder != NULL;
		 lpilSubFolder = getNextItemID(lpilSubFolder)) {
		tviFolder.hItem = htiFolder;
		tviFolder.mask = TVIF_PARAM;
		TreeView_GetItem(m_hTreeView,&tviFolder);
		LPTVITEMINFO lptviInfoFolder = (LPTVITEMINFO)tviFolder.lParam;
		if (addChildFolders(htiFolder,lptviInfoFolder) <= 0) {
			htiChild = NULL;
			break;
		}
		LPSHELLFOLDER lpsfFolder = lptviInfoFolder->m_pShellFolder;
		if (lpsfFolder == NULL) {
			htiChild = NULL;
			break;
		}
		LPCITEMIDLIST lpilThisOne = copyIDList(lpilSubFolder);
		for (htiChild = TreeView_GetChild(m_hTreeView,htiFolder);
			 htiChild != NULL;
			 htiChild = TreeView_GetNextSibling(m_hTreeView,htiChild)) {
			tviFolder.hItem = htiChild;
			tviFolder.mask = TVIF_PARAM;
			TreeView_GetItem(m_hTreeView,&tviFolder);
			LPTVITEMINFO lptviInfoChild = (LPTVITEMINFO)tviFolder.lParam;
			if (lpsfFolder->CompareIDs(0,
									lptviInfoChild->m_pItemIDList,
									lpilThisOne) == 0)
				break;
		}
		m_pMAlloc->Free((void*)lpilThisOne);
		if (htiChild == NULL) {
			break;
		}
		htiFolder = htiChild;
	}
	m_pMAlloc->Free(lpilFolder);
	if (htiChild != NULL) {
		TreeView_EnsureVisible(m_hTreeView,htiChild);
		TreeView_SelectItem(m_hTreeView,htiChild);
	}
}

BOOL
SelectDirByDlg::initTreeView(HWND hTreeView)
{
	m_hTreeView = hTreeView;
	setImgListToTreeView();
	if (m_flag&DBPA_DISABLEMODIFY) {
		LONG old = ::GetWindowLong(hTreeView,GWL_STYLE);
		::SetWindowLong(hTreeView,GWL_STYLE,old&~TVS_EDITLABELS);
	}

	LPITEMIDLIST lpidDesktop = (LPITEMIDLIST)m_pMAlloc->Alloc(sizeof(USHORT));
	lpidDesktop->mkid.cb = 0;
	m_htiDesktop = insertTreeItem(TVI_ROOT,TVI_FIRST,
								m_psfDesktop,
								lpidDesktop);

	setInitialFolder(m_IniDir);

	return TRUE;
}

void
SelectDirByDlg::freeResources(HTREEITEM htItem)
{
	if (htItem == NULL) return;
	TVITEM tvItem;
	tvItem.hItem = htItem;
	tvItem.mask = TVIF_PARAM;
	TreeView_GetItem(m_hTreeView,&tvItem);
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	LPSHELLFOLDER& pShellFolder = lptviInfo->m_pShellFolder;
	if (pShellFolder != m_psfDesktop && pShellFolder != NULL)
		pShellFolder->Release();
	m_pMAlloc->Free((void*)lptviInfo->m_pItemIDList);
	delete (LPTVITEMINFO)tvItem.lParam;
	for (HTREEITEM htiChild = TreeView_GetChild(m_hTreeView,htItem);
		 htiChild != NULL;
		 htiChild = TreeView_GetNextSibling(m_hTreeView,htiChild)) {
		freeResources(htiChild);
	}
}

void
SelectDirByDlg::uninitTreeView()
{
	if (::IsWindow(m_hTreeView))
		freeResources(TreeView_GetRoot(m_hTreeView));
}

BOOL
SelectDirByDlg::insertNewFolder(HTREEITEM hItem)
{
	TCHAR buf[MAX_PATH];
	::ZeroMemory(buf,MAX_PATH);
	if (!getFullPathFolderName(hItem,buf)) return FALSE;
	int len = lstrlen(buf);
	if (*(buf + len - 1) == '\\') len--;
	lstrcpy(buf+len,"\\新しいフォルダ");
	if (!::CreateDirectory(buf,NULL)) return FALSE;
	TVITEM tvItem;
	tvItem.hItem = hItem;
	tvItem.mask = TVIF_PARAM;
	TreeView_GetItem(m_hTreeView,&tvItem);
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	LPSHELLFOLDER pShellFolder = lptviInfo->m_pShellFolder;
	HTREEITEM hNewItem = NULL;
	if (pShellFolder != NULL) {
		LPWSTR subname = L"新しいフォルダ";
		ULONG num;
		LPITEMIDLIST pidl = NULL;
		if (pShellFolder->ParseDisplayName(m_hTreeView,NULL,subname,
											&num,&pidl,NULL)
			!= NOERROR) return FALSE;
		hNewItem = insertTreeItem(hItem,TVI_LAST,pShellFolder,pidl);
	} else {
		if (addChildFolders(hItem,lptviInfo) <= 0) return FALSE;
		for (hNewItem = TreeView_GetChild(m_hTreeView,hItem);
			 hNewItem != NULL;
			 hNewItem = TreeView_GetNextSibling(m_hTreeView,hNewItem)) {
			buf[0] = '\0';
			tvItem.hItem = hNewItem;
			tvItem.mask = TVIF_TEXT;
			tvItem.pszText = buf;
			tvItem.cchTextMax = MAX_PATH;
			TreeView_GetItem(m_hTreeView,&tvItem);
			if (lstrcmpi("新しいフォルダ",buf) == 0) break;
		}
	}
	if (hNewItem == NULL) return FALSE;
	TreeView_EnsureVisible(m_hTreeView,hNewItem);
	TreeView_SelectItem(m_hTreeView,hNewItem);
	return TreeView_EditLabel(m_hTreeView,hNewItem) != NULL;
}

BOOL
SelectDirByDlg::renameFolder(TVITEM& tvItem)
{
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	LPSHELLFOLDER pShellFolder = getParentFolder(tvItem.hItem);
	if (pShellFolder == NULL) return FALSE;
	WCHAR newname[MAX_PATH];
	::ZeroMemory(newname,sizeof(WCHAR)*MAX_PATH);
	StringBuffer(tvItem.pszText).toUnicode(newname);
	LPITEMIDLIST pNewIDList = NULL;
	if (pShellFolder->SetNameOf(m_hTreeView,
								lptviInfo->m_pItemIDList,
								newname,
								SHGDN_INFOLDER|SHGDN_FORPARSING,
								&pNewIDList) == NOERROR &&
		pNewIDList != NULL) {
		m_pMAlloc->Free(lptviInfo->m_pItemIDList);
		lptviInfo->m_pItemIDList = pNewIDList;
		return TRUE;
	}
	return FALSE;
}

BOOL
SelectDirByDlg::deleteFolder(HTREEITEM hItem)
{
	TCHAR buf[MAX_PATH];
	::ZeroMemory(buf,MAX_PATH);
	if (!getFullPathFolderName(hItem,buf)) return FALSE;
	SHFILEOPSTRUCT shFop;
	shFop.hwnd = m_hTreeView;
	shFop.wFunc = FO_DELETE;
	shFop.pFrom = buf;
	shFop.pTo = NULL;
	shFop.fFlags = FOF_SILENT;
	if (::SHFileOperation(&shFop) != 0) return FALSE;
	freeResources(hItem);
	return TreeView_DeleteItem(m_hTreeView,hItem);
}

BOOL
SelectDirByDlg::getItemAttributes(HTREEITEM hItem, DWORD& attr)
{
	TVITEM tvItem;
	tvItem.hItem = hItem;
	tvItem.mask = TVIF_PARAM;
	TreeView_GetItem(m_hTreeView,&tvItem);
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	LPSHELLFOLDER pShellFolder = getParentFolder(tvItem.hItem);
	if (pShellFolder == NULL) return FALSE; // desktop
	return pShellFolder->GetAttributesOf(1,
									(LPCITEMIDLIST*)&lptviInfo->m_pItemIDList,
									&attr) == NOERROR;
}

BOOL CALLBACK
DirBrowseProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			::SetWindowLong(hDlg,DWL_USER,lParam);
			SelectDirByDlg* psdbd = (SelectDirByDlg*)lParam;
			::SetDlgItemText(hDlg,IDC_TITLE,psdbd->getTitle());
			psdbd->initTreeView(::GetDlgItem(hDlg,IDC_TREE));
			return FALSE;
		}
	case WM_COMMAND:
		{
			SelectDirByDlg*
				psdbd = (SelectDirByDlg*)::GetWindowLong(hDlg,DWL_USER);
			HWND hTreeView = ::GetDlgItem(hDlg,IDC_TREE);
			switch (LOWORD(wParam)) {
			case IDOK:
			case IDCANCEL:
				if (!psdbd->isEditMode())
					::SendMessage(hDlg,WM_COMMAND,
								LOWORD(wParam) == IDOK ? IDC_OK : IDC_CANCEL,
								0);
				else if (HIWORD(wParam) != EN_UPDATE &&
						 HIWORD(wParam) != EN_CHANGE)
					TreeView_EndEditLabelNow(hTreeView,LOWORD(wParam) != IDOK);
				break;
			case IDC_NEWFOLDER:
				psdbd->insertNewFolder(TreeView_GetSelection(hTreeView));
				break;
			case IDC_RENAME:
				TreeView_EditLabel(hTreeView,
								TreeView_GetSelection(hTreeView));
				break;
			case IDC_DELETE:
				psdbd->deleteFolder(TreeView_GetSelection(hTreeView));
				break;
			case IDC_OK:
				{
					StringBuffer buf(MAX_PATH);
					psdbd->getFullPathFolderName(
									TreeView_GetSelection(hTreeView),
									buf.getBufPtr());
					buf.length();
					psdbd->setSelectDir(buf);
				}
				// through down
			case IDC_CANCEL:
				psdbd->uninitTreeView();
				::EndDialog(hDlg,0);
				break;
			default:
				return FALSE;
			}
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		if (wParam == IDC_TREE) {
			SelectDirByDlg*
				psdbd = (SelectDirByDlg*)::GetWindowLong(hDlg,DWL_USER);
			HWND hTreeView = ::GetDlgItem(hDlg,IDC_TREE);
			TVITEM* ptvItem;
			switch (((NMHDR*)lParam)->code) {
			case TVN_GETDISPINFO:
				ptvItem = &((NMTVDISPINFO*)lParam)->item;
				if (ptvItem->mask & TVIF_CHILDREN) {
					LPTVITEMINFO lptviInfo = (LPTVITEMINFO)ptvItem->lParam;
					ptvItem->cChildren
						= (lptviInfo->m_inserted & TVITEMINFO_HASSUBFOLDER) != 0;
					return TRUE;
				}
				break;
			case TVN_ITEMEXPANDING:
				if (((NMTREEVIEW*)lParam)->action == TVE_EXPAND) {
					ptvItem = &(((NMTREEVIEW*)lParam)->itemNew);
					LPTVITEMINFO lptviInfo = (LPTVITEMINFO)ptvItem->lParam;
					if (lptviInfo->m_pShellFolder == NULL)
						psdbd->addChildFolders(ptvItem->hItem,lptviInfo);
					ptvItem->cChildren
						= (lptviInfo->m_inserted & TVITEMINFO_HASSUBFOLDER) != 0;
					return TRUE;
				}
				break;
			case TVN_SELCHANGED:
				{
					DWORD attr = SFGAO_FILESYSTEM|SFGAO_READONLY|
								SFGAO_CANRENAME|SFGAO_CANDELETE;
					BOOL bCanOK = psdbd->getItemAttributes(
										((NMTREEVIEW*)lParam)->itemNew.hItem,
										attr);
					BOOL bEnableModify
							= (psdbd->getFlag()&DBPA_DISABLEMODIFY) == 0;
					::EnableWindow(::GetDlgItem(hDlg,IDC_NEWFOLDER),
									bCanOK && bEnableModify &&
									(attr&SFGAO_READONLY) == 0);
					::EnableWindow(::GetDlgItem(hDlg,IDC_RENAME),
									bCanOK && bEnableModify &&
									(attr&SFGAO_CANRENAME) != 0);
					::EnableWindow(::GetDlgItem(hDlg,IDC_DELETE),
									bCanOK && bEnableModify &&
									(attr&SFGAO_CANDELETE) != 0);
					::EnableWindow(::GetDlgItem(hDlg,IDC_OK),
									bCanOK &&
									(attr & SFGAO_FILESYSTEM) != 0);
					return TRUE;
				}
				break;
			case TVN_BEGINLABELEDIT:
				{
					TVITEM& tvItem = ((LPNMTVDISPINFO)lParam)->item;
					DWORD attr = SFGAO_CANRENAME;
					if (psdbd->getItemAttributes(
								tvItem.hItem,
								attr) &&
						(attr & SFGAO_CANRENAME) != 0) {
						psdbd->setEditMode(TRUE);
						return FALSE;
					}
					psdbd->setEditMode(FALSE);
					TreeView_EndEditLabelNow(hTreeView,TRUE);
					return TRUE;
				}
				break;
			case TVN_ENDLABELEDIT:
				{
					TVITEM& tvItem = ((LPNMTVDISPINFO)lParam)->item;
					if (tvItem.pszText == NULL) {
						psdbd->setEditMode(FALSE);
						return FALSE;
					}
					DWORD attr = SFGAO_CANRENAME;
					BOOL bRename = (psdbd->getItemAttributes(tvItem.hItem,
															attr) &&
									(attr & SFGAO_CANRENAME) != 0);
					if (bRename) {
						if (psdbd->renameFolder(tvItem)) {
							TVITEM tviNew;
							tviNew.hItem = tvItem.hItem;
							tviNew.mask = TVIF_TEXT;
							tviNew.pszText = tvItem.pszText;
							TreeView_SetItem(hTreeView,&tviNew);
						}
					}
					psdbd->setEditMode(FALSE);
					return bRename;
				}
				break;
			case NM_RCLICK:
				{
					if ((psdbd->getFlag()&DBPA_DISABLEMODIFY) != 0)
						return TRUE;
					TV_HITTESTINFO tvhtInfo;
					::GetCursorPos(&tvhtInfo.pt);
					::ScreenToClient(hTreeView,&tvhtInfo.pt);
					TreeView_HitTest(hTreeView,&tvhtInfo);
					if (tvhtInfo.hItem != NULL &&
						(tvhtInfo.flags&TVHT_ONITEM) != 0) {
						DWORD attr = SFGAO_READONLY|SFGAO_CANRENAME|
									SFGAO_CANDELETE;
						if (!psdbd->getItemAttributes(tvhtInfo.hItem,
													attr)) return TRUE;
						psdbd->setMenuState(attr); 
						::GetCursorPos(&tvhtInfo.pt);
						int ret = psdbd->showPopupMenu(tvhtInfo.pt);
						if (ret == 0) return TRUE;
						TreeView_SelectItem(hTreeView,tvhtInfo.hItem);
						switch (ret) {
						case IDM_ADDFOLDER:
							psdbd->insertNewFolder(tvhtInfo.hItem);
							break;
						case IDM_RENAME:
							TreeView_EditLabel(hTreeView,tvhtInfo.hItem);
							break;
						case IDM_DELETE:
							psdbd->deleteFolder(tvhtInfo.hItem);
							break;
						}
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

StringBuffer
SelectDirByDlg::doModal(
	HWND hWnd,
	int flags,
	const StringBuffer& title,
	const StringBuffer& inidir)
{
	DWORD oldem
			= ::SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

	m_flag = flags;
	m_Title = title;
	m_IniDir = inidir;
	m_bEditMode = FALSE;
	m_SelectDir = nullStr;
	::DialogBoxParam(m_hInstance,
					MAKEINTRESOURCE(IDD_DIRBROWSE),
					hWnd,
					(DLGPROC)DirBrowseProc,
					(LPARAM)this);

	::SetErrorMode(oldem);

	return m_SelectDir;
}

