//	$Id: rec_op.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	rec_op.cpp
 *	再帰ファイル操作クラス群
 */

#include <exception>
#include "rec_op.h"
#include "file.h"
#include "dirlist.h"
#include "auto_ptr.h"

static StringBuffer	msg = nullStr;	//	メッセージのバッファ

int
ConfirmOverRide(const PathName &org, const PathName &dest, DWORD flag)
{
	DWORD attr = dest.getAttributes();

	//	上書き先が存在しないため当然ＯＫ
	if (attr == 0xFFFFFFFF) return IDYES;

	//	ファイル・ディレクトリ属性の整合性チェック
	if (org.isDirectory() != ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0))
		return IDNO;

	//	上書き対象と元オブジェクトが同じかどうかのチェック
	//	（注：名前のチェックしか行っていない）
	if (!org.compareTo(dest,FALSE)) return IDNO;

	//	全ての場合について上書きするかどうか確認
	if ((flag&FLAG_OVERRIDE_CONFIRM) != 0) {
		msg = org;
		msg.append(" で\n").append(dest).append("を\n上書きしますか？");
		return ::MessageBox(NULL,msg,"上書きの確認",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
	}

	//	古いファイルのみを上書き
	if ((flag&FLAG_OVERRIDE_NOTNEWER) != 0 &&
		::CompareFileTime(org.getTime(),dest.getTime()) <= 0) return IDNO;

	//	強制的に上書き
	if ((flag&FLAG_OVERRIDE_FORCED) != 0) return IDYES;

	//	読取専用またはシステムファイルの上書きを確認
	if ((attr&(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM)) != 0) {
		msg = dest;
		msg.append("\nは読取専用またはシステムファイルです。\n"
				"上書きしますか？");
		int	ret = ::MessageBox(NULL,msg,"上書きの確認",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
		if (ret != IDYES) return ret;	//	上書きしないまたはキャンセル

		//	上書きできるように属性値を変更
		attr &= ~(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM);

		return ::SetFileAttributes(dest,attr) ? IDYES : IDNO;
	}

	return IDYES;	//	上書きＯＫ
}

int
ConfirmRemove(const PathName &file, DWORD flag)
{
	DWORD attr = file.getAttributes();

	//	削除対象が存在しないので当然ＯＫ
	if (attr == 0xFFFFFFFF) return IDYES;

	//	全ての場合について上書きするかどうか確認
	if ((flag&FLAG_REMOVE_CONFIRM) != 0) {
		msg = file;
		msg.append(" を\n削除しますか？");
		return ::MessageBox(NULL,msg,"削除の確認",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
	}

	//	強制的に削除
	if ((flag&FLAG_REMOVE_FORCED) != 0) return IDYES;

	//	読取専用またはシステムファイルの削除を確認
	if ((attr&(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM)) != 0) {
		msg = file;
		msg.append(" は\n読取専用またはシステムファイルです。\n"
				"削除しますか？");
		int ret = ::MessageBox(NULL,msg,"削除の確認",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
		if (ret != IDYES) return ret;	//	削除しないまたはキャンセル

		//	削除できるように属性値を変更
		attr &= ~(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM);

		return ::SetFileAttributes(file,attr) ? IDYES : IDNO;
	}

	return IDYES;	//	削除ＯＫ
}

int
SafetyCopyFile(const PathName &orgfile, const PathName &destfile, DWORD flags)
{
	switch (ConfirmOverRide(orgfile,destfile,flags)) {
	case IDCANCEL:
		return RO_STOP;
	case IDNO:
		return RO_FAILED;
	}
	return ::CopyFile(orgfile,destfile,FALSE) ? RO_SUCCESS : RO_FAILED;
}

int
SafetyMoveFile(const PathName &orgfile, const PathName &destfile, DWORD flags)
{
	switch (ConfirmOverRide(orgfile,destfile,flags)) {
	case IDCANCEL:
		return RO_STOP;
	case IDNO:
		return RO_FAILED;
	}
	if (destfile.isValid()) {
		//	delete old dest file
		StringBuffer tmpname(destfile,-1,4);
		tmpname.append(".old");
		::MoveFile(destfile,tmpname);
		if (!orgfile.isValid()) {
			//	orgfile may same as destfile, so return RO_FAILED.
			//	NOTE:
			//		Names of them are already compared into ConfirmOverRide().
			//		Nevertheless, this verbose check should be done
			//		to avoid the case that
			//		ether of them is UNC-path and otherwise is local-path.
			::MoveFile(tmpname,destfile);
			return RO_FAILED;
		}
		::DeleteFile(tmpname);
	}
	return ::MoveFile(orgfile,destfile) ? RO_SUCCESS : RO_FAILED;
}

int
SafetyRemoveFile(const PathName &file, DWORD flags)
{
	switch (ConfirmRemove(file,flags)) {
	case IDCANCEL:
		return RO_STOP;
	case IDNO:
		return RO_FAILED;
	}
	return ::DeleteFile(file) ? RO_SUCCESS : RO_FAILED;
}

int
SafetyRemoveDirectory(const PathName &dir, DWORD flags)
{
	switch (ConfirmRemove(dir,flags)) {
	case IDCANCEL:
		return RO_STOP;
	case IDNO:
		return RO_FAILED;
	}
	return ::RemoveDirectory(dir) ? RO_SUCCESS : RO_FAILED;
}

int
SetAttributes(const PathName &file, DWORD flags)
{
	DWORD attr = file.getAttributes();
	if ((flags&ATTRIBUTE_MASK_REP) != 0) {
		attr &= FILE_ATTRIBUTE_DIRECTORY;
		attr |= (ATTRIBUTE_FLAG_ARCHIVE&flags) != 0 ?
				FILE_ATTRIBUTE_ARCHIVE : 0;
		attr |= (ATTRIBUTE_FLAG_READONLY&flags) != 0 ?
				FILE_ATTRIBUTE_READONLY : 0;
		attr |= (ATTRIBUTE_FLAG_HIDDEN&flags) != 0 ?
				FILE_ATTRIBUTE_HIDDEN : 0;
		attr |= (ATTRIBUTE_FLAG_SYSTEM&flags) != 0 ?
				FILE_ATTRIBUTE_SYSTEM : 0;
	} else {
		if ((flags&(ATTRIBUTE_MASK_DEL&ATTRIBUTE_FLAG_ARCHIVE)) != 0)
			attr &= ~FILE_ATTRIBUTE_ARCHIVE;
		if ((flags&(ATTRIBUTE_MASK_DEL&ATTRIBUTE_FLAG_READONLY)) != 0)
			attr &= ~FILE_ATTRIBUTE_READONLY;
		if ((flags&(ATTRIBUTE_MASK_DEL&ATTRIBUTE_FLAG_HIDDEN)) != 0)
			attr &= ~FILE_ATTRIBUTE_HIDDEN;
		if ((flags&(ATTRIBUTE_MASK_DEL&ATTRIBUTE_FLAG_SYSTEM)) != 0)
			attr &= ~FILE_ATTRIBUTE_SYSTEM;
		if ((flags&(ATTRIBUTE_MASK_ADD&ATTRIBUTE_FLAG_ARCHIVE)) != 0)
			attr |= FILE_ATTRIBUTE_ARCHIVE;
		if ((flags&(ATTRIBUTE_MASK_ADD&ATTRIBUTE_FLAG_READONLY)) != 0)
			attr |= FILE_ATTRIBUTE_READONLY;
		if ((flags&(ATTRIBUTE_MASK_ADD&ATTRIBUTE_FLAG_HIDDEN)) != 0)
			attr |= FILE_ATTRIBUTE_HIDDEN;
		if ((flags&(ATTRIBUTE_MASK_ADD&ATTRIBUTE_FLAG_SYSTEM)) != 0)
			attr |= FILE_ATTRIBUTE_SYSTEM;
	}
	return ::SetFileAttributes(file,attr) ? RO_SUCCESS : RO_FAILED;
}

int
SetTime(const PathName &file, const FILETIME *pft, DWORD flags)
{
	if (!file.isValid() && (flags&TOUCH_FILECREATE) == 0) return RO_FAILED;
	File fFile(file,GENERIC_WRITE,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL);
	return (fFile.isValid() &&
			fFile.setFileTime(
							NULL,
							(flags&TOUCH_ACCESSTIME) ? pft : NULL,
							(flags&TOUCH_MODIFYTIME) ? pft : NULL)) ?
			RO_SUCCESS : RO_FAILED;
}

int
CopyPath(const PathName &file, const PathName &dest, DWORD flags)
{
	DWORD attr = file.getAttributes();
	if (attr == 0xFFFFFFFF) return RO_FAILED;
	if ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0) {
		if ((flags&FLAG_RECURSIVE) == 0) return RO_FAILED;
		RecursiveCopyDir rcd(file,dest,flags);
		return rcd.doOperation();
	}
	return SafetyCopyFile(file,dest,flags);
}

int
MovePath(const PathName &file, const PathName &dest, DWORD flags)
{
	DWORD attr = file.getAttributes();
	if (attr == 0xFFFFFFFF) return RO_FAILED;
	if ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0) {
		if ((flags&FLAG_RECURSIVE) == 0) return RO_FAILED;
		if (file[0] != dest[0] || !ISDRIVELETTER(file[0])) {
			RecursiveMoveDir rmd(file,dest,flags);
			return rmd.doOperation();
		}
	}
	return SafetyMoveFile(file,dest,flags);
}

int
RemovePath(const PathName &file, DWORD flags)
{
	DWORD attr = file.getAttributes();
	if (attr == 0xFFFFFFFF) return RO_FAILED;
	if ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0) {
		if ((flags&FLAG_RECURSIVE) == 0) return RO_FAILED;
		RecursiveRemoveDir rmd(file,flags);
		return rmd.doOperation();
	}
	return SafetyRemoveFile(file,flags);
}

int
SetPathAttributes(const PathName &file, DWORD flags)
{
	if (file.isDirectory()) {
		if ((flags&ATTRIBUTE_RECURSIVE) == 0) return RO_FAILED;
		RecursiveSetAttributes rsa(file,flags);
		return rsa.doOperation();
	}
	return SetAttributes(file,flags);
}

int
SetPathTime(const PathName &file, const FILETIME *pft, DWORD flags)
{
	if (file.isDirectory()) {
		if ((flags&TOUCH_RECURSIVE) == 0) return RO_FAILED;
		RecursiveSetTime rst(file,pft,flags);
		return rst.doOperation();
	}
	return SetTime(file,pft,flags);
}

int
RecursiveMakeDir(const PathName &dir)
{
	int len = dir.length();
	LPSTR buf = NULL;
	try {
		buf = new TCHAR[len + 1];
	} catch (exception&) {
		return RO_FAILED;
	}
	::CopyMemory(buf,(LPCSTR)dir,len + 1);

	LPSTR lpsep = lstrchr(buf,'\\');
	if (lpsep == NULL) {
		delete [] buf;
		return RO_FAILED;
	}
	for (lpsep = lstrchr(++lpsep,'\\');
		lpsep != NULL;
		lpsep = lstrchr(++lpsep,'\\')) {
		*lpsep = '\0';
		DWORD attr = ::GetFileAttributes(buf);
		if (attr == 0xFFFFFFFF) {
			if (!::CreateDirectory(buf,NULL)) {
				delete [] buf;
				return RO_FAILED;
			}
		} else if ((attr&FILE_ATTRIBUTE_DIRECTORY) == 0) {
			delete [] buf;
			return RO_FAILED;
		}
		*lpsep = '\\';
	}
	if (::GetFileAttributes(buf) == 0xFFFFFFFF) {
		if (!::CreateDirectory(buf,NULL)) {
			delete [] buf;
			return RO_FAILED;
		}
	}

	delete [] buf;

	return RO_SUCCESS;
}

RecursiveOperation::RecursiveOperation(
	const PathName &orgfile,
	const PathName &destdir,
	DWORD flag)
	:	m_fConfirm(flag),
		m_OrgPathBuf(orgfile),
		m_DestDirBuf(destdir),
		m_pFindData(new WIN32_FIND_DATA)
{
	//	nothing to do more.
}

RecursiveOperation::~RecursiveOperation()
{
	delete m_pFindData;
}

int RecursiveOperation::doOperation()
{
	return this->recursiveDirOp();
}

int RecursiveOperation::recursiveDirOp()
{
	if (!m_OrgPathBuf.isValid()) return RO_FAILED;

	int	ret;
	if (!m_OrgPathBuf.isDirectory()) {
		ret = this->FiletoFile();
	} else {
		if ((ret = this->preDirOp()) != RO_SUCCESS) return ret;
		m_OrgPathBuf.addPath(anyPathName);
		HANDLE hFindFile = ::FindFirstFile(m_OrgPathBuf,m_pFindData);
		LPCSTR name;
		do {
			name = m_pFindData->cFileName;
			if (IsPathNameDots(name)) continue;
			m_OrgPathBuf.setBaseName(name);
			m_DestDirBuf.addPath(name);
			ret = this->recursiveDirOp();
			m_DestDirBuf.delPath(1);
		} while ((ret&RO_STOP) == 0 && ::FindNextFile(hFindFile,m_pFindData));
		::FindClose(hFindFile);
		m_OrgPathBuf.delPath(1);
		if ((ret&RO_STOP) == 0) ret |= this->postDirOp();
	}
	return ret;
}

int
RecursiveOperation::preDirOp()
{
	return RO_SUCCESS;
}

int
RecursiveOperation::postDirOp()
{
	return RO_SUCCESS;
}

int
RecursiveOperation::FiletoFile()
{
#ifdef _DEBUG
	StringBuffer msgbuf("Operation is done as:\nFrom: ",-1,2*MAX_PATH);
	msgbuf.append(m_OrgPathBuf).append("\nTo: ").append(m_DestDirBuf);
	::MessageBox(NULL,msgbuf,NULL,MB_OK);
	return RO_SUCCESS;
#else
	return RO_SUCCESS;	//	これが呼ばれることは有り得ない!!
#endif
}

RecursiveCopyDir::RecursiveCopyDir(
	const PathName& orgdir,
	const PathName& destdir,
	DWORD flags)
	: RecursiveOperation(orgdir,destdir,flags)
{
	//	nothing to do here.
}

int
RecursiveCopyDir::preDirOp()
{
	if (m_DestDirBuf.isValid())
		return m_DestDirBuf.isDirectory() ? RO_SUCCESS : RO_FAILED;
	return ::CreateDirectory(m_DestDirBuf,NULL) ? RO_SUCCESS : RO_FAILED;
}

int
RecursiveCopyDir::FiletoFile()
{
	return SafetyCopyFile(m_OrgPathBuf,m_DestDirBuf,m_fConfirm);
}

RecursiveMoveDir::RecursiveMoveDir(
	const PathName& orgdir,
	const PathName& destdir,
	DWORD flag)
	: RecursiveOperation(orgdir,destdir,flag)
{
	//	nothing to do here.
}

int
RecursiveMoveDir::preDirOp()
{
	if (m_DestDirBuf.isValid())
		return m_DestDirBuf.isDirectory() ? RO_SUCCESS : RO_FAILED;
	return ::CreateDirectory(m_DestDirBuf,NULL) ? RO_SUCCESS : RO_FAILED;
}

int
RecursiveMoveDir::postDirOp()
{
	if (!m_OrgPathBuf.isValid()) return RO_SUCCESS;
	if (!m_OrgPathBuf.isDirectory()) return RO_FAILED;
	return SafetyRemoveDirectory(m_OrgPathBuf,0) ? RO_SUCCESS : RO_FAILED;
}

int
RecursiveMoveDir::FiletoFile()
{
	return SafetyMoveFile(m_OrgPathBuf,m_DestDirBuf,m_fConfirm);
}

RecursiveRemoveDir::RecursiveRemoveDir(
	const PathName& destdir,
	DWORD flags)
	: RecursiveOperation(destdir,destdir,flags)
{
	//	nothing to do here.
}

int
RecursiveRemoveDir::postDirOp()
{
	if (!m_OrgPathBuf.isValid()) return RO_SUCCESS;
	if (!m_OrgPathBuf.isDirectory()) return RO_FAILED;
	return SafetyRemoveDirectory(m_OrgPathBuf,m_fConfirm);
}

int
RecursiveRemoveDir::FiletoFile()
{
	return SafetyRemoveFile(m_OrgPathBuf,m_fConfirm);
}

RecursiveSetAttributes::RecursiveSetAttributes(
	const PathName& file,
	DWORD flags)
	: RecursiveOperation(file,file,flags)
{
	//	nothing to do here.
}

int
RecursiveSetAttributes::postDirOp()
{
	return SetAttributes(m_OrgPathBuf,m_fConfirm);
}

int
RecursiveSetAttributes::FiletoFile()
{
	return SetAttributes(m_OrgPathBuf,m_fConfirm);
}

RecursiveSetTime::RecursiveSetTime(
	const PathName& file,
	const FILETIME* pft,
	DWORD flags)
	: RecursiveOperation(file,file,flags)
{
	::CopyMemory(&m_ft,pft,sizeof(FILETIME));
}

int
RecursiveSetTime::FiletoFile()
{
	return SetTime(m_OrgPathBuf,&m_ft,m_fConfirm);
}

