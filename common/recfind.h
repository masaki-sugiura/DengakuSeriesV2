//	$Id: recfind.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
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

#define RECFIND_REVERSE		0x00001000	//	�t���Ƀ\�[�g
#define RECFIND_RECURSIVE	0x00002000	//	�ċA�I�Ɍ���
#define RECFIND_SHOWDIR		0x00004000	//	�f�B���N�g�����\��

// �p�X�����N���X�Q�̊��N���X
class FindData {
public:
	FindData(const PathName&);
	virtual ~FindData();

	BOOL isValid() const;
	virtual BOOL findNext();

	virtual StringBuffer getBaseName() const;

	DWORD getAttributes()
	{
		return m_pnPathBuf.getAttributes();
	}

	DWORD getSize()
	{
		return m_pnPathBuf.getSize();
	}

	const FILETIME*	getTime()
	{
		return m_pnPathBuf.getTime();
	}

protected:
	PathName m_pnPathBuf;
	StringBuffer m_sbBaseName;
	EnumSortedPath* m_pCurrentPath;	//	cannot be Auto_Ptr, see source

	FindData(const FindData&);
	FindData& operator=(const FindData&);

	BOOL setPath(const StringBuffer&, const StringBuffer&, DWORD);
};

//	�t�@�C�������̂��߂̃N���X
class FindFile : public FindData {
public:
	FindFile(
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDFILE|flags);
	}
};

//	�f�B���N�g�������̂��߂̃N���X
class FindDir : public FindData {
public:
	FindDir(
		const PathName& path,
		const StringBuffer& filter,
		const StringBuffer& s_order,
		DWORD flags)
		: FindData(path)
	{
		this->setPath(filter,s_order,ENUMPATH_FINDDIR|flags);
	}
};

//	�i�ċA�j�t�@�C���E�f�B���N�g�������̂��߂�(����)���N���X
class RecFindData : public FindData {
public:
	RecFindData(
		const PathName&,
		const StringBuffer&,
		const StringBuffer&,
		const StringBuffer&,
		DWORD);
	~RecFindData();

	BOOL findNext() = 0;
	StringBuffer getBaseName() const;

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
