// $Id: ddeserv.cpp,v 1.2 2002-02-19 15:34:22 sugiura Exp $
/*
 *	ddeserv.cpp
 *	DdeServer �N���X�̎���
 */

#include "common.h"
#include "ddeserv.h"
#include "ddesinfo.h"
#include "ddeobjs.h"
#include "cmdline.h"
#include "pdparser.h"

//	�N���X�����o�̏�����
BOOL DdeServer::m_bActive;
BOOL DdeServer::m_bStrictEmulation;
HINSTANCE DdeServer::m_hInstance;
HWND DdeServer::m_hwndDlg;
DWORD DdeServer::m_ddeInst;
Auto_Ptr<File> DdeServer::m_pfLogFile(NULL);
Auto_Ptr<StringBuffer> DdeServer::m_psbLogFileName(NULL);
Auto_Ptr<DdeString> DdeServer::m_pdsServiceName(NULL);
LinkList<ConvData> DdeServer::m_ConvDataList;
const StringBuffer
DdeServer::m_sbTopicNames(GetServerString(SVR_STR_TOPICNAMES));

//	Ddeml ���C�u�������g�p���� DDE �ʐM����������R�[���o�b�N�֐�
HDDEDATA CALLBACK
ddeCallBack(UINT uType, UINT uFmt, HCONV hconv,
			HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
			DWORD dwData1, DWORD dwData2)
{
	switch (uType) {
	case XTYP_REGISTER: // DDE �T�[�o�̊J�n
		return DdeServer::regist(hconv,hsz1,hsz2,hdata);

	case XTYP_UNREGISTER: // DDE �T�[�o�̏I��
		return DdeServer::unregist(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT: // DDE �N���C�A���g����̐ڑ��v��
		return DdeServer::connect(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT_CONFIRM: // DDE �ʐM�̊m��
		//	�ʐM�f�[�^���X�g�ɓo�^
		return DdeServer::connectconfirm(hconv,hsz1,hsz2,hdata);

	case XTYP_ERROR: // DDE �ʐM�G���[
		return DdeServer::error(hconv,hsz1,hsz2,hdata);

	case XTYP_DISCONNECT: // ����� DDE �ʐM�̏I��
		return DdeServer::disconnect(hconv,hsz1,hsz2,hdata);

	case XTYP_EXECUTE: // DDE Execute �R�}���h�̎��s
		return DdeServer::execute(hconv,hsz1,hsz2,hdata);

	case XTYP_REQUEST: // DDE Request �R�}���h�̎��s
		return DdeServer::request(hconv,hsz1,hsz2,hdata);

	case XTYP_POKE: // DDE Poke �R�}���h�̎��s
		return DdeServer::poke(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTART: // Advice Loop �̊J�n
		return DdeServer::advstart(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTOP: // Advice Loop �̏I��
		return DdeServer::advstop(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVREQ: // Advice Data �̎擾�v��
		return DdeServer::advreq(hconv,hsz1,hsz2,hdata);

	default:
		break;
	}
	return HDDE_NULL;
}

//	Ddeml ���C�u�������g�p���� DDE �ʐM����������R�[���o�b�N�֐�(���O����)
HDDEDATA CALLBACK
ddeCallBack_with_Log(
	UINT uType,
	UINT uFmt,
	HCONV hconv,
	HSZ hsz1,
	HSZ hsz2,
	HDDEDATA hdata,
	DWORD dwData1,
	DWORD dwData2)
{
	switch (uType) {
	case XTYP_REGISTER: // DDE �T�[�o�̊J�n
		return DdeServer::lregist(hconv,hsz1,hsz2,hdata);

	case XTYP_UNREGISTER: // DDE �T�[�o�̏I��
		return DdeServer::lunregist(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT: // DDE �N���C�A���g����̐ڑ��v��
		return DdeServer::lconnect(hconv,hsz1,hsz2,hdata);

	case XTYP_CONNECT_CONFIRM: // DDE �ʐM�̊m��
		//	�ʐM�f�[�^���X�g�ɓo�^
		return DdeServer::lconnectconfirm(hconv,hsz1,hsz2,hdata);

	case XTYP_ERROR: // DDE �ʐM�G���[
		return DdeServer::lerror(hconv,hsz1,hsz2,hdata);

	case XTYP_DISCONNECT: // ����� DDE �ʐM�̏I��
		return DdeServer::ldisconnect(hconv,hsz1,hsz2,hdata);

	case XTYP_EXECUTE: // DDE Execute �R�}���h�̎��s
		return DdeServer::lexecute(hconv,hsz1,hsz2,hdata);

	case XTYP_REQUEST: // DDE Request �R�}���h�̎��s
		return DdeServer::lrequest(hconv,hsz1,hsz2,hdata);

	case XTYP_POKE: // DDE Poke �R�}���h�̎��s
		return DdeServer::lpoke(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTART: // Advice Loop �̊J�n
		return DdeServer::ladvstart(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVSTOP: // Advice Loop �̏I��
		return DdeServer::ladvstop(hconv,hsz1,hsz2,hdata);

	case XTYP_ADVREQ: // Advice Data �̎擾�v��
		return DdeServer::ladvreq(hconv,hsz1,hsz2,hdata);

	default:
		break;
	}
	return HDDE_NULL;
}

//	DDE �T�[�o�̏�����
BOOL
DdeServer::init(DDEServerInfo& dsi)
{
	if (m_ddeInst != 0) return m_bActive;

	//	���̑��̃����o�̐ݒ�
	m_hInstance = dsi.m_hInst;
	m_hwndDlg = dsi.m_hwndMain;
	m_bStrictEmulation = dsi.m_bStrictEmulation;

	//	Ddeml ���C�u������ ddeCallBack �֐���o�^
	if (startLog(dsi.m_psbServerLogFileName)) {
		//	���O����
		if (::DdeInitialize(&m_ddeInst,ddeCallBack_with_Log,
							APPCLASS_STANDARD,0) != DMLERR_NO_ERROR) {
			stopLog();
			return FALSE;
		}
	} else {
		//	���O�Ȃ�
		if (::DdeInitialize(&m_ddeInst,ddeCallBack,
							APPCLASS_STANDARD,0) != DMLERR_NO_ERROR) {
			return FALSE;
		}
	}

	try {
		m_pdsServiceName = new DdeString(m_ddeInst,dsi.m_sbServiceName);
	} catch (...) {
		return FALSE;
	}

	//	Ddeml ���C�u�����Ɋ�{�T�[�r�X����o�^(DDE �T�[�o�̊J�n)
	m_bActive = (BOOL)::DdeNameService(m_ddeInst,
										m_pdsServiceName->getHandle(),
										0L,DNS_REGISTER);

	return m_bActive;
}

//	DDE �T�[�o�̏I��
BOOL
DdeServer::uninit()
{
	if (m_ddeInst == 0) return TRUE;

	if (m_bActive) {
		try {
			//	�A�N�e�B�u�ȒʐM��S�Ĕj��
			if (m_ConvDataList.itemNum() > 0)
				DdeServer::deleteConvData(NULL);
		} catch (...) {
			// nothing to do
		}

		//	Ddeml ���C�u���������{�T�[�r�X���̓o�^���폜
		::DdeNameService(m_ddeInst,
						m_pdsServiceName->getHandle(),
						0L,DNS_UNREGISTER);
		m_bActive = FALSE;
	}

	//	Ddeml ���C�u��������R�[���o�b�N�֐��̓o�^���폜
	BOOL bRet = ::DdeUninitialize(m_ddeInst);
	m_ddeInst = 0;

	//	���O�̒�~
	stopLog();

	return bRet;
}

//	�g�s�b�N�������������ǂ���
BOOL
DdeServer::isSupportedTopic(const HSZ hszTopicName)
{
	return m_sbTopicNames.find(DdeString(m_ddeInst,hszTopicName)) >= 0;
}

ConvData*
DdeServer::isStoredCD(const HSZ topicname)
{
	try {
		DdeString dsTopicName(m_ddeInst,topicname);
		const StringBuffer& sbTn = (const StringBuffer&)dsTopicName;
		m_ConvDataList.initSequentialGet();
		ConvData* cd;
		while ((cd = m_ConvDataList.getNextItem()) != NULL) {
			if (sbTn.compareTo(cd->getKeepName()) == 0) return cd;
		}
	} catch (...) {
		// nothing to do
	}
	return NULL;
}

//	�ʐM�N���X�̐����A�o�^
int
DdeServer::addConvData(const HCONV hconv, const HSZ hszTn)
{
	ConvData* pCd = isStoredCD(hszTn);
	int	ret = -1;
	if (pCd) {
//		if (pCd->gethConv()) {}	//	Share? or not share?
		pCd->sethConv(hconv);
		pCd->setAdvTopicName(hszTn);
		ret = m_ConvDataList.itemNum();
	} else {
		try {
			pCd = new ConvData(m_hInstance,
								m_ddeInst,
								hconv,
								DdeString(m_ddeInst,hszTn));
		} catch (...) {
			return -1;
		}
		ret = m_ConvDataList.addItem(pCd);
	}
	//	getConvData() �̂��߁A�ʐM�f�[�^�Ƀ|�C���^��o�^
	::DdeSetUserHandle(hconv,QID_SYNC,reinterpret_cast<DWORD>(pCd));
	return ret;
}

static void
DelCD(ConvData* pCd, LinkList<ConvData>& cdlist)
{
	pCd->setKeepName(nullStr);
	HCONV hconv = pCd->gethConv();
	if (hconv != NULL) {
		if (DdeServer::isLogging())
			DdeServer::ldisconnect(hconv,NULL,NULL,HDDE_NULL);
		else
			DdeServer::disconnect(hconv,NULL,NULL,HDDE_NULL);
		::DdeDisconnect(hconv);
	} else {
		cdlist.delItemByPtr(pCd);
	}
}

//	�T�[�o������̒ʐM�̏I��
//	disconnect() ���ĂԂ͕̂K�v�Ȃ烍�O����邽��
void
DdeServer::deleteConvData(ConvData* pCd)
{
	if (m_ConvDataList.itemNum() <= 0) return;
	try {
		if (pCd != NULL) {
			//	����̒ʐM���I��
			DelCD(pCd,m_ConvDataList);
		} else {
			//	�S�ʐM���I��
			//	onDisconnect() �͓����� m_pConvDataList->delItemByIndex(0) ��
			//	�Ă�ł���̂� initSequentialGet() & getNextItem() �̃y�A��
			//	�g�킸�Ɉ���������K�v������B
			while (m_ConvDataList.itemNum() > 0) {
				DelCD(m_ConvDataList.getItemByIndex(0),m_ConvDataList);
			}
		}
	} catch (...) {
		// nothing to do
	}
}

//	HCONV -> ConvData*
ConvData*
DdeServer::getConvData(const HCONV hconv)
{
	if (hconv == NULL) return NULL;
	//	���[�U�[�f�[�^�Ƀ|�C���^���o�^����Ă��� (see addConvData())
	CONVINFO ci;
	ci.cb = sizeof(CONVINFO);
	ci.hUser = 0L;
	::DdeQueryConvInfo(hconv,QID_SYNC,&ci);
	return reinterpret_cast<ConvData*>(ci.hUser);
}

//	�T�[�o�{�̂̃v���Z�X���� DdeAdvReq() ���ĂԂ��߂̃��\�b�h
void
DdeServer::notifyPostAdvise(ConvData* pCd)
{
	//	���Ԃ��ɂ����̂ōŒ���̃`�F�b�N�������Ȃ�
	if (pCd != NULL) {
		::DdePostAdvise(m_ddeInst,
						pCd->getAdvTopicName(),
						pCd->getAdvItemName());
	}
}

//	���O�̊J�n
BOOL
DdeServer::startLog(const StringBuffer *psbLogFileName)
{
	if (psbLogFileName == NULL) return FALSE;
	try {
		m_psbLogFileName = new StringBuffer(*psbLogFileName);
	} catch (...) {
		return FALSE;
	}
	m_pfLogFile = new File(*m_psbLogFileName,
							GENERIC_WRITE,
							OPEN_ALWAYS,
							FILE_FLAG_SEQUENTIAL_SCAN);
	if (!m_pfLogFile->isValid()) return FALSE;
	m_pfLogFile->setFilePointer(0,0,FILE_END);
	return TRUE;
}

//	���O�̒�~
void
DdeServer::stopLog()
{
	if (isLogging()) {
		m_pfLogFile->flushFileBuffers();
		m_pfLogFile = NULL;
		m_psbLogFileName = NULL;
	}
}

BOOL
DdeServer::isLogging()
{
	return m_pfLogFile.ptr() != NULL && m_pfLogFile->isValid();
}

//	���O�t�@�C���ւ̏�������
void
DdeServer::writeLog(LPCSTR logdata)
{
	try {
		StringBuffer buf(40+lstrlen(logdata));
		buf.append((TCHAR)'[').append((ULONG)::GetTickCount()).append("] ")
			.append(logdata).append((TCHAR)'\n');
		if (m_pfLogFile->writeFile((UCHAR*)(LPCSTR)buf,buf.length())
			!= (DWORD)buf.length()) {
			DdeServer::stopLog();
		}
	} catch (...) {
		// nothing to do
	}
	//	���ꂪ����Ƃ��Ȃ葬�x���ቺ����(������O(^^;)
//	m_pfLogFile->flushFileBuffers();
}

//	���O�t�@�C���ւ̏�������(����̒ʐM�ɂ���)
void
DdeServer::writeLog(const HCONV hconv, LPCSTR logdata)
{
	try {
		StringBuffer buf("ConvID: ",-1,40+lstrlen(logdata));
		writeLog(buf.append((ULONG)hconv).append(" : ").append(logdata));
	} catch (...) {
	}
}

//	�R�}���h���������O�ɏ�������
void
DdeServer::writeArgvToLog(
	const HCONV hconv,
	CmdLineParser& ha,
	StringBuffer& header)
{
	try {
		ha.initSequentialGet();
		if (header.length()) {
			writeLog(hconv,header.append(ha.getNextArgv()));
		}
		header.reset(" Argv: ");
		LPCSTR	pav;
		while ((pav = ha.getNextArgv()) != NULL) {
			writeLog(hconv,header.append(pav));
			header.setlength(7);
		}
	} catch (...) {
	}
}

//	DdemlCallBack() �̃��b�Z�[�W�n���h��(���O����)
HDDEDATA
DdeServer::regist(const HCONV, const HSZ, const HSZ, const HDDEDATA)
{
	return HDDE_NULL; // Ddeml ignores it.
}

HDDEDATA
DdeServer::unregist(const HCONV, const HSZ, const HSZ, const HDDEDATA)
{
	return HDDE_NULL; // Ddeml ignores it.
}

//	�ʐM�v��
//	hsz1 : �g�s�b�N��
//	hsz2 : ��{�T�[�r�X��
HDDEDATA
DdeServer::connect(const HCONV, const HSZ hsz1, const HSZ hsz2, const HDDEDATA)
{
	if (m_pdsServiceName.ptr() == NULL ||
		m_pdsServiceName->compareTo(hsz2) != 0)
		return HDDE_FALSE;
	else if (isSupportedTopic(hsz1)) return HDDE_TRUE;
	return isStoredCD(hsz1) ? HDDE_TRUE : HDDE_FALSE;
}

//	�ʐM�̊m��
//	hconv : �ʐM�n���h��
//	hsz1  : �g�s�b�N��
HDDEDATA
DdeServer::connectconfirm(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ, // not used
	const HDDEDATA) // not used
{
	//	Ddeml ignores it.
	return addConvData(hconv,hsz1) > 0 ? HDDE_TRUE : HDDE_FALSE;
}

//	�G���[
HDDEDATA
DdeServer::error(const HCONV, const HSZ, const HSZ, const HDDEDATA)
{
	::PostMessage(m_hwndDlg,WM_USER_DDE_STOP,0,0);
	return HDDE_NULL;	//	Ddeml ignores it.
}

//	�ʐM�̔j��
HDDEDATA
DdeServer::disconnect(const HCONV hconv, const HSZ, const HSZ, const HDDEDATA)
{
	try {
		//	���X�g����o�^����
		ConvData* cd = getConvData(hconv);
		if (cd != NULL) {
			if (!cd->isKeepAlive())
				m_ConvDataList.delItemByPtr(cd);
			else
				cd->sethConv(NULL);
		}
	} catch (...) {
	}
	return HDDE_NULL;	//	Ddeml ignores it.
}

//	ddeexecute
HDDEDATA
DdeServer::execute(
	const HCONV hconv,
	const HSZ, // not used
	const HSZ, // not used
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		ReceivedDdeData rdd(hdata);
		RealCmdLineParser argv((const StringBuffer&)rdd);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		PFNDDECMD_EXECUTE pfnCmd = getExecCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // cmd name
		return cd->ddeExecute(pfnCmd,argv);
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::request(
	const HCONV hconv,
	const HSZ, // not used
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		DdeString dsItem(m_ddeInst,hsz2);
		RealCmdLineParser argv((const StringBuffer&)dsItem);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		PFNDDECMD_REQUEST pfnCmd = getReqCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // req name
		return cd->ddeRequest(pfnCmd,argv,dsItem);
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::poke(
	const HCONV hconv,
	const HSZ, // not used
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		PFNDDECMD_POKE pfnCmd = getPokeCmd(DdeString(m_ddeInst,hsz2));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		ReceivedDdeData rdd(hdata);
		PokeDataParser argv((const StringBuffer&)rdd);
		return cd->ddePoke(pfnCmd,argv);
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::advstart(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	return cd != NULL ? cd->ddeAdvStart(hsz1,hsz2) : HDDE_FALSE;
}

HDDEDATA
DdeServer::advstop(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	//	Ddeml ignores it.
	return cd != NULL ? cd->ddeAdvStop(hsz1,hsz2) : HDDE_FALSE;
}

HDDEDATA
DdeServer::advreq(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA) // not used
{
	ConvData* cd = getConvData(hconv);
	return cd != NULL ? cd->ddeAdvReq(hsz1,hsz2) : HDDE_NULL;
}

//	DdemlCallBack() �̃��b�Z�[�W�n���h��(���O�L��)
HDDEDATA
DdeServer::lregist(
	const HCONV,
	const HSZ,
	const HSZ,
	const HDDEDATA)
{
	try {
		StringBuffer buf("Start Server : ",-1,32);
		writeLog(buf.append((LPCSTR)*m_pdsServiceName));
	} catch (...) {
	}
	return HDDE_NULL;	//	Ddeml ignores it.
}

HDDEDATA
DdeServer::lunregist(
	const HCONV,
	const HSZ,
	const HSZ,
	const HDDEDATA)
{
	try {
		writeLog("Stop Server");
	} catch (...) {
	}
	return HDDE_NULL;	//	Ddeml ignores it.
}

HDDEDATA
DdeServer::lconnect(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	return connect(hconv,hsz1,hsz2,hdata);
}

HDDEDATA
DdeServer::lconnectconfirm(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = connectconfirm(hconv,hsz1,hsz2,hdata);
		StringBuffer msg("Connect ",-1,32);
		msg.append(hret == HDDE_TRUE ? "Success" : "Failed");
		msg.append(": ").append((LPCSTR)DdeString(m_ddeInst,hsz1));
		writeLog(hconv,msg);
		return hret;	//	Ddeml ignores it.
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::lerror(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		writeLog("Unrecoverable error(s)!! Stop Server");
		return error(hconv,hsz1,hsz2,hdata);
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::ldisconnect(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		writeLog(hconv,"Disconnect");
		return disconnect(hconv,hsz1,hsz2,hdata);
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::lexecute(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		ReceivedDdeData rdd(hdata);
		RealCmdLineParser argv((const StringBuffer&)rdd);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		StringBuffer buf("Execute: ");
		writeArgvToLog(hconv,argv,buf);
		PFNDDECMD_EXECUTE pfnCmd = getExecCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // cmd name
		HDDEDATA hret = cd->ddeExecute(pfnCmd,argv);
		buf = hret == HDDE_FACK ? " Return: Success" : " Return: Failed";
		writeLog(hconv,buf);
		return hret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::lrequest(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		DdeString dsItem(m_ddeInst,hsz2);
		RealCmdLineParser argv((const StringBuffer&)dsItem);
		if (argv.itemNum() <= 0) return HDDE_FNOTPROCESSED;
		StringBuffer buf("Request: ");
		writeArgvToLog(hconv,argv,buf);
		PFNDDECMD_REQUEST pfnCmd = getReqCmd(argv.getArgvStr(0));
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		argv.delArgv(0); // req name
		HDDEDATA hret = cd->ddeRequest(pfnCmd,argv,dsItem);
		buf = " Return: ";
		if (hret != HDDE_FNOTPROCESSED) {
			buf.append((const StringBuffer&)TemporalDdeData(hret));
		} else {
			buf.append("Failed");
		}
		writeLog(hconv,buf);
		return hret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::lpoke(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	ConvData* cd = getConvData(hconv);
	if (cd == NULL) return HDDE_FNOTPROCESSED;

	try {
		DdeString dsItem(m_ddeInst,hsz2);
		PFNDDECMD_POKE pfnCmd = getPokeCmd(dsItem);
		if (pfnCmd == 0) return HDDE_FNOTPROCESSED;
		ReceivedDdeData rdd(hdata);
		PokeDataParser argv((const StringBuffer&)rdd);
		StringBuffer buf("Poke : ",-1,dsItem.length());
		writeLog(hconv,buf.append((LPCSTR)dsItem));
		buf = nullStr;
		writeArgvToLog(hconv,argv,buf);
		HDDEDATA hret = cd->ddePoke(pfnCmd,argv);
		buf = hret == HDDE_FACK ? " Return: Success" : " Return: Failed";
		writeLog(hconv,buf);
		return hret;
	} catch (...) {
		return HDDE_FNOTPROCESSED;
	}
}

HDDEDATA
DdeServer::ladvstart(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = advstart(hconv,hsz1,hsz2,hdata);
		writeLog(hconv,hret == HDDE_TRUE ? "Start Advice Success" :
										   "Start Advice Failed");
		return hret;
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::ladvstop(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = advstop(hconv,hsz1,hsz2,hdata);
		writeLog(hconv,hret == HDDE_TRUE ? "Stop Advice Success" :
										   "Stop Advice Ignored");
		return hret;
	} catch (...) {
		return HDDE_FALSE;
	}
}

HDDEDATA
DdeServer::ladvreq(
	const HCONV hconv,
	const HSZ hsz1,
	const HSZ hsz2,
	const HDDEDATA hdata)
{
	try {
		HDDEDATA hret = advreq(hconv,hsz1,hsz2,hdata);
		StringBuffer buf("Request Advice Data ",-1,16);
		if (hret != HDDE_NULL) {
			writeLog(hconv,buf.append("Success"));
			buf.reset(" Data: ");
			writeLog(hconv,buf.append((const StringBuffer&)TemporalDdeData(hret)));
		} else {
			writeLog(hconv,buf.append("Failed"));
			buf.reset(" Error code: ");
			writeLog(hconv,buf.append((ULONG)::DdeGetLastError(m_ddeInst)));
		}
		return hret;
	} catch (...) {
		return HDDE_NULL;
	}
}

