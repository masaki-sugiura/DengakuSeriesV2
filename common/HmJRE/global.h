// HmJre.dll copyright(c) 2003 有限会社サイトー企画
// written by 斉藤秀夫
// 参考URL  http://www.labyrinth.net.au/~dbareis/3rdpsrc.htm#HENRY_SPENCER_REGEXP_SOURCE

// HmJre.dllのグローバルヘッダ。
#define SMALLENT PASCAL

#if _MSC_VER < 1300
typedef UINT	UINT_PTR;
typedef INT	INT_PTR;
typedef size_t		SIZE_T;
typedef DWORD	DWORD_PTR;
#endif


#define	PROGRAM_END				0		// ここで終了
#define PROGRAM_CLOSE_FRONTUNMATCH	1		// FRONTUNMATCHに対応するカッコ閉じ
#define PROGRAM_LINE_BEGIN		2		// 行の先頭、"^"
#define PROGRAM_LINE_END		3		// 行の末尾、"$"
#define PROGRAM_ANY				4		// 任意の文字、"."
#define PROGRAM_EXACT			5		// 文字列にマッチするかどうか。オペランドが続く。 使ってないけど処理だけ残す。
#define PROGRAM_EXACT1			6		// 文字列にマッチするかどうか。オペランドが続く。
#define PROGRAM_EXACT2			7		// 文字列にマッチするかどうか。オペランドが続く。
#define PROGRAM_EXACT3			8		// 文字列にマッチするかどうか。オペランドが続く。
#define PROGRAM_EXACT4			9		// 文字列にマッチするかどうか。オペランドが続く。
#define PROGRAM_EXACT5OVER		10		// 文字列にマッチするかどうか。オペランドが続く。
#define PROGRAM_EXACT_LONG		11		// 文字列にマッチするかどうか。オペランドが続く。
#define PROGRAM_OPEN_NESTZERO_EXACT		12		// (aaa)や(aaa)*のようなのを高速化する用。PROGRAM_OPEN_NESTZEROと同等だけど、中身はEXACTが１つあるのみ。
#define PROGRAM_WORD			13		// 
#define PROGRAM_WORD_BEGIN		14		// \<
#define PROGRAM_WORD_END		15		// \>
//やっぱり廃止#define PROGRAM_WORD_SPLIT		9		// \b
#define PROGRAM_ANYOF			16		// オペランドで指定された文字の中のどれか。オペランドが続く。［A-Z]など。
#define PROGRAM_ANYBUT			17		// オペランドで指定された文字の中のどれにもヒットしない。[^A-Z]など。
#define PROGRAM_ANYOF_FAST		18		// オペランドで指定された文字の中のどれか。オペランドが続く。［A-Z]など。
#define PROGRAM_ANYBUT_FAST		19		// オペランドで指定された文字の中のどれにもヒットしない。[^A-Z]など。
#define PROGRAM_BRANCH			20		// 分岐の途中。"|"。PROGRAM_OPEN/PROGRAM_CLOSEで囲まれているはず。
#define PROGRAM_STAR			21		// パターンの0回以上の繰り返し。後ろにPROGRAMが１つ続く。
#define PROGRAM_PLUS			22		// パターンの1回以上の繰り返し。後ろにPROGRAMが１つ続く。
#define PROGRAM_QUESTION		23		// パターンが0回または1回繰り返し。後ろにPROGRAMが１つ続く。
#define PROGRAM_REPEAT			24		// \{n,m\}。mが省略されてる場合も含む。後ろ2バイトがn、さらに後ろ2バイトがm、その後ろにPROGRAMが１つ続く。
#define PROGRAM_ANYANY			25		// 「.*」のケース。これだけ高速処理する。
#define PROGRAM_STAR_SHORT		26		// 後ろに?が付くパターン。
#define PROGRAM_PLUS_SHORT		27		// 
#define PROGRAM_QUESTION_SHORT	28		// 
#define PROGRAM_REPEAT_SHORT	29		// 
#define PROGRAM_TAILMATCH		30		// (?=expression)
#define PROGRAM_TAILUNMATCH		31		// (?!expression)
#define PROGRAM_FRONTMATCH		32		// (?<=expression)
#define PROGRAM_FRONTUNMATCH	33		// (?<!expression)
#define PROGRAM_OPEN			34		// カッコ
#define PROGRAM_CLOSE			35		// カッコ閉じる
#define PROGRAM_OPEN_NESTZERO	36		// 一番外側ネストでのカッコ。タグ付き正規表現で使う。'1'～'9'の１バイトが続く。
#define PROGRAM_CLOSE_NESTZERO	37		// 一番外側ネストでのカッコ閉じ。タグ付き正規表現で使う。'1'～'9'の１バイトが続く。
#define PROGRAM_TAG				38		// \1～\9、'1'～'9'の１バイトが続く。
#define PROGRAM_BRANCHBOOST		39		// (a|b|c|d|...)のようなパターンの時の高速化用。先頭文字の最小/最大, ２番目文時の最小/最大、先頭文字のマッチ用フラグ配列を持つ。
#define PROGRAM_BRANCHBOOST2	40		// (|a|b|c|d|...)のようなパターンの時の高速化用。SkipFirstMatchとある程度の除外だけ可能。

//hCompData2をフラグとして使う。
#define JREFLAG_POINTER_EXIST	1
#define JREFLAG_SKIPABLE		2			// SkipFirstMatch可能かどうか。
#define JREFLAG_SKIPABLE2		4			// FRONTMATCH, FRONTUNMATCHの時に、本体部分がSkipFirstMatch可能かどうか。
//V1.10 廃止#define JREFLAG_ANYANYSTART	8			// 「.*」で始まるパターンかどうか。このフラグがONの場合は、「.*」の式は除去される。
#define JREFLAG_ANYANYEND	0x10		// 「.*」で終わるパターンかどうか。このフラグがONの場合は、「.*」の式は除去される。
//V1.10 廃止#define JREFLAG_LINEBEGIN_ANYANYSTART	0x20	// 「^.*」で始まるパターンかどうか。

char* SkipProgramWithFree( char* pbProgram, DWORD cbBufferNeed = 0xFFFFFFFF );

extern HINSTANCE	hinstDll;
extern HANDLE		hmutex;

struct SHAREMEM {
	struct CONFIGDATA {
		DWORD	flagsFuzzy;
		char	aszCustom[4][MAX_PATH];
		DWORDLONG	aftCustom[4];
		char	szCustomIgnore[128];				//V1.09から。
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
//	BYTE	afIncludingChar[256];		// 含んでいる必要のある文字をビットで表す。
	int		chTagMax;
	TAGBEGINEND	aTag[9];				// 1～9までの9個
	int		chTagReturn;
};

struct TAGPOSITIONSAVE {
	TAGBEGINEND	aTag[9];			// 1～9までの9個
};


extern HANDLE		hShareMem;
extern SHAREMEM*	pShareMem;

extern HANDLE		hmapSynonym;
extern char*		pbSynonymMap;
extern int			iSynonymFileMappingCount;

// 同一視文字列の変換テーブル仕様：
// １バイト目に単語のバイト数、その後に単語そのもの。そのセットが続く。
// 同一視する単語が終了する時点で、NULL文字がある。さらにその後NULL文字があれば、そこで全部終了。


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

