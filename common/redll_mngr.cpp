//	DengakuServer v1: Copyright(C) 1999-2000, M. Sugiura
/*
 *	redll_mngr.cpp
 *	REDLL_Manager クラスの実装
 */

#include "redll_mngr.h"

REDLL_Manager::REDLL_Manager(const StringBuffer& filename)
{
	m_hModuleDll = ::LoadLibrary(filename);
	if (m_hModuleDll == NULL) throw DllNotFoundError();
}

REDLL_Manager::~REDLL_Manager()
{
	// DLL のアンロードの順序によりアクセス違反が起こる可能性あり
	if (m_hModuleDll) {
		::FreeLibrary(m_hModuleDll);
	}
}

