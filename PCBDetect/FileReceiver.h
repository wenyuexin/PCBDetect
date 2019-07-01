#pragma once

#include <array>
#include <fstream>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/asio/read_until.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

class Session
    : public std::enable_shared_from_this<Session>
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;

    Session(TcpSocket t_socket);

    void start()
    {
        doRead();
    }

private:
    void doRead();
    void processRead(size_t t_bytesTransferred);
    void createFile();
    void readData(std::istream &stream);
    void doReadFileContent(size_t t_bytesTransferred);
    void handleError(std::string const& t_functionName, boost::system::error_code const& t_ec);


    TcpSocket m_socket;
    enum { MaxLength = 1024*40 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf_;
    std::ofstream m_outputFile;
    size_t m_fileSize;
    std::string m_fileName;
	std::string m_filePath;
	std::string m_fullName;
};


class FileReceiver
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using IoService = boost::asio::io_service;

    FileReceiver( short t_port, std::string const& t_workDirectory);//指定接收端口和文件夹
	~FileReceiver();
	void startListen();//开始监听

private:
    void doAccept();
    void createWorkDirectory();


private:
	std::string m_workpath;//文件存储路径


	IoService  *m_iomanager;
    TcpSocket *m_socket;
    TcpAcceptor *m_acceptor;

    std::string m_workDirectory;
	short m_port;
};
