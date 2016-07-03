#pragma once

#include <vector>
namespace GameEngine
{
  struct DirEntry
  {
    std::string path;
    bool isDirectory;
  };

  class IOManager
  {
  public:
    //read the file to the buffer ( a vector of uchars, like a c-string )
    static bool ReadFileToBuffer(const std::string& _filePath, std::vector<unsigned char>& _buffer);
    //read the file to the buffer ( a normal string )
    static bool ReadFileToBuffer(const std::string& _filePath, std::string& _buffer);
    // Gets all directory entries in the directory specified by path and stores in rvEntries.
    // Returns false if path is not a directory.
    static bool GetDirectoryEntries(const char* _path, std::vector<DirEntry>& _rvEntries);
    //creates a directory in the specified path
    static bool MakeDirectory(const char* _path);
  };
}


