//	$Id: recfind.h,v 1.3 2002-02-15 17:46:08 sugiura Exp $
/*
 *	recfind.h
 *	enum** �֐��̂��߂̃p�X�����N���X
 */

#ifndef	DENGAKUSERIES_CLASSES_RECFINDFILE
#define	DENGAKUSERIES_CLASSES_RECFINDFILE

#include "strbuf.h"
#include "stack.h"
#include "pathname.h"
#include "enumpath.h"
#include "enumerator.h"

#define RECFIND_REVERSE		0x00001000	//	�t���Ƀ\�[�g
#define RECFIND_RECURSIVE	0x00002000	//	�ċA�I�Ɍ���
#define RECFIND_SHOWDIR		0x00004000	//	�f�B���N�g�����\��

// predeclaration
class DirList;

// �p�X�����N���X�Q�̊��N���X
class FindData : public Enumerator<StringBuffer> {
public:
	FindData(const DirList& dirlist, const PathName&);
	virtual ~FindData();

	// virtual methods of Enumerator
	BOOL isValid() const;
	BOOL findNext();
	StringBuffer getValue() const;

	DWORD getAttributes();
	DWORD getSize();
	const FILETIME*	getTime();

protected:
	const DirList& m_DirList;
	PathName m_pnPathBuf, m_pnForGetAttrib;
	StringBuffer m_sbBaseName;
	EnumSortedPath* m_pCurrentPath;	//	cannot be Auto_Ptr, see source
	BOOL m_bGotAttrib;

	FindData(const FindData&);
	FindData& operator=(const FindData&);

	BOOL setPath(const StringBuffer&, const StringBuffer&, DWORD);
};

//	�t�@�C�������̂��߂̃N���X
class FindFile : public FindData {
public:
	FindFile(
		const DirList& dirlist,
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(dirlist, path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDFILE|flags);
	}
};

//	�f�B���N�g�������̂��߂̃N���X
class FindDir : public FindData {
public:
	FindDir(
		const DirList& dirlist,
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(dirlist, path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDDIR|flags);
	}
};

//	�i�ċA�j�t�@�C���E�f�B���N�g�������̂��߂�(����)���N���X
class RecFindData : public FindData {
public:
	RecFindData(
		const DirList& dirlist,
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);
	~RecFindData();

	BOOL findNext() = 0; // to be overridden by extended classes
	StringBuffer getValue() const;

protected:
	const StringBuffer m_sDirFilter;	//	�f�B���N�g�������̃t�B���^
	const StringBuffer m_sFileFilter;	//	�t�@�C�������̃t�B���^
	const StringBuffer m_sSortOrder;	//	�\�[�g�I�[�_�[
	DWORD m_flags;	//	�f�B���N�g���������ΏۂɊ܂߂邩�ǂ���
	int	m_nIniLength;		//	m_pnPathBuf �̒����̏����l
	BOOL m_bDirFirstTime;	//	�h���N���X�ɂ���Ďg�������قȂ�̂Ń\�[�X�Q��
	Stack<EnumSortedPath> m_Stack;	//	EnumSortedPath* �̃X�^�b�N

	//	�ŏ��̌����̏�����
	BOOL initFileSearch();
	BOOL initDirSearch();
	virtual BOOL initSearch() = 0;

	//	�X�^�b�N�Ɍ����f�[�^���v�b�V��
	void pushPath();
	EnumSortedPath*	popPath();
};

class RecFindForward : public RecFindData {
public:
	RecFindForward(
		const DirList& dirlist,
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);

	BOOL findNext();

protected:
	BOOL initSearch();
};

class RecFindBackward : public RecFindData {
public:
	RecFindBackward(
		const DirList& dirlist,
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);

	BOOL findNext();

protected:
	BOOL initSearch();
};

#endif
