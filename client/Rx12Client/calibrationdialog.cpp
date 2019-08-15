#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"
#include "commands.h"

#include <QMessageBox>

CalibrationDialog::CalibrationDialog(QSerialPort *serialPort, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calibrationDialog), state(FIRST_VOLTAGE), port(serialPort)
{
    ui->setupUi(this);
}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}

void CalibrationDialog::on_cancelButton_clicked()
{
    if (state == FIRST_VOLTAGE) {
        reject();
    } else {
        //Single value calibration
        actualVoltage2 = 0.0;
        voltage2 = 0;
        calculateCalibration();
        state = DONE;
        QMessageBox::information(this, "Success", "Single value calibration complete");
        accept();
    }
}

void CalibrationDialog::on_nextButton_clicked()
{
    char buffer[4];
    int bytesRead = 0;
    int attempts = 0;
    if (state == FIRST_VOLTAGE) {
        actualVoltage1 = static_cast<float>(ui->voltageSpinBox->value());
        buffer[0] = GET_VOLTAGE;
        port->write(buffer, 1);
        while(bytesRead < 4 && attempts < 10) {
            port->waitForReadyRead(50);
            if (port->bytesAvailable() > 0) {
                bytesRead += port->read(&buffer[bytesRead], 4 - bytesRead);
            } else {
                ++attempts;
            }
        }
        if (bytesRead == 4) {
            voltage1 = static_cast<int>(*(reinterpret_cast<unsigned int *>(buffer)));
        } else {
            QMessageBox::critical(this, "Read Error", "Unable to get device voltage");
            reject();
        }
        ui->instructionLabel->setText("Second voltage (5.50 - 6.00 recommended)");
        ui->voltageSpinBox->setValue(6.0);
        state = SECOND_VOLTAGE;
        ui->cancelButton->setText("Single Value");
    } else if (state == SECOND_VOLTAGE) {
        actualVoltage2 = static_cast<float>(ui->voltageSpinBox->value());
        buffer[0] = GET_VOLTAGE;
        port->write(buffer, 1);
        while(bytesRead < 4 && attempts < 10) {
            port->waitForReadyRead(50);
            if (port->bytesAvailable() > 0) {
                bytesRead += port->read(&buffer[bytesRead], 4 - bytesRead);
            } else {
                ++attempts;
            }
        }
        if (bytesRead == 4) {
            voltage2 = static_cast<int>(*(reinterpret_cast<unsigned int *>(buffer)));
        } else {
            QMessageBox::critical(this, "Error", "Unable to read device voltage");
            reject();
        }
        calculateCalibration();
        state = DONE;
        QMessageBox::information(this, "Success", "Calibration complete");
        accept();
    }
}

void CalibrationDialog::calculateCalibration() {
    float m = static_cast<float>(actualVoltage1 - actualVoltage2) / static_cast<float>(voltage1 - voltage2);
    float b = static_cast<float>(actualVoltage1 - (m * voltage1));
    memcpy(&calibration1, &m, 4);
    memcpy(&calibration2, &b, 4);
}

bool CalibrationDialog::isDone() const
{
    return (state == DONE);
}

unsigned int CalibrationDialog::getCalibration1() const
{
    return calibration1;
}

unsigned int CalibrationDialog::getCalibration2() const
{
    return calibration2;
}
