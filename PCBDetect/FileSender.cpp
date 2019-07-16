#include "FileSender.h"

using std::string;


FileSender::FileSender(std::string _ip, std::string _port): address(_ip), port(_port)
{
}

FileSender::~FileSender()
{
	qDebug() << "~FileSender";
}

//发送文件
void FileSender::sendFiles(const std::vector<std::string> &fileList)
{
	for (int i = 0; i < fileList.size(); i++) {
		asio::io_service ioService;
		asio::ip::tcp::resolver resolver(ioService);
		auto endpointIterator = resolver.resolve({ address, port });
		Client client(ioService, endpointIterator, hierarchy, fileList[i]);
		ioService.run();
	}
}



/****************************************/

Client::Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator,
	std::string hierarchy, std::string const& t_path)
	: m_ioService(t_ioService), m_socket(t_ioService),
	m_endpointIterator(t_endpointIterator), m_path(t_path)
{
	doConnect();
	openFile(hierarchy, m_path);
}


void Client::openFile(std::string hierarchy, std::string const& t_path)
{
	m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
	if (m_sourceFile.fail()) {
		throw std::fstream::failure("打开文件失败" + t_path);
	}

	m_sourceFile.seekg(0, m_sourceFile.end);
	auto fileSize = m_sourceFile.tellg();
	m_sourceFile.seekg(0, m_sourceFile.beg);

	std::ostream requestStream(&m_request);
	//filesystem::path p(t_path);
	requestStream << hierarchy << "\n"; //将目录层次写入头文件中
	requestStream << t_path << "\n"; //将文件绝对路径写入头文件中
	requestStream << fileSize << "\n\n"; //将文件大小放入头文件中
	//std::cout << fileSize << std::endl;
}


void Client::doConnect()
{
	asio::async_connect(m_socket, m_endpointIterator,
		[this](asio::error_code ec, TcpResolverIterator)
	{
		if (!ec) {
			writeBuffer(m_request);
		}
		else {
			std::cout << "无法连接到发送端，请检查发送端是否开启或者网络是否正常" << std::endl;
			std::cout << "Error: " << ec.message();
		}
	});
}


void Client::doWriteFile(const asio::error_code & t_ec)
{
	if (!t_ec) {
		if (m_sourceFile) {
			m_sourceFile.read(m_buf.data(), m_buf.size());
			if (m_sourceFile.fail() && !m_sourceFile.eof()) {
				auto msg = "打开文件失败";
				throw std::fstream::failure(msg);
			}
			std::stringstream ss;
			ss << "Send " << m_sourceFile.gcount() << " bytes, total: "
				<< m_sourceFile.tellg() << " bytes";
			std::cout << ss.str() << std::endl;//调试用信息，查看已发送文件大小，以及总大小

			auto buf = asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
			writeBuffer(buf);
		}
	}
	else {
		std::cout << "Error: " << t_ec.message();
	}
}


template<class Buffer>
void Client::writeBuffer(Buffer& t_buffer)
{
	asio::async_write(m_socket,
		t_buffer,
		[this](asio::error_code ec, size_t /*length*/)
	{
		doWriteFile(ec);
	});
}