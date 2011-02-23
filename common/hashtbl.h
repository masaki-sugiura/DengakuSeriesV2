//	$Id: hashtbl.h,v 1.3 2002/03/29 17:11:59 sugiura Exp $
/*
 *	hashtbl.h
 *	ハッシュテーブルクラス(テンプレート)
 */

#ifndef	DENGAKUSERIES_CLASSES_HASHTABLE
#define	DENGAKUSERIES_CLASSES_HASHTABLE

#include "strbuf.h"
#include "array.h"

class RawHashTable {
protected:
	RawHashTable(int n);
	virtual ~RawHashTable() = 0;

	int setValue_(const StringBuffer&, LPVOID);
	LPVOID getValue_(const StringBuffer&) const;

private:
	//	キーと値をまとめたクラス
	class Key {
	public:
		Key* m_next;
		const StringBuffer m_key;
		LPVOID m_value;
		Key(const StringBuffer& key, LPVOID value)
			: m_next(NULL), m_key(key), m_value(value)
		{}
	};

	Array<Key*> m_keylist;

	RawHashTable(const RawHashTable&);
	RawHashTable& operator=(const RawHashTable&);

	static UINT getHash(LPCSTR, UINT);
};

//	ハッシュテーブルのテンプレートクラス
//	class T は int またはポインタで、
//	しかも無効なデータは 0 (or NULL) になっている必要がある
template<class T, int n>
class HashTable : private RawHashTable {
public:
	HashTable() : RawHashTable(n) {}

	int setValue(const StringBuffer& keyname, T value)
	{
		return setValue_(keyname, (LPVOID)value);
	}

	T getValue(const StringBuffer& keyname) const
	{
		return (T)getValue_(keyname);
	}
};

#endif

