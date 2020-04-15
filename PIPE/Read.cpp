#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

int main(void)
{
	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;
	DWORD dwWritten;

//	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
	hPipe = CreateNamedPipe(TEXT(".\\Pipe"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		PIPE_WAIT,
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				/* add terminating zero */
				buffer[dwRead] = '\0';

				/* do something with data in buffer */
				printf("%s", buffer);

				//WriteFile(hPipe,
				//	"Read File\n",
				//	12,   // = length of string + terminating '\0' !!!
				//	&dwWritten,
				//	NULL);
			}
		}

		DisconnectNamedPipe(hPipe);
	}

	return 0;
}
