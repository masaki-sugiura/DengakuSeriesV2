//	$Id: opencomdlg.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	opencomdlg.h
 *	�R�����_�C�A���O��\�����邽�߂̊֐�
 */
//	filename:	opencomdlg.h

#ifndef	DENGAKUSERVER_OPENCOMDLG
#define	DENGAKUSERVER_OPENCOMDLG

#include <windows.h>

class ConvData;
class CmdLineParser;

typedef void (*PFNOPENCOMDLGPROC)(HWND, ConvData&, CmdLineParser&);

// �u�t�@�C�����J���v�A�u�t�H���_�̎w��v�_�C�A���O��\������֐��ւ̈���
typedef struct {
	HWND m_hwndOwner;
	ConvData* m_pConvData;
	CmdLineParser* m_pArgv;
	PFNOPENCOMDLGPROC m_pfnComDlg;
} OPENCOMDLGPROCARGS, *LPOPENCOMDLGPROCARGS;

//	�u�t�@�C�����J���v�R�����_�C�A���O���J�����t�@�C�����̎擾
void GetFileNameByDlg(HWND, ConvData&, CmdLineParser&);
//	�u�t�H���_�̎w��v�R�����_�C�A���O���J�����t�H���_���̎擾
void GetDirNameByDlg(HWND, ConvData&, CmdLineParser&);
//	�u�F�̎w��v�R�����_�C�A���O���J�����F�w��("#RRGGBB")�̎擾
void GetColorByDlg(HWND, ConvData&, CmdLineParser&);
//	�u�t�H���g�̎w��v�R�����_�C�A���O���J�����t�H���g�w��̎擾
void GetFontByDlg(HWND, ConvData&, CmdLineParser&);

#endif
