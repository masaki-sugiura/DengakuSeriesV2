// $Id: convdata.h,v 1.2 2002-02-20 16:48:40 sugiura Exp $
/*
 *	convdata.h
 *	DDE �ʐM���ɗp�ӂ��郊�\�[�X�Ǘ��ƃR�}���h�������s���N���X�̒�`
 */

#ifndef DENGAKUSERVER_CLASSES_CONVDATA
#define DENGAKUSERVER_CLASSES_CONVDATA

#include "session.h"
#include "ddeobjs.h"
#include "opencomdlg.h"

class ConvData;
class CmdLineParser;

//	DDE �R�}���h���������� ConvData::On_??() �ւ̃|�C���^
typedef	int (ConvData::*PFNDDECMD_EXECUTE)(CmdLineParser&);
typedef StringBuffer (ConvData::*PFNDDECMD_REQUEST)(CmdLineParser&);
typedef PFNDDECMD_EXECUTE PFNDDECMD_POKE;

// �X�� DDE �ʐM����������N���X
class ConvData : public SessionInstance {
public:
	ConvData(HINSTANCE, DWORD, HCONV, const StringBuffer&);
	~ConvData();

	//	�����o�擾���\�b�h
	HCONV gethConv() const { return m_hConv; }
	void sethConv(HCONV hconv) { m_hConv = hconv; }

	const DdeString& getAdvTopicName() const { return m_dsAdvTopicName; }
	const DdeString& setAdvTopicName(HSZ hszTopicName)
	{
		return (m_dsAdvTopicName = DdeString(m_ddeInst,hszTopicName));
	}
	const DdeString& getAdvItemName() const { return m_dsAdvItemName; }

	//	�I�u�W�F�N�g�̎����̐���
	BOOL isKeepAlive() const { return m_sbKeepName.length() > 0; }
	const StringBuffer&	getKeepName() const { return m_sbKeepName; }
	void setKeepName(const StringBuffer &kn) { m_sbKeepName = kn; }

	//	�A�h�o�C�X���[�v����
	HDDEDATA ddeAdvStart(HSZ, HSZ);
	HDDEDATA ddeAdvStop(HSZ, HSZ);
	HDDEDATA ddeAdvReq(HSZ, HSZ);

	void setNotify(const StringBuffer&);

	//	�R�}���h���s
	HDDEDATA ddeExecute(PFNDDECMD_EXECUTE, CmdLineParser&);
	HDDEDATA ddeRequest(PFNDDECMD_REQUEST, CmdLineParser&, const DdeString&);
	HDDEDATA ddePoke(PFNDDECMD_POKE, CmdLineParser&);

	//	�R�}���h�n���h��
#define	DECLARE_DDECMD_HEADER
#define	DECLARE_DDECMD_EXECUTE(name) int On_##name (CmdLineParser&);
#define	DECLARE_DDECMD_REQUEST(name) StringBuffer On_##name (CmdLineParser&);
#define	DECLARE_DDECMD_POKE(name)    int On_##name (CmdLineParser&);
#define	DECLARE_DDECMD_FINAL
#define	DECLARE_DDECMD_FOOTER
#include "cmd.h"
#undef	DECLARE_DDECMD_HEADER
#undef	DECLARE_DDECMD_EXECUTE
#undef	DECLARE_DDECMD_REQUEST
#undef	DECLARE_DDECMD_POKE
#undef	DECLARE_DDECMD_FINAL
#undef	DECLARE_DDECMD_FOOTER

private:
	DWORD m_ddeInst; //	DDE �T�[�o�̃C���X�^���X�n���h��
	HCONV m_hConv; // �ʐM�n���h��
	StringBuffer m_sbKeepName; // KeepAlive ���̃I�u�W�F�N�g���ʖ�
	DdeString m_dsAdvTopicName; // �A�h�o�C�X���[�v�̃g�s�b�N��
	DdeString m_dsAdvItemName; // �A�h�o�C�X���[�v�̃A�C�e����
	HANDLE m_hAdvEvent;	// �A�h�o�C�X�f�[�^�̃A�N�Z�X���������
						// �C�x���g�I�u�W�F�N�g�̃n���h��
	DWORD m_dwLastError; //	�Ō�� Execute �R�}���h�̃G���[�l

	Auto_Ptr<Thread> m_pComDlgThread; // �R�����_�C�A���O��\������X���b�h

	ConvData(const ConvData&);
	ConvData& operator=(const ConvData&);

	int do_opencomdlg(CmdLineParser&, PFNOPENCOMDLGPROC);
};

// �R�����_�C�A���O�A���j���[�̐e�E�B���h�E�̍쐬���R�}���h���s
BOOL CreateDummyDlg(DLGPROC, LONG, WPARAM, LPARAM);

#endif

