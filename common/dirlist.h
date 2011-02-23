//	$Id: dirlist.h,v 1.2 2002/01/16 15:57:23 sugiura Exp $
/*
 *	dirlist.h
 *	�e�h���C�u���̃J�����g�f�B���N�g���̊Ǘ����s��
 */

#ifndef	DENGAKUSERIES_CLASSES_FILESYSTEM
#define	DENGAKUSERIES_CLASSES_FILESYSTEM

#include "strbuf.h"

class PathName;

//	�S�h���C�u�̃J�����g�f�B���N�g�����Ǘ�����N���X
class	DirList	{
public:
	DirList(BOOL bTrueCD = TRUE);
	~DirList();

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
