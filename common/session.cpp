//	$Id: session.cpp,v 1.3 2002-01-16 15:57:23 sugiura Exp $
/*
 *	session.cpp
 *	SessionInstance �N���X�̃R�A�����̎���
 */

#include "session.h"

SessionInstance::SessionInstance(HINSTANCE hInstance, BOOL bTrueCD)
	:	m_hInstance(hInstance),	//	�C���X�^���X�n���h��
		m_dwLastError(0L), // DdeExecute() �� LastError
		m_DirList(bTrueCD), // DirList �N���X
		m_pSPIManager(NULL), // SPI_Manager �N���X�ւ̃|�C���^
		m_pEnvManager(NULL), // EnvManager �N���X�ւ̃|�C���^
		m_pFindData(NULL), // enum* �̌��ʂ�ێ�����N���X�ւ̃|�C���^
		m_pStrToken(NULL), // gettoken �̌��ʂ�ێ�����N���X�ւ̃|�C���^
		m_pMenuThread(NULL), // ���j���[�\���X���b�h�ւ̃|�C���^
		m_pDlgThread(NULL), // �_�C�A���O�\���X���b�h�ւ̃|�C���^
		m_pBRegExp_Manager(NULL)
{
	m_DlgFrame.setSessionInstance(this);
}

SessionInstance::~SessionInstance()
{
	//	prevent thread stop before closing window
	if (m_pMenuThread.ptr() != NULL) this->stopMenuThread();
	if (m_pDlgThread.ptr() != NULL) this->si_enddialog(0); // no wait
}

