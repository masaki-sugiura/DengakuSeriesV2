/*
Visual Basic ���炨�g���̂Ƃ��͎���Declare�����g���Ă��������B
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
	const char *outp;	// BSubst �u���f�[�^�̐擪�|�C���^
	const char *outendp;	// BSubst �u���f�[�^�̍ŏI�|�C���^+1
	const int  splitctr;	// BSplit �z��
	const char **splitp;	// BSplit �f�[�^�|�C���^
	int	rsv1;		// ���U�[�u�@���R�Ɏg�p�\ 
	char *parap;		// �p�^�[���f�[�^�|�C���^
	char *paraendp;		// �p�^�[���f�[�^�|�C���^+1
	char *transtblp;	// BTrans �ϊ��e�[�u���|�C���^
	char **startp;		// �}�b�`�����f�[�^�̐擪�|�C���^
	char **endp;		// �}�b�`�����f�[�^�̍ŏI�|�C���^+1
	int nparens;		// �p�^�[���̒���() �̐��B $1,$2, �𒲂ׂ�Ƃ��Ɏg�p
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