#include "Multiplayer.h"
#include <exception>

#define DEFAULT_PORT "27015"

Multiplayer::Multiplayer()
{
  m_connectSocket = INVALID_SOCKET;

  struct addrinfo * ptr = nullptr, hints;

  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
  if (iResult != 0)
  {
    throw std::exception();
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &m_result);
  if (iResult != 0)
  {
    CleanUp();
    throw std::exception();
  }

  // Attempt to connect to an address until one succeeds
  for (ptr = m_result; ptr != nullptr; ptr = ptr->ai_next)
  {
    // Create a SOCKET for connecting to server
    m_connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (m_connectSocket == INVALID_SOCKET)
    {
      CleanUp();
      throw std::exception();
    }
    // Connect to server.
    iResult = connect(m_connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
      closesocket(m_connectSocket);
      m_connectSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(m_result);

  if (m_connectSocket == INVALID_SOCKET)
  {
    CleanUp();
    throw std::exception();
  }
}


Multiplayer::~Multiplayer()
{
}

void Multiplayer::Loop()
{
  int iResult = 0;

  iResult = recv(m_connectSocket, m_recvbuf, m_recvbuflen, 0);

  for (size_t i = 0; i < iResult; i++)
  {
    if (m_recvbuf[i] != '\n')
    {
      m_buffer += m_recvbuf[i];
    }
    else
    {
      m_messages.push_back(m_buffer);
      m_buffer = "";
    }
  }

  ProcessMessages();
}

void Multiplayer::ProcessMessages()
{
  while (m_messages.size() > 0)
  {
    // pos,6,4.3,8.9
    m_messages.erase(m_messages.begin() + 0);
  }
}

void Multiplayer::CleanUp()
{
  if (m_connectSocket != INVALID_SOCKET)
  {
    closesocket(m_connectSocket);
  }
  freeaddrinfo(m_result);
  WSACleanup();
}
void Multiplayer::Send(const std::string& _message)
{
  // Send an initial buffer
  std::string toSend = _message + '\n';
  int iResult;
  iResult = send(m_connectSocket, toSend.c_str(), (int)(toSend.length()), 0);
  if (iResult == SOCKET_ERROR)
  {
    CleanUp();
    throw std::exception();
  }
}
