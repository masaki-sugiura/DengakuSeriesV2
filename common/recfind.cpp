//	$Id: recfind.cpp,v 1.4 2002-03-05 14:19:35 sugiura Exp $
/*
 *	recfind.cpp
 *	enum** �n�̊֐��̂��߂̃N���X
 */

#include "recfind.h"
#include "dirlist.h"

static const StringBuffer nullPath = ".";

FindData::FindData(const DirList& dirlist, const PathName& path)
	:	m_DirList(dirlist),
		m_pnPathBuf(path),
		m_sbBaseName(nullStr),
		m_pCurrentPath(NULL),
		m_bGotAttrib(FALSE)
{
	//	no operation.
	m_pnPathBuf.addPath(nullPath);
}

FindData::~FindData()
{
	delete m_pCurrentPath;
}

BOOL
FindData::isValid() const
{
	return m_pCurrentPath != NULL ? m_pCurrentPath->isValid() : FALSE;
}

BOOL
FindData::findNext()
{
	if (!this->isValid()) return FALSE;
	m_pnPathBuf.delPath(1);
	m_sbBaseName = m_pCurrentPath->getNext();
	if (m_sbBaseName.length() <= 0) {
		m_pnPathBuf.addPath(nullPath);
		m_sbBaseName = nullStr;
		return FALSE;
	}
	m_pnPathBuf.addPath(m_sbBaseName);
	m_bGotAttrib = FALSE;
	return TRUE;
}

DWORD
FindData::getAttributes()
{
	if (!m_bGotAttrib) {
		m_DirList.getPathName(m_pnPathBuf, m_pnForGetAttrib, TRUE);
		m_bGotAttrib = TRUE;
	}
	return m_pnForGetAttrib.getAttributes();
}

DWORD
FindData::getSize()
{
	if (!m_bGotAttrib) {
		m_DirList.getPathName(m_pnPathBuf, m_pnForGetAttrib, TRUE);
		m_bGotAttrib = TRUE;
	}
	return m_pnForGetAttrib.getSize();
}

const FILETIME*
FindData::getTime()
{
	if (!m_bGotAttrib) {
		m_DirList.getPathName(m_pnPathBuf, m_pnForGetAttrib, TRUE);
		m_bGotAttrib = TRUE;
	}
	return m_pnForGetAttrib.getTime();
}

StringBuffer
FindData::getValue() const
{
	return m_sbBaseName;
}

BOOL
FindData::setPath(
	const StringBuffer& filter,
	const StringBuffer& s_order,
	DWORD flags)
{
	if (m_pCurrentPath != NULL) delete m_pCurrentPath;
	try {
		if (nullPath.compareTo(m_pnPathBuf.getBaseName()) == 0) {
			m_pnPathBuf.delPath(1);
			m_pCurrentPath = new EnumSortedPath(m_pnPathBuf,filter,s_order,flags);
			m_pnPathBuf.addPath(nullPath);
		} else {
			m_pCurrentPath = new EnumSortedPath(m_pnPathBuf,filter,s_order,flags);
		}
		if (this->isValid()) {
			if (m_pCurrentPath->doEnum() <= 0) return FALSE;
			return TRUE;
		}
	} catch (InvalidArgumentException&) {
		//	do nothing...
	}
	delete m_pCurrentPath;
	m_pCurrentPath = NULL;
	return FALSE;
}


RecFindData::RecFindData(
	const DirList& dirlist,			//	�J�����g�t�H���_���̏��
	const PathName& dir,			//	�����p�X
	const StringBuffer& dirfilter,	//	�f�B���N�g�������̃t�B���^
	const StringBuffer& filefilter,	//	�t�@�C�������̃t�B���^
	const StringBuffer& s_order,	//	�\�[�g�I�[�_�[
	DWORD flags)					//	�����t���O
	:	FindData(dirlist, dir),
		m_sDirFilter(dirfilter),
		m_sFileFilter(filefilter),
		m_sSortOrder(s_order),
		m_flags(flags),
		m_nIniLength(m_pnPathBuf.length() - nullPath.length()),
		m_bDirFirstTime(TRUE),
		m_Stack(MAX_PATH/3)
{
	//	no operation.
}

RecFindData::~RecFindData()
{
	EnumSortedPath* pesp;
	while ((pesp = m_Stack.pop()) != NULL) delete pesp;
}

void
RecFindData::pushPath()
{
	if (m_pCurrentPath != NULL) {
		m_Stack.push(m_pCurrentPath);
		m_pCurrentPath = NULL;
	}
	m_pnPathBuf.addPath(nullPath);
	m_sbBaseName = nullStr;
}

EnumSortedPath*
RecFindData::popPath()
{
	m_pnPathBuf.delPath(1);
	m_sbBaseName = m_pnPathBuf.getBaseName();
	if (m_pCurrentPath != NULL) delete m_pCurrentPath;
	return m_pCurrentPath = m_Stack.pop();
}

StringBuffer
RecFindData::getValue() const
{
	if (m_pCurrentPath == NULL) return nullStr;
//	if (m_pnPathBuf.length() == m_nIniLength)
//		return m_sbBaseName;
//	else 
//	if (m_pnPathBuf[m_nIniLength - 1] == '\\')
//		return m_pnPathBuf.extract(m_nIniLength,-1);
//				.append((TCHAR)'\\').append(m_sbBaseName);
//	else
	return m_pnPathBuf.extract(m_nIniLength,-1);
//				.append((TCHAR)'\\').append(m_sbBaseName);
}

BOOL
RecFindData::initFileSearch()
{
	m_bDirFirstTime = FALSE;
	return	this->setPath(m_sFileFilter,
						m_sSortOrder,
						ENUMPATH_FINDFILE|m_flags);
}

BOOL
RecFindData::initDirSearch()
{
	m_bDirFirstTime = TRUE;
	return	this->setPath(m_sDirFilter,
						m_sSortOrder,
						ENUMPATH_FINDDIR|ENUMPATH_EXCLUDEDOTS|m_flags);
}

RecFindForward::RecFindForward(
	const DirList& dirlist,			//	�J�����g�t�H���_���̏��
	const PathName& dir,			//	�����p�X
	const StringBuffer& dirfilter,	//	�f�B���N�g�������̃t�B���^
	const StringBuffer& filefilter,	//	�t�@�C�������̃t�B���^
	const StringBuffer& s_order,	//	�\�[�g�I�[�_�[
	DWORD flags)					//	�f�B���N�g���������Ώۂɂ��邩
	: RecFindData(dirlist,dir,dirfilter,filefilter,s_order,flags)
{
	initSearch();
}

BOOL
RecFindForward::initSearch()
{
	return this->initFileSearch() || this->initDirSearch();
}

BOOL
RecFindForward::findNext()
{
	if (!isValid()) return FALSE;
	if ((m_pCurrentPath->getFlag()&ENUMPATH_FINDFILE) != 0) {
		//	�t�@�C���̌���
		if (this->FindData::findNext()) return TRUE;
		//	�t�@�C���̌����͏I��
		//	�����K�w�̃f�B���N�g����
		if (this->initDirSearch() ||
			//	�f�B���N�g���͑��݂��Ȃ� -> ���̊K�w�ɖ߂�
			this->popPath() != NULL) {
			//	�ċA�ďo��
			m_bDirFirstTime = TRUE;
			return this->findNext();
		}
	} else {
		if (m_bDirFirstTime) {
			m_bDirFirstTime = FALSE;
			//	m_pCurrentPath �Ƀf�B���N�g���̃f�[�^���Z�b�g
			if (!this->FindData::findNext()) {
				//	�����f�B���N�g���͂Ȃ�
				m_bDirFirstTime = TRUE;	//	���̊K�w�̃f�[�^��
										//	�K���f�B���N�g���̃f�[�^�ł���
				if (this->popPath() != NULL)	//	���̊K�w��
					return this->findNext(); // �܂�����܂�
				else
					return FALSE;	//	���������܂�
			} else {
				//	�f�B���N�g����\������H
				if ((m_flags&RECFIND_SHOWDIR) != 0) return TRUE;
				//	through down.
			}
		}
		//	�f�B���N�g���K�w������
		this->pushPath();
		if (this->initSearch() || // �t�@�C���E�܂��̓f�B���N�g��
			this->popPath() != NULL) { // �f�B���N�g���͋� -> ����
			return this->findNext();	//	�t�@�C���E�f�B���N�g���̌�����
		}
	}
	return FALSE;	//	�S�Ă̌����̏I��
}

RecFindBackward::RecFindBackward(
	const DirList& dirlist,			//	�J�����g�t�H���_���̏��
	const PathName& dir,			//	�����p�X
	const StringBuffer& dirfilter,	//	�f�B���N�g�������̃t�B���^
	const StringBuffer& filefilter,	//	�t�@�C�������̃t�B���^
	const StringBuffer& s_order,	//	�\�[�g�I�[�_�[
	DWORD flags)					//	�f�B���N�g���������Ώۂɂ��邩
	: RecFindData(dirlist,dir,dirfilter,filefilter,s_order,
			ENUMPATH_SORTREVERSE|RECFIND_REVERSE|flags)
{
	initSearch();
}

BOOL
RecFindBackward::initSearch()
{
	return this->initDirSearch() || this->initFileSearch();
}

BOOL
RecFindBackward::findNext()
{
	if (!isValid()) return FALSE;
	if (m_pCurrentPath->getFlag()&ENUMPATH_FINDFILE) {
		//	�t�@�C���̌���
		if (this->FindData::findNext()) return TRUE;
		//	�t�@�C���̌����͏I��
		if (this->popPath() != NULL) {	//	���̊K�w�̃f�B���N�g����
			m_bDirFirstTime = FALSE;	//	pop ���ꂽ�f�B���N�g�����Z�b�g
			return this->findNext();	//	�f�B���N�g���̌�����
		}
	} else {
		//	�f�B���N�g���̌���
		if ((m_flags&RECFIND_SHOWDIR) != 0 && !m_bDirFirstTime) {
			//	�f�B���N�g���̃f�[�^���c��
			m_bDirFirstTime = TRUE;
			return TRUE;
		}
		if (this->FindData::findNext()) {
			//	�f�B���N�g�����������炷���ɂ��̉�������
			this->pushPath();
			if (this->initSearch() ||
				this->popPath() != NULL) {
				return this->findNext(); // �f�B���N�g���E�t�@�C���̌�����
			}
		} else {
			if (this->initFileSearch() ||	//	�t�@�C�����܂�����
				this->popPath() != NULL) {	//	�����Ȃ��̂ň��̊K�w��
				m_bDirFirstTime = FALSE;	//	pop ���ꂽ�f�B���N�g�����Z�b�g
				return this->findNext();	//	�f�B���N�g���̌�����
			}
		}
	}
	return FALSE;	//	�S�Ă̌����̏I��
}

