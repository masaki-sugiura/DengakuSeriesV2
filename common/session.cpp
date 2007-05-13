//	$Id: session.cpp,v 1.6 2007-05-13 17:02:41 sugiura Exp $
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
		m_pFileOpResult(NULL), // �t�@�C������n�֐��̌��ʂ�ێ�����N���X�ւ̃|�C���^
		m_pEnumerator(NULL), // enum* �̌��ʂ�ێ�����N���X�ւ̃|�C���^
		m_pStrToken(NULL), // gettoken �̌��ʂ�ێ�����N���X�ւ̃|�C���^
		m_pMenuThread(NULL), // ���j���[�\���X���b�h�ւ̃|�C���^
		m_pDlgThread(NULL), // �_�C�A���O�\���X���b�h�ւ̃|�C���^
		m_pBRegExp_Manager(NULL)
{
	m_DlgFrame.setSessionInstance(this);

	DWORD dwVersion = GetVersionInfo(hInstance);

	TCHAR buf[16];
	wsprintf(buf, "%d.%d%d%s", (BYTE)(dwVersion >> 24), (BYTE)(dwVersion >> 16), (BYTE)(dwVersion >> 8),
			(BYTE)dwVersion == 0 ? "" : "b");
	m_strVersion = buf;
}

SessionInstance::~SessionInstance()
{
	//	prevent thread stop before closing window
	if (m_pMenuThread.ptr() != NULL) this->stopMenuThread();
	if (m_pDlgThread.ptr() != NULL) this->si_enddialog(0); // no wait
}

