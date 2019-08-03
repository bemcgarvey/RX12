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
    enum {IDLE, WAIT_SETTINGS, WAIT_LOG} state;
    int bytesNeeded;
    char buffer[64];
    int bufferPos;
    void setSettingsButtons(unsigned int *values);
    double calculateVoltage(unsigned int calibration, unsigned int value);

private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void on_actionExit_triggered();
    void on_readButton_clicked();
    void on_readyRead(void);
};

#endif // MAINWINDOW_H
