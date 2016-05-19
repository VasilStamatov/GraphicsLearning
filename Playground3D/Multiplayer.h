#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>
#include <vector>

#define DEFAULT_BUFLEN 512

class Multiplayer
{
public:
  Multiplayer();
  ~Multiplayer();

  void Loop();
  void CleanUp();
  void Send(const std::string& _message);

private:
  SOCKET m_connectSocket;
  WSADATA m_wsaData;
  char m_recvbuf[DEFAULT_BUFLEN];
  int m_recvbuflen = DEFAULT_BUFLEN;
  struct addrinfo *m_result = nullptr;
  std::vector<std::string> m_messages;
  std::string m_buffer;

  void ProcessMessages();
};

