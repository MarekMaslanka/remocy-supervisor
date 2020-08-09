#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QDataStream>
#include <QUdpSocket>
#include <QProcess>
#include <QTimer>
#include "src/api.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void startDaemon();
	void stopDaemon();
	void processErrorOccurred(QProcess::ProcessError error);
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void processStarted();
	void readFortune();
	void displayError(QAbstractSocket::SocketError socketError);
	void sendPing();
	void finishProcessTimeout();
	void refreshScreens();
	void quit();

private:
	Ui::MainWindow *ui;

	QAction *startAction;
	QAction *stopAction;
	QAction *quitAction;

	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;

	QTimer *pingTimer;
	QTimer *waitForProcessStopTimer;

	QUdpSocket *socket;
	QDataStream in;
	uint64_t lastMsgSeq;

	QProcess process;
	bool isRunning = false;
	bool isQuitting = false;

	void createActions();
	void createTrayIcon();
	void processStopped();
	int sendMsg(ClientMsg &msg, QByteArray *data = nullptr);
	void connectToServer();
	void sendToServer();
};
#endif // MAINWINDOW_H
