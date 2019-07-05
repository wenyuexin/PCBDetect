#include "FileSender.h"


FileSender::FileSender(std::string _ip, std::string _port): address(_ip),port(_port)
{
}

FileSender::~FileSender()
{
}

void FileSender::SendFolder(const std::string & dir)
{
	std::vector<std::string> fileList;
	if (!getFilesName(dir, fileList)) {
		std::cout << "输入文件夹为空，请检查输入！";
		return;
	}

	for (int i = 0; i < fileList.size(); i++) {
		boost::asio::io_service ioService;
		boost::asio::ip::tcp::resolver resolver(ioService);
		auto endpointIterator = resolver.resolve({ address, port });
		Client client(ioService, endpointIterator, fileList[i]);
		ioService.run();
	}
}


int FileSender::getFilesName(const std::string & dir, std::vector<std::string>& filenames)
{
	boost::filesystem::path path(dir);
	if (!boost::filesystem::exists(path)) return -1;

	boost::filesystem::directory_iterator end_iter;
	for (boost::filesystem::directory_iterator iter(path); iter != end_iter; ++iter) {
		if (boost::filesystem::is_regular_file(iter->status())) {
			filenames.push_back(iter->path().string());
		}

		if (boost::filesystem::is_directory(iter->status())) {
			getFilesName(iter->path().string(), filenames);
		}
	}
	return filenames.size();
}


Client::Client(IoService& t_ioService, TcpResolverIterator t_endpointIterator,
	std::string const& t_path)
	: m_ioService(t_ioService), m_socket(t_ioService),
	m_endpointIterator(t_endpointIterator), m_path(t_path)
{
	doConnect();
	openFile(m_path);
}


void Client::openFile(std::string const& t_path)
{
	m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
	if (m_sourceFile.fail()) {
		throw std::fstream::failure("打开文件失败" + t_path);
	}

	m_sourceFile.seekg(0, m_sourceFile.end);
	auto fileSize = m_sourceFile.tellg();
	m_sourceFile.seekg(0, m_sourceFile.beg);

	std::ostream requestStream(&m_request);
	boost::filesystem::path p(t_path);
	requestStream << p.string() << "\n" << fileSize << "\n\n";
	std::cout << fileSize << std::endl;
}


void Client::doConnect()
{
	boost::asio::async_connect(m_socket, m_endpointIterator,
		[this](boost::system::error_code ec, TcpResolverIterator)
	{
		if (!ec) {
			writeBuffer(m_request);
		}
		else {
			std::cout << "无法连接到发送端，请检查发送端是否开启或者网络是否正常" << std::endl;
			BOOST_LOG_TRIVIAL(error) << "Error: " << ec.message();
		}
	});
}


void Client::doWriteFile(const boost::system::error_code& t_ec)
{
	if (!t_ec) {
		if (m_sourceFile) {
			m_sourceFile.read(m_buf.data(), m_buf.size());
			if (m_sourceFile.fail() && !m_sourceFile.eof()) {
				auto msg = "打开文件失败";
				BOOST_LOG_TRIVIAL(error) << msg;
				throw std::fstream::failure(msg);
			}
			std::stringstream ss;
			ss << "Send " << m_sourceFile.gcount() << " bytes, total: "
				<< m_sourceFile.tellg() << " bytes";
			std::cout << ss.str() << std::endl;

			auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
			writeBuffer(buf);
		}
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "Error: " << t_ec.message();
	}
}


template<class Buffer>
void Client::writeBuffer(Buffer& t_buffer)
{
	boost::asio::async_write(m_socket,
		t_buffer,
		[this](boost::system::error_code ec, size_t /*length*/)
	{
		doWriteFile(ec);
	});
}