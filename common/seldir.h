//	$Id: seldir.h,v 1.2 2002-02-10 09:27:32 sugiura Exp $
/*
 *	seldir.h
 *	ディレクトリ選択ダイアログ表示関数
 */

#ifndef DENGAKUSERIES_CLASSES_SELECTDIRBYDLG
#define DENGAKUSERIES_CLASSES_SELECTDIRBYDLG

#include "shobj_wrapper.h"
#include "auto_ptr.h"
#include "pathname.h"

class ShellIcons;

#define DBPA_DISABLEMODIFY 0x1
#define TVITEMINFO_HASSUBFOLDER 0x02

class TVITEMINFO {
public:
	TVITEMINFO(const StringBuffer& str, const LPIDL_Wrapper& pidl, int inserted)
		: m_pShellFolder(NULL),
		  m_pItemIDList(pidl),
		  m_inserted(inserted),
		  m_sorted(FALSE),
		  m_strbuf(str)
	{}

	LPSF_Wrapper  m_pShellFolder;
	LPIDL_Wrapper m_pItemIDList;
	int m_inserted;
	BOOL m_sorted;
	StringBuffer m_strbuf;
};

typedef TVITEMINFO* LPTVITEMINFO;

static inline LPTVITEMINFO
GetLPTVITEMINFO(HWND hwndTreeView, HTREEITEM htItem)
{
	TVITEM tvItem;
	tvItem.hItem = htItem;
	tvItem.mask  = TVIF_PARAM;
	TreeView_GetItem(hwndTreeView, &tvItem);
	return (LPTVITEMINFO)tvItem.lParam;
}

class SelectDirByDlg {
public:
	SelectDirByDlg(HINSTANCE);
	~SelectDirByDlg();

	StringBuffer doModal(HWND, int,
						 const StringBuffer&,
						 const StringBuffer&);

	void setMenuState(DWORD);
	StringBuffer getFullPathFolderName(HTREEITEM);
	BOOL initTreeView(HWND);
	void uninitTreeView();
	int addChildFolders(HTREEITEM,LPTVITEMINFO);
	BOOL insertNewFolder(HTREEITEM);
	BOOL renameFolder(TVITEM&);
	BOOL deleteFolder(HTREEITEM);
	BOOL getItemAttributes(HTREEITEM,DWORD&);

	int showPopupMenu(POINT&);

	void setEditMode(BOOL bState) { m_bEditMode = bState; }
	BOOL isEditMode() const { return m_bEditMode; }

	int getFlag() const { return m_flag; }

	const StringBuffer& getTitle() const { return m_Title; }
	const StringBuffer& getIniDir() const { return m_IniDir; }
	StringBuffer& getSelectDir() { return m_SelectDir; }
	void setSelectDir(const StringBuffer& newdir) { m_SelectDir = newdir; }

private:
	//	getdirname で使うグローバルデータ
	HINSTANCE m_hInstance;
	Auto_Ptr<ShellIcons> m_pShellIcons;
	HIMAGELIST m_hImgList;
	LPSF_Wrapper m_psfDesktop;
	HMENU m_hPopupMenu;
	HTREEITEM m_htiDesktop;
	HWND m_hTreeView;
	int m_flag;
	BOOL m_bEditMode;
	StringBuffer m_Title;
	PathName m_IniDir; // attribute を得るため
	StringBuffer m_SelectDir;

	SelectDirByDlg(const SelectDirByDlg&);
	SelectDirByDlg& operator=(const SelectDirByDlg&);

	static HMENU createTreePopupMenu();
	void setImgListToTreeView();
	BOOL getIconFromInterface(LPEI_Wrapper&, int*, int*);
	BOOL getIconFromFileInfo(const LPSF_Wrapper&, const LPIDL_Wrapper&, int*, int*);
	BOOL getIconIndex(const LPSF_Wrapper&, const LPIDL_Wrapper&, int*, int*);
	LPIDL_Wrapper getItemIDFromFSPath(const StringBuffer&);
	LPIDL_Wrapper getItemIDFromUNCPath(const StringBuffer&);
	HTREEITEM insertTreeItem(HTREEITEM, HTREEITEM, LPSF_Wrapper&, LPIDL_Wrapper);
	const LPSF_Wrapper& getParentFolder(HTREEITEM);
	void setInitialFolder();
	void freeResources(HTREEITEM);
};

#endif
