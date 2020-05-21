

#include <string>  
#include <fstream>  
#include <streambuf>  
#include <shlobj.h>
#include <shlwapi.h>
#include <process.h>
#include <locale.h>
#pragma comment(lib, "shlwapi.lib")
#include "filesystem/filesystem.hpp"
#include "EstTools.h"

#ifdef _MSC_VER
#include <filesystem>
namespace fs = ghc::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
//#include <ghc/filesystem.hpp>
//namespace fs = ghc::filesystem;



#include <string>
#include <codecvt>
#include <locale>

using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

std::string wstr2str(std::wstring wstr)
{
  return strconverter.to_bytes(wstr);
}

std::wstring to_wstring(std::string str)
{
  return strconverter.from_bytes(str);
}


//namespace EduWeb {

bool IsFileExists(const std::wstring& filename) {
  std::error_code ec;
  return fs::exists(filename, ec);
}

bool MakeDir(const std::wstring& newdir) {
  std::error_code ec;
  return fs::create_directories(newdir, ec);
}

void RemoveDir(const std::wstring& foldername) {
  std::error_code ec;
  fs::remove_all(foldername, ec);
}

bool IsDirectory(const std::wstring& path) {
  std::error_code ec;
  return fs::is_directory(path, ec);
}

std::wstring GetParentDir(const std::wstring& filepath) {
  fs::path p(filepath);
  return p.parent_path().wstring();
}

//std::wstring GetCurrentPath() { return fs::current_path().wstring(); }

std::vector<std::wstring> GetFilesFromDir(const std::wstring& path) {
  std::vector<std::wstring> v;
  fs::recursive_directory_iterator itr(path);
  for (itr = fs::begin(itr); itr != fs::end(itr); itr++) {
	if (!fs::is_directory(itr->path())) {
	  v.push_back(itr->path().wstring());
	}
  }

  return v;
}

std::wstring GetFileNameFromPath(const std::wstring& fullPath) {
  fs::path p(fullPath);
  return p.filename().wstring();
}

std::wstring GetLastDirNameFromPath(const std::wstring& path) {
  if (path.length() == 0)
	return L"";
  if (path.length() == 1 && path == L".")
	return L"";
  if (path.length() == 2 && path == L"..")
	return L"";

  assert(IsDirectory(path));

  std::wstring path2 = path;

#ifdef _WIN32
  if (path2[path2.length() - 1] == L'\\') {
	path2 = path2.substr(0, path2.length() - 1);
  }
#else
  if (path2[path2.length() - 1] == L'/') {
	path2 = path2.substr(0, path2.length() - 1);
  }
#endif

  fs::path p(path2);
  return p.filename().wstring();
}

#ifdef _WIN32
std::string UnicodeToAnsi(const std::wstring& str, unsigned int code_page /*= 0*/) {
  std::string strRes;
  int iSize = ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  if (iSize == 0)
	return strRes;

  char* szBuf = new (std::nothrow) char[iSize];

  if (!szBuf)
	return strRes;

  memset(szBuf, 0, iSize);

  ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::wstring AnsiToUnicode(const std::string& str, unsigned int code_page /*= 0*/) {
  std::wstring strRes;

  int iSize = ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, NULL, 0);

  if (iSize == 0)
	return strRes;

  wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

  if (!szBuf)
	return strRes;

  memset(szBuf, 0, iSize * sizeof(wchar_t));

  ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, szBuf, iSize);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::string UnicodeToUtf8(const std::wstring& str) {
  std::string strRes;

  int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  if (iSize == 0)
	return strRes;

  char* szBuf = new (std::nothrow) char[iSize];

  if (!szBuf)
	return strRes;

  memset(szBuf, 0, iSize);

  ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::wstring Utf8ToUnicode(const std::string& str) {
  std::wstring strRes;
  int iSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

  if (iSize == 0)
	return strRes;

  wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

  if (!szBuf)
	return strRes;

  memset(szBuf, 0, iSize * sizeof(wchar_t));
  ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize);

  strRes = szBuf;
  delete[] szBuf;

  return strRes;
}

std::string AnsiToUtf8(const std::string& str, unsigned int code_page /*= 0*/) {
  return UnicodeToUtf8(AnsiToUnicode(str, code_page));
}

std::string Utf8ToAnsi(const std::string& str, unsigned int code_page /*= 0*/) {
  return UnicodeToAnsi(Utf8ToUnicode(str), code_page);
}

#endif


#if _MSC_VER >= 1900

std::string utf16_to_utf8(std::wstring utf16_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  auto p = reinterpret_cast<const wchar_t*>(utf16_string.data());
  return convert.to_bytes(p, p + utf16_string.size());
}

std::wstring utf8_to_utf16(std::string utf8_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  auto p = reinterpret_cast<const char*>(utf8_string.data());
  auto str = convert.from_bytes(p, p + utf8_string.size());
  std::wstring u16_str(str.begin(), str.end());
  return u16_str;
}
#else

std::string utf16_to_utf8(std::wstring utf16_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.to_bytes(utf16_string);
}

std::wstring utf8_to_utf16(std::string utf8_string) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
  return convert.from_bytes(utf8_string);
}
#endif

std::wstring TryToUnicode(const std::string& src) {
#ifdef _WIN32
  return AnsiToUnicode(src, 0);
#else
  return utf8_to_utf16(src);
#endif
}

std::string TryFromUnicode(const std::wstring& src) {
#ifdef _WIN32
  return UnicodeToAnsi(src, 0);
#else
  return utf16_to_utf8(src);
#endif
}

//}  // namespace easyzip

std::wstring Str2Wstr(std::string &str)
{
  if (str.length() == 0)
	return L"";

  std::wstring wstr;
  wstr.assign(str.begin(), str.end());
  return wstr;
}
std::vector<std::wstring> EstTools::zFoldersFromDirectory(const std::wstring &dir)
{
  std::vector<std::wstring> v;
  fs::recursive_directory_iterator itr(dir);
  for (itr = fs::begin(itr); itr != fs::end(itr); itr++) {
	if (fs::is_directory(itr->path())) {
	  v.push_back(itr->path().wstring());
	}
  }
  return v;
}
std::vector<std::wstring> EstTools::zFoldersFromDirectory_OneStage(const std::wstring& dir)
{
  std::vector<std::wstring> v;
  fs::directory_iterator itr(dir);
  for (itr = fs::begin(itr); itr != fs::end(itr); itr++) {
	if (fs::is_directory(itr->path())) {
	  v.push_back(itr->path().wstring());
	}
  }
  return v;
}
//从目录获取所有文件
std::vector<std::wstring> EstTools::zFilesFromDirectory(const std::wstring& path)
{
  std::vector<std::wstring> files; 
  try
  {
    files = GetFilesFromDir(path);
  }
  catch (fs::filesystem_error *e)
  {
    std::wcout << " filesystem exception " << e->what() << std::endl;
  }
  catch (std::exception &e)
  {   
    //中文乱码todo
    std::cout << " =>filesystem std exception " << fs::detail::toUtf8(e.what()) << std::endl;
	std::cout << " <=filesystem std exception " << (e.what()) << std::endl;

 //   //string里面的中文打印不出来
	//std::locale loc("chs");
	//std::wcout.imbue(loc);
 //   std::wcout << " filesystem std exception " << (e.what()) << std::endl;
  }
  return std::move(files);
}
//根据路径获取到文件的名字，可以某文件夹下所有的文件而且是绝对路径打印
std::wstring EstTools::zFileNameFromPath(const std::wstring& path)
{
  return GetFileNameFromPath(path);
}
bool EstTools::zIsExist(std::wstring& fileordir)
{
  return IsFileExists(fileordir);
}
void enableChinese()
{
  std::locale loc("chs");
  //locale loc( "Chinese-simplified" );
  //locale loc( "ZHI" );
  //locale loc( ".936" );
  std::wcout.imbue(loc);
}
void EstTools::zwcout()
{
  enableChinese();
}
EstErr EstTools::zMakedir(const std::wstring& newdir)
{
  //检查是否已经存在的逻辑，放在外面？错误原因怎么返回？
  if (IsFileExists(newdir))
  {
	std::wcout << "mkdir newdir err, already exist ";
	return EST_ERR_FILEORDIR_ALREADY_EXIST;
  }
  //具体的错误码要解析下std errcode todo
  bool ret = MakeDir(newdir);
  return ret == true ? EST_SUCC : EST_FAIL;
}

bool EstTools::zIsDirectory(const std::wstring& path)
{
  return IsDirectory(path);
}

uint64_t EstTools::GetFileSize(std::wstring &filepath)
{
  enableChinese();
  uint64_t filesize;
  try
  {
    filesize = fs::file_size(filepath);
  }
  catch (const std::exception& e)
  {
    std::string err = e.what();
   // std::wcout << " filepath get size fail"<< Str2Wstr(err)<<" path: "<< filepath << std::endl;
    filesize = 0;
  }
  return filesize;
}
std::string EstTools::zFile2Str(std::wstring &filename)
{
  std::ifstream t(filename);
  std::string content((std::istreambuf_iterator<char>(t)),
    std::istreambuf_iterator<char>());
  return std::move(content);
}
std::wstring EstTools::zStr2Wstr(std::string &str)
{
  return Str2Wstr(str);
}
#if 0
std::string Tools::zGetFolderBaseName(const std::string& folder)
{
  bool bIsDir = CDirEntry::isDir(folder);
  if (bIsDir)
  {
	return CDirEntry::baseName(folder);
  }
  else
  {
	std::cout << " folder " << folder << " is not dir " << std::endl;
  }
  return ZIPPER_INVALID_STR;
}

std::string Tools::folderTagName(const std::string& path)
{
  std::string basename = zGetFolderBaseName(path);
  if (basename != ZIPPER_INVALID_STR)
  {
	std::string::size_type start = basename.find_first_of("_");
#ifdef WIN32 // WIN32 also understands '/' as the separator.

	if (start == std::string::npos)
	{
	  //异常吧，没有"_"
	  return ZIPPER_INVALID_STR;
	}
#endif
	std::string first = basename.substr(0, start);
	return first;
  }
  return ZIPPER_INVALID_STR;
}
//teacher_2020-04-18-18-20-14_1587205214207
std::string Tools::folderDate(const std::string& path)
{
  std::string basename = zGetFolderBaseName(path);
  std::cout << " basename " << basename << std::endl;
  if (basename != ZIPPER_INVALID_STR)
  {
	std::string::size_type start = basename.find_first_of("_");

#ifdef WIN32 // WIN32 also understands '/' as the separator.

	if (start == std::string::npos)
	{
	  //异常吧，没有"_"
	  return ZIPPER_INVALID_STR;
	}
#endif
	std::string::size_type end = basename.find_last_of("_");
	std::cout << " start " << start << " end " << end << std::endl;
	if (end == std::string::npos)
	{
	  //异常吧，没有"_"
	  return ZIPPER_INVALID_STR;
	}
	std::string::size_type begin = start + 1;
	std::string second = basename.substr(begin, end - begin);
	return second;
  }
  return ZIPPER_INVALID_STR;


  //if (start == std::string::npos) start = 0;
  //else start++; // We do not want the separator.

  //std::string::size_type end = path.find_last_of(".");

  //if (end == std::string::npos || end < start)
  //  end = path.length();

  //return path.substr(start, end - start);
}
uint64_t Tools::folderTimestamp(const std::string& path)
{
  std::string basename = zGetFolderBaseName(path);
  if (basename != ZIPPER_INVALID_STR)
  {
	std::string::size_type start = basename.find_first_of("_");

#ifdef WIN32 // WIN32 also understands '/' as the separator.

	if (start == std::string::npos)
	{
	  //异常吧，没有"_"
	  return ZIPPER_INVALID_UINT;
	}
#endif
	std::cout << " basename " << basename << std::endl;
	std::string::size_type end = basename.find_last_of("_");
	std::cout << " start " << start << " end " << end << std::endl;
	if (end == std::string::npos)
	{
	  //异常吧，没有"_"
	  return ZIPPER_INVALID_UINT;
	}
	std::string::size_type begin = end + 1;
	std::string timestamp = basename.substr(begin, basename.size() - begin);
	return str2uint64(timestamp);
  }
  return ZIPPER_INVALID_UINT;


  //if (start == std::string::npos) start = 0;
  //else start++; // We do not want the separator.

  //std::string::size_type end = path.find_last_of(".");

  //if (end == std::string::npos || end < start)
  //  end = path.length();

  //return path.substr(start, end - start);
}

std::vector<std::string> Tools::zFoldersFromDirectory(const std::string& path)
{
  std::vector<std::string> folders;
  //std::vector<std::string> files;
  DIR* dir;
  struct dirent* entry;

  dir = opendir(path.c_str());

  if (dir == NULL)
  {
	return folders;
  }

  for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
  {
	std::string filename(entry->d_name);

	if (filename == "." || filename == "..") continue;
	std::string tmppath = path + CDirEntry::Separator + filename;
	//只需要一层
	if (CDirEntry::isDir(tmppath))
	{
	  //文件夹清单
	  folders.emplace_back(tmppath);
	  ////文件清单
	  //std::vector<std::string> moreFiles = filesFromDirectory(tmppath);
	  //std::copy(moreFiles.begin(), moreFiles.end(), std::back_inserter(files));
	  continue;
	}

	// files.push_back(path + CDirEntry::Separator + filename);
  }

  closedir(dir);


  return folders;
}






bool Tools::zIsExist(std::string& fileORdir)
{
  return checkFileExists(fileORdir);
}

bool  Tools::zMoveFile(const std::string& from,
  const std::string& to)
{
  //必须保证 from是个文件，而且要存在
  bool ret = checkFileExists(from);
  if (!ret)
  {
	std::cout << "Tools::zMoveFile err, from not exist " << from;
	return ret;
  }
  //目的必须是个文件夹，不存在可以创建
  bool retDstFolder = checkFileExists(to);
  if (!retDstFolder)
  {
	std::cout << "Tools::zMoveFile to not exist " << to << " try to mkdir " << std::endl;
	bool ret = zMakedir(to);
	if (!ret)
	{
	  std::cout << "Tools::zMoveFile zMakedir " << to << " fail " << std::endl;
	  return false;
	}
  }

  return CDirEntry::zMoveFile(from, to);

}
bool  Tools::zMoveFolder(const std::string& fromFolder,
  const std::string& toFolder)
{
  //必须保证 from是个文件夹，而且要存在
  bool ret = checkFileExists(fromFolder);
  if (!ret)
  {
	std::cout << "Tools::zMoveFolder err, fromFolder not exist " << fromFolder;
	return ret;
  }
  //目的必须是个文件夹，不存在可以创建
  bool retDstFolder = checkFileExists(toFolder);
  if (!retDstFolder)
  {
	std::cout << "Tools::zMoveFolder to not exist " << toFolder << " try to mkdir " << std::endl;
	bool ret = zMakedir(toFolder);
	if (!ret)
	{
	  std::cout << "Tools::zMoveFolder zMakedir " << toFolder << " fail " << std::endl;
	  return false;
	}
  }

  return CDirEntry::zMoveFolder(fromFolder, toFolder);

}
#endif
//std::string Tools::zAppdataDir()
//{
//#if 1 //unicode
//  wchar_t buffer[MAX_PATH] = { 0 };
//
//  ::SHGetSpecialFolderPath(NULL, buffer, CSIDL_APPDATA, NULL);
//
//  std::string strPath = wstr2str(std::wstring(buffer)).append("\\100EducationTeacher\\");
//
//  //SHCreateDirectoryEx(NULL, strPath.utf16(), NULL);
//  SHCreateDirectoryEx(NULL, Str2Wstr(strPath).c_str(), NULL);
//  return strPath;
//#endif
//}
#if 0
std::wstring GetUserAppDataRoot()
{
  TCHAR buffer[MAX_PATH] = { 0 };

  SHGetSpecialFolderPath(NULL, buffer, CSIDL_APPDATA, NULL);

  std::wstring strPath = std::wstring(buffer) + L"\\" + KAppdataSubDirectory + GetProcessName() + L"\\";

  if (!IsDirExist(strPath.c_str()))
  {
	CreateDirectory(strPath.c_str());
  }

  return strPath;
}

std::wstring _getSubPath(LPCWSTR strSubDir, LPCWSTR strChildDir)
{
  if (!strSubDir || '\0' == *strSubDir)
  {
	return L"";
  }

// 一级目录
std::wstring strDir = GetUserAppDataRoot() + strSubDir;

if (strDir.empty() /*|| (!_isDirExist(strDir.c_str()) && !_createDirectory(strDir.c_str()))*/)
{
  return L"";
}

// 还有下一级子目录
if (strChildDir && '\0' != strChildDir[0])
{
  WCHAR wchEnd = strDir[strDir.size() - 1];
  if (wchEnd != '\\' && wchEnd != '/')
  {
	strDir += L"\\";
  }
  strDir += strChildDir;
  strDir += L"\\";
  if (strDir.empty() || !_isDirExist(strDir.c_str()) && !_createDirectory(strDir.c_str()))
  {
	return L"";
  }
}

return strDir;
}
#endif
std::wstring EstTools::zAppdataWstrDir()
{
  //const WCHAR* KLogDirectory = L"log\\";

  wchar_t buffer[MAX_PATH] = { 0 };

  ::SHGetSpecialFolderPath(NULL, buffer, CSIDL_APPDATA, NULL);

   std::string strPath = wstr2str(std::wstring(buffer)).append("\\100EducationTeacher\\log\\");

   //这是没有就去创建一个把
   //SHCreateDirectoryEx(NULL, strPath.utf16(), NULL);
   //SHCreateDirectoryEx(NULL, Str2Wstr(strPath).c_str(), NULL);
   return Str2Wstr(strPath);
}
