//	$Id: env_var.cpp,v 1.3 2002-09-26 13:13:24 sugiura Exp $
/*
 *	env_var.cpp
 *	���ϐ��̊Ǘ��Ɋւ���N���X
 */

#include "env_var.h"
#include <assert.h>

EnvManager::EnvManager(const StringBuffer& env_name)
	:	m_env_name(env_name,-1,8), m_enumindex(0)
{
	m_psma = new SMAlloc(env_name);
	if (!m_psma->isShared()) {	//	�V�K�Ƀ}�b�s���O�I�u�W�F�N�g���쐬����
		m_psma->lock();
		loadEnv();	//	�i���I�ϐ��̃��[�h
		m_psma->release();
	}
}

EnvManager::~EnvManager()
{
	if (!m_psma->isShared()) {
		m_psma->lock();
		saveEnv();	//	���L����Ă��Ȃ��c�i���I�ϐ��̕ۑ�
		m_psma->release();
	}
	//	��n��
	delete m_psma;
}

int
EnvManager::count()
{
	m_psma->lock();
	int ret = (int)m_psma->sharedNum();
	m_psma->release();
	return ret;
}

StringBuffer
EnvManager::read(const StringBuffer& env_name)
{
	if (env_name.length() <= 0) return nullStr;
	m_psma->lock();
	//	�ŏ��̕ϐ��� ENV_HEADER �\���̂ւ̃|�C���^���擾
	LPENV_HEADER
		pEnvHeader = (LPENV_HEADER)m_psma->addr(m_psma->getMasterIndex());
	while (m_psma->index(pEnvHeader) != 0) {
		if (env_name.compareTo((LPCSTR)m_psma->addr(pEnvHeader->m_pEnvName))
			== 0) break;	//	�ϐ�����������
		//	���̕ϐ��� ENV_HEADER �ւ̃|�C���^����
		pEnvHeader = (LPENV_HEADER)m_psma->addr(pEnvHeader->m_pNextHeader);
	}
	StringBuffer str = nullStr;
	if (m_psma->index(pEnvHeader) != 0)	//	�������Ă���
		str = (LPCSTR)m_psma->addr(pEnvHeader->m_pEnvString);
	m_psma->release();
	return str;
}

void
EnvManager::write(const StringBuffer& env_name, const StringBuffer& env_var)
{
	int en_len = env_name.length(), ev_len = env_var.length();
	if (en_len <= 0) return;
	m_psma->lock();
	LPENV_HEADER
		pEnvHeader = (LPENV_HEADER)m_psma->addr(m_psma->getMasterIndex()),
		pPrevEnvHeader = (LPENV_HEADER)m_psma->addr(0);
	assert(!IsBadReadPtr(pEnvHeader, sizeof(ENV_HEADER)));
	//	���ɕϐ�����`����Ă��邩�𒲂ׂ�
	while (m_psma->index(pEnvHeader) != 0) {
		LPCSTR name = (LPCSTR)m_psma->addr(pEnvHeader->m_pEnvName);
		assert(!IsBadStringPtr(name, 1));
		if (env_name.compareTo(name) == 0) break;
		pPrevEnvHeader = pEnvHeader;
		pEnvHeader = (LPENV_HEADER)m_psma->addr(pEnvHeader->m_pNextHeader);
		assert(!IsBadReadPtr(pEnvHeader, sizeof(ENV_HEADER)));
	}
	if (m_psma->index(pEnvHeader) == 0) {
		//	�ϐ��͖���`�c�V���ɒ�`����
		int index = m_psma->alloc(sizeof(ENV_HEADER));
		if (index == 0) {
			m_psma->release();
			throw NoMemoryError();
		}
		//	�ϐ��������`����Ă��Ȃ��ꍇ�ւ̑Ώ�
		if (m_psma->index(pPrevEnvHeader) == 0) m_psma->setMasterIndex(index);
		else pPrevEnvHeader->m_pNextHeader = index;
		//	�w�b�_�̏�����
		pEnvHeader = (LPENV_HEADER)m_psma->addr(index);
		pEnvHeader->m_pNextHeader = 0;	//	���̕ϐ��͂܂��Ȃ�
		pEnvHeader->m_pEnvName = m_psma->alloc(en_len + 1);
		if (pEnvHeader->m_pEnvName == 0) {
			m_psma->free(index);
			m_psma->release();
			throw NoMemoryError();
		}
		::CopyMemory((LPSTR)m_psma->addr(pEnvHeader->m_pEnvName),
					(LPCSTR)env_name,en_len + 1);
		pEnvHeader->m_pEnvString = m_psma->alloc(ev_len + 1);
		if (pEnvHeader->m_pEnvString == 0) {
			m_psma->free(pEnvHeader->m_pEnvName);
			m_psma->free(index);
			m_psma->release();
			throw NoMemoryError();
		}
	} else if (lstrlen((LPCSTR)m_psma->addr(pEnvHeader->m_pEnvString))
		!= env_var.length()) {
		//	�ϐ������ɒ�`����Ă���A�l�̃o�b�t�@�T�C�Y���قȂ�
		int newindex = m_psma->alloc(ev_len + 1);
		if (newindex == 0) {
			m_psma->release();
			throw NoMemoryError();
		}
		m_psma->free(pEnvHeader->m_pEnvString);
		pEnvHeader->m_pEnvString = newindex;
	}
	//	�l�̍X�V
	::CopyMemory((LPSTR)m_psma->addr(pEnvHeader->m_pEnvString),
				(LPCSTR)env_var, ev_len + 1);
	m_psma->release();
}

void
EnvManager::del(const StringBuffer& env_name)
{
	if (env_name.length() <= 0) return;
	m_psma->lock();
	LPENV_HEADER
		pEnvHeader = (LPENV_HEADER)m_psma->addr(m_psma->getMasterIndex()),
		pPrevEnvHeader = (LPENV_HEADER)m_psma->addr(0);
	//	���ɕϐ�����`����Ă��邩���ׂ�
	while (m_psma->index(pEnvHeader) != 0) {
		if (env_name.compareTo((LPCSTR)m_psma->addr(pEnvHeader->m_pEnvName))
			== 0) break;
		pPrevEnvHeader = pEnvHeader;
		pEnvHeader = (LPENV_HEADER)m_psma->addr(pEnvHeader->m_pNextHeader);
	}
	if (m_psma->index(pEnvHeader) != 0) {
		//	��������
		if (m_enumindex == m_psma->index(pEnvHeader))	//	enum_env() ��
			m_enumindex = pEnvHeader->m_pNextHeader;
		if (m_psma->index(pPrevEnvHeader) == 0)	//	�ŏ��̕ϐ�������
			m_psma->setMasterIndex(pEnvHeader->m_pNextHeader);
		else
			pPrevEnvHeader->m_pNextHeader = pEnvHeader->m_pNextHeader;
		//	�������̊J��
		m_psma->free(pEnvHeader->m_pEnvName);
		m_psma->free(pEnvHeader->m_pEnvString);
		m_psma->free(m_psma->index(pEnvHeader));
	}
	m_psma->release();
}

BOOL
EnvManager::exist(const StringBuffer& env_name)
{
	if (env_name.length() <= 0) return FALSE;
	m_psma->lock();
	LPENV_HEADER
		pEnvHeader = (LPENV_HEADER)m_psma->addr(m_psma->getMasterIndex());
	while (m_psma->index(pEnvHeader) != 0) {
		if (env_name.compareTo((LPCSTR)m_psma->addr(pEnvHeader->m_pEnvName))
			== 0) break;
		pEnvHeader = (LPENV_HEADER)m_psma->addr(pEnvHeader->m_pNextHeader);
	}
	m_psma->release();
	return m_psma->index(pEnvHeader) != 0;
}

void
EnvManager::enum_env()
{
	m_enumindex = m_psma->getMasterIndex();
}

StringBuffer
EnvManager::next_env()
{
	StringBuffer name = nullStr;
	m_psma->lock();
	if (m_enumindex != 0) {
		LPENV_HEADER pEnvHeader = (LPENV_HEADER)m_psma->addr(m_enumindex);
		name.reset((LPCSTR)m_psma->addr(pEnvHeader->m_pEnvName));
		m_enumindex = pEnvHeader->m_pNextHeader;
	}
	m_psma->release();
	return name;
}

BOOL
EnvManager::hasmore_env()
{
	return m_enumindex != 0;
}

BOOL
EnvManager::loadEnv()
{
	HKEY hDgKey;
	if (::RegOpenKeyEx(
				HKEY_CURRENT_USER,
				ENV_REGKEY_NAME_ROOT,
				0,
				KEY_ALL_ACCESS,
				&hDgKey
			) != ERROR_SUCCESS) {
		//	�i���I�ϐ��͂Ȃ��͂�
		m_psma->setMasterIndex(0);
	} else {
		TCHAR namebuf[MAX_PATH];
		LPENV_HEADER pHeader = NULL;
		DWORD	dwIndex = 0, dwType = REG_SZ,
				dwNameLength = MAX_PATH, dwVarLength = 0;
		while (::RegEnumValue(
						hDgKey,
						dwIndex,
						namebuf,&dwNameLength,
						NULL,&dwType,
						NULL,&dwVarLength
					) != ERROR_NO_MORE_ITEMS) {
			int sma_index = m_psma->alloc(sizeof(ENV_HEADER));
			if (sma_index == 0) break;	//	�������m�ۂɎ��s
			if (dwIndex == 0) m_psma->setMasterIndex(sma_index);
			else pHeader->m_pNextHeader = sma_index;
			pHeader = (LPENV_HEADER)m_psma->addr(sma_index);
			pHeader->m_pNextHeader = 0;
			pHeader->m_pEnvName = m_psma->alloc((int)dwNameLength+1);
			if (pHeader->m_pEnvName == 0) {
				m_psma->free(sma_index);
				break;
			}
			::CopyMemory((LPSTR)m_psma->addr(pHeader->m_pEnvName),
						namebuf,dwNameLength + 1);
			pHeader->m_pEnvString = m_psma->alloc((int)dwVarLength + 1);
			if (pHeader->m_pEnvString == 0) {
				m_psma->free(pHeader->m_pEnvName);
				m_psma->free(sma_index);
				break;
			}
			::RegQueryValueEx(
					hDgKey,namebuf,NULL,&dwType,
					(BYTE*)m_psma->addr(pHeader->m_pEnvString),
					&++dwVarLength
				);
			dwNameLength = MAX_PATH;
			dwVarLength = 0;
			dwIndex++;
		}
	}
	return ::RegCloseKey(hDgKey) == ERROR_SUCCESS;
}

BOOL
EnvManager::saveEnv()
{
	TCHAR namebuf[MAX_PATH];
	namebuf[0] = '\0';
	HKEY hDgKey;
	DWORD dwExist;
	if (::RegCreateKeyEx(
				HKEY_CURRENT_USER,
				ENV_REGKEY_NAME_ROOT,
				0,
				namebuf,
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				NULL,
				&hDgKey,
				&dwExist
			) != ERROR_SUCCESS) throw InvalidObjectNameException();
	//	�����̃��W�X�g���f�[�^�̍폜
	DWORD	dwIndex = 0, dwType = REG_SZ,
			dwNameLength = MAX_PATH, dwVarLength = 0;
	while (::RegEnumValue(
					hDgKey,
					dwIndex,
					namebuf,&dwNameLength,
					NULL,&dwType,
					NULL,&dwVarLength
				) != ERROR_NO_MORE_ITEMS) {
		::RegDeleteValue(hDgKey,namebuf);
		dwNameLength = MAX_PATH;
		dwVarLength = 0;
		dwIndex++;
	}
	//	�i���I�ϐ��̓o�^
	for (LPENV_HEADER
			pHeader = (LPENV_HEADER)m_psma->addr(m_psma->getMasterIndex());
		 m_psma->index(pHeader) != 0;
		 pHeader = (LPENV_HEADER)m_psma->addr(pHeader->m_pNextHeader)) {
		LPCSTR name = (LPCSTR)m_psma->addr(pHeader->m_pEnvName);
		assert(!IsBadStringPtr(name, 1));
		if (*name == '@') {
			//	�i���I�ϐ�
			const BYTE* value = (const BYTE*)m_psma->addr(pHeader->m_pEnvString);
			assert(!IsBadStringPtr((LPCSTR)value, 1));
			::RegSetValueEx(
					hDgKey,
					name,
					0,
					REG_SZ,
					value,
					lstrlen((LPCSTR)value) + 1
				);
		}
	}
	return ::RegCloseKey(hDgKey) == ERROR_SUCCESS;
}

