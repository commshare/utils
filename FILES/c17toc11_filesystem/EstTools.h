#pragma once
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <assert.h>
#include <istream>
#include <string>
#include <vector>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif
//namespace EduWeb {
  void GetFileCrc(std::istream& input_stream, std::vector<char>& buff, unsigned long& result_crc);
  bool IsLargeFile(std::istream& input_stream);
  bool IsFileExists(const std::wstring& filename);
  bool MakeDir(const std::wstring& newdir);
  void RemoveDir(const std::wstring& foldername);
  std::wstring GetParentDir(const std::wstring& filepath);
  //std::wstring GetCurrentPath();
  bool IsDirectory(const std::wstring& path);
  std::vector<std::wstring> GetFilesFromDir(const std::wstring& path);
  std::wstring GetFileNameFromPath(const std::wstring& path);
  std::wstring GetLastDirNameFromPath(const std::wstring& path); // the path must a dir

  std::wstring TryToUnicode(const std::string& src);
  std::string TryFromUnicode(const std::wstring& src);
//}  // namespace easyzip

enum EstErr
{
  EST_SUCC = 0,
  EST_FAIL = -1,
  EST_ERR_FILEORDIR_ALREADY_EXIST = 2000,
};


class EstTools
{
public:
  // time: [16:07:34:426]
  static inline std::string GetLocalTime() {
	char szbuffer[128] = { 0 };
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	sprintf_s(szbuffer, "[%02d:%02d:%02d:%03d]", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	return szbuffer;
  }
  // -2020-04-17-16-07-34
  static inline std::string GetLocalDay() {
	char szbuffer[128] = { 0 };
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	sprintf_s(szbuffer, "_%04d-%02d-%02d-%02d-%02d-%02d_", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	return szbuffer;
  }

  //∫¡√Î
  static inline std::wstring GetWstrHaomiao()
  {
	std::chrono::time_point<std::chrono::system_clock> now =
	  std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	std::uint64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return std::to_wstring(millis);
  }
  //∫¡√Î
  static inline std::string GetCurHaomiao()
  {
	std::chrono::time_point<std::chrono::system_clock> now =
	  std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	std::uint64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return std::to_string(millis);
  }
  static inline uint64_t GetHaomiao()
  {
	std::chrono::time_point<std::chrono::system_clock> now =
	  std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  }
 // static inline std::pair<uint64_t, uint64_t> GetBs2Timeout(uint64_t timeoutHaomiao)
 // {
	//auto duration = std::chrono::system_clock::now().time_since_epoch();
 //   uint64_t _now = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	//auto timeoutMs = std::chrono::milliseconds(timeoutHaomiao);
	//auto time = duration + timeoutMs;    
	//uint64_t _timeout =  std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
 //   return std::make_pair(_now,_timeout);
 // }
  static std::vector<std::wstring> zFilesFromDirectory(const std::wstring& path);
  static  std::wstring zFileNameFromPath(const std::wstring& path);
  static std::string zGetFolderBaseName(const std::string& folder);
  static std::string folderTagName(const std::string& path);
  static std::string folderDate(const std::string& path);
  static uint64_t folderTimestamp(const std::string& path);
  static std::vector<std::wstring> zFoldersFromDirectory(const std::wstring& dir);
  static std::vector<std::wstring> zFoldersFromDirectory_OneStage(const std::wstring& dir);
  static EstErr zMakedir(const std::wstring& newdir);
  static void zwcout();
  static bool zIsExist(std::wstring& fileordir);
  static bool zIsDirectory(const std::wstring& path);
  static std::wstring zAppdataWstrDir();
  static uint64_t GetFileSize(std::wstring &filepath);
  static bool zMoveFile(const std::string& from,
	const std::string& to);
  static std::string zFile2Str(std::wstring& filename);
  static std::wstring zStr2Wstr(std::string &str);
  static bool zMoveFolder(const std::string& fromFolder,
	const std::string& toFolder);
};