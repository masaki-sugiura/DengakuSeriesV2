//	$Id: env_var.h,v 1.1.1.1 2001-10-07 14:41:22 sugiura Exp $
/*
 *	env_var.h
 *	環境変数の管理に関するクラス
 */

#ifndef DENGAKUSERIES_CLASSES_ENV_VAR
#define DENGAKUSERIES_CLASSES_ENV_VAR

#include "smalloc.h"

//	共有メモリのサイズ(bytes)
#define ENV_PAGESIZE (512*1024)
//	永続的変数を保存するレジストリキー
#define ENV_REGKEY_NAME_ROOT "Software\\Sugi_Macro\\DengakuSeries\\Env"

//	変数の名前、値を管理するための構造体
//	ここでいう「ポインタ」とは、SMAlloc クラスで管理される共有メモリ内のみで
//	有効な相対アドレスを格納する変数を指す。
#pragma pack(push,env_header,4)
typedef struct {
	UINT m_pNextHeader;	//	次の変数の ENV_HEADER 構造体へのポインタ
	UINT m_pEnvName;	//	変数名へのポインタ
	UINT m_pEnvString;	//	変数値へのポインタ
} ENV_HEADER, *LPENV_HEADER;
#pragma pack(pop,env_header)

class EnvManager {
public:
	EnvManager(const StringBuffer& env_name);
	~EnvManager();

	int count();	//	共有数を返す

	//	変数の参照・定義・破棄を行うメソッド
	StringBuffer read(const StringBuffer& env_name);
	void write(const StringBuffer& env_name, const StringBuffer& env_var);
	void del(const StringBuffer& env_name);
	//	変数が存在するかどうかを返す
	BOOL exist(const StringBuffer& env_name);
	//	現在定義されている変数の変数名を全て「なめる」ためのメソッド
	void enum_env();	//	初期化
	StringBuffer next_env();	//	次の変数名を取り出す
	BOOL hasmore_env();	//	まだ残っている変数があるかを返す

private:
	StringBuffer m_env_name;	//	共有メモリの名前
	SMAlloc* m_psma;	//	メモリマッピングオブジェクト内のメモリ管理クラス
	UINT m_enumindex;	//	next_env() で返す変数の
						//	ENV_HEADER への(SMAlloc の意味の)ポインタ

	BOOL loadEnv();		//	永続的変数の読込みを行う
	BOOL saveEnv();		//	永続的変数の保存を行う

	EnvManager(const EnvManager&);
	EnvManager& operator=(const EnvManager&);
};

#endif

