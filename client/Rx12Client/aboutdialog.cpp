#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

AboutDialog::AboutDialog(uint16_t firmwareVersion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    ui->setupUi(this);
    ui->clientVersionLabel->setText(QString("%1.%2").arg(MAJOR_VERSION).arg(MINOR_VERSION));
    ui->firmwareVersionLabel->setText(QString("%1.%2").arg(firmwareVersion >> 8).arg(firmwareVersion & 0xff));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_closeButton_clicked()
{
    accept();
}
