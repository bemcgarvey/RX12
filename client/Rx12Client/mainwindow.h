#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSerialPort>
#include "logdata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *portLabel;
    QLabel *connectedLabel;
    QSerialPort *port;
    enum {IDLE, WAIT_SETTINGS, WAIT_LOG, CALIBRATING, WAIT_LOG_COUNT, WAIT_VERSION} state;
    enum {OUTPUT_TYPE_PWM = 0x37, OUTPUT_TYPE_PPM = 0x92};
    int bytesNeeded;
    char buffer[64];
    int bufferPos;
    void displaySettings(unsigned int *values);
    double calculateVoltage(unsigned int value);
    LogData *logData;
    void displayLog(int index);
    int currentLog;
    int numLogs;
    float calibration1;
    float calibration2;
    double firmwareVersion;

private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void on_actionExit_triggered();
    void on_readButton_clicked();
    void on_readyRead(void);
    void on_saveButton_clicked();
    void on_calibrateButton_clicked();
    void on_resetLogButton_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_loadButton_clicked();
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void on_actionAbout_triggered();
};

#endif // MAINWINDOW_H
