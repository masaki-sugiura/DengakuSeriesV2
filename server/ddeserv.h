// $Id: ddeserv.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	ddeserv.h
 *	DDE �T�[�o�{�̂̃N���X��`
 */

#ifndef DENGAKUSERVER_CLASSES_DDESERVER
#define DENGAKUSERVER_CLASSES_DDESERVER

#include "session.h"
#include "resource.h"
#include "ddeobjs.h"
#include "convdata.h"
#include "cmdname.h"
#include "ctrlname.h"
#include "file.h"
#include "common.h"

class DDEServerInfo;
class CmdLineParser;

//	�����������Ƃ͂����Ƃ���Ƃ��ė~���� :-< ���l��
//	HDDE_FALSE, HDDE_NULL, HDDE_FNOTPROCESSED (= 0) ��
//	�Ȃ��Ă�����͌����Ȃ����ǁc(^^;
#define	HDDE_TRUE			(reinterpret_cast<HDDEDATA>(TRUE))
#define	HDDE_FALSE			(reinterpret_cast<HDDEDATA>(FALSE))
#define	HDDE_FACK			(reinterpret_cast<HDDEDATA>(DDE_FACK))
#define	HDDE_FNOTPROCESSED	(reinterpret_cast<HDDEDATA>(DDE_FNOTPROCESSED))
#define	HDDE_NULL			(static_cast<HDDEDATA>(NULL))

//	���b�Z�[�W�n���h���̃p�����[�^���X�g
#define MH_ARGS (const HCONV, const HSZ, const HSZ, const HDDEDATA)

/*
	DDE �T�[�o�̎��̂ƂȂ�N���X

	DdeServer �N���X�̓N���X�����o�ƃN���X���\�b�h�݂̂��琬��B
*/
class DdeServer {
public:
	//	�T�[�o�̏�����/�I���A�J�n/��~
	static BOOL init(DDEServerInfo&); // DDE �T�[�o�̏�����
	static BOOL uninit(); // DDE �T�[�o�̏I������

	//	��Ԏ擾���\�b�h
	static BOOL isActive() { return m_bActive; }
	static BOOL isStrictEmulation() { return m_bStrictEmulation; }

	//	�g�s�b�N�����T�|�[�g���ꂽ���̂��ǂ���
	static BOOL isSupportedTopic(const HSZ);

	//	�����o�l�擾���\�b�h
	static DWORD getddeInst() { return m_ddeInst; }
	static HINSTANCE gethInst() { return m_hInstance; }
	static HWND gethwndDlg() { return m_hwndDlg; }
	static const DdeString& getServiceName()
	{
		return *m_pdsServiceName;
	}

	//	ConvData �N���X���������\�b�h
	static int getConvNum()
	{
		return m_ConvDataList.itemNum();
	}
	static int addConvData(const HCONV hconv, const HSZ hszTn);
	static void deleteConvData(ConvData* pCd = NULL);
	static ConvData* getConvData(const HCONV);
	static ConvData* isStoredCD(const HSZ);

	static void notifyPostAdvise(ConvData* pCd);

	//	DdemlCallback �֐��̃��b�Z�[�W�n���h���̎���(���O����)
	static HDDEDATA regist MH_ARGS;
	static HDDEDATA unregist MH_ARGS;
	static HDDEDATA connect MH_ARGS;
	static HDDEDATA connectconfirm MH_ARGS;
	static HDDEDATA error MH_ARGS;
	static HDDEDATA disconnect MH_ARGS;
	static HDDEDATA execute MH_ARGS;
	static HDDEDATA request MH_ARGS;
	static HDDEDATA poke MH_ARGS;
	static HDDEDATA advstart MH_ARGS;
	static HDDEDATA advstop MH_ARGS;
	static HDDEDATA advreq MH_ARGS;

	//	DdemlCallback �֐��̃��b�Z�[�W�n���h���̎���(���O�L��)
	static HDDEDATA lregist MH_ARGS;
	static HDDEDATA lunregist MH_ARGS;
	static HDDEDATA lconnect MH_ARGS;
	static HDDEDATA lconnectconfirm MH_ARGS;
	static HDDEDATA lerror MH_ARGS;
	static HDDEDATA ldisconnect MH_ARGS;
	static HDDEDATA lexecute MH_ARGS;
	static HDDEDATA lrequest MH_ARGS;
	static HDDEDATA lpoke MH_ARGS;
	static HDDEDATA ladvstart MH_ARGS;
	static HDDEDATA ladvstop MH_ARGS;
	static HDDEDATA ladvreq MH_ARGS;

	static BOOL isLogging(); // ���O���L�����ǂ���

private:
	static BOOL m_bActive; // �T�[�o�����쒆���ǂ���
	static BOOL m_bStrictEmulation; // �}�N���T�[�o�݊����[�h���ǂ���
	static HINSTANCE m_hInstance; // ���W���[���C���X�^���X�n���h��
	static DWORD m_ddeInst; // DDE �T�[�o�̃C���X�^���X�n���h��
	static HWND m_hwndDlg; // ���C���E�B���h�E�̃n���h��
	static Auto_Ptr<DdeString> m_pdsServiceName; // ��{�T�[�r�X��
	static const StringBuffer m_sbTopicNames; // �g�s�b�N���̃��X�g
	static LinkList<ConvData> m_ConvDataList; // DDE �ʐM�f�[�^�̃��X�g
	static Auto_Ptr<File> m_pfLogFile; // ���O�t�@�C��
	static Auto_Ptr<StringBuffer> m_psbLogFileName; // ���O�t�@�C����

	//	�R�}���h��->�֐��A�h���X���擾���郁�\�b�h
	static PFNDDECMD_EXECUTE getExecCmd(const StringBuffer& name);
	static PFNDDECMD_REQUEST getReqCmd(const StringBuffer& name);
	static PFNDDECMD_POKE getPokeCmd(const StringBuffer& name);

	//	���O�t�@�C���֘A�̃��\�b�h
	static BOOL startLog(const StringBuffer*); // ���O�t�@�C���̍쐬
	static void stopLog(); // ���O�t�@�C�������
	static void writeLog(LPCSTR); // ���O�Ƀf�[�^����������
	//	����̒ʐM�̌��ʂ����O�ɏ�������
	static void writeLog(const HCONV, LPCSTR);
	static void writeArgvToLog(const HCONV, CmdLineParser&, StringBuffer&);
};

#endif

