//	$Id: hashtbl.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	hashtbl.cpp
 *	ハッシュ値を生成する関数
 */

#include <windows.h>

//	現在は一番簡単な奴だがいずれは…
UINT
getHash(LPCSTR key, UINT size)
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

