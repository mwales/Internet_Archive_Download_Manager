#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

#include <QtDebug>
#include <QFileDialog>

ConfigDialog::ConfigDialog(ConfigData* configData, QWidget *parent):
   theCfg(configData),
   QDialog(parent),
   ui(new Ui::ConfigDialog)
{
   ui->setupUi(this);

   ui->theToolLocationEdit->setText(theCfg->getPathToIaTool());
   ui->theMetadataSwitchesEdit->setText(theCfg->getMetadataSwitches());
   ui->theDownloadSwitchesEdit->setText(theCfg->getDownloadSwitches());

   connect(ui->theDialogButtonBox, &QDialogButtonBox::accepted,
           this, &ConfigDialog::saveGuiConfiguration);
   connect(ui->theChooseToolLocationButton, &QPushButton::pressed,
           this, &ConfigDialog::selectFileForIaTool);
}

ConfigDialog::~ConfigDialog()
{
   delete ui;
}

void ConfigDialog::saveGuiConfiguration()
{
   qDebug() << "Save GUI configuration";

   theCfg->setPathToIaTool(ui->theToolLocationEdit->text());
   theCfg->setMetadataSwitches(ui->theMetadataSwitchesEdit->text());
   theCfg->setDownloadSwitches(ui->theDownloadSwitchesEdit->text());
}

void ConfigDialog::selectFileForIaTool()
{

   QString iaPath = QFileDialog::getOpenFileName(this, "Where is the IA tool?");
   if (iaPath != "")
   {
      ui->theToolLocationEdit->setText(iaPath);
   }

}
