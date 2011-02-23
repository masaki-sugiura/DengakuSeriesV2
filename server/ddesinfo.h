//	$Id: ddesinfo.h,v 1.1.1.1 2001/10/07 14:41:22 sugiura Exp $
/*
 *	ddesinfo.h
 *	�v���Z�X�EDDE �T�[�o�̏����W�߂��N���X(�Ƃ�����茫���\����(^^;)
 */

#ifndef DENGAKUSERVER_CLASSES_DDESINFO
#define DENGAKUSERVER_CLASSES_DDESINFO

#include "strbuf.h"

class DDEServerInfo {
public:
	HINSTANCE m_hInst;	//	���W���[���C���X�^���X�n���h��
	HWND m_hwndMain;	//	���C���E�B���h�E�̃E�B���h�E�n���h��
	BOOL m_bStrictEmulation;	//	�}�N���T�[�o�݊����[�h���ǂ���
	BOOL m_bShareResources;	//	�P�� ConvData �I�u�W�F�N�g�����L���邩�ǂ���
	BOOL m_bHideIcon;	//	�A�C�R������\�����ǂ���
	NOTIFYICONDATA m_NotifyIconData;	//	�A�C�R����o�^���邽�߂̍\����
	StringBuffer m_sbServiceName;	//	��{�T�[�r�X��
	StringBuffer m_sbMutexName;	//	�����N����}�����邽�߂�
								//	�~���[�e�b�N�X�I�u�W�F�N�g��
	StringBuffer m_sbWndTitle;		//	���C���E�B���h�E�̃^�C�g��
	StringBuffer m_sbModuleName;	//	���s�t�@�C����
	StringBuffer* m_psbServerLogFileName;	//	�T�[�o�̃��O�t�@�C����

	DDEServerInfo(HINSTANCE hInst)
		:	m_hInst(hInst),
			m_hwndMain(NULL),
			m_bStrictEmulation(FALSE),
			m_bShareResources(FALSE),
			m_bHideIcon(FALSE),
			m_sbServiceName(nullStr),
			m_sbMutexName(nullStr),
			m_sbWndTitle(nullStr),
			m_sbModuleName(nullStr),
			m_psbServerLogFileName(NULL)
	{
		::ZeroMemory(&m_NotifyIconData,sizeof(NOTIFYICONDATA));
	}

	~DDEServerInfo()
	{
		delete m_psbServerLogFileName;
	}
};

#endif

