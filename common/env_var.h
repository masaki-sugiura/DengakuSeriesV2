//	$Id: env_var.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	env_var.h
 *	���ϐ��̊Ǘ��Ɋւ���N���X
 */

#ifndef DENGAKUSERIES_CLASSES_ENV_VAR
#define DENGAKUSERIES_CLASSES_ENV_VAR

#include "smalloc.h"

//	���L�������̃T�C�Y(bytes)
#define ENV_PAGESIZE (512*1024)
//	�i���I�ϐ���ۑ����郌�W�X�g���L�[
#define ENV_REGKEY_NAME_ROOT "Software\\Sugi_Macro\\DengakuSeries\\Env"

//	�ϐ��̖��O�A�l���Ǘ����邽�߂̍\����
//	�����ł����u�|�C���^�v�Ƃ́ASMAlloc �N���X�ŊǗ�����鋤�L���������݂̂�
//	�L���ȑ��΃A�h���X���i�[����ϐ����w���B
#pragma pack(push,env_header,4)
typedef struct {
	UINT m_pNextHeader;	//	���̕ϐ��� ENV_HEADER �\���̂ւ̃|�C���^
	UINT m_pEnvName;	//	�ϐ����ւ̃|�C���^
	UINT m_pEnvString;	//	�ϐ��l�ւ̃|�C���^
} ENV_HEADER, *LPENV_HEADER;
#pragma pack(pop,env_header)

class EnvManager {
public:
	EnvManager(const StringBuffer& env_name);
	~EnvManager();

	int count();	//	���L����Ԃ�

	//	�ϐ��̎Q�ƁE��`�E�j�����s�����\�b�h
	StringBuffer read(const StringBuffer& env_name);
	void write(const StringBuffer& env_name, const StringBuffer& env_var);
	void del(const StringBuffer& env_name);
	//	�ϐ������݂��邩�ǂ�����Ԃ�
	BOOL exist(const StringBuffer& env_name);
	//	���ݒ�`����Ă���ϐ��̕ϐ�����S�āu�Ȃ߂�v���߂̃��\�b�h
	void enum_env();	//	������
	StringBuffer next_env();	//	���̕ϐ��������o��
	BOOL hasmore_env();	//	�܂��c���Ă���ϐ������邩��Ԃ�

private:
	StringBuffer m_env_name;	//	���L�������̖��O
	SMAlloc* m_psma;	//	�������}�b�s���O�I�u�W�F�N�g���̃������Ǘ��N���X
	UINT m_enumindex;	//	next_env() �ŕԂ��ϐ���
						//	ENV_HEADER �ւ�(SMAlloc �̈Ӗ���)�|�C���^

	BOOL loadEnv();		//	�i���I�ϐ��̓Ǎ��݂��s��
	BOOL saveEnv();		//	�i���I�ϐ��̕ۑ����s��

	EnvManager(const EnvManager&);
	EnvManager& operator=(const EnvManager&);
};

#endif

