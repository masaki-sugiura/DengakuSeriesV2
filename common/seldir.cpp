//	$Id: seldir.cpp,v 1.9 2011-01-07 16:08:38 sugiura Exp $
/*
 *	seldir.cpp
 *	ディレクトリ選択ダイアログの実装
 */

#include "seldir.h"
#include "shicons.h"
#include "tokenizer.h"
#include "resource.h"
#include "session.h"

#define IDM_ADDFOLDER 9001
#define IDM_RENAME    9002
#define IDM_DELETE    9003

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
		m_pShellIcons(new ShellIcons()),
		m_psfDesktop(LPSF_Wrapper::getDesktopFolder()),
		m_Title(nullStr),
		m_IniDir(nullStr),
		m_SelectDir(nullStr)
{
	::InitCommonControls();

	m_hImgList = ImageList_Create(m_pShellIcons->getIconSize(),
								m_pShellIcons->getIconSize(),
								ILC_COLOR24 | ILC_MASK,	//	環境依存？
								0,20);
	m_hPopupMenu = createTreePopupMenu();
	m_htiDesktop = NULL;
	m_hTreeView = NULL;
	m_bEditMode = FALSE;
}

SelectDirByDlg::~SelectDirByDlg()
{
	::DestroyMenu(m_hPopupMenu);
	ImageList_Destroy(m_hImgList);
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

#ifdef _DEBUG
static void
ShowShellErrorMsg(DWORD num, LPCSTR fname, DWORD ret, DWORD index, int size)
{
	StringBuffer msg("ShellError: #");
	msg.append(num);
	msg.append("\nfilename = ").append(fname);
	msg.append("\nretcode = ").append(ret);
	msg.append("\npselindex = ").append(index);
	if (size > 0) msg.append("\nsize = ").append(size);
	ShowErrorMsg(msg);
}
#else
#define ShowShellErrorMsg(n,f,r,i,s)  /* do nothing */
#endif

static BOOL
GetIconAndAddEntry(ShellIcons* pShellIcons,
				   int* pindex,
				   HIMAGELIST hImgList,
				   LPEI_Wrapper& peIcon,
				   DWORD flag,
				   int size)
{
	TCHAR buf[MAX_PATH];
	UINT ret;
	if (peIcon->GetIconLocation(flag,
								buf,
								MAX_PATH,
								pindex,
								&ret) != NOERROR) {
		ShowShellErrorMsg(150, buf, ret, *pindex, 0);
		return FALSE;
	}
	HICON hIcon, hSmIcon;
	if (peIcon->Extract(buf, *pindex, &hIcon, &hSmIcon, size) != NOERROR) {
		ShowShellErrorMsg(156, buf, ret, *pindex, size);
		return FALSE;
	}
	*pindex = pShellIcons->addIconEntry(hImgList, buf, *pindex, hSmIcon);
	return TRUE;
}

BOOL
SelectDirByDlg::getIconFromInterface(
	LPEI_Wrapper& peIcon,
	int* pindex,
	int* pselindex)
{
	int size = (m_pShellIcons->getIconSize() << 16) + 32;	//	環境依存？
	if (!GetIconAndAddEntry(m_pShellIcons.ptr(), pselindex, m_hImgList,
							peIcon, GIL_OPENICON, size)) return FALSE;
	if (pindex != NULL &&
		!GetIconAndAddEntry(m_pShellIcons.ptr(), pindex, m_hImgList,
							peIcon, 0, size))
		*pindex = *pselindex;
	return TRUE;
}

BOOL
SelectDirByDlg::getIconFromFileInfo(
	const LPSF_Wrapper& psfParent,
	const LPIDL_Wrapper& pidlFolder,
	int* pindex,
	int* pselindex)
{
	//	このメソッドが呼び出される可能性は、今の所 Windows2000 上で
	//	ＨＤＤのアイコンを extract する時だけ、であることを仮定する。
	//	何故なら、SHGetFileInfo はフルパスのファイル名しか受け付けないから。

	StringBuffer dispName(psfParent.getDisplayNameOf(pidlFolder,
													 SHGDN_NORMAL|SHGDN_FORPARSING
													 /* |SHGDN_FORADDRESSBAR */));
	if (dispName.length() == 0) {
		StringBuffer buf;
		buf.append("ShellError: #140");
		ShowErrorMsg(buf);
		return FALSE;
	}

	//	この時点で buf には "C:\" 等のドライブ名か、
	//	(コントロールパネルの場合？) CLSID が入っている

	SHFILEINFO shFileInfo;
	if (::SHGetFileInfo(dispName, 0,
						&shFileInfo, sizeof(SHFILEINFO),
						SHGFI_ICON|SHGFI_SMALLICON|SHGFI_OPENICON) == 0)
		return FALSE;
	*pselindex = m_pShellIcons->addIconEntry(m_hImgList, shFileInfo);

	if (pindex == NULL) return TRUE;

	if (::SHGetFileInfo(dispName, 0,
						&shFileInfo, sizeof(SHFILEINFO),
						SHGFI_ICON|SHGFI_SMALLICON/*|SHGFI_OPENICON*/) == 0) {
		*pindex = *pselindex;
		return TRUE;
	}
	*pindex = m_pShellIcons->addIconEntry(m_hImgList, shFileInfo);

	return TRUE;
}

//	フォルダに関連付けられたアイコンをイメージリストに登録し、
//	イメージリストのインデックスを返す
//	状態が１つしかないフォルダについては *pselindex に格納
BOOL
SelectDirByDlg::getIconIndex(
	const LPSF_Wrapper& lpsfParent,	//	親フォルダのインターフェイス
	const LPIDL_Wrapper& lpilFolder,	//	登録対象のサブフォルダのＩＤ
	int* pindex,				//	インデックスを格納する int へのポインタ
	int* pselindex)				//	選択状態の　〃
{
	if (pselindex == NULL) return FALSE;

	LPEI_Wrapper lpeIcon(lpsfParent.getLPEI(lpilFolder));
	if (lpeIcon != NULL &&
		getIconFromInterface(lpeIcon, pindex, pselindex)) return TRUE;
	return getIconFromFileInfo(lpsfParent, lpilFolder, pindex, pselindex);
}

LPIDL_Wrapper
SelectDirByDlg::getItemIDFromFSPath(const StringBuffer& iniDir)
{
	return m_psfDesktop.parseDisplayName(iniDir);
}

#ifdef _DEBUG
static StringBuffer
CheckDisplayName(LPSF_Wrapper& lpsfParent, LPIDL_Wrapper& lpidlChild)
{
	StringBuffer
		sname(lpsfParent.getDisplayNameOf(lpidlChild,
											SHGDN_INFOLDER |
											SHGDN_FORPARSING));
	return sname;
}
#else
#define CheckDisplayName(lpsf,lpidl)  /* no effect */
#endif

class LPIDLW_List {
public:
	LPIDLW_List*  m_next;
	LPIDL_Wrapper m_lpidlFolder;
	LPSF_Wrapper  m_lpsfFolder;
	int           m_lpidlLen;
	LPIDLW_List(LPIDL_Wrapper& lpidlFolder)
		: m_next(NULL), m_lpidlFolder(lpidlFolder), m_lpsfFolder(NULL)
	{
		m_lpidlLen = m_lpidlFolder.getListSize();
	}
	~LPIDLW_List()
	{
		if (m_next != NULL) delete m_next;
	}

	int getTotalListSize() const
	{
		const LPIDLW_List* ptr = this;
		int ret = 0;
		while (ptr != NULL) {
			ret += ptr->m_lpidlLen;
			ptr = ptr->m_next;
		}
		return ret;
	}

	void copyListTo(LPBYTE buf) const
	{
		const LPIDLW_List* ptr = this;
		while (ptr != NULL) {
			::CopyMemory(buf, (LPCITEMIDLIST&)ptr->m_lpidlFolder, ptr->m_lpidlLen);
			buf += ptr->m_lpidlLen;
			ptr = ptr->m_next;
		}
	}
};

static BOOL
FindChildFolderByName(
	LPIDLW_List* plParent,
	const StringBuffer& name,
	DWORD depth)
{
	LPSF_Wrapper& lpsfParent = plParent->m_lpsfFolder;
	if (lpsfParent == NULL) return FALSE;
	lpsfParent.initSequentialGet(SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN);
	for (;;) {
		LPIDL_Wrapper lpidlChild(lpsfParent.getNextSubFolderLPIDL());
		if (lpidlChild == NULL) break;
		CheckDisplayName(lpsfParent, lpidlChild);
		LPIDLW_List* plChild = new LPIDLW_List(lpidlChild);
		plChild->m_lpsfFolder = lpsfParent.bindTo(lpidlChild);
		if (name.compareTo(lpsfParent.getDisplayNameOf(lpidlChild,
													   SHGDN_INFOLDER |
													   SHGDN_FORPARSING),
						   FALSE) // case insensitive
			== 0 ||
			(depth > 0 &&
			 FindChildFolderByName(plChild,
								   name,
								   depth - 1))) {
			plParent->m_next = plChild;
			return TRUE;
		}
		delete plChild;
	}
	return FALSE;
}

LPIDL_Wrapper
SelectDirByDlg::getItemIDFromUNCPath(const StringBuffer& iniDir)
{
	Tokenizer tkn((LPCSTR)iniDir + 2, "\\");
	if (!tkn.hasMoreTokens()) return NULL;
	StringBuffer server(tkn.getNextToken());
	if (!tkn.hasMoreTokens()) return NULL;
	StringBuffer share(tkn.getNextToken());

	// NetworkRoot から２階層に一致する名前のサーバがあるかどうかを調べる
	// Win2K/98 なら "近くのネットワーク" 以下をサーチすることを期待しているが…
	LPIDL_Wrapper
		lpidlNetworkRoot(LPIDL_Wrapper::getSpecialFolderLPIDL(CSIDL_NETWORK));
	CheckDisplayName(m_psfDesktop, lpidlNetworkRoot);
	LPSF_Wrapper lpsfNetworkRoot(m_psfDesktop.bindTo(lpidlNetworkRoot));

	Auto_Ptr<LPIDLW_List> plParent(new LPIDLW_List(lpidlNetworkRoot));
	plParent->m_lpsfFolder = lpsfNetworkRoot;

	// Win2K/XP では「ネットワーク全体」「Microsoft Windows Network」
	// 以下に「ワークグループ/ドメイン」「サーバ名」が来る
	LPIDLW_List* plServer = plParent.ptr();

	if (FindChildFolderByName(plServer, "ネットワーク全体", 0)) {
		LPIDLW_List* plAllNetworks = plServer;
		while (plAllNetworks->m_next != NULL) {
			plAllNetworks = plAllNetworks->m_next;
		}

		if (FindChildFolderByName(plAllNetworks, "Microsoft Windows Network", 0)) {
			LPIDLW_List* plMSWinNetwork = plAllNetworks;
			while (plMSWinNetwork->m_next != NULL) {
				plMSWinNetwork = plMSWinNetwork->m_next;
			}
			plServer = plMSWinNetwork;
		}
	}
	
	if (!FindChildFolderByName(plServer,
							   server,
							   2)) {
		return NULL;
	}
	while (plServer->m_next != NULL)
		plServer = plServer->m_next;

	LPSF_Wrapper& lpsfServer = plServer->m_lpsfFolder;

	// 共有名のサーチ
#if 1

	if (!FindChildFolderByName(plServer, share, 0)) {
		return NULL;
	}
	LPIDL_Wrapper& lpidlShare = plServer->m_next->m_lpidlFolder;

#else
	// こちらの方が効率がよさそうだが何故か常に失敗する
	LPIDL_Wrapper lpidlShare = lpsfServer.parseDisplayName(share);
	if (lpidlShare == NULL) return NULL;

#endif

	// 残りのパス文字列を取得
	tkn.setSeparatorChars(nullStr);
	StringBuffer dir(tkn.getNextToken());

	// 残りのパスの IDL を取得
	LPIDL_Wrapper lpidlDirectory(NULL);
	if (dir.length() > 0) {
		LPSF_Wrapper lpsfShare(lpsfServer.bindTo(lpidlShare));
		if (lpsfShare == NULL) return NULL;
		lpidlDirectory = lpsfShare.parseDisplayName(dir);
	}

	// ITEMIDLIST の作成
	plServer = plParent.ptr();

	int lpidlLen = plServer->getTotalListSize(),
		drlen = lpidlDirectory.getListSize();
	LPITEMIDLIST
		lpidlFolder = (LPITEMIDLIST)g_SHOBJ_Allocator.allocate(
							lpidlLen + drlen + sizeof(USHORT)
						);
	if (lpidlFolder == NULL) return NULL;

	plServer = plParent.ptr();
	plServer->copyListTo((LPBYTE)lpidlFolder);

	if (drlen > 0)
		::CopyMemory(((LPBYTE)lpidlFolder) + lpidlLen,
					(LPITEMIDLIST&)lpidlDirectory, drlen);

	*(USHORT*)(((LPBYTE)lpidlFolder) + lpidlLen + drlen) = 0;

	return lpidlFolder;
}

//	ツリービューコントロールと shell 操作をあわせた関数群

//	ツリービューに項目を追加
//	結果は追加された項目のハンドル(HTREEITEM)
HTREEITEM
SelectDirByDlg::insertTreeItem(
	HTREEITEM hParent,	//	追加対象の親となる項目のハンドル
	HTREEITEM hInsertAfter,	//	追加場所を指定するハンドル
	LPSF_Wrapper& lpsfParent,	//	追加対象の親フォルダを表す IShellFolder
								//	インターフェイス
	LPIDL_Wrapper lpilFolder)	//	追加対象のサブフォルダを表す
								//	ITEMIDLIST (相対値)
{
	if (hParent == NULL || lpsfParent == NULL || lpilFolder == NULL)
		return NULL;

	ULONG mask = SFGAO_CAPABILITYMASK | SFGAO_CONTENTSMASK |
				 SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FOLDER;
	BOOL isDesktop = (hParent == TVI_ROOT);
	if (isDesktop)
		mask = SFGAO_HASSUBFOLDER;	//	desktop itself.
	else
		lpsfParent->GetAttributesOf(1, &(LPCITEMIDLIST&)lpilFolder, &mask);
	BOOL bTwoState = ((mask & SFGAO_FOLDER) != 0 &&
					  (mask & SFGAO_FILESYSTEM) != 0);

	int index = -1, selindex = -1;
	if (!getIconIndex(lpsfParent,
					  lpilFolder,
					  (bTwoState || isDesktop) ? (&index) : NULL,
					  &selindex)) {
		index = selindex = 0;
	}

	//	まだ展開されていない項目の
	//	lptviInfo->m_pShellFolder は NULL。
	//	展開された時点で自分自身の IShellFolder インターフェイスに置き換わる。
	//	また、lpilFolder の所有権はこの時点で lptviInfo のメンバオブジェクトに
	//	移行している。
	LPTVITEMINFO lptviInfo = new TVITEMINFO(
				lpsfParent.getDisplayNameOf(lpilFolder,
											SHGDN_INFOLDER),
				lpilFolder,
				(mask & SFGAO_HASSUBFOLDER) ? TVITEMINFO_HASSUBFOLDER : 0);

	TVINSERTSTRUCT is;
	is.hParent = hParent;
	is.hInsertAfter = hInsertAfter;
	is.item.mask = TVIF_CHILDREN|TVIF_IMAGE|TVIF_TEXT|TVIF_PARAM|TVIF_SELECTEDIMAGE;
	is.item.iImage = (bTwoState || isDesktop) ? index : selindex;
	is.item.iSelectedImage = isDesktop ? index : selindex;
	is.item.cChildren = I_CHILDRENCALLBACK;
	is.item.pszText = lptviInfo->m_strbuf.getBufPtr();
	is.item.cchTextMax = lptviInfo->m_strbuf.length() + 1;
	is.item.lParam = (LPARAM)lptviInfo;

	HTREEITEM	hTreeItem = TreeView_InsertItem(m_hTreeView, &is);
	if (hTreeItem != NULL && !isDesktop)
	{
		LPTVITEMINFO	ptviParent = GetLPTVITEMINFO(m_hTreeView, hParent);
		if (ptviParent != NULL)
		{
			ptviParent->m_inserted	|= TVITEMINFO_HASSUBFOLDER;
		}
	}

	return hTreeItem;
}

static const LPSF_Wrapper nullLPSF(NULL);

const LPSF_Wrapper&
SelectDirByDlg::getParentFolder(HTREEITEM hItem)
{
	HTREEITEM hParentItem = TreeView_GetParent(m_hTreeView, hItem);
	if (hParentItem == NULL) return nullLPSF;
	else if (hParentItem == TVI_ROOT) return m_psfDesktop;
	LPTVITEMINFO lptviInfo = GetLPTVITEMINFO(m_hTreeView, hParentItem);
	if (lptviInfo == NULL) return nullLPSF;
	return lptviInfo->m_pShellFolder;
}

int
SelectDirByDlg::addChildFolders(
	HTREEITEM htiParent,
	LPTVITEMINFO lptiParent)
{
	//	lptiParent->m_pShellFolder を自分自身に置き換える
	LPSF_Wrapper& lpsfParent = lptiParent->m_pShellFolder;
	if (htiParent == m_htiDesktop) {
		// デスクトップの IShellFolder の所有権を
		// lptiParent->m_pShellFolder に移行
		lpsfParent = m_psfDesktop;	//	desktop.
	} else {
		const LPSF_Wrapper& psfGrandParent = getParentFolder(htiParent);
		if (psfGrandParent == NULL) {
			return 0; // 有り得ないはず
		}
		lpsfParent = psfGrandParent.bindTo(lptiParent->m_pItemIDList);
		if (lpsfParent == NULL) {
			return 0; // これも有り得ないはず
		}
	}

	lpsfParent.initSequentialGet(SHCONTF_FOLDERS|SHCONTF_INCLUDEHIDDEN, m_hTreeView);
	int ret = 0;
	while (insertTreeItem(htiParent,
						  TVI_LAST,
						  lpsfParent,
						  lpsfParent.getNextSubFolderLPIDL()) != NULL) {
		ret++;
	}

	lptiParent->m_inserted = TRUE;
	if (ret > 0) lptiParent->m_inserted |= TVITEMINFO_HASSUBFOLDER;

	return ret;
}

StringBuffer
SelectDirByDlg::getFullPathFolderName(HTREEITEM hItem)
{
	if (!::IsWindow(m_hTreeView) || hItem == NULL) return nullStr;

	HTREEITEM	htiParent = TreeView_GetParent(m_hTreeView, hItem);
	if (htiParent != NULL && htiParent != TVI_ROOT)
	{
		LPTVITEMINFO	ptviParent = GetLPTVITEMINFO(m_hTreeView, htiParent);
		if (ptviParent != NULL)
		{
			const LPSF_Wrapper& pSFGrandParent = getParentFolder(htiParent);
			if (pSFGrandParent != NULL)
			{
				ptviParent->m_pShellFolder	= pSFGrandParent.bindTo(ptviParent->m_pItemIDList);
			}
		}
	}

	const LPSF_Wrapper& pShellFolder = getParentFolder(hItem);
	if (pShellFolder == NULL)
		return nullStr; // hItem = desktop ?

	LPTVITEMINFO lptviInfo = GetLPTVITEMINFO(m_hTreeView, hItem);

	//	ここで一度

	return pShellFolder.getDisplayNameOf(lptviInfo->m_pItemIDList,
										 SHGDN_NORMAL|SHGDN_FORPARSING);
										 /* |SHGDN_FORADDRESSBAR */
}

void
SelectDirByDlg::setInitialFolder()
{
	if (!m_IniDir.isValid()) return;

	if (!m_IniDir.isDirectory()) {
		m_IniDir.delPath(1);
		if (!m_IniDir.isValid()) return;
	}

	LPIDL_Wrapper
		lpilFolder((m_IniDir.getPathType() & ISPATHREMOTE) != 0 ?
					getItemIDFromUNCPath(m_IniDir) :
					getItemIDFromFSPath(m_IniDir));
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

	HTREEITEM htiFolder = m_htiDesktop, htiChild = NULL;
	lpilFolder.initSequentialGet();
	while (lpilFolder.hasMoreItems()) {
		LPIDL_Wrapper lpilSubFolder(lpilFolder.getNextItem());
		LPTVITEMINFO lptviInfoFolder = GetLPTVITEMINFO(m_hTreeView, htiFolder);
		if (addChildFolders(htiFolder, lptviInfoFolder) <= 0) {
			htiChild = NULL;
			break;
		}
		LPSF_Wrapper& lpsfFolder = lptviInfoFolder->m_pShellFolder;
		if (lpsfFolder == NULL) {
			htiChild = NULL;
			break;
		}
		for (htiChild = TreeView_GetChild(m_hTreeView, htiFolder);
			 htiChild != NULL;
			 htiChild = TreeView_GetNextSibling(m_hTreeView, htiChild)) {
			LPTVITEMINFO lptviInfoChild = GetLPTVITEMINFO(m_hTreeView, htiChild);
			if (lpsfFolder->CompareIDs(0,
									lptviInfoChild->m_pItemIDList,
									lpilSubFolder) == 0)
				break;
		}
		if (htiChild == NULL) {
			break;
		}
		htiFolder = htiChild;
	}
	if (htiChild != NULL) {
		TreeView_EnsureVisible(m_hTreeView, htiChild);
		TreeView_SelectItem(m_hTreeView, htiChild);
	}
}

BOOL
SelectDirByDlg::initTreeView(HWND hTreeView)
{
	m_hTreeView = hTreeView;
	setImgListToTreeView();
	if (m_flag & DBPA_DISABLEMODIFY) {
		LONG old = ::GetWindowLong(hTreeView, GWL_STYLE);
		::SetWindowLong(hTreeView, GWL_STYLE, old & ~TVS_EDITLABELS);
	}

	LPITEMIDLIST
		lpidDesktop = (LPITEMIDLIST)g_SHOBJ_Allocator.allocate(sizeof(USHORT));
	lpidDesktop->mkid.cb = 0;
	m_htiDesktop = insertTreeItem(TVI_ROOT,TVI_FIRST,
								m_psfDesktop,
								lpidDesktop);

	setInitialFolder();

	return TRUE;
}

void
SelectDirByDlg::freeResources(HTREEITEM htItem)
{
	if (htItem == NULL) return;

	for (HTREEITEM htiChild = TreeView_GetChild(m_hTreeView, htItem);
		 htiChild != NULL;
		 htiChild = TreeView_GetNextSibling(m_hTreeView, htiChild)) {
		freeResources(htiChild);
	}

	delete GetLPTVITEMINFO(m_hTreeView, htItem);
}

void
SelectDirByDlg::uninitTreeView()
{
	if (::IsWindow(m_hTreeView))
		freeResources(TreeView_GetRoot(m_hTreeView));
}

static const StringBuffer subname("新しいフォルダ");

BOOL
SelectDirByDlg::insertNewFolder(HTREEITEM hItem)
{
	// 新しく作成するフォルダの名前を生成
	StringBuffer buf(getFullPathFolderName(hItem));
	if (buf.length() == 0) return FALSE;
	if (buf.charAt(-1) == '\\') buf.setcharAt(-1, '\0');
	buf.append((TCHAR)'\\').append(subname);

	// hItem に関連付けられた TVITEMINFO の m_pShellFolder が
	// NULL でないことを保証する
	LPTVITEMINFO lptviInfo = GetLPTVITEMINFO(m_hTreeView, hItem);
	LPSF_Wrapper& pShellFolder = lptviInfo->m_pShellFolder;
	if (pShellFolder == NULL) {
		if (addChildFolders(hItem, lptviInfo) < 0 ||
			pShellFolder == NULL) return FALSE;
	}

	// フォルダを実際に作成
	if (!::CreateDirectory(buf, NULL)) return FALSE;

	LPIDL_Wrapper pidl(pShellFolder.parseDisplayName(subname));
	if (pidl == NULL) return FALSE;

	HTREEITEM hNewItem = insertTreeItem(hItem, TVI_LAST, pShellFolder, pidl);
	if (hNewItem == NULL) return FALSE;

	TreeView_EnsureVisible(m_hTreeView, hNewItem);
	TreeView_SelectItem(m_hTreeView, hNewItem);

	return TreeView_EditLabel(m_hTreeView, hNewItem) != NULL;
}

BOOL
SelectDirByDlg::renameFolder(TVITEM& tvItem)
{
	LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
	const LPSF_Wrapper& pShellFolder = getParentFolder(tvItem.hItem);
	if (pShellFolder == NULL) return FALSE;

	LPIDL_Wrapper pidlNew(pShellFolder.setNameOf(tvItem.pszText,
												 lptviInfo->m_pItemIDList,
												 SHGDN_INFOLDER|SHGDN_FORPARSING));
	if (pidlNew == NULL) return FALSE;

	lptviInfo->m_pItemIDList = pidlNew;
	lptviInfo->m_strbuf = tvItem.pszText;

	lptviInfo->m_pShellFolder	= pShellFolder.bindTo(lptviInfo->m_pItemIDList);

	return TRUE;
}

BOOL
SelectDirByDlg::deleteFolder(HTREEITEM hItem)
{
	StringBuffer buf(getFullPathFolderName(hItem));
	if (buf.length() == 0) return FALSE;

	SHFILEOPSTRUCT shFop;
	shFop.hwnd = m_hTreeView;
	shFop.wFunc = FO_DELETE;
	shFop.pFrom = buf;
	shFop.pTo = NULL;
	shFop.fFlags = FOF_SILENT;
	if (::SHFileOperation(&shFop) != 0) return FALSE;

	freeResources(hItem);

	return TreeView_DeleteItem(m_hTreeView, hItem);
}

BOOL
SelectDirByDlg::getItemAttributes(HTREEITEM hItem, DWORD& attr)
{
	LPTVITEMINFO lptviInfo = GetLPTVITEMINFO(m_hTreeView, hItem);
	LPSF_Wrapper& pShellFolder = const_cast<LPSF_Wrapper&>(getParentFolder(hItem));
	if (pShellFolder == NULL) return FALSE; // desktop

	return pShellFolder->GetAttributesOf(1,
									&(LPCITEMIDLIST&)lptviInfo->m_pItemIDList,
									&attr) == NOERROR;
}

BOOL CALLBACK
DirBrowseProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			::SetWindowLongPtr(hDlg, DWLP_USER, lParam);
			SelectDirByDlg* psdbd = (SelectDirByDlg*)lParam;
			::SetDlgItemText(hDlg, IDC_TITLE, psdbd->getTitle());
			psdbd->initTreeView(::GetDlgItem(hDlg, IDC_TREE));
			return FALSE;
		}

	case WM_COMMAND:
		{
			SelectDirByDlg*
				psdbd = (SelectDirByDlg*)::GetWindowLongPtr(hDlg, DWLP_USER);
			HWND hTreeView = ::GetDlgItem(hDlg, IDC_TREE);
			switch (LOWORD(wParam)) {
			case IDOK:
			case IDCANCEL:
				if (!psdbd->isEditMode())
					::SendMessage(hDlg,WM_COMMAND,
								LOWORD(wParam) == IDOK ? IDC_OK : IDC_CANCEL,
								0);
				else if (HIWORD(wParam) != EN_UPDATE &&
						 HIWORD(wParam) != EN_CHANGE)
					TreeView_EndEditLabelNow(hTreeView, LOWORD(wParam) != IDOK);
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
					StringBuffer
						buf(psdbd->getFullPathFolderName(
												TreeView_GetSelection(hTreeView)
											));
					if (buf.length() > 0) psdbd->setSelectDir(buf);
				}
				// through down
			case IDC_CANCEL:
				psdbd->uninitTreeView();
				::EndDialog(hDlg, 0);
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
				psdbd = (SelectDirByDlg*)::GetWindowLongPtr(hDlg, DWLP_USER);
			HWND hTreeView = ::GetDlgItem(hDlg, IDC_TREE);
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
					{
						psdbd->addChildFolders(ptvItem->hItem, lptviInfo);
					}
					if (!lptviInfo->m_sorted) {
						DWORD attr = SFGAO_FILESYSTEM;
						BOOL bCanOK = psdbd->getItemAttributes(
											ptvItem->hItem,
											attr);
						if (bCanOK && (attr & SFGAO_FILESYSTEM) != 0) {
							TreeView_SortChildren(hTreeView, ptvItem->hItem, NULL);
						}
						lptviInfo->m_sorted = TRUE;
					}
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
							= (psdbd->getFlag() & DBPA_DISABLEMODIFY) == 0;
					::EnableWindow(::GetDlgItem(hDlg, IDC_NEWFOLDER),
									bCanOK && bEnableModify &&
									(attr & SFGAO_READONLY) == 0);
					::EnableWindow(::GetDlgItem(hDlg, IDC_RENAME),
									bCanOK && bEnableModify &&
									(attr & SFGAO_CANRENAME) != 0);
					::EnableWindow(::GetDlgItem(hDlg, IDC_DELETE),
									bCanOK && bEnableModify &&
									(attr & SFGAO_CANDELETE) != 0);
					::EnableWindow(::GetDlgItem(hDlg, IDC_OK),
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
//						LPSTR buf = new TCHAR[MAX_PATH];
//						tvItem.cchTextMax = MAX_PATH;
//						lstrcpy(buf, tvItem.pszText);
//						tvItem.pszText = buf;
						psdbd->setEditMode(TRUE);
						return FALSE;
					}
					psdbd->setEditMode(FALSE);
					TreeView_EndEditLabelNow(hTreeView, TRUE);
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
					BOOL bRename = psdbd->getItemAttributes(tvItem.hItem, attr) &&
								   (attr & SFGAO_CANRENAME) != 0;
					if (bRename) {
						if (psdbd->renameFolder(tvItem)) {
							TVITEM tviNew;
							tviNew.hItem = tvItem.hItem;
							tviNew.mask = TVIF_TEXT;
							tviNew.pszText = tvItem.pszText;
							TreeView_SetItem(hTreeView, &tviNew);
						}
					}
					LPTVITEMINFO lptviInfo = (LPTVITEMINFO)tvItem.lParam;
//					delete [] tvItem.pszText;
					tvItem.pszText = lptviInfo->m_strbuf.getBufPtr();
					tvItem.cchTextMax = lptviInfo->m_strbuf.length() + 1;
					psdbd->setEditMode(FALSE);
					attr = SFGAO_FILESYSTEM;
					HTREEITEM hParent = TreeView_GetParent(hTreeView, tvItem.hItem);
					if (psdbd->getItemAttributes(hParent, attr) &&
						(attr & SFGAO_FILESYSTEM) != 0) {
						TreeView_SortChildren(hTreeView, hParent, NULL);
					}
					lptviInfo->m_sorted = TRUE;
					return bRename;
				}
				break;

			case NM_RCLICK:
				{
					if ((psdbd->getFlag() & DBPA_DISABLEMODIFY) != 0)
						return TRUE;
					TV_HITTESTINFO tvhtInfo;
					::GetCursorPos(&tvhtInfo.pt);
					::ScreenToClient(hTreeView, &tvhtInfo.pt);
					TreeView_HitTest(hTreeView, &tvhtInfo);
					if (tvhtInfo.hItem != NULL &&
						(tvhtInfo.flags & TVHT_ONITEM) != 0) {
						DWORD attr = SFGAO_READONLY|SFGAO_CANRENAME|
									SFGAO_CANDELETE;
						if (!psdbd->getItemAttributes(tvhtInfo.hItem,
													attr)) return TRUE;
						psdbd->setMenuState(attr); 
						::GetCursorPos(&tvhtInfo.pt);
						int ret = psdbd->showPopupMenu(tvhtInfo.pt);
						if (ret == 0) return TRUE;
						TreeView_SelectItem(hTreeView, tvhtInfo.hItem);
						switch (ret) {
						case IDM_ADDFOLDER:
							psdbd->insertNewFolder(tvhtInfo.hItem);
							break;
						case IDM_RENAME:
							TreeView_EditLabel(hTreeView, tvhtInfo.hItem);
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

