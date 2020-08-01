#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/api.h"
#ifdef Q_OS_MAC
#include "src/macos.h"
#endif

#include <QMessageBox>
#include <QScreen>

using namespace std;

#if defined (Q_OS_WIN)
#define DAEMON_PROG "lunremote.exe"
#elif defined (Q_OS_MAC)
#define DAEMON_PROG "lunremote"
#else
#define DAEMON_PROG "lunremote"
#endif

#define DAEMON_PATH "daemon/" DAEMON_PROG
#define DAEMON_PATH "/Users/marek/workspaces/other/remocy-server/xcode/Release/" DAEMON_PROG

ClientMsg makeMsg(MsgType type)
{
	static uint32_t seq = 1;
// 	ClientMsg msg = {};
	msg.type = type;
	msg.seq = seq++;
	return msg;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), socket(new QUdpSocket(this))
{
    ui->setupUi(this);

	createActions();
	createTrayIcon();
	process.setProgram(DAEMON_PATH);

	pingTimer = new QTimer(this);
	pingTimer->setInterval(500);
	connect(pingTimer, &QTimer::timeout, this, &MainWindow::sendPing);

	waitForProcessStopTimer = new QTimer(this);
	pingTimer->setInterval(500);

	connect(socket, &QUdpSocket::readyRead, this, &MainWindow::readFortune);
//	connect(socket, &QUdpSocket::errorOccurred, this, &MainWindow::displayError);

	connect(&process, &QProcess::errorOccurred, this, &MainWindow::processErrorOccurred);
	connect(&process, &QProcess::started, this, &MainWindow::processStarted);
	connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::processStarted);

	socket->connectToHost(QHostAddress::LocalHost, 5800);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::createActions()
{
	startAction = new QAction(tr("&Start server"), this);
	connect(startAction, &QAction::triggered, this, &MainWindow::startDaemon);

	stopAction = new QAction(tr("S&top server"), this);
	connect(stopAction, &QAction::triggered, this, &MainWindow::stopDaemon);

	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, &QAction::triggered, this, &MainWindow::quit);

	startAction->setEnabled(true);
	stopAction->setEnabled(false);
}

void MainWindow::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(startAction);
	trayIconMenu->addAction(stopAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

#ifdef Q_OS_MAC
	QString suffix = Macos::isDarkMode() ? "white" : "black";
	QIcon icon = QIcon(":/icons/tray-icon-macos-" + suffix);
#else
	QIcon icon = QIcon(":/icons/remocy-icon");
#endif
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(icon);
	trayIcon->show();
}

void MainWindow::startDaemon()
{
	isRunning = true;
	process.start();
	startAction->setEnabled(false);
	stopAction->setEnabled(true);
}

void MainWindow::stopDaemon()
{
	waitForProcessStopTimer->setSingleShot(true);
	waitForProcessStopTimer->start();
	isRunning = false;
	process.terminate();
	socket->abort();
	startAction->setEnabled(true);
	stopAction->setEnabled(false);
}

void MainWindow::processErrorOccurred(QProcess::ProcessError error)
{
	qDebug() << "processErrorOccurred:" << error;

	if(isQuitting)
	{
		QCoreApplication::quit();
	}
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug() << "processFinished:" << exitCode << exitStatus;
	waitForProcessStopTimer->stop();

	if(isQuitting)
	{
		QCoreApplication::quit();
	}
}

void MainWindow::processStarted()
{
	qDebug() << "processStarted";
}

void MainWindow::connectToServer()
{
	//socket->abort();
}

void MainWindow::sendToServer()
{
	auto screens = QGuiApplication::screens();
	QByteArray data;
	data.resize(sizeof(FrameRect) * screens.length());
	ClientMsg msg = makeMsg(MsgScreensGeometry);
	msg.msg.screens.count = screens.length();
	FrameRect *rect = (FrameRect *)data.data();
	for(auto screen : screens)
	{
		auto r = screen->geometry();
		rect->x = r.x();
		rect->y = r.x();
		rect->width = r.width();
		rect->height = r.height();
	}
	msg.msg.screens.primaryIndex = screens.indexOf(QGuiApplication::primaryScreen());
	sendMsg(msg, &data);
}

void MainWindow::readFortune()
{

//	if (blockSize == 0) {
//		// Relies on the fact that QDataStream serializes a quint32 into
//		// sizeof(quint32) bytes
//		if (socket->bytesAvailable() < (int)sizeof(quint32))
//			return;
//		in >> blockSize;
//	}

//	if (socket->bytesAvailable() < blockSize || in.atEnd())
//		return;

//	QString nextFortune;
//	in >> nextFortune;

////	if (nextFortune == currentFortune) {
////		QTimer::singleShot(0, this, &Client::requestNewFortune);
////		return;
////	}

////	currentFortune = nextFortune;
////	statusLabel->setText(currentFortune);
////	getFortuneButton->setEnabled(true);
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) {
//	case QAbstractSocket::SocketError::ServerNotFoundError:
//		QMessageBox::information(this, tr("Local Fortune Client"),
//								 tr("The host was not found. Please make sure "
//									"that the server is running and that the "
//									"server name is correct."));
//		break;
//	case QLocalSocket::ConnectionRefusedError:
//		QMessageBox::information(this, tr("Local Fortune Client"),
//								 tr("The connection was refused by the peer. "
//									"Make sure the fortune server is running, "
//									"and check that the server name "
//									"is correct."));
//		break;
//	case QLocalSocket::PeerClosedError:
//		break;
	default:
		QMessageBox::information(this, tr("Local Fortune Client"),
								 tr("The following error occurred: %1.")
								 .arg(socket->errorString()));
	}

	//	getFortuneButton->setEnabled(true);
}

void MainWindow::sendPing()
{
	auto msg = makeMsg(MsgPing);
	lastMsgSeq = msg.seq;
	sendMsg(msg);
}

void MainWindow::finishProcessTimeout()
{
	qWarning() << "Can't close daemon. Force closing.";
	process.kill();
}

void MainWindow::refreshScreens()
{

}

void MainWindow::quit()
{
	isQuitting = true;
	stopDaemon();
//	QTimer::singleShot(1000, []() {
//		QCoreApplication::quit();
//	});
}

int MainWindow::sendMsg(ClientMsg &msg, QByteArray *data)
{
	msg.hash = 0;
	if(data != nullptr)
	{
		static vector<uint8_t> buf(PACKET_SIZE);
		int dataLen = qMin((unsigned long) buf.size(), (unsigned long)data->size());
		ClientMsg *newMsg = (ClientMsg *)&buf[0];
		memcpy(newMsg, &msg, sizeof(ClientMsg));
		memcpy(newMsg->data, data->data(), dataLen);
		newMsg->dataSize = dataLen;
		dataLen += sizeof(ClientMsg);
//		newMsg->hash = Utils::crc16((const uint8_t *)&buf[0], dataLen);
		socket->write((const char *)&buf[0], dataLen);
	}
	else
	{
//		msg.hash = Utils::crc16((const uint8_t *)&msg, sizeof(msg));
		socket->write((const char *)&msg, sizeof(msg));
	}
	socket->flush();
	return 0;
}
