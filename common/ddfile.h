//	$Id: ddfile.h,v 1.2.2.1 2005-07-06 14:20:48 sugiura Exp $
/*
 *	ddfile.h
 *	ダイアログデータのデータファイルを扱うクラス
 */

#ifndef	DENGAKUSERIES_CLASSES_DDFILE
#define	DENGAKUSERIES_CLASSES_DDFILE

#include "strbuf.h"

class PathName;

class DlgDataFile {
public:
	DlgDataFile(const PathName&);

	BOOL isValid();
	void reset();

	void setSecName(const StringBuffer&);

	//	2nd and 3rd args of all of functions are keyname and secname.
	BOOL read(int*, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL read(WORD*, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL read(DWORD*, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL read(StringBuffer&, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(int, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(WORD, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(DWORD, const StringBuffer&, const StringBuffer& sec = nullStr);
	BOOL write(const StringBuffer&, const StringBuffer&, const StringBuffer& sec = nullStr);

	//	2nd + 3rd and 4th args of all of functions are keyname and secname.
#if 1	// 0 にするとdlgdata.cppで内部コンパイルエラー...？？
	BOOL read(int* pnData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = read(pnData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL read(WORD* pwData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = read(pwData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL read(DWORD* pdwData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = read(pdwData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL read(StringBuffer& strData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = read(strData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL write(int nData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = write(nData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL write(WORD wData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = write(wData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL write(DWORD dwData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = write(dwData, key, sec);
		key.setlength(len);
		return bRet;
	}
	BOOL write(const StringBuffer& strData, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = write(strData, key, sec);
		key.setlength(len);
		return bRet;
	}
#else
	template<class T>
	BOOL read(T val, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = read(val, key, sec);
		key.setlength(len);
		return bRet;
	}
	template<class T>
	BOOL write(T val, StringBuffer& key, const StringBuffer& subkey, const StringBuffer& sec = nullStr)
	{
		int len = key.length();
		key.append(subkey);
		BOOL bRet = write(val, key, sec);
		key.setlength(len);
		return bRet;
	}
#endif

private:
	const StringBuffer m_filename;
	StringBuffer m_secname;

	DlgDataFile(const DlgDataFile&);
	DlgDataFile& operator=(const DlgDataFile&);
};

#endif
