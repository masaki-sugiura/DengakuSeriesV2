//	$Id: hashtbl.cpp,v 1.2 2002-03-29 17:11:59 sugiura Exp $
/*
 *	hashtbl.cpp
 *	ハッシュ値を生成する関数
 */

#include "hashtbl.h"

RawHashTable::RawHashTable(int n)
	: m_keylist(n)
{
	m_keylist.zero();
}

RawHashTable::~RawHashTable()
{
	int n = m_keylist.size();
	for (int i = 0; i < n; i++) {
		Key* ckey = m_keylist[i];
		while (ckey != NULL) {
			Key* nkey = ckey->m_next;
			delete ckey;
			ckey = nkey;
		}
	}
}

int
RawHashTable::setValue_(const StringBuffer& keyname, LPVOID value)
{
	if (keyname.length() <= 0) return -1;
	int hash = (int)getHash(keyname, m_keylist.size());
	Key* newkey = new Key(keyname, value);
	newkey->m_next = m_keylist[hash];
	m_keylist[hash] = newkey;
	return hash;
}

LPVOID
RawHashTable::getValue_(const StringBuffer& keyname) const
{
	if (keyname.length() <= 0) return 0;
	int hash = (int)getHash(keyname, m_keylist.size());
	Key* key = m_keylist[hash];
	while (key != NULL) {
		if (key->m_key.compareTo(keyname) == 0)
			return key->m_value; // possibly NULL value
		key = key->m_next;
	}
	return 0;
}

//	現在は一番簡単な奴だがいずれは…
UINT
RawHashTable::getHash(LPCSTR key, UINT size)
{
	ULONG hash = 0, g;

	while (*key != '\0') {
		hash = (hash << 4) + *key++;
		if ((g = hash & 0xF0000000) != 0) {
			hash ^= (g >> 24);
			hash ^= g;
		}
	}
	return hash % size;
}

