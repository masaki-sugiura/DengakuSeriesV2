//	$Id: rec_op.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	rec_op.h
 *	�ċA�t�@�C������̂��߂̃N���X
 */

#ifndef	DENGAKUSERIES_CLASSES_RECURSIVEOP
#define	DENGAKUSERIES_CLASSES_RECURSIVEOP

#include "pathname.h"

//	���N���X
class RecursiveOperation {
public:
	RecursiveOperation(const PathName&, const PathName&, DWORD);
	virtual	~RecursiveOperation();

	int	doOperation();

protected:
	DWORD m_fConfirm;
	PathName m_OrgPathBuf;
	PathName m_DestDirBuf;
	LPWIN32_FIND_DATA m_pFindData;

	RecursiveOperation(const RecursiveOperation&);
	RecursiveOperation& operator=(const RecursiveOperation&);

	virtual	int	recursiveDirOp();
	virtual	int	preDirOp();
	virtual	int	postDirOp();
	virtual	int	FiletoFile();
};

class RecursiveCopyDir : public RecursiveOperation {
public:
	RecursiveCopyDir(const PathName&, const PathName&, DWORD);

protected:
	int	preDirOp();
	int	FiletoFile();
};

class RecursiveMoveDir : public RecursiveOperation {
public:
	RecursiveMoveDir(const PathName&, const PathName&, DWORD);

protected:
	int	preDirOp();
	int	postDirOp();
	int	FiletoFile();
};

class RecursiveRemoveDir : public RecursiveOperation {
public:
	RecursiveRemoveDir(const PathName&, DWORD);

protected:
	int	postDirOp();
	int	FiletoFile();
};

class RecursiveSetAttributes : public RecursiveOperation {
public:
	RecursiveSetAttributes(const PathName&, DWORD);

protected:
	int	postDirOp();
	int	FiletoFile();
};

class RecursiveSetTime : public RecursiveOperation {
public:
	RecursiveSetTime(const PathName&, const FILETIME*, DWORD);

protected:
	FILETIME m_ft;
	int	FiletoFile();
};

//	�ċA�����n�̊֐��̕Ԃ�l
#define	RO_SUCCESS	0	/* ���� */
#define	RO_FAILED	1	/* �����̎��s */
#define	RO_STOP		2	/* �����̒�~�v�� */

//	�t�@�C���n�̊֐��̓���t���O
//		�㏑�����̓���
#define	FLAG_OVERRIDE_DEFAULT	0x00000000	/* �f�t�H���g�̓��� */
#define	FLAG_OVERRIDE_CONFIRM	0x00000001	/* �S�Ăɂ��Ċm�F */
#define	FLAG_OVERRIDE_NOTNEWER	0x00000002	/* �Â��t�@�C���̂ݏ㏑�� */
#define	FLAG_OVERRIDE_FORCED	0x00000004	/* �����I�ɏ㏑�� */
//		�폜���̓���
#define	FLAG_REMOVE_DEFAULT		0x00000000	/* �f�t�H���g�̓��� */
#define	FLAG_REMOVE_CONFIRM		0x00000100	/* �S�Ăɂ��Ċm�F */
#define	FLAG_REMOVE_FORCED		0x00000200	/* �����I�ɍ폜 */
//		�f�B���N�g�����ċA�I�ɏ���
#define	FLAG_RECURSIVE			0x80000000	/* �ċA�����̎��s */

//	�����l�t���O
#define	ATTRIBUTE_FLAG_ARCHIVE		0x00010101	/* �A�[�J�C�u */
#define	ATTRIBUTE_FLAG_READONLY		0x00020202	/* �ǎ��p */
#define	ATTRIBUTE_FLAG_HIDDEN		0x00040404	/* �B�� */
#define	ATTRIBUTE_FLAG_SYSTEM		0x00080808	/* �V�X�e�� */
#define	ATTRIBUTE_RECURSIVE			0x80000000	/* �ċA�����̎��s */
//	�����l�}�X�N
#define	ATTRIBUTE_MASK_ADD			0x000000FF	/* �ǉ� */
#define	ATTRIBUTE_MASK_DEL			0x0000FF00	/* �폜 */
#define	ATTRIBUTE_MASK_REP			0x00FF0000	/* �u�� */

//	�^�C���X�^���v�ύX�t���O
#define	TOUCH_ACCESSTIME	0x00000001
#define	TOUCH_MODIFYTIME	0x00000002
#define	TOUCH_FILECREATE	0x00000004
#define	TOUCH_RECURSIVE		0x80000000

int	ConfirmOverRide(const PathName &org, const PathName &dest, DWORD flag);
int	ConfirmRemove(const PathName &org, DWORD flag);
int	SafetyCopyFile(const PathName &org, const PathName &dest, DWORD flag);
int	SafetyMoveFile(const PathName &org, const PathName &dest, DWORD flag);
int	SafetyRemoveFile(const PathName &file, DWORD flag);
int	SafetyRemoveDirectory(const PathName &dir, DWORD flag);
int	CopyPath(const PathName &org, const PathName &dest, DWORD flag);
int	MovePath(const PathName &org, const PathName &dest, DWORD flag);
int	RemovePath(const PathName &file, DWORD flag);
int	SetPathAttributes(const PathName &file, DWORD flag);
int	SetPathTime(const PathName &file, const FILETIME *ft, DWORD flag);
int	RecursiveMakeDir(const PathName &dir);

#endif