//	$Id: si_variable.cpp,v 1.2 2002/09/26 13:13:24 sugiura Exp $
/*
 *	si_variable.cpp
 *	環境変数サービスの関数
 */

#include "session.h"
#include "cmdline.h"
#include "env_var.h"

static LPCSTR envmngr_name = "DengakuSeries_EnvManager";

int
SessionInstance::si_setvar(
	const StringBuffer& name,
	const StringBuffer& value)
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		m_pEnvManager->write(name, value);
		return TRUE;
	} catch (InvalidObjectNameException&) {
	} catch (NoMemoryError&) {
	}
	return FALSE;
}

StringBuffer
SessionInstance::si_getvar(const StringBuffer& name)
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		return m_pEnvManager->read(name);
	} catch (InvalidObjectNameException&) {
	}
	return nullStr;
}

int
SessionInstance::si_delvar(const StringBuffer& name)
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		m_pEnvManager->del(name);
		return TRUE;
	} catch (InvalidObjectNameException&) {
	}
	return FALSE;
}

int
SessionInstance::si_existvar(const StringBuffer& name)
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		return m_pEnvManager->exist(name);
	} catch (InvalidObjectNameException&) {
	}
	return FALSE;
}

int
SessionInstance::si_enumvar()
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		m_pEnvManager->enum_env();
		return m_pEnvManager->hasmore_env();
	} catch (InvalidObjectNameException&) {
	}
	return FALSE;
}

StringBuffer
SessionInstance::si_nextvar()
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		return m_pEnvManager->next_env();
	} catch (InvalidObjectNameException&) {
	}
	return nullStr;
}

int
SessionInstance::si_hasmorevars()
{
	if (m_pEnvManager.ptr() == NULL)
		m_pEnvManager = new EnvManager(envmngr_name);
	try {
		return m_pEnvManager->hasmore_env();
	} catch (InvalidObjectNameException&) {
	}
	return FALSE;
}

