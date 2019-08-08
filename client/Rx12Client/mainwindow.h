#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSerialPort>

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
    enum {IDLE, WAIT_SETTINGS, WAIT_LOG, CALIBRATING} state;
    int bytesNeeded;
    char buffer[64];
    int bufferPos;
    void displaySettings(unsigned int *values);
    double calculateVoltage(float calibration1, float calibration2, unsigned int value);

private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void on_actionExit_triggered();
    void on_readButton_clicked();
    void on_readyRead(void);
    void on_saveButton_clicked();
    void on_calibrateButton_clicked();
    void on_resetLogButton_clicked();
};

#endif // MAINWINDOW_H
