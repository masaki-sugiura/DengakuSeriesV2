//	$Id: seldir.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	seldir.h
 *	ディレクトリ選択ダイアログ表示関数
 */

#ifndef DENGAKUSERIES_CLASSES_SELECTDIRBYDLG
#define DENGAKUSERIES_CLASSES_SELECTDIRBYDLG

#include "strbuf.h"
#include <shlobj.h>

class ShellIcons;

#define DBPA_DISABLEMODIFY 0x1
#define TVITEMINFO_HASSUBFOLDER 0x02

typedef struct {
	LPSHELLFOLDER m_pShellFolder;
	LPITEMIDLIST m_pItemIDList;
	int m_inserted;
	TCHAR m_strbuf[MAX_PATH];
} TVITEMINFO, *LPTVITEMINFO;

class SelectDirByDlg {
public:
	SelectDirByDlg(HINSTANCE);
	~SelectDirByDlg();

	StringBuffer doModal(HWND, int, const StringBuffer&, const StringBuffer&);

	void setMenuState(DWORD);
	BOOL getFullPathFolderName(HTREEITEM,LPSTR);
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
	ShellIcons* m_pShellIcons;
	HIMAGELIST m_hImgList;
	LPMALLOC m_pMAlloc;
	LPSHELLFOLDER m_psfDesktop;
	HMENU m_hPopupMenu;
	HTREEITEM m_htiDesktop;
	HWND m_hTreeView;
	int m_flag;
	BOOL m_bEditMode;
	StringBuffer m_Title;
	StringBuffer m_IniDir;
	StringBuffer m_SelectDir;

	SelectDirByDlg(const SelectDirByDlg&);
	SelectDirByDlg& operator=(const SelectDirByDlg&);

	static HMENU createTreePopupMenu();
	static LPCITEMIDLIST getNextItemID(LPCITEMIDLIST);
	static int getIDListSize(LPCITEMIDLIST);
	void setImgListToTreeView();
	BOOL getIconFromInterface(IExtractIcon*,int*,int*);
	BOOL getIconFromFileInfo(LPSHELLFOLDER,LPCITEMIDLIST,int*,int*);
	BOOL getIconIndex(LPSHELLFOLDER,LPCITEMIDLIST,int*,int*);
	LPITEMIDLIST copyIDList(LPCITEMIDLIST);
	BOOL getDisplayNameToBuffer(const STRRET&,LPCITEMIDLIST,LPSTR);
	void getSubFolderIDList(LPSHELLFOLDER,LPCSTR,LPITEMIDLIST&);
	BOOL getItemIDFromFSPath(const StringBuffer&,int,LPITEMIDLIST&);
	BOOL getItemIDFromUNCPath(const StringBuffer&,int,LPITEMIDLIST&);
	HTREEITEM insertTreeItem(HTREEITEM,HTREEITEM,LPSHELLFOLDER,LPCITEMIDLIST);
	LPSHELLFOLDER getParentFolder(HTREEITEM);
	void setInitialFolder(const StringBuffer&);
	void freeResources(HTREEITEM);
};

#endif
