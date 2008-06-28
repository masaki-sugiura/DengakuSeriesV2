// HmJre.dll copyright(c) 2003 有限会社サイトー企画
// written by 斉藤秀夫
// 参考URL  http://www.labyrinth.net.au/~dbareis/3rdpsrc.htm#HENRY_SPENCER_REGEXP_SOURCE

#ifndef JREFUZZY
#define JREFUZZY

// JreFuzzy.h
#define FUZZYOPTION_NOCASE				1		// 半角英数字の大文字／小文字を区別しない
//廃止#define FUZZYOPTION_NOCASEZENKAKU		2		// 全角英数字の大文字／小文字を区別しない
#define FUZZYOPTION_NOHANZEN			4		// 半角／全角を区別しない
#define FUZZYOPTION_NOHIRAKATA			8		// ひらがなとカタカナを区別しない
#define FUZZYOPTION_NOBIGSMALLKANA		0x10	// 大きい「あ」と小さい「ぁ」を区別しない
#define FUZZYOPTION_NODAKUON			0x20	// 濁音、半濁音を区別しない。「は」、「ば」、「ぱ」を同一視する。
#define FUZZYOPTION_KANAHYPHEN			0x40	// かなの「ー」と英数の「－」を同一視する
#define FUZZYOPTION_NOSPACE				0x80	// 空白／タブ文字を無視する
#define FUZZYOPTION_NORETURN			0x100	// 改行を無視する
#define FUZZYOPTION_CUSTOMIGNORE		0x200	// 特定文字を無視する。V1.09から。

#define FUZZYOPTION_CUSTOM1			0x00010000
#define FUZZYOPTION_CUSTOM2			0x00020000
#define FUZZYOPTION_CUSTOM3			0x00040000
#define FUZZYOPTION_CUSTOM4			0x00080000

#define FUZZYOPTION_NORETURN_MASK		0x70000000	// 0～7 --> 1～8に対応。
#define FUZZYOPTION_NORETURN_SHIFT		28			// 28ビットシフトさせる。


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
	char*	pszTargetOrigin;	// 変換前のpszTarget、V0.04から。
	char*	pbSynonym;
	int		cbSynonym;
	int		cSynonymMultiplyMax;
	int		cchSynonymWordMax;
	int		cbFindConvedAlloc;			//V0.06から。
	int		cbTargetConvedAlloc;		//V0.06から。
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

// Version 1.5から
extern "C" int WINAPI JreGetTagPosition( LPJRE2 lpjreJre, char chTagWantGet, int* pcchMatch );

#define FUZZYERROR_TAG_NOT_FOUND		101
#define FUZZYERROR_FRONTMATCH_IN_MIDDLE	102
#define FUZZYERROR_FRONTUNMATCH_IN_MIDDLE	103
#define FUZZYERROR_TAILMATCH_IN_KAKKO		104
#define FUZZYERROR_TAILUNMATCH_IN_KAKKO		105
#define FUZZYERROR_PATTERN_AFTER_TAILMATCH		106


#endif
