#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonTestDetector_clicked()
{
    _lib.setFileName(QApplication::applicationDirPath() + "/" + libName);
QString s = _lib.fileName();
    if (!_lib.load()) {

        qDebug() << "Ssl Failed To Load Library" << _lib.errorString();
        return;
    }
    _initFunc = (Init)_lib.resolve("Init");
    _aiocFunc = (AfteInitOnCreate)_lib.resolve("AfteInitOnCreate");
    _closeFunc = (Dll_Close)_lib.resolve("p_dll_Close");
    _getBuffFunc = (GetBufer)_lib.resolve("GetBufer");
    _getPixelPerString = (GetPixelPerString)_lib.resolve("p_dll_GetPixelPerString");
    _sendIcmp = (SendIcmpPackage)_lib.resolve("p_dll_SendIcmp");
    _getChError = (GetChError)_lib.resolve("p_dll_GetChError");
    _setBinning = (SetBinning)_lib.resolve("p_dll_SendBinning");
    _setModeRun = (SetModeRun)_lib.resolve("p_dll_SetModeRun");
    _getError = (GetError)_lib.resolve("p_dll_GetError");
    _putTimePerString = (PutTimePerString)_lib.resolve("p_dll_PutTimePerString");
    _putGaneMtr = (PutGaneMtr)_lib.resolve("p_dll_PutTimePerString");

    qDebug() << _getError << _initFunc << _aiocFunc << _closeFunc << _sendIcmp;

    qDebug() << QDir::toNativeSeparators(QString(QApplication::applicationDirPath()));
    const char* pathPointer = QDir::toNativeSeparators(QString(QApplication::applicationDirPath())).toStdString().data();
    QDir dirInitFileSslDetector(QStandardPaths::locate(QStandardPaths::DataLocation, + "ssl_detector2.ini").remove("ssl_detector2.ini"));
    qDebug() << "init func" << QDir::toNativeSeparators(dirInitFileSslDetector.absolutePath());
    _initFunc(pathPointer, QDir::toNativeSeparators(
                  dirInitFileSslDetector.absolutePath()).toStdString().data());

    char *t_out = NULL;
    if(_getError(&t_out))
    {
       qDebug() << t_out << "Выход";
       exit(-3);
    }

    qDebug() << "init func end";
     _aiocFunc();
     char* str;
    qDebug() << _getError(&str);
}
