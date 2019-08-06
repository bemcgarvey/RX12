#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QDebug>
#include "commands.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    port(nullptr), state(IDLE)
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
        connectedLabel->setText("Connected");
        connect(port, &QSerialPort::readyRead, this, &MainWindow::on_readyRead);
        ui->readButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
    } else {
        delete port;
        port = nullptr;
        portLabel->setText("----");
        connectedLabel->setText("Not Connected");
        ui->readButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
    }
}


void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_readButton_clicked()
{
    bufferPos = 0;
    bytesNeeded = 24;
    state = WAIT_SETTINGS;
    buffer[0] = GET_SETTINGS;
    port->write(buffer, 1);
}

void MainWindow::on_readyRead(void) {
    int bytesReceived;
    switch (state) {
    case WAIT_SETTINGS:
        bytesReceived = static_cast<int>(port->read(&buffer[bufferPos], bytesNeeded));
        bytesNeeded -= bytesReceived;
        bufferPos += bytesReceived;
        if (bytesNeeded == 0) {
            if (buffer[0] == FRAME_11MS || buffer[0] == FRAME_22MS) {
                setSettingsButtons(reinterpret_cast<unsigned int *>(buffer));
                ui->statusBar->showMessage("Read from device successful", 2000);
            } else {
                ui->statusBar->showMessage("Error reading from device", 2000);
            }
            state = IDLE;
        }
        break;
    case WAIT_LOG:
        break;
    case IDLE:
        break;
    }
    //Read any leftover data
    while (port->bytesAvailable() > 0) {
        port->read(buffer, 64);
    }
}

void MainWindow::setSettingsButtons(unsigned int *values) {
    ui->frame22RadioButton->setChecked(false);
    ui->frame11RadioButton->setChecked(false);
    if (values[0] == FRAME_22MS) {
        ui->frame22RadioButton->setChecked(true);
    } else if (values[0] == FRAME_11MS) {
        ui->frame11RadioButton->setChecked(true);
    }
    ui->dsm2_1024radioButton->setChecked(false);
    ui->dsm2_2048RadioButton->setChecked(false);
    ui->dsmxRadioButton->setChecked(false);
    if (values[1] == SYSTEM_TYPE_DSM2_1024) {
        ui->dsm2_1024radioButton->setChecked(true);
    } else if (values[1] == SYSTEM_TYPE_DSM2_2048) {
        ui->dsm2_2048RadioButton->setChecked(true);
    } else {
        ui->dsmxRadioButton->setChecked(true);
    }
    ui->presetFailsafeRadioButton->setChecked(false);
    ui->normalFailsafeRadioButton->setChecked(false);
    if (values[2] == PRESET_FAILSAFE) {
        ui->presetFailsafeRadioButton->setChecked(true);
    } else {
        ui->normalFailsafeRadioButton->setChecked(true);
    }
    ui->loggingEnabledRadioButton->setChecked(false);
    ui->loggingDisabledRadioButton->setChecked(false);
    if (values[3] == LOGGING_MAGIC_NUMBER) {
        ui->loggingEnabledRadioButton->setChecked(true);
    } else {
        ui->loggingDisabledRadioButton->setChecked(true);
    }
    double voltage = calculateVoltage(values[4], values[5]);
    ui->measuredVoltagelabel->setText(QString("%1").arg(voltage, 0, 'f', 2));
}

double MainWindow::calculateVoltage(unsigned int calibration, unsigned int value) {
    double result;
    result = value * 3.3 / 4096;
    result *= (43.0/10.0);
    return result;
}
