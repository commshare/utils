#include "EwFile.h"
#include <iostream>
#include "EwString_view.h"
#include "filesystem/filesystem.hpp"

using namespace std::literals;
using namespace nonstd::literals;
using namespace nonstd;

#ifdef _MSC_VER
#include <filesystem>
namespace fs = ghc::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#include <fstream>
#include <string>
#include "EwUtils.h"
namespace EduWeb {

void EwFile::write(const char* data, size_t size) {
  file_size_ += size;
  file_.write(data, size);
}

bool EwFile::open(const std::string& file_name) {
  file_.open(file_name, std::ios::binary | std::ios::app);
  bool r = file_.is_open();
  if (r) file_path_ = std::string(&file_name[0], file_name.length());

  return r;
}

bool EwFile::remove() {
  file_.close();
  bool flag = fs::remove(fs::path(file_path_.c_str()));
  file_path_ = "";
  file_size_ = 0;
  return flag;
}

bool EwFile::copy_to(const std::string& directory_path,
                     const std::string& file_name) const {
  check_and_create_directory(directory_path);
  auto write_file_name = file_path_.substr(file_path_.rfind("/") + 1);
  if (!file_name.empty()) {
    write_file_name = file_name;
  }
  auto write_directory_path =
      directory_path.back() == '/' ? directory_path : directory_path + "/";
  bool flag = fs::copy_file(fs::path(file_path_.c_str()),
                            fs::path(write_directory_path + write_file_name));
  return flag;
}

bool EwFile::move_to(const std::string& directory_path,
                     const std::string& file_name) {
  check_and_create_directory(directory_path);
  auto write_file_name = file_path_.substr(file_path_.rfind("/") + 1);
  if (!file_name.empty()) {
    write_file_name = file_name;
  }
  auto write_directory_path =
      directory_path.back() == '/' ? directory_path : directory_path + "/";
  bool flag0 = fs::copy_file(fs::path(file_path_.c_str()),
                             fs::path(write_directory_path + write_file_name));
  file_.close();
  bool flag1 = fs::remove(fs::path(file_path_.c_str()));
  file_path_ = write_directory_path + write_file_name;
  return (flag0 && flag1);
}

void EwFile::rename_file(const std::string& new_file_name) {
  // auto directory_path = file_path_.substr(0,file_path_.rfind("/"));
  // auto new_file_path = directory_path+"/"+new_file_name;
  std::error_code code;
  fs::rename(file_path_, new_file_name, code);
  if (!code) {
    file_path_ = new_file_name;
  }
}

void EwFile::close() { file_.close(); }

size_t EwFile::get_file_size() const { return file_size_; }

std::string EwFile::get_file_path() const { return file_path_; }

bool EwFile::is_open() const { return file_.is_open(); }

void EwFile::check_and_create_directory(
    const std::string& direcotry_path) const {
  auto vec = EduWeb::split(
      string_view(direcotry_path.data(), direcotry_path.size()), "/");
  std::string tmp_directory = "";
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    tmp_directory += std::string(iter->data(), iter->size()) + "/";
    if (iter != vec.begin()) {
      auto current_direcotry =
          tmp_directory.substr(0, tmp_directory.size() - 1);
      if (!fs::exists(current_direcotry)) {
        fs::create_directory(current_direcotry.data());
      }
    }
  }
}

std::vector<std::wstring> GetFilesFromDir(const std::wstring& path) {
  std::vector<std::wstring> v;
  filesystem::recursive_directory_iterator itr(path);
  for (itr = filesystem::begin(itr); itr != filesystem::end(itr); itr++) {
	if (!filesystem::is_directory(itr->path())) {
	  v.push_back(itr->path().wstring());
	}
  }

  return v;
}

}  // namespace EduWeb