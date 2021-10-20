#include "Socket.h"

int Socket::Receive(char* outBuf, size_t recvLen, int flags) const
{
	return ::recv(socket, outBuf, recvLen, 0);
}
bool Socket::Connect(const std::string& ip, size_t port)
{
	sockaddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	sockaddr.sin_port = htons(port);
	//创建套接字
	if (WorkType == NetWorkType::TCP) {
		socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else {
		//UDPD的
		socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	if (connect(socket, (SOCKADDR *)&sockaddr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		printf("%s\n", "Connection failed");
		return false;
	}
	else {
		return true;
	}
}
bool Socket::Bind(const std::string& ip, size_t port) {
	//TCP的

	if (WorkType == NetWorkType::TCP) {
		socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else {
		//UDPD的
		socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	if (socket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
		return false;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	sockaddr.sin_port = htons(port);
	//----------------------
	// Bind the socket.
	if (::bind(socket, (SOCKADDR *)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) {
		wprintf(L"bind failed with error %u\n", WSAGetLastError());
		closesocket(socket);
		return false;
	}
	else
	{
		this->Port = port;
		this->Address = ip;
		return true;
	}
	return false;
}
bool Socket::Listen(int backlog) {
	return !::listen(socket, backlog);
}
Socket Socket::Accep() const {
	for (;;) {
		sockaddr_in  ClientAddr;
		int  AddrLen = sizeof(ClientAddr);
		SOCKET clientSocket = ::accept(socket, (LPSOCKADDR)&ClientAddr, &AddrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			continue;
		}
		Socket skt(clientSocket);
		skt.Port = ClientAddr.sin_port;
		skt.WorkType = WorkType;
		char* c_address = new char[15]{ 0 };
		sprintf_s(c_address, sizeof(c_address), "%d.%d.%d.%d", ClientAddr.sin_addr.S_un.S_un_b.s_b1, ClientAddr.sin_addr.S_un.S_un_b.s_b2, ClientAddr.sin_addr.S_un.S_un_b.s_b3, ClientAddr.sin_addr.S_un.S_un_b.s_b4);
		skt.Address = c_address;
		delete[] c_address;
		return Socket(skt);
	}
	return NULL;
}
int Socket::Write(const char * buff, int size) const
{
	int totalCount = 0;
	const char*ptr = buff;
	for (; totalCount != size;) {
		int result = ::send(socket, buff, size - totalCount, 0);
		if (result == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			break;
		}
		else {
			ptr += result;
			totalCount += result;
		}
	}
	return totalCount;
}
void Socket::Close() const {
	::closesocket(this->socket);
}
Socket::Socket(NetWorkType type)
{
	WorkType = type;
	sockaddr.sin_family = AF_INET;
	Init();
}
Socket::Socket(SOCKET socket)
{
	this->socket = socket;
}
Socket::~Socket()
{

}