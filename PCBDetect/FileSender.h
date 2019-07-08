#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <memory>
#include <asio.hpp>


class FileSender
{
public:
	FileSender(std::string _ip,std::string port = "8888");//IPv4��ַ��ʼ���������˿�Ĭ��Ϊ8888
	~FileSender();
	void SendFolder(const std::string& dir);//������Ҫ�����ļ�������·��

private:
	std::string address;
	std::string port ;
private:
	int getFilesName(const std::string& dir, std::vector<std::string>& filenames);//��ȡ�ļ��������ļ���
};


class Client
{
public:
	using IoService = asio::io_service;
	using TcpResolver = asio::ip::tcp::resolver;
	using TcpResolverIterator = TcpResolver::iterator;
	using TcpSocket = asio::ip::tcp::socket;

	Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator,
		std::string const& t_path);

private:
	void openFile(std::string const& t_path);
	void doConnect();
	void doWriteFile();
	template<class Buffer>
	void writeBuffer(Buffer& t_buffer);


	TcpResolver m_ioService;
	TcpSocket m_socket;
	TcpResolverIterator m_endpointIterator;
	enum { MessageSize = 1024 * 40 };
	std::array<char, MessageSize> m_buf;
	asio::streambuf m_request;
	std::ifstream m_sourceFile;
	std::string m_path;
};

