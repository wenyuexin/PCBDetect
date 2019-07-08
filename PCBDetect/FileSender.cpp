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
		std::cout << "�����ļ���Ϊ�գ��������룡";
		return;
	}

	for (int i = 0; i < fileList.size(); i++) {
		asio::io_service ioService;
		asio::ip::tcp::resolver resolver(ioService);
		auto endpointIterator = resolver.resolve({ address, port });
		Client client(ioService, endpointIterator, fileList[i]);
		ioService.run();
	}
}


int FileSender::getFilesName(const std::string &dir, std::vector<std::string> &filenames)
{
	//filesystem::path path(dir);
	//if (!filesystem::exists(path)) return -1;

	//filesystem::directory_iterator end_iter;
	//for (filesystem::directory_iterator iter(path); iter != end_iter; ++iter) {
	//	if (filesystem::is_regular_file(iter->status())) {
	//		filenames.push_back(iter->path().string());
	//	}

	//	if (filesystem::is_directory(iter->status())) {
	//		getFilesName(iter->path().string(), filenames);
	//	}
	//}
	//return filenames.size();
	return 0;
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
		throw std::fstream::failure("���ļ�ʧ��" + t_path);
	}

	m_sourceFile.seekg(0, m_sourceFile.end);
	auto fileSize = m_sourceFile.tellg();
	m_sourceFile.seekg(0, m_sourceFile.beg);

	std::ostream requestStream(&m_request);
	//filesystem::path p(t_path);
	//requestStream << p.string() << "\n" << fileSize << "\n\n";
	std::cout << fileSize << std::endl;
}


void Client::doConnect()
{
	asio::async_connect(m_socket, m_endpointIterator,
		[this]( TcpResolverIterator)
	{
	
			writeBuffer(m_request);
		
	});
}


void Client::doWriteFile()
{
	
		if (m_sourceFile) {
			m_sourceFile.read(m_buf.data(), m_buf.size());
			if (m_sourceFile.fail() && !m_sourceFile.eof()) {
				auto msg = "���ļ�ʧ��";
				//BOOST_LOG_TRIVIAL(error) << msg;
				throw std::fstream::failure(msg);
			}
			std::stringstream ss;
			ss << "Send " << m_sourceFile.gcount() << " bytes, total: "
				<< m_sourceFile.tellg() << " bytes";
			std::cout << ss.str() << std::endl;

			auto buf = asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
			writeBuffer(buf);
		}
	
	
}


template<class Buffer>
void Client::writeBuffer(Buffer& t_buffer)
{
	asio::async_write(m_socket,
		t_buffer,
		[this]( size_t /*length*/)
	{
		doWriteFile();
	});
}