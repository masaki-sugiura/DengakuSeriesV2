// HmJre.dll copyright(c) 2003 �L����ЃT�C�g�[���
// written by �ē��G�v
// �Q�lURL  http://www.labyrinth.net.au/~dbareis/3rdpsrc.htm#HENRY_SPENCER_REGEXP_SOURCE


// config.h  for HmJre

class CConfig {
	static HKEY	hkey;
	static int GetInt( const char* pszKey, int nDefault = 0 );
	static void GetString( const char* pszKey, char* pszBuffer, int cchBuffer );
	static void WriteInt( const char* pszKey, int nData );
	static void WriteString( const char* pszKey, const char* pszData );
	static BOOL OpenReg();
	static void CloseReg();

	static void ReplaceConvCode( UINT codeSrc, UINT codeDest );
public:
	static void Load();
	static void Save();
};

extern CConfig config;
