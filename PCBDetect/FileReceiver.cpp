
#include "FileReceiver.h"


Session::Session(TcpSocket t_socket)
    : m_socket(std::move(t_socket))
{
}


void Session::doRead()
{
    auto self = shared_from_this();
    async_read_until(m_socket, m_requestBuf_, "\n\n", //�����ļ���+�ļ�size����"\n\n"��β��
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec)
                processRead(bytes);//��ʼ��ȡ�׽���
            else
                handleError(__FUNCTION__, ec);
        });
}


void Session::processRead(size_t t_bytesTransferred)
{
  
    std::istream requestStream(&m_requestBuf_);
    readData(requestStream);

	auto pos1 = m_fullName.find("PCBData");
	auto pos2 = m_fullName.find_last_of("\\");
	if (pos1 != std::string::npos&& pos2 != std::string::npos) {
		m_filePath = "." + m_fullName.substr(pos1+7, pos2-pos1-7+1);//�ļ�·������Ŀ¼���ļ���ǰ
		m_fileName =m_filePath+ m_fullName.substr(pos2+1);//�ļ���
	}
	
    createFile();//����Ŀ¼���ļ�

    //дʣ�µ�����
    do {
        requestStream.read(m_buf.data(), m_buf.size());
        m_outputFile.write(m_buf.data(), requestStream.gcount());
    } while (requestStream.gcount() > 0);

    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec)
                doReadFileContent(bytes);
            else
                handleError(__FUNCTION__, ec);
        });
}


void Session::readData(std::istream &stream)//��ȡ�ļ������ļ���С
{
    stream >> m_fullName;//�ļ���
    stream >> m_fileSize;//��С
    stream.read(m_buf.data(), 2);
}


void Session::createFile()
{
	if(!boost::filesystem::exists(m_filePath))
		boost::filesystem::create_directories(m_filePath);
    m_outputFile.open(m_fileName, std::ios_base::binary);
    if (!m_outputFile) {
        BOOST_LOG_TRIVIAL(error) << __LINE__ << ": �����ļ�ʧ��: " << m_fileName;
        return;
    }
}


void Session::doReadFileContent(size_t t_bytesTransferred)
{
    if (t_bytesTransferred > 0) {
        m_outputFile.write(m_buf.data(), static_cast<std::streamsize>(t_bytesTransferred));

        if (m_outputFile.tellp() >= static_cast<std::streamsize>(m_fileSize)) {
            std::cout << "Received file: " << m_fileName << std::endl;
            return;
        }
    }
    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            doReadFileContent(bytes);
        });
}


void Session::handleError(std::string const& t_functionName, boost::system::error_code const& t_ec)
{
    BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << " in " << t_functionName << " due to " 
        << t_ec << " " << t_ec.message() << std::endl;
}







FileReceiver::FileReceiver(short t_port, std::string const& t_workDirectory) :
	m_port(t_port), m_workDirectory(t_workDirectory)
{
	m_iomanager = new IoService;
	m_socket = new TcpSocket(*m_iomanager);
	m_acceptor = new TcpAcceptor(*m_iomanager, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_port));
	createWorkDirectory();

	doAccept();
	std::cout << "FileReceiver started\n";
}

FileReceiver::~FileReceiver()
{
	delete m_acceptor;
	delete m_socket;
	delete m_iomanager;
}

void FileReceiver::startListen()
{
	m_iomanager->run();
}

void FileReceiver::doAccept()
{
    m_acceptor->async_accept(*m_socket,
        [this](boost::system::error_code ec)
    {
        if (!ec)
            std::make_shared<Session>(std::move(*m_socket))->start();

        doAccept();
    });
}


void FileReceiver::createWorkDirectory()
{
    using namespace boost::filesystem;
    auto currentPath = path(m_workDirectory);
    if (!exists(currentPath) && !create_directory(currentPath))
        BOOST_LOG_TRIVIAL(error) << "Coudn't create working directory: " << m_workDirectory;
    current_path(currentPath);
}
