//	$Id: es_func.cpp,v 1.3 2002-02-19 15:34:22 sugiura Exp $
/*
 *	es_func.cpp
 *	環境変数サービスの関数
 */

#include "DengakuDLL.h"

//	変数をセットする
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETVAR(LPCSTR env_name, LPCSTR env_var)
{
	try {
		return g_pSessionInstance->si_setvar(env_name,env_var);
	} catch (...) {
		return 0;
	}
}

//	変数の値を返す
DENGAKUDLL_API LPCSTR
GETVAR(LPCSTR env_name)
{
	try {
		g_strBuffer = g_pSessionInstance->si_getvar(env_name);
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	変数を削除する
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELVAR(LPCSTR env_name)
{
	try {
		return g_pSessionInstance->si_delvar(env_name);
	} catch (...) {
		return 0;
	}
}

//	変数が存在するかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
EXISTVAR(LPCSTR env_name)
{
	try {
		return g_pSessionInstance->si_existvar(env_name);
	} catch (...) {
		return 0;
	}
}

//	変数の列挙を開始する
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMVAR()
{
	try {
		return g_pSessionInstance->si_enumvar();
	} catch (...) {
		return 0;
	}
}

//	ENUMVAR() で列挙した変数の名前を返す
DENGAKUDLL_API LPCSTR
NEXTVAR()
{
	try {
		g_strBuffer = g_pSessionInstance->si_nextvar();
		return g_strBuffer;
	} catch (...) {
		return "";
	}
}

//	ENUMVAR(), NEXTVAR() で列挙した変数がまだあるかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
HASMOREVARS()
{
	try {
		return g_pSessionInstance->si_hasmorevars();
	} catch (...) {
		return 0;
	}
}

