// IniDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "IniDll.h"
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>

LPCWSTR stringToLPCWSTR(std::string orig) {
	wchar_t *wcstring = 0;
	try {
		size_t origsize = orig.length() + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		if (orig == "") {
			wcstring = (wchar_t *)malloc(0);
			mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		}
		else {
			wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
			mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		}
	}
	catch (std::exception e) {
	}
	return wcstring;
}

char * wchar2char(const wchar_t* wchar)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

wchar_t * char2wchar(const char* cchar)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

/******************************************************************************************
Function:        ConvertLPWSTRToLPSTR
Description:     LPWSTRתchar*
Input:           lpwszStrIn:��ת����LPWSTR����
Return:          ת�����char*����
*******************************************************************************************/
char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	try
	{
		if (lpwszStrIn != NULL)
		{
			int nInputStrLen = wcslen(lpwszStrIn);

			// Double NULL Termination  
			int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
			pszOut = new char[nOutputStrLen];

			if (pszOut)
			{
				memset(pszOut, 0x00, nOutputStrLen);
				WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
			}
		}
	}
	catch (std::exception e)
	{
	}

	return pszOut;
}

int GetSections_num(char* path_str)
{
	const short int max_Section_sec = 256;
	const short int MAX_SECTION_sec = 256;
	//ȷ��ini��ַ
	LPCWSTR lpFileName = stringToLPCWSTR(path_str);

	wchar_t chSectionNames[max_Section_sec] = { 0 };//�ܵ���������ַ���

	wchar_t chSection[MAX_SECTION_sec] = { 0 };//���һ��С����\

	char* sec_set = NULL;

	int i;
	int pos = 0;

	int count = 0;

	int ret = GetPrivateProfileSectionNames(chSectionNames, max_Section_sec, lpFileName);//��ȡini�ļ�Section����������

	int m;

	for (i = 0; i < max_Section_sec; i++)//ѭ���õ�Section����
	{
		if (chSectionNames[i] != 0 && chSectionNames[i + 1] == 0 && chSectionNames[i + 2] == 0) {
			for (m = pos; m <= i; m++)

			{
				chSection[m - pos] = chSectionNames[m];//��ȡС����				
			}
			pos = i + 1;
			sec_set = wchar2char(chSection);
			count++;
		}
		else if (chSectionNames[i] != 0 && chSectionNames[i+1] == 0 && chSectionNames[i + 2] != 0)//�ж�Section�����Ƿ����
		{
			for (m = pos; m <= i; m++)

			{
				chSection[m - pos] = chSectionNames[m];//��ȡС����				
			}
			pos = i + 1;
			sec_set = wchar2char(chSection);
			count++;
		}
	}
	return count;
}

char* GetSections(char* path_str, int tag)
{
	const short int max_Section_sec = 256;
	const short int MAX_SECTION_sec = 256;
	//ȷ��ini��ַ
	LPCWSTR lpFileName = stringToLPCWSTR(path_str);

	wchar_t chSectionNames[max_Section_sec] = { 0 };//�ܵ���������ַ���

	wchar_t chSection[MAX_SECTION_sec] = { 0 };//���һ��С����\

	char* sec_set = NULL;

	int i;
	int pos = 0;

	int count = 0;

	int ret = GetPrivateProfileSectionNames(chSectionNames, max_Section_sec, lpFileName);//��ȡini�ļ�Section����������

	int m;

	for (i = 0; i < max_Section_sec; i++)//ѭ���õ�Section����
	{
		if (chSectionNames[i] == 0 && chSectionNames[i + 1] == 0) {
			for (m = pos; m <= i; m++)

			{
				chSection[m - pos] = chSectionNames[m];//��ȡС����				
			}
			pos = i + 1;
			sec_set = wchar2char(chSection);
			count++;
			if(tag == count)
				return sec_set;
		}
		else if (chSectionNames[i] == 0 && chSectionNames[i + 1] != 0)//�ж�Section�����Ƿ����
		{
			for (m = pos; m <= i; m++)

			{
				chSection[m - pos] = chSectionNames[m];//��ȡС����				
			}
			pos = i + 1;
			sec_set = wchar2char(chSection);
			count++;
			if (tag == count) {
				return sec_set;
			}
		}		
	}
	return NULL;
}

void Ini_Write(char* sec_str, char* key_str, char* value_str, char* path_str) {

	LPCWSTR sec = stringToLPCWSTR(sec_str);
	LPCWSTR key = stringToLPCWSTR(key_str);
	LPCWSTR value = stringToLPCWSTR(value_str);
	LPCWSTR path = stringToLPCWSTR(path_str);
	WritePrivateProfileString(sec, key, value, path);
}

char* Ini_Read(char* sec_str, char* key_str, char* path_str) {

	LPCWSTR sec = stringToLPCWSTR(sec_str);
	LPCWSTR key = stringToLPCWSTR(key_str);
	LPCWSTR path = stringToLPCWSTR(path_str);
	wchar_t buf[128];
	char* default_name;
	GetPrivateProfileString(sec, key, L"Not Found", buf, 128, path);

	default_name = ConvertLPWSTRToLPSTR(buf);

	return default_name;
}

void Ini_Del_Key(char* sec_str, char* key_str, char* path_str) {

	LPCWSTR sec = stringToLPCWSTR(sec_str);
	LPCWSTR key = stringToLPCWSTR(key_str);
	LPCWSTR path = stringToLPCWSTR(path_str);
	//GetPrivateProfileStruct(sec, key, NULL, 0, path);
	WritePrivateProfileString(sec, key, NULL, path);
}

void Ini_Del_Sec(char* sec_str, char* path_str) {

	LPCWSTR sec = stringToLPCWSTR(sec_str);
	LPCWSTR path = stringToLPCWSTR(path_str);
	//GetPrivateProfileStruct(sec, key, NULL, 0, path);
	WritePrivateProfileString(sec, NULL, NULL, path);
}

//int Ini_Read(char* sec_str, char* key_str, char* path_str, INT int_true) {
//
//	LPCWSTR sec = stringToLPCWSTR(sec_str);
//	LPCWSTR key = stringToLPCWSTR(key_str);
//	LPCWSTR path = stringToLPCWSTR(path_str);
//	int value;
//	value = GetPrivateProfileInt(sec, key, NULL, path);
//
//	return value;
//}


