//	$Id: es_func.cpp,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	es_func.cpp
 *	環境変数サービスの関数
 */

#include "DengakuDLL.h"

//	変数をセットする
DENGAKUDLL_API HIDEDLL_NUMTYPE
SETVAR(LPCSTR env_name, LPCSTR env_var)
{
	return g_pSessionInstance->si_setvar(env_name,env_var);
}

//	変数の値を返す
DENGAKUDLL_API LPCSTR
GETVAR(LPCSTR env_name)
{
	*g_pStrBuffer = g_pSessionInstance->si_getvar(env_name);
	return *g_pStrBuffer;
}

//	変数を削除する
DENGAKUDLL_API HIDEDLL_NUMTYPE
DELVAR(LPCSTR env_name)
{
	return g_pSessionInstance->si_delvar(env_name);
}

//	変数が存在するかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
EXISTVAR(LPCSTR env_name)
{
	return g_pSessionInstance->si_existvar(env_name);
}

//	変数の列挙を開始する
DENGAKUDLL_API HIDEDLL_NUMTYPE
ENUMVAR()
{
	return g_pSessionInstance->si_enumvar();
}

//	ENUMVAR() で列挙した変数の名前を返す
DENGAKUDLL_API LPCSTR
NEXTVAR()
{
	*g_pStrBuffer = g_pSessionInstance->si_nextvar();
	return *g_pStrBuffer;
}

//	ENUMVAR(), NEXTVAR() で列挙した変数がまだあるかどうかを返す
DENGAKUDLL_API HIDEDLL_NUMTYPE
HASMOREVARS()
{
	return g_pSessionInstance->si_hasmorevars();
}

