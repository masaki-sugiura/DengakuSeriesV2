
#include <windows.h>
#include <stdio.h>
#include "sharedbuf.h"

SharedBuffer* s_buf;

DWORD WINAPI
ProducerThreadProc(LPVOID count)
{
	StringBuffer* val = new StringBuffer(8);
	for (int i = 0; i < (int)count; i++) {
		val->reset("");
		val->append((DWORD)i);
		s_buf->set(*val);
		printf("Put string: %s\n",(LPCSTR)*val);
		::Sleep((DWORD)rand()%100);
	}
	delete val;
	::ExitThread(0);
	return -1;
}

int
main(int ac, char** av)
{
	int count = 500;

	if (ac > 1) {
		count = ival(*++av);
		if (count < 0) count = 500;
	}

	s_buf = new SharedBuffer();

	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL,0,ProducerThreadProc,
									(LPVOID)count,NULL,&dwThreadID);

	StringBuffer buf("        ");
	for (int i = 0; i < count; ) {
		if (s_buf->get(buf,10)) {
			printf("Get string: %s\n",(LPCSTR)buf);
			i++;
		} else {
			printf("Request time out: %d\n",i);
		}
		::Sleep((DWORD)rand()%100);
	}
	::WaitForSingleObject(hThread,INFINITE);
	::CloseHandle(hThread);

	delete s_buf;

	return 0;
}

