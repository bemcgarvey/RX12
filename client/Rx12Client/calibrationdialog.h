#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class calibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog(QSerialPort *serialPort, QWidget *parent = nullptr);
    ~CalibrationDialog();
    unsigned int getCalibration2() const;
    unsigned int getCalibration1() const;
    bool isDone() const;

private slots:
    void on_cancelButton_clicked();
    void on_nextButton_clicked();

private:
    Ui::calibrationDialog *ui;
    unsigned int calibration1;
    unsigned int calibration2;
    int voltage1;
    int voltage2;
    float actualVoltage1;
    float actualVoltage2;
    enum {FIRST_VOLTAGE, SECOND_VOLTAGE, DONE} state;
    QSerialPort *port;
    void calculateCalibration();
};

#endif // CALIBRATIONDIALOG_H
