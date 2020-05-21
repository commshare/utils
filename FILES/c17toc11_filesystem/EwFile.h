#pragma once
#include <fstream>

namespace EduWeb {
class EwFile {
 public:
  EwFile(std::string filename) { file_name_ = filename; }
  EwFile() {}
  ~EwFile() { file_name_ = ""; }
  size_t get_file_size() const;
  void write(const char* data, size_t size);
  bool open(const std::string& file_name);
  bool remove();
  bool copy_to(const std::string& directory_path,
               const std::string& file_name = "") const;
  bool move_to(const std::string& directory_path,
               const std::string& file_name = "");
  void rename_file(const std::string& new_file_name);
  void close();
  std::string EwFile::get_file_path() const;
  bool is_open() const;

 private:
  void check_and_create_directory(const std::string& direcotry_path) const;

 private:
  std::string file_name_;
  std::string file_path_;
  std::ofstream file_;
  size_t file_size_ = 0;
};
std::vector<std::wstring> GetFilesFromDir(const std::wstring& path);

}  // namespace EduWeb