/*
Visual Basic からお使いのときは次のDeclare文を使ってください。
Private Declare Function Match Lib "bregexp" _
      (szRegstr As String, szTarget As String) As String


Private Declare Function Replace Lib "bregexp" _
      (szRegstr As String, szTarget As String) As String

Private Declare Function Translate Lib "bregexp" _
      (szRegstr As String, szTarget As String, ret As String) As Long

Private Declare Function Split Lib "bregexp" _
      (szRegstr As String, szTarget As String, limit As Long) As Variant

Private Declare Function MatchEx Lib "bregexp" _
      (szRegstr As String, szTarget As String, mode As Long) As Variant

*/


#ifdef _BREGEXP_
#define BREGEXPAPI __declspec(dllexport) 
#else
#define BREGEXPAPI __declspec(dllimport) 
#endif

#if 0
typedef struct bregexp {
	const char *outp;			/* result string start ptr   */
	const char *outendp;		/* result string end ptr     */ 
	const int  splitctr;		/* split result counter     */ 
	const char **splitp;		/* split result pointer ptr     */ 
	int     rsv1;				/* reserved for external use    */ 
} BREGEXP;
#endif

typedef struct bregexp {
	const char *outp;	// BSubst 置換データの先頭ポインタ
	const char *outendp;	// BSubst 置換データの最終ポインタ+1
	const int  splitctr;	// BSplit 配列数
	const char **splitp;	// BSplit データポインタ
	int	rsv1;		// リザーブ　自由に使用可能 
	char *parap;		// パターンデータポインタ
	char *paraendp;		// パターンデータポインタ+1
	char *transtblp;	// BTrans 変換テーブルポインタ
	char **startp;		// マッチしたデータの先頭ポインタ
	char **endp;		// マッチしたデータの最終ポインタ+1
	int nparens;		// パターンの中の() の数。 $1,$2, を調べるときに使用
} BREGEXP;

#if defined(__cplusplus)
extern "C"
{
#endif

BREGEXPAPI
int BMatch(char* str,char *target,char *targetendp,
								BREGEXP **rxp,char *msg) ;
BREGEXPAPI
int BSubst(char* str,char *target,char *targetendp,
								BREGEXP **rxp,char *msg) ;
BREGEXPAPI
int BTrans(char* str,char *target,char *targetendp,
								BREGEXP **rxp,char *msg) ;
BREGEXPAPI
int BSplit(char* str,char *target,char *targetendp,
						int limit,BREGEXP **rxp,char *msg);
BREGEXPAPI
void BRegfree(BREGEXP* rx);

BREGEXPAPI
char* BRegexpVersion(void);

#if defined(__cplusplus)
}
#endif


#undef BREGEXPAPI
