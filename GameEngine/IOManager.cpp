#include "IOManager.h"

#include <fstream>
#include <filesystem>

// Namespace alias
namespace fs = std::tr2::sys;

namespace GameEngine
{
  bool IOManager::ReadFileToBuffer(const std::string& _filePath, std::vector<unsigned char>& _buffer)
  {
    std::ifstream file(_filePath, std::ios::binary);
    if (file.fail())
    {
      perror(_filePath.c_str());
      return false;
    }

    //seek to the end
    file.seekg(0, std::ios::end);

    //get the file size
    unsigned int fileSize = (unsigned int)file.tellg();
    file.seekg(0, std::ios::beg);

    //reduce the file size by any header bytes that might be present
    fileSize -= (unsigned int)file.tellg();

    //resize the buffer to the exact size we need it
    _buffer.resize(fileSize);

    //read the file to the buffer
    file.read((char *)&(_buffer[0]), fileSize);

    //close the file
    file.close();
    return true;
  }

  bool IOManager::ReadFileToBuffer(const std::string& _filePath, std::string& _buffer)
  {
    std::ifstream file(_filePath, std::ios::binary);
    if (file.fail())
    {
      perror(_filePath.c_str());
      return false;
    }

    //seek to the end
    file.seekg(0, std::ios::end);

    //Get the file size
    unsigned int fileSize = (unsigned int)file.tellg();
    file.seekg(0, std::ios::beg);

    //Reduce the file size by any header bytes that might be present
    fileSize -= (unsigned int)file.tellg();

    //resize the buffer to the exact size we need it
    _buffer.resize(fileSize);

    //read the file to the buffer
    file.read((char *)&(_buffer[0]), fileSize);

    //close the file
    file.close();

    return true;
  }

  bool IOManager::GetDirectoryEntries(const char* _path, std::vector<DirEntry>& _rvEntries)
  {
    auto dpath = fs::path(_path);
    // Must be directory
    if (!fs::is_directory(dpath)) return false;

    for (auto it = fs::directory_iterator(dpath); it != fs::directory_iterator(); ++it)
    {
      _rvEntries.emplace_back();
      _rvEntries.back().path = it->path().string();
      //check if it's a directory
      if (is_directory(it->path()))
      {
        _rvEntries.back().isDirectory = true;
      }
      else
      {
        _rvEntries.back().isDirectory = false;
      }
    }
    return true;
  }

  bool IOManager::MakeDirectory(const char* _path)
  {
    return fs::create_directory(fs::path(_path));
  }
}
