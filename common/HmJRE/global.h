// HmJre.dll copyright(c) 2003 �L����ЃT�C�g�[���
// written by �ē��G�v
// �Q�lURL  http://www.labyrinth.net.au/~dbareis/3rdpsrc.htm#HENRY_SPENCER_REGEXP_SOURCE

// HmJre.dll�̃O���[�o���w�b�_�B
#define SMALLENT PASCAL

#if _MSC_VER < 1300
typedef UINT	UINT_PTR;
typedef INT	INT_PTR;
typedef size_t		SIZE_T;
typedef DWORD	DWORD_PTR;
#endif


#define	PROGRAM_END				0		// �����ŏI��
#define PROGRAM_CLOSE_FRONTUNMATCH	1		// FRONTUNMATCH�ɑΉ�����J�b�R��
#define PROGRAM_LINE_BEGIN		2		// �s�̐擪�A"^"
#define PROGRAM_LINE_END		3		// �s�̖����A"$"
#define PROGRAM_ANY				4		// �C�ӂ̕����A"."
#define PROGRAM_EXACT			5		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B �g���ĂȂ����Ǐ��������c���B
#define PROGRAM_EXACT1			6		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B
#define PROGRAM_EXACT2			7		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B
#define PROGRAM_EXACT3			8		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B
#define PROGRAM_EXACT4			9		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B
#define PROGRAM_EXACT5OVER		10		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B
#define PROGRAM_EXACT_LONG		11		// ������Ƀ}�b�`���邩�ǂ����B�I�y�����h�������B
#define PROGRAM_OPEN_NESTZERO_EXACT		12		// (aaa)��(aaa)*�̂悤�Ȃ̂�����������p�BPROGRAM_OPEN_NESTZERO�Ɠ��������ǁA���g��EXACT���P����̂݁B
#define PROGRAM_WORD			13		// 
#define PROGRAM_WORD_BEGIN		14		// \<
#define PROGRAM_WORD_END		15		// \>
//����ς�p�~#define PROGRAM_WORD_SPLIT		9		// \b
#define PROGRAM_ANYOF			16		// �I�y�����h�Ŏw�肳�ꂽ�����̒��̂ǂꂩ�B�I�y�����h�������B�mA-Z]�ȂǁB
#define PROGRAM_ANYBUT			17		// �I�y�����h�Ŏw�肳�ꂽ�����̒��̂ǂ�ɂ��q�b�g���Ȃ��B[^A-Z]�ȂǁB
#define PROGRAM_ANYOF_FAST		18		// �I�y�����h�Ŏw�肳�ꂽ�����̒��̂ǂꂩ�B�I�y�����h�������B�mA-Z]�ȂǁB
#define PROGRAM_ANYBUT_FAST		19		// �I�y�����h�Ŏw�肳�ꂽ�����̒��̂ǂ�ɂ��q�b�g���Ȃ��B[^A-Z]�ȂǁB
#define PROGRAM_BRANCH			20		// ����̓r���B"|"�BPROGRAM_OPEN/PROGRAM_CLOSE�ň͂܂�Ă���͂��B
#define PROGRAM_STAR			21		// �p�^�[����0��ȏ�̌J��Ԃ��B����PROGRAM���P�����B
#define PROGRAM_PLUS			22		// �p�^�[����1��ȏ�̌J��Ԃ��B����PROGRAM���P�����B
#define PROGRAM_QUESTION		23		// �p�^�[����0��܂���1��J��Ԃ��B����PROGRAM���P�����B
#define PROGRAM_REPEAT			24		// \{n,m\}�Bm���ȗ�����Ă�ꍇ���܂ށB���2�o�C�g��n�A����Ɍ��2�o�C�g��m�A���̌���PROGRAM���P�����B
#define PROGRAM_ANYANY			25		// �u.*�v�̃P�[�X�B���ꂾ��������������B
#define PROGRAM_STAR_SHORT		26		// ����?���t���p�^�[���B
#define PROGRAM_PLUS_SHORT		27		// 
#define PROGRAM_QUESTION_SHORT	28		// 
#define PROGRAM_REPEAT_SHORT	29		// 
#define PROGRAM_TAILMATCH		30		// (?=expression)
#define PROGRAM_TAILUNMATCH		31		// (?!expression)
#define PROGRAM_FRONTMATCH		32		// (?<=expression)
#define PROGRAM_FRONTUNMATCH	33		// (?<!expression)
#define PROGRAM_OPEN			34		// �J�b�R
#define PROGRAM_CLOSE			35		// �J�b�R����
#define PROGRAM_OPEN_NESTZERO	36		// ��ԊO���l�X�g�ł̃J�b�R�B�^�O�t�����K�\���Ŏg���B'1'�`'9'�̂P�o�C�g�������B
#define PROGRAM_CLOSE_NESTZERO	37		// ��ԊO���l�X�g�ł̃J�b�R���B�^�O�t�����K�\���Ŏg���B'1'�`'9'�̂P�o�C�g�������B
#define PROGRAM_TAG				38		// \1�`\9�A'1'�`'9'�̂P�o�C�g�������B
#define PROGRAM_BRANCHBOOST		39		// (a|b|c|d|...)�̂悤�ȃp�^�[���̎��̍������p�B�擪�����̍ŏ�/�ő�, �Q�Ԗڕ����̍ŏ�/�ő�A�擪�����̃}�b�`�p�t���O�z������B
#define PROGRAM_BRANCHBOOST2	40		// (|a|b|c|d|...)�̂悤�ȃp�^�[���̎��̍������p�BSkipFirstMatch�Ƃ�����x�̏��O�����\�B

//hCompData2���t���O�Ƃ��Ďg���B
#define JREFLAG_POINTER_EXIST	1
#define JREFLAG_SKIPABLE		2			// SkipFirstMatch�\���ǂ����B
#define JREFLAG_SKIPABLE2		4			// FRONTMATCH, FRONTUNMATCH�̎��ɁA�{�̕�����SkipFirstMatch�\���ǂ����B
//V1.10 �p�~#define JREFLAG_ANYANYSTART	8			// �u.*�v�Ŏn�܂�p�^�[�����ǂ����B���̃t���O��ON�̏ꍇ�́A�u.*�v�̎��͏��������B
#define JREFLAG_ANYANYEND	0x10		// �u.*�v�ŏI���p�^�[�����ǂ����B���̃t���O��ON�̏ꍇ�́A�u.*�v�̎��͏��������B
//V1.10 �p�~#define JREFLAG_LINEBEGIN_ANYANYSTART	0x20	// �u^.*�v�Ŏn�܂�p�^�[�����ǂ����B

char* SkipProgramWithFree( char* pbProgram, DWORD cbBufferNeed = 0xFFFFFFFF );

extern HINSTANCE	hinstDll;
extern HANDLE		hmutex;

struct SHAREMEM {
	struct CONFIGDATA {
		DWORD	flagsFuzzy;
		char	aszCustom[4][MAX_PATH];
		DWORDLONG	aftCustom[4];
		char	szCustomIgnore[128];				//V1.09����B
	} config;

	int		iSynonymFileMappingCount;
	int		cbSynonymFileMapping;
	int		cSynonymMultiplyMax;
	int		cchSynonymWordMax;

	BOOL	fCustomConvInit;
	BOOL	fCustomConvExist;
	BYTE	afCustomConvHigh[(0xFC - 0xE0 + 1) + (0x9F - 0x81 + 1) + 1];
	WORD	aawCustomConv[ (0xFC - 0xE0 + 1) + (0x9F - 0x81 + 1) + 1 ][ 0x100 - 0x40 ];
};

struct TAGBEGINEND {
	int	xBegin, xEnd;
};

struct COMPDATA3 {
	BYTE	afDBCS[128];
//	BYTE	fIncludingCharFlagsValid;
//	BYTE	iIncludingMin;
//	BYTE	iIncludingMax;
//	BYTE	bReserve1;
//	BYTE	afIncludingChar[256];		// �܂�ł���K�v�̂��镶�����r�b�g�ŕ\���B
	int		chTagMax;
	TAGBEGINEND	aTag[9];				// 1�`9�܂ł�9��
	int		chTagReturn;
};

struct TAGPOSITIONSAVE {
	TAGBEGINEND	aTag[9];			// 1�`9�܂ł�9��
};


extern HANDLE		hShareMem;
extern SHAREMEM*	pShareMem;

extern HANDLE		hmapSynonym;
extern char*		pbSynonymMap;
extern int			iSynonymFileMappingCount;

// ���ꎋ������̕ϊ��e�[�u���d�l�F
// �P�o�C�g�ڂɒP��̃o�C�g���A���̌�ɒP�ꂻ�̂��́B���̃Z�b�g�������B
// ���ꎋ����P�ꂪ�I�����鎞�_�ŁANULL����������B����ɂ��̌�NULL����������΁A�����őS���I���B


void MakeCustomConv();
void FreeCustomConv();

extern BYTE	afDBCS932[128];
extern BOOL IsKanji2( const char* psz, int x, BYTE* pbDBCS );


#if 1
extern HLOCAL SMALLENT LocalAlloc_Debug( UINT uFlags, UINT nBytes, char* pszCppFile, int lineNo );
extern HLOCAL SMALLENT LocalFree_Debug( HLOCAL hMem, char* pszCppFile, int lineNo );
extern HLOCAL SMALLENT LocalReAlloc_Debug( HLOCAL hMem, UINT nBytes, UINT flags, char* pszCppFile, int lineNo );
extern void* SMALLENT LocalLock_Debug( HLOCAL hMem, char* pszCppFile, int lineNo );
extern BOOL SMALLENT LocalUnlock_Debug( HLOCAL hMem, char* pszCppFile, int lineNo );
#define LocalAlloc2( a, b ) LocalAlloc_Debug( (a), (b), __FILE__, __LINE__ )
#define LocalFree2( a ) LocalFree_Debug( (a), __FILE__, __LINE__ )
#define LocalReAlloc2( a,b,c ) LocalReAlloc_Debug( (a), (b), (c), __FILE__, __LINE__ )
#define LocalLock2( a ) LocalLock_Debug( (a), __FILE__, __LINE__ )
#define LocalUnlock2( a ) LocalUnlock_Debug( (a), __FILE__, __LINE__ )
#define LocalSize2( a ) (LocalSize( (a) ) - 4)
#else
#define LocalAlloc2(a,b) LocalAlloc((a),(b))
#define LocalFree2(a) LocalFree((a))
#define LocalReAlloc2(a,b,c) LocalReAlloc((a),(b),(c))
#define LocalLock2( a ) LocalLock( (a) )
#define LocalUnlock2( a ) LocalUnlock( (a) )
#define LocalSize2( a ) LocalSize( (a) );
#endif

extern void SMALLENT CheckLocalMem( HLOCAL hMem );

extern void DEBUGMESSAGE( char* psz );
extern UINT GetCharCode( const char** ppszSrc, BYTE* pbDBCS );

BOOL SMALLENT IsSkipFirstMatchable( char* pbProgram );
char* SMALLENT SkipBranchByProgram( char* pbProgram, BOOL fAll );

