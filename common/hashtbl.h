//	$Id: hashtbl.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	hashtbl.h
 *	ハッシュテーブルクラス(テンプレート)
 */

#ifndef	DENGAKUSERIES_CLASSES_HASHTABLE
#define	DENGAKUSERIES_CLASSES_HASHTABLE

#include "strbuf.h"
#include <exception>

UINT getHash(LPCSTR, UINT);

//	ハッシュテーブルのテンプレートクラス
//	class T は int またはポインタで、
//	しかも無効なデータは 0 (or NULL) になっている必要がある
template<class T, UINT n>
class HashTable {
public:
	HashTable();
	~HashTable();

	int setValue(const StringBuffer&, T);
	T getValue(const StringBuffer&) const;

	//	キーと値をまとめたクラス
	template<class U>
	class Key {
	public:
		Key* m_next;
		const StringBuffer m_key;
		U m_value;
		Key(const StringBuffer& key, U value)
			: m_next(NULL), m_key(key), m_value(value)
		{}
	};

private:
	Key<T>* m_keylist[n];
	Key<T>& operator=(const Key<T>&);

	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);
};

template<class T, UINT n>
HashTable<T,n>::HashTable()
{
	::ZeroMemory(m_keylist,sizeof(Key<T>*)*n);
}

template<class T, UINT n>
HashTable<T,n>::~HashTable()
{
	for (UINT i = 0; i < n; i++) {
		if (m_keylist[i] != NULL) {
			Key<T>	*ckey = m_keylist[i], *nkey;
			while (ckey != NULL) {
				nkey = ckey->m_next;
				delete ckey;
				ckey = nkey;
			}
		}
	}
}

template<class T, UINT n>
int
HashTable<T,n>::setValue(const StringBuffer& keyname, T value)
{
	if (keyname.length() <= 0) return -1;
	UINT hash = getHash(keyname,n);
	try {
		Key<T>* newkey = new Key<T>(keyname,value);
		newkey->m_next = m_keylist[hash];
		m_keylist[hash] = newkey;
	} catch (exception&) {
		return -1;
	}
	return hash;
}

template<class T, UINT n>
T
HashTable<T,n>::getValue(const StringBuffer& keyname) const
{
	if (keyname.length() <= 0) return 0;
	UINT hash = getHash(keyname,n);
	Key<T>* key = m_keylist[hash];
	while (key != NULL) {
		if (key->m_key.compareTo(keyname) == 0)
			return key->m_value; // possibly NULL value
		key = key->m_next;
	}
	return 0;
}

#endif

