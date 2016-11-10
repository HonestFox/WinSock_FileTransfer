#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <WinSock2.h>
#include <cassert>
#include <iostream>

#define PORT 8087
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#pragma comment(lib, "WS2_32")

SOCKET StartUp()
{
	//������ַ�ṹ
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	//��ʼ�� socket dll
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	assert(WSAStartup(socketVersion, &wsaData) == 0);

	//����socket
	SOCKET m_socket = socket(AF_INET, SOCK_STREAM, 0);
	assert(SOCKET_ERROR != m_socket);

	//��
	assert(SOCKET_ERROR != bind(m_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)));

	//����
	assert(SOCKET_ERROR != listen(m_socket, 10));

	return m_socket;
}

int main()
{
	//��ʼ����Server�׽���
	SOCKET m_socket = StartUp();
	//�ȴ�����
	while (1)
	{
		std::cout << "Listening To Client ..." << std::endl;
		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);
		SOCKET m_new_socket = accept(m_socket, (sockaddr *)&client_addr, &client_addr_len);
		assert(SOCKET_ERROR != m_new_socket);

		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		assert(recv(m_new_socket, buffer, BUFFER_SIZE, 0) >= 0);

		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
		strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));  //����ȡ�϶̵�
		std::cout << file_name;

		FILE *fp = fopen(file_name, "rb");  //��ֻ���������Ƶķ�ʽ��һ���ļ�
		assert(NULL != fp);
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;

		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
		{
			assert(send(m_new_socket, buffer, length, 0) >= 0);
			memset(buffer, 0, BUFFER_SIZE);
		}
		fclose(fp);
		std::cout << "File : " << file_name << "Transfer Successful !" << std::endl;
		closesocket(m_new_socket);
	}
	closesocket(m_socket);
	//�ͷ� winsock ��
	WSACleanup();
	return 0;
}