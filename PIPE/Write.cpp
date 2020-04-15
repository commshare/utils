#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

int main(void)
{
	HANDLE hPipe;
	DWORD dwWritten; 
	char buffer[1024];
	DWORD dwRead;


	hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	while (true)
	{
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			WriteFile(hPipe,
				"Write to File\n",
				15,   // = length of string + terminating '\0' !!!
				&dwWritten,
				NULL);
		//	while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
		//	{
		//		/* add terminating zero */
		//		buffer[dwRead] = '\0';

		//		/* do something with data in buffer */
		//		printf("%s", buffer);
		//	}
		}
		system("pause");
	}
	CloseHandle(hPipe);

	
	return (0);
}
