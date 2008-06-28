// HmJre.dll copyright(c) 2003 �L����ЃT�C�g�[���
// written by �ē��G�v
// �Q�lURL  http://www.labyrinth.net.au/~dbareis/3rdpsrc.htm#HENRY_SPENCER_REGEXP_SOURCE

#ifndef JREFUZZY
#define JREFUZZY

// JreFuzzy.h
#define FUZZYOPTION_NOCASE				1		// ���p�p�����̑啶���^����������ʂ��Ȃ�
//�p�~#define FUZZYOPTION_NOCASEZENKAKU		2		// �S�p�p�����̑啶���^����������ʂ��Ȃ�
#define FUZZYOPTION_NOHANZEN			4		// ���p�^�S�p����ʂ��Ȃ�
#define FUZZYOPTION_NOHIRAKATA			8		// �Ђ炪�ȂƃJ�^�J�i����ʂ��Ȃ�
#define FUZZYOPTION_NOBIGSMALLKANA		0x10	// �傫���u���v�Ə������u���v����ʂ��Ȃ�
#define FUZZYOPTION_NODAKUON			0x20	// �����A����������ʂ��Ȃ��B�u�́v�A�u�΁v�A�u�ρv�𓯈ꎋ����B
#define FUZZYOPTION_KANAHYPHEN			0x40	// ���Ȃ́u�[�v�Ɖp���́u�|�v�𓯈ꎋ����
#define FUZZYOPTION_NOSPACE				0x80	// �󔒁^�^�u�����𖳎�����
#define FUZZYOPTION_NORETURN			0x100	// ���s�𖳎�����
#define FUZZYOPTION_CUSTOMIGNORE		0x200	// ���蕶���𖳎�����BV1.09����B

#define FUZZYOPTION_CUSTOM1			0x00010000
#define FUZZYOPTION_CUSTOM2			0x00020000
#define FUZZYOPTION_CUSTOM3			0x00040000
#define FUZZYOPTION_CUSTOM4			0x00080000

#define FUZZYOPTION_NORETURN_MASK		0x70000000	// 0�`7 --> 1�`8�ɑΉ��B
#define FUZZYOPTION_NORETURN_SHIFT		28			// 28�r�b�g�V�t�g������B


#define FUZZYOPTION_ALL			0xFFFFFFFF

struct JREFUZZYDATA {
	DWORD	dwSize;
	DWORD	flags;
	char*	pszFindConved;
	int		cchFindConved;
	char*	pszTargetConved;
	int		cchTargetConved;
	short*	psTargetPos;
	WORD*	pwCustomConv;
	char*	pszTargetOrigin;	// �ϊ��O��pszTarget�AV0.04����B
	char*	pbSynonym;
	int		cbSynonym;
	int		cSynonymMultiplyMax;
	int		cchSynonymWordMax;
	int		cbFindConvedAlloc;			//V0.06����B
	int		cbTargetConvedAlloc;		//V0.06����B
	DWORD	adwReserve[6];
};

extern "C" BOOL WINAPI Fuzzy_Open( JREFUZZYDATA* pData, BOOL fDummy = FALSE );
extern "C" BOOL WINAPI Fuzzy_Close( JREFUZZYDATA* pData );
extern "C" BOOL WINAPI Fuzzy_ConvertTarget( JREFUZZYDATA* pData, const char* pszSrc );
extern "C" BOOL WINAPI Fuzzy_ConvertFindString( JREFUZZYDATA* pData, const char* pszSrc, BOOL fRegular );
extern "C" int WINAPI Fuzzy_FindPos2RealPos( JREFUZZYDATA* pData, int x );
extern "C" int WINAPI Fuzzy_FindArea2RealArea( JREFUZZYDATA* pData, int x, int* pcchMatch );
extern "C" int WINAPI Fuzzy_RealPos2FindPos( JREFUZZYDATA* pData, int x );
extern "C" JREFUZZYDATA* WINAPI Fuzzy_GetFuzzyDataInJre( JRE2* pJre );
extern "C" BOOL _cdecl Fuzzy_OptionDialog( HWND hwndParent, UINT flagsDisable );

#define FUZZYERROR_FINDSTRING_NOTHING		100

// Version 1.5����
extern "C" int WINAPI JreGetTagPosition( LPJRE2 lpjreJre, char chTagWantGet, int* pcchMatch );

#define FUZZYERROR_TAG_NOT_FOUND		101
#define FUZZYERROR_FRONTMATCH_IN_MIDDLE	102
#define FUZZYERROR_FRONTUNMATCH_IN_MIDDLE	103
#define FUZZYERROR_TAILMATCH_IN_KAKKO		104
#define FUZZYERROR_TAILUNMATCH_IN_KAKKO		105
#define FUZZYERROR_PATTERN_AFTER_TAILMATCH		106


#endif
