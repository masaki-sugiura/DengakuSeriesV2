//	$Id: dirlist.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	dirlist.h
 *	�e�h���C�u���̃J�����g�f�B���N�g���̊Ǘ����s��
 */

#ifndef	DENGAKUSERIES_CLASSES_FILESYSTEM
#define	DENGAKUSERIES_CLASSES_FILESYSTEM

#include "strbuf.h"

class PathName;

#define	ISPATHINVALID			0x00000000	/* �����ȃp�X�� */

#define	ISPATHSPECIFIED			0x00000001	/* ...dir1\dir2\foo */
#define	ISPATHFROMROOT			0x00000002	/* [header]\dir1\dir2\foo */
#define	ISDRIVESPECIFIED		0x00000010	/* X:... */
#define	ISSERVERSPECIFIED		0x00000020	/* \\computer... */
#define	ISSHARENAMESPECIFIED	0x00000040	/* \\computer\share... */

#define	ISPATHLOCAL				0x00000100	/* X:\.. �^�̃p�X */
#define	ISPATHREMOTE			0x00000200	/* \\computer\share\.. �^�̃p�X */

#define	INVALIDPATH		ISPATHINVALID
#define	ABSOLUTEPATH	(ISPATHLOCAL|ISDRIVESPECIFIED|ISPATHSPECIFIED|ISPATHFROMROOT)
#define	DRIVERELPATH	(ISPATHLOCAL|ISDRIVESPECIFIED|ISPATHSPECIFIED)
#define	ROOTPATH		(ISPATHLOCAL|ISPATHSPECIFIED|ISPATHFROMROOT)
#define	RELPATH			(ISPATHLOCAL|ISPATHSPECIFIED)


inline int
DRIVENUM(TCHAR ch)
{
	return IsCharLowerCase(ch) ? (ch - 'a' + 1) : (ch - 'A' + 1);
}

inline BOOL
ISDRIVELETTER(TCHAR ch)
{
	return IsCharAlphabet(ch);
}

//	�S�h���C�u�̃J�����g�f�B���N�g�����Ǘ�����N���X
class	DirList	{
public:
	DirList(BOOL bTrueCD = TRUE);
	~DirList();

	//	�^����ꂽ�p�X�̎�ނ�Ԃ�
	static int getPathType(const StringBuffer&);

	//	�^����ꂽ�p�X�����t���p�X���ɕϊ�����
	BOOL getPathName(const StringBuffer& name,
					PathName& buf,
					BOOL bChkValid = FALSE) const;

	//	�J�����g�f�B���N�g���̎擾
	LPCSTR getCurrentDir(int drive = 0) const;
	//	�J�����g�f�B���N�g���̕ύX
	LPCSTR setCurrentDir(const StringBuffer& dirname);

private:
	StringBuffer* m_ppsbCurDir[32];	//	�J�����g�f�B���N�g���̔z��
	int m_nCurDrive; //	�J�����g�h���C�u�̃C���f�b�N�X
	BOOL m_bTrueCD; // setCurrentDir() �Ŏ��ۂɃf�B���N�g�����ړ����邩

	DirList(const DirList&);
	DirList& operator=(const DirList&);

	//	���΃p�X����������
	static BOOL resolveRelPath(int, const StringBuffer&, PathName&);
};

#endif
