//	$Id: strbuf.h,v 1.5 2003-07-06 16:27:46 sugiura Exp $
/*
 *	strbuf.h
 *	文字列クラス
 */

#ifndef	DENGAKUSERIES_CLASSES_STRBUF
#define	DENGAKUSERIES_CLASSES_STRBUF

#include "array.h"
#include "refcount.h"

#define	BUF_LEN	2

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
		this->StringBuffer::reset(rhs);
		return *this;
	}
	StringBuffer& operator+=(const StringBuffer& rhs)
	{
		this->StringBuffer::append(rhs); // avoid virtual function call
		return *this;
	}

	void setlength(int);	//	文字列を切りつめる

	int size() const	//	バッファサイズを返す
	{
		if (!m_sbuf->isShareable()) m_sbuf->recalc();
		return m_sbuf->m_buf.size() - 1; // NULL 文字領域を含めない
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

	//	(２バイト文字を意識しない)文字列の反転
	StringBuffer& reverse();

	//	(２バイト文字を意識した)文字列の反転
	StringBuffer& reverse2();

	//	２バイト文字を意識しない文字列の変換
	StringBuffer& toLower();
	StringBuffer& toUpper();

	//	２バイト文字を意識する文字列の変換
	StringBuffer& toLower2();
	StringBuffer& toUpper2();

	// 全角<->半角変換
	StringBuffer& toHankaku();
	StringBuffer& toZenkakuHira();
	StringBuffer& toZenkakuKata();

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
//		int m_bufsize;	//	バッファのサイズ
		int m_len;		//	現在格納中の文字列の長さ
		Array<TCHAR> m_buf;	//	バッファの実態

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

