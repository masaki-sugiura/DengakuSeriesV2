// HmJre.dll copyright(c) 2003 有限会社サイトー企画
// written by 斉藤秀夫
// 参考URL  http://www.labyrinth.net.au/~dbareis/3rdpsrc.htm#HENRY_SPENCER_REGEXP_SOURCE


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
