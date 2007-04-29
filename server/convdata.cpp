// $Id: convdata.cpp,v 1.7 2007-04-29 16:11:35 sugiura Exp $
/*
 *	convdata.cpp
 *	ConvData �N���X�̃R�A�����̎���
 */

#include "misc.h"
#include "convdata.h"
#include "ddeserv.h"
#include "cmdline.h"
#include "common.h"

ConvData::ConvData(
	HINSTANCE hInstance,
	DWORD ddeInst,
	HCONV hconv,
	const StringBuffer &strTopicName)
	:	SessionInstance(hInstance,FALSE), // ���N���X
		m_ddeInst(ddeInst), // DDE �T�[�o�̃C���X�^���X�n���h��
		m_hConv(hconv), // �ʐM�n���h��
		m_sbKeepName(nullStr), // KeepAlive ���̎��ʖ�
		m_dsAdvTopicName(ddeInst,strTopicName), // �g�s�b�N��
		m_dsAdvItemName(ddeInst,"dlgresult"), // �A�h�o�C�X���[�v�A�C�e����
		m_dwLastError(0L), // DdeExecute() �� LastError
		m_pComDlgThread(NULL) // �R�����_�C�A���O�̃X���b�h�ւ̃|�C���^
{
	// �A�h�o�C�X�f�[�^�ւ̃A�N�Z�X�̓����I�u�W�F�N�g
	m_hAdvEvent	= ::CreateEvent(NULL,TRUE,FALSE,NULL);
}

ConvData::~ConvData()
{
	//	��n��
	::CloseHandle(m_hAdvEvent);
}

//	�A�h�o�C�X���[�v�̊J�n
HDDEDATA
ConvData::ddeAdvStart(HSZ hsz1, HSZ hsz2)
{
	DEBUG_OUTPUT(("Enter ddeAdvStart()"));
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) {
		DdeString ds1(m_ddeInst, hsz1), ds2(m_ddeInst, hsz2);
		DEBUG_OUTPUT(("topic = %s, item = %s", (LPCSTR)ds1, (LPCSTR)ds2));
		return HDDE_FALSE;
	}
	::SetEvent(m_hAdvEvent); // setAdvData() �̌ďo��������
	DEBUG_OUTPUT(("Leave ddeAdvStart()"));
	return HDDE_TRUE;
}

//	�A�h�o�C�X���[�v�̒�~
HDDEDATA
ConvData::ddeAdvStop(HSZ hsz1, HSZ hsz2)
{
	DEBUG_OUTPUT(("Enter ddeAdvStop()"));
	if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
		m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_FALSE;
	::ResetEvent(m_hAdvEvent); // setAdvData() �̌ďo�����֎~
	DEBUG_OUTPUT(("Leave ddeAdvStop()"));
	return HDDE_TRUE;
}

//	�A�h�o�C�X�f�[�^�̃Z�b�g
void
ConvData::setNotify(const StringBuffer& data)
{
	//	ddeAdvStart() ���Ă΂��܂őҋ@
	::WaitForSingleObject(m_hAdvEvent,INFINITE);
	try {
		//	�A�h�o�C�X�f�[�^�̃Z�b�g
		this->SessionInstance::setNotify(data);
		//	�f�[�^���Z�b�g���ꂽ���Ƃ��N���C�A���g�ɒʒm
		//	�����̌��Q�� DdeString::operator HSZ() ���Ă�ł���
		::PostMessage(DdeServer::gethwndDlg(),WM_USER_NOTIFYPOSTADV,
						0,(LPARAM)this);
	} catch (...) {
		// nothing to do
	}
}

//	�A�h�o�C�X�f�[�^���N���C�A���g�ɓn��
HDDEDATA
ConvData::ddeAdvReq(HSZ hsz1, HSZ hsz2)
{
	try {
		if (m_dsAdvTopicName.compareTo(hsz1) != 0 ||
			m_dsAdvItemName.compareTo(hsz2)  != 0) return HDDE_NULL;
		//	�ʒm�R�[�h�̎擾
		StringBuffer buf;
		this->getNotify(buf,INFINITE);
		//	���j���[�X���b�h�̒�~�c���������^�C�~���O���Ȃ��̂��Ȃ��c�H�H
		if (m_pMenuThread.ptr() != NULL) this->stopMenuThread();
		if (m_pComDlgThread.ptr() != NULL) {
			m_pComDlgThread->stop();
			m_pComDlgThread = NULL;
		}
		//	�N���C�A���g�ɓn�� DDE �f�[�^���쐬
		//	SendingDdeData �͔j������ DdeFreeDataHandle() ���Ă΂Ȃ�
		//	(���烍�[�J���I�u�W�F�N�g�ł��悢)
		DEBUG_OUTPUT(("Enter Create DDE Data"));
		HDDEDATA hData = SendingDdeData(m_ddeInst,
							m_dsAdvItemName.getHandle(),
							buf).getHandle();
		DEBUG_OUTPUT(("Leave Create DDE Data : hData = %08x", hData));

		return hData;
	} catch (...) {
		return HDDE_NULL;
	}
}

//	DDE Execute �R�}���h�̎��s
HDDEDATA
ConvData::ddeExecute(PFNDDECMD_EXECUTE pfnCmd, CmdLineParser& argv)
{
	try {
		m_dwLastError = 0L;
		::SetLastError(m_dwLastError);
		HDDEDATA ret = (this->*pfnCmd)(argv) ? HDDE_FACK : HDDE_FNOTPROCESSED;
		m_dwLastError = ::GetLastError();

		return ret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

//	DDE Request �R�}���h�̎��s
HDDEDATA
ConvData::ddeRequest(
	PFNDDECMD_REQUEST pfnCmd,
	CmdLineParser& argv,
	const DdeString& dsItem)
{
	try {
		//	SendingDdeData �͔j������ DdeFreeDataHandle() ���Ă΂Ȃ�
		//	(���烍�[�J���I�u�W�F�N�g�ł��悢)
		return SendingDdeData(m_ddeInst,
							dsItem.getHandle(),
							(this->*pfnCmd)(argv)).getHandle();
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

//	DDE Poke �R�}���h�̎��s
HDDEDATA
ConvData::ddePoke(PFNDDECMD_POKE pfnCmd, CmdLineParser& argv)
{
	try {
		return (this->*pfnCmd)(argv) ? HDDE_FACK : HDDE_FNOTPROCESSED;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

