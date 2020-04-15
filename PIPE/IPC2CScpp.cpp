#include <windows.h>
#include <iostream>
using namespace std;

#define WRITEPIPENAME "\\\\.\\Pipe\\exeWrite"
#define READPIPENAME "\\\\.\\Pipe\\exeRead"

HANDLE hPipe;
HANDLE hWritePipe;
void ConsoleThread()
{
	while (1)
	{
		//��ͨ�ܵ�
		if (WaitNamedPipe(TEXT(WRITEPIPENAME), NMPWAIT_WAIT_FOREVER) == FALSE)
		{
			cout << "�ȴ������ܵ�ʵ��ʧ�ܣ�" << endl;
			Sleep(5000);
			continue;
		}
		HANDLE hPipe = CreateFile(TEXT(WRITEPIPENAME),
			GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			cout << "���������ܵ�ʧ�ܣ�" << endl;
			CloseHandle(hPipe);
			Sleep(5000);
			continue;
		}
		//������
		char buffer[1024];
		DWORD WriteNum;
		cout << "�������Ϸ�����" << endl;
		while (1)
		{
			memset(buffer, 0, sizeof(buffer));
			cin >> buffer;
			if (WriteFile(hPipe, buffer, strlen(buffer), &WriteNum, NULL) == false)
			{
				cout << "����д��ܵ�ʧ�ܣ�" << endl;
				cout << "�رչܵ���" << endl;
				CloseHandle(hPipe);
				break;
			}
		}
	}
}

void main()
{
	char buffer[1024];
	DWORD ReadNum;
	//������һ�������ܵ�
	hPipe = CreateNamedPipe(TEXT(READPIPENAME),
		PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT
		, PIPE_UNLIMITED_INSTANCES, 0, 0, NMPWAIT_WAIT_FOREVER, 0);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		cout << "���������ܵ�ʧ�ܣ�" << "   errorNo =" << GetLastError() << endl;
		CloseHandle(hPipe);
		return;
	}

	if (ConnectNamedPipe(hPipe, NULL) == FALSE)
	{
		cout << "��ͻ�������ʧ�ܣ�" << endl;
		CloseHandle(hPipe);
		return;
	}
	cout << "��ͻ������ӳɹ���" << endl;

	//����д�ܵ�


	//�����߳� д����
	HANDLE console = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleThread, NULL, 1, 0);
	ResumeThread(console);  //�������߳�

	while (1)
	{
		int int_, S1_length;
		int N1;
		string S1;
		float F1;

		DWORD rlen = 0;
		BOOL rRead;
		char c[500] = {};
		rRead = ReadFile(
			hPipe,
			c,
			500,
			&rlen,
			0);
		if (rRead)
		{
			c[rlen] = '\0';
			printf("�����ɹ�,c = %s\n", c);
		}
	}

	cout << "�رչܵ���" << endl;
	CloseHandle(hPipe);
	system("pause");
}