#include "calibrationdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include "commands.h"
#include "logdata.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    port(nullptr), state(IDLE), logData(nullptr), firmwareVersion(0)
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
    if (logData != nullptr) {
        delete [] logData;
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
        port->read(buffer, 64); //clear out extra bytes from device power on
        connect(port, &QSerialPort::readyRead, this, &MainWindow::on_readyRead);
        ui->tabWidget->setEnabled(true);
        ui->tabWidget->setTabEnabled(1, false);
        ui->readButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->calibrateButton->setEnabled(true);
    } else {
        delete port;
        port = nullptr;
        portLabel->setText("----");
        connectedLabel->setText("Not Connected");
        ui->tabWidget->setEnabled(false);
    }
}


void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_readButton_clicked()
{
    bufferPos = 0;
    bytesNeeded = 32;
    state = WAIT_SETTINGS;
    buffer[0] = GET_SETTINGS;
    buffer[1] = GET_VERSION;
    port->write(buffer, 2);
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
                displaySettings(reinterpret_cast<unsigned int *>(buffer));
                ui->statusBar->showMessage("Read from device successful", 2000);
                ui->tabWidget->setTabEnabled(1, true);
            } else {
                ui->statusBar->showMessage("Error reading from device", 2000);
                ui->tabWidget->setTabEnabled(1, false);
            }
            state = IDLE;
        }
        break;
    case WAIT_LOG_COUNT:
        bytesReceived = static_cast<int>(port->read(&buffer[bufferPos], bytesNeeded));
        bytesNeeded -= bytesReceived;
        bufferPos += bytesReceived;
        if (bytesNeeded == 0) {
            int count;
            count = static_cast<int>(buffer[0]);
            ui->flightsAvailableLabel->setText(QString().setNum(count));
            if (count > 0) {
                ui->loadButton->setEnabled(true);
            } else {
                ui->loadButton->setEnabled(false);
            }
            ui->countSpinBox->setMaximum(count);
            state = IDLE;
        }
        break;
    case WAIT_LOG:
        bytesReceived = static_cast<int>(port->read((reinterpret_cast<char *>(logData)) + bufferPos, bytesNeeded));
        bytesNeeded -= bytesReceived;
        bufferPos += bytesReceived;
        if (bytesNeeded == 0) {
            ui->statusBar->showMessage("Success", 2000);
            currentLog = 0;
            displayLog(0);
            ui->nextButton->setEnabled(false);
            if (numLogs > 1) {
                ui->prevButton->setEnabled(true);
            } else {
                ui->prevButton->setEnabled(false);
            }
            state = IDLE;
        }
        break;
    case IDLE:
        break;
    case CALIBRATING:
        return;
    }
    //Read any leftover data
    while (port->bytesAvailable() > 0) {
        port->read(buffer, 64);
    }
}

void MainWindow::displaySettings(unsigned int *values) {
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
    calibration1 = *(reinterpret_cast<float *>((&values[4])));
    calibration2 = *(reinterpret_cast<float *>((&values[5])));
    double voltage = calculateVoltage(values[6]);
    ui->measuredVoltagelabel->setText(QString("%1V").arg(voltage, 0, 'f', 2));
    firmwareVersion = values[7];
}

double MainWindow::calculateVoltage(unsigned int value) {
    float result;
    result = value * calibration1;
    result += calibration2;
    return static_cast<double>(result);
}

void MainWindow::displayLog(int index)
{
    ui->sequenceLabel->setText(QString().setNum(logData[index].sequenceNumber));
    ui->durationLabel->setText(QString("%1 sec").arg(logData[index].duration / 1000));
    if (logData[index].fades[0] == 0xffffffff) {
        ui->rx1FadesLabel->setText("NA");
    } else {
        ui->rx1FadesLabel->setText(QString().setNum(logData[index].fades[0]));
    }
    if (logData[index].fades[1] == 0xffffffff) {
        ui->rx2FadesLabel->setText("NA");
    } else {
        ui->rx2FadesLabel->setText(QString().setNum(logData[index].fades[1]));
    }
    if (logData[index].fades[2] == 0xffffffff) {
        ui->rx3FadesLabel->setText("NA");
    } else {
        ui->rx3FadesLabel->setText(QString().setNum(logData[index].fades[2]));
    }
    ui->totalPacketsLabel->setText(QString().setNum(logData[index].totalPackets));
    double lowVoltage = calculateVoltage(logData[index].rxLowVoltage);
    double highVoltage = calculateVoltage(logData[index].rxHighVoltage);
    ui->rxLowVoltageLabel->setText(QString("%1V").arg(lowVoltage, 0, 'f', 2));
    ui->rxHighVoltageLabel->setText(QString("%1V").arg(highVoltage, 0, 'f', 2));
    QString status;
    if (logData[index].statusFlags == STATUS_NONE) {
        status = "--";
    } else {
        if (logData[index].statusFlags & STATUS_CF) {
            status += "CF ";
        }
        if (logData[index].statusFlags & STATUS_WDTO) {
            status += "WD ";
        }
        if (logData[index].statusFlags & STATUS_FAILSAFE) {
            status += "FS";
        }
    }
    ui->alertsLabel->setText(status);
}

void MainWindow::on_saveButton_clicked()
{
    if (ui->frame11RadioButton->isChecked()) {
        buffer[0] = SET_FRAME_11;
    } else {
        buffer[0] = SET_FRAME_22;
    }
    if (ui->loggingEnabledRadioButton->isChecked()) {
        buffer[1] = ENABLE_LOGGING;
    } else {
        buffer[1] = DISABLE_LOGGING;
    }
    port->write(buffer, 2);
    ui->statusBar->showMessage("Settings saved", 2000);
}

void MainWindow::on_calibrateButton_clicked()
{
    state = CALIBRATING;
    CalibrationDialog *dlg = new CalibrationDialog(port, this);
    if (dlg->exec() == QDialog::Accepted) {
        buffer[0] = SET_VOLTAGE_CALIBRATION;
        port->write(buffer, 1);
        unsigned int c1 = dlg->getCalibration1();
        unsigned int c2 = dlg->getCalibration2();
        *reinterpret_cast<unsigned int *>(&buffer[0]) = c1;
        *reinterpret_cast<unsigned int *>(&buffer[4]) = c2;
        port->write(buffer, 8);
    }
    delete dlg;
    state = IDLE;
    on_readButton_clicked();
}

void MainWindow::on_resetLogButton_clicked()
{
    buffer[0] = CLEAR_LOG;
    port->write(buffer, 1);
    ui->flightsAvailableLabel->setText("0");
    ui->loadButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->prevButton->setEnabled(false);
    ui->statusBar->showMessage("Flight logs reset", 2000);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 1) {
        bufferPos = 0;
        bytesNeeded = 1;
        state = WAIT_LOG_COUNT;
        buffer[0] = GET_LOG_COUNT;
        port->write(buffer, 1);
    }
}

void MainWindow::on_loadButton_clicked()
{
    numLogs = ui->countSpinBox->value();
    if (logData != nullptr) {
        delete [] logData;
    }
    logData = new LogData[numLogs];
    bufferPos = 0;
    bytesNeeded = numLogs * static_cast<int>(sizeof(LogData));
    state = WAIT_LOG;
    buffer[0] = GET_LOG;
    buffer[1] = static_cast<char>(numLogs);
    port->write(buffer, 2);
    ui->statusBar->showMessage("Loading ...", 0);
}

void MainWindow::on_prevButton_clicked()
{
    ++currentLog;
    displayLog(currentLog);
    if (currentLog == numLogs - 1) {
        ui->prevButton->setEnabled(false);
    }
    ui->nextButton->setEnabled(true);
}

void MainWindow::on_nextButton_clicked()
{
    --currentLog;
    displayLog(currentLog);
    if (currentLog == 0) {
        ui->nextButton->setEnabled(false);
    }
    ui->prevButton->setEnabled(true);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog *dlg = new AboutDialog(firmwareVersion, this);
    dlg->exec();
    delete dlg;
}
