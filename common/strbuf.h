//	$Id: strbuf.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	strbuf.h
 *	文字列クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_STRBUF
#define	DENGAKUSERIES_CLASSES_STRBUF

#include "refcount.h"

BOOL	isnumber(LPCSTR);
int		ival(LPCSTR);
double	dval(LPCSTR);
int		bintoi(LPCSTR);
int		octtoi(LPCSTR);
int		hextoi(LPCSTR);
void	itooct(LPSTR, int);
void	itobin(LPSTR, int);
int		lstrcmpn(LPCSTR, LPCSTR, int);
int		lstrcmpni(LPCSTR, LPCSTR, int);
LPSTR	lstrchr(LPCSTR, TCHAR);
LPSTR	lstrrchr(LPCSTR, TCHAR);
LPSTR	lstrstr(LPCSTR, LPCSTR);
int		lstrlen2(LPCSTR);
LPSTR	lstrninc(LPCSTR, int);

#define	BUF_LEN	2
#define	lval(num)	((long)ival(num))

#define IS_LOWERCASE 0x01
#define IS_UPPERCASE 0x02
#define IS_DIGIT     0x04
#define IS_XDIGIT    0x08
#define IS_SPACE     0x10
#define IS_LEADBYTE  0x20
#define IS_TRAILBYTE 0x40
#define IS_READABLE  0x80

extern UCHAR char_property[];

inline int
ABS(int i)
{
	return (i >= 0) ? i : -i;
}

inline BOOL
IsCharUpperCase(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_UPPERCASE) != 0;
}

inline BOOL
IsCharLowerCase(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_LOWERCASE) != 0;
}

inline BOOL
IsCharAlphabet(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & (IS_UPPERCASE|IS_LOWERCASE)) != 0;
}

inline BOOL
IsCharDigit(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_DIGIT) != 0;
}

inline BOOL
IsCharXDigit(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_XDIGIT) != 0;
}

inline BOOL
IsCharSpace(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_SPACE) != 0;
}

inline BOOL
IsCharLeadByte(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_LEADBYTE) != 0;
}

inline BOOL
IsCharTrailByte(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_TRAILBYTE) != 0;
}

inline BOOL
IsCharReadable(TCHAR ch)
{
	return (char_property[(UCHAR)ch] & IS_READABLE) != 0;
}

inline BOOL
IsValidPtr(LPCSTR str)
{
	return (str != NULL && !::IsBadStringPtr(str,1));
}

inline LPSTR
ToNextChar(LPCSTR ptr)
{
	return (LPSTR)(IsCharLeadByte(*ptr) ? (ptr + 2) : (ptr + 1));
}

//	文字列クラス
class StringBuffer {
public:
	StringBuffer(int len = BUF_LEN);
	StringBuffer(LPCSTR str, int len = -1, int exlen = BUF_LEN);
	StringBuffer(const StringBuffer& that)
		: m_sbuf(that.m_sbuf)
	{
		// nothing to do.
	}
	virtual	~StringBuffer();

	//	required in pathname.h
	operator LPCSTR() const { return m_sbuf->m_buf; }
	//	operator	LPSTR();
	LPSTR getBufPtr();

	StringBuffer& operator=(const StringBuffer& rhs)
	{
		if (this != &rhs) m_sbuf = rhs.m_sbuf;
		return *this;
	}
	StringBuffer& operator=(LPCSTR rhs)
	{
		this->StringBuffer::reset(rhs); // avoid virtual function call
		return *this;
	}
	StringBuffer& operator+=(const StringBuffer& rhs)
	{
		this->StringBuffer::append(rhs);
		return *this;
	}

	void setlength(int);	//	文字列を切りつめる

	int size() const	//	バッファサイズを返す
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		return m_sbuf->m_bufsize;
	}

	void resize(int newsize)	//	バッファのサイズを増やす(減らさない)
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		this->dup();
		m_sbuf->resize(newsize);
	}

	virtual	void reset(LPCSTR str = NULL, int head = 0, int len = -1);

	void reset(LPCWSTR str, int head = 0, int len = -1);
	int toUnicode(LPWSTR) const;

	//	指定場所の文字の変更
	void setcharAt(int at = -1, TCHAR ch = '\0');

	//	文字(列)の追加
	StringBuffer& append(LPCSTR str, int head = 0, int len = -1);
	StringBuffer& append(const StringBuffer& str, int head = 0, int len = -1)
	{ return this->append((LPCSTR)str,head,len); }
	StringBuffer& append(TCHAR);
	StringBuffer& append(int);
	StringBuffer& append(long);
	StringBuffer& append(ULONG);

	//	指定文字列の置き換え
	StringBuffer& replaceStr(LPCSTR ostr, LPCSTR dstr, int num = -1);

	//	２バイト文字を意識しない文字列の変換
	StringBuffer& toLower();
	StringBuffer& toUpper();

	//	２バイト文字を意識する文字列の変換
	StringBuffer& toLower2();
	StringBuffer& toUpper2();

	//	文字列の比較
	int compareTo(LPCSTR str, BOOL cs = TRUE, int len = -1) const;

	//	raw-level access to the character
	TCHAR operator[](int at) const
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		return m_sbuf->m_buf[at];
	}

	//	共通の情報取得メソッド
	TCHAR charAt(int at = -1) const;	//	指定場所の文字を返す
	int count(LPCSTR) const;	//	指定文字列の出現数を返す
	int find(TCHAR) const;	//	指定文字の最初の出現位置を返す
	int rfind(TCHAR) const;	//	指定文字の最後の出現位置を返す

	//	２バイト文字を認識しない情報取得メソッド
	int length() const;			//	文字列のバイト数を返す
	int find(LPCSTR) const;	//	指定文字列の最初の出現位置を返す
	int rfind(LPCSTR) const;	//	指定文字列の最後の出現位置を返す
	//	指定バイト数以降の指定バイト数分の文字列を抜き出す
	StringBuffer extract(int head, int len = -1) const;

	//	２バイト文字を認識する情報取得メソッド
	int length2() const;		//	文字列の文字数を返す
	int find2(LPCSTR) const;	//	指定文字列の最初の出現位置を返す
	int rfind2(LPCSTR) const;	//	指定文字列の最後の出現位置を返す
	//	指定文字数以降の指定文字数分の文字列を抜き出す
	StringBuffer extract2(int head, int len = -1) const;

protected:
	//	StringBuffer クラスから共有されるバッファの実体クラス
	struct StringBuffer_rep : public RCObject {
#ifdef _DEBUG
		int m_id;
#endif
		int m_bufsize;	//	バッファのサイズ
		int m_len;		//	現在格納中の文字列の長さ
		LPSTR m_buf;	//	バッファへのポインタ

		StringBuffer_rep(int inisize);
		StringBuffer_rep(LPCSTR str, int len = -1, int exsize = BUF_LEN);
		StringBuffer_rep(const StringBuffer_rep&);
		~StringBuffer_rep();

		void init(LPCSTR,int,int);
		void recalc();
		void resize(int);

		friend class RCPtr<StringBuffer_rep>;
	};

	RCPtr<StringBuffer_rep>	m_sbuf;	//	バッファクラスへの参照自動カウント
									//	スマートポインタ

	void dup();	//	バッファクラスの複製＆参照カウントの調整
};

extern const StringBuffer nullStr;

#endif

