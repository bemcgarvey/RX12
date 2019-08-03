#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    port(nullptr)
{
    ui->setupUi(this);
    portLabel = new QLabel();
    portLabel->setText("----");
    connectedLabel = new QLabel();
    connectedLabel->setText("Not Connected");
    ui->statusBar->addPermanentWidget(portLabel);
    ui->statusBar->addWidget(connectedLabel);
    connect(ui->menuPort, &QMenu::aboutToShow, this, &MainWindow::updatePortMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (port != nullptr) {
        port->close();
        delete port;
    }
}

void MainWindow::updatePortMenu()
{
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    ui->menuPort->clear();
    for (auto i : portList) {
        QAction * action = ui->menuPort->addAction(i.portName(), this, &MainWindow::comPortSelected);
        action->setCheckable(true);
        if (i.portName() == portLabel->text()) {
            action->setChecked(true);
        }
    }
}

void MainWindow::comPortSelected()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    if (port != nullptr) {
        port->close();
        delete port;
    }
    port = new QSerialPort(action->text(), this);
    if (port->open(QIODevice::ReadWrite)) {
        port->setBaudRate(QSerialPort::Baud115200);
        port->setDataBits(QSerialPort::Data8);
        port->setParity(QSerialPort::NoParity);
        port->setStopBits(QSerialPort::OneStop);
        portLabel->setText(action->text());
    } else {
        delete port;
        port = nullptr;
        portLabel->setText("----");
        connectedLabel->setText("Not Connected");
    }
}


void MainWindow::on_actionExit_triggered()
{
    close();
}
