//	$Id: rec_op.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	rec_op.cpp
 *	�ċA�t�@�C������N���X�Q
 */

#include <exception>
#include "rec_op.h"
#include "file.h"
#include "dirlist.h"
#include "auto_ptr.h"

static StringBuffer	msg = nullStr;	//	���b�Z�[�W�̃o�b�t�@

int
ConfirmOverRide(const PathName &org, const PathName &dest, DWORD flag)
{
	DWORD attr = dest.getAttributes();

	//	�㏑���悪���݂��Ȃ����ߓ��R�n�j
	if (attr == 0xFFFFFFFF) return IDYES;

	//	�t�@�C���E�f�B���N�g�������̐������`�F�b�N
	if (org.isDirectory() != ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0))
		return IDNO;

	//	�㏑���Ώۂƌ��I�u�W�F�N�g���������ǂ����̃`�F�b�N
	//	�i���F���O�̃`�F�b�N�����s���Ă��Ȃ��j
	if (!org.compareTo(dest,FALSE)) return IDNO;

	//	�S�Ă̏ꍇ�ɂ��ď㏑�����邩�ǂ����m�F
	if ((flag&FLAG_OVERRIDE_CONFIRM) != 0) {
		msg = org;
		msg.append(" ��\n").append(dest).append("��\n�㏑�����܂����H");
		return ::MessageBox(NULL,msg,"�㏑���̊m�F",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
	}

	//	�Â��t�@�C���݂̂��㏑��
	if ((flag&FLAG_OVERRIDE_NOTNEWER) != 0 &&
		::CompareFileTime(org.getTime(),dest.getTime()) <= 0) return IDNO;

	//	�����I�ɏ㏑��
	if ((flag&FLAG_OVERRIDE_FORCED) != 0) return IDYES;

	//	�ǎ��p�܂��̓V�X�e���t�@�C���̏㏑�����m�F
	if ((attr&(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM)) != 0) {
		msg = dest;
		msg.append("\n�͓ǎ��p�܂��̓V�X�e���t�@�C���ł��B\n"
				"�㏑�����܂����H");
		int	ret = ::MessageBox(NULL,msg,"�㏑���̊m�F",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
		if (ret != IDYES) return ret;	//	�㏑�����Ȃ��܂��̓L�����Z��

		//	�㏑���ł���悤�ɑ����l��ύX
		attr &= ~(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM);

		return ::SetFileAttributes(dest,attr) ? IDYES : IDNO;
	}

	return IDYES;	//	�㏑���n�j
}

int
ConfirmRemove(const PathName &file, DWORD flag)
{
	DWORD attr = file.getAttributes();

	//	�폜�Ώۂ����݂��Ȃ��̂œ��R�n�j
	if (attr == 0xFFFFFFFF) return IDYES;

	//	�S�Ă̏ꍇ�ɂ��ď㏑�����邩�ǂ����m�F
	if ((flag&FLAG_REMOVE_CONFIRM) != 0) {
		msg = file;
		msg.append(" ��\n�폜���܂����H");
		return ::MessageBox(NULL,msg,"�폜�̊m�F",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
	}

	//	�����I�ɍ폜
	if ((flag&FLAG_REMOVE_FORCED) != 0) return IDYES;

	//	�ǎ��p�܂��̓V�X�e���t�@�C���̍폜���m�F
	if ((attr&(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM)) != 0) {
		msg = file;
		msg.append(" ��\n�ǎ��p�܂��̓V�X�e���t�@�C���ł��B\n"
				"�폜���܂����H");
		int ret = ::MessageBox(NULL,msg,"�폜�̊m�F",
				MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONQUESTION|MB_SETFOREGROUND);
		if (ret != IDYES) return ret;	//	�폜���Ȃ��܂��̓L�����Z��

		//	�폜�ł���悤�ɑ����l��ύX
		attr &= ~(FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM);

		return ::SetFileAttributes(file,attr) ? IDYES : IDNO;
	}

	return IDYES;	//	�폜�n�j
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
	return RO_SUCCESS;	//	���ꂪ�Ă΂�邱�Ƃ͗L�蓾�Ȃ�!!
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

