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

private slots:
    void updatePortMenu(void);
    void comPortSelected(void);
    void on_actionExit_triggered();
};

#endif // MAINWINDOW_H
