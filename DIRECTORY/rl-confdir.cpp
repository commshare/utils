#include"rl-confdir.h"
#include"rr-stdinc.h"
#include<stdlib.h>
#include<string>
#include<sstream>

#ifdef __APPLE__
#include<sys/types.h>
#include<sys/stat.h>
#elif defined(__unix__)
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pwd.h>
#elif defined(_WIN32)
#include<Windows.h>
#endif

std::string rl_confdir::getConfDir(){
#ifdef __APPLE__
  // macOS. Defaults to ~/Library/Robotics Lite
  std::stringstream ostream;
  ostream << getenv("HOME") << "/Library/Robotics Lite";
  if(mkdir(ostream.str().c_str(),0755) != 0){
    if(errno != EEXIST)
      return ".";
  }
  return ostream.str();
#elif defined(__unix__)
  // UNIXes. Defaults to ~/.roboticslite
  std::stringstream ostream;
  const char* home;
  if((home = getenv("HOME")) == NULL)
    home = getpwuid(getuid())->pw_dir;
  ostream << home << "/.roboticslite";
  if(mkdir(ostream.str().c_str(),0755) != 0){
    if(errno != EEXIST)
      return ".";
  }
  return ostream.str();
#elif defined(_WIN32)
  // Windows. Defaults to %APPDATA%\Robotics Lite
  std::stringstream ostream;
  const char* appdata;
  if((appdata = getenv("APPDATA")) == NULL)
    return ".";
  ostream << appdata << "\\Robotics Lite";
  if(CreateDirectory(ostream.str().c_str(),NULL))
    return ostream.str();
  else if(GetLastError() == ERROR_ALREADY_EXISTS)
    return ostream.str();
  else
    return ".";
#else
  // Unknown / Everything Else, just use the current directory
  return ".";
#endif
}

std::string rl_confdir::getHSFileName(){
  std::stringstream ostream;
  ostream << rl_confdir::getConfDir() << "/highscores";
  return ostream.str();
}

std::string rl_confdir::getKeybindFileName(){
  std::stringstream ostream;
  ostream << rl_confdir::getConfDir() << "/keybinds";
  return ostream.str();
}
