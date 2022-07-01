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
   ui->theMetadataSwitchesEdit->addItems(theCfg->getMetadataSwitches());
   ui->theDownloadSwitchesEdit->addItems(theCfg->getDownloadSwitches());

   connect(ui->theDialogButtonBox, &QDialogButtonBox::accepted,
           this, &ConfigDialog::saveGuiConfiguration);
   connect(ui->theChooseToolLocationButton, &QPushButton::pressed,
           this, &ConfigDialog::selectFileForIaTool);

   connect(ui->theDlArgLineEdit, &QLineEdit::returnPressed,
           this, &ConfigDialog::addDownloadArg);
   connect(ui->theDlAddArgButton, &QPushButton::pressed,
           this, &ConfigDialog::addDownloadArg);
   connect(ui->theDlRemoveArgButton, &QPushButton::pressed,
           this, &ConfigDialog::removeDownloadArg);

   connect(ui->theMdArgLineEdit, &QLineEdit::returnPressed,
           this, &ConfigDialog::addMetadataArg);
   connect(ui->theMdAddArgButton, &QPushButton::pressed,
           this, &ConfigDialog::addMetadataArg);
   connect(ui->theMdRemoveArgButton, &QPushButton::pressed,
           this, &ConfigDialog::removeMetadataArg);

}

ConfigDialog::~ConfigDialog()
{
   delete ui;
}

void ConfigDialog::saveGuiConfiguration()
{
   qDebug() << "Save GUI configuration";

   theCfg->setPathToIaTool(ui->theToolLocationEdit->text());

   QStringList mdList;
   for(int i = 0; i < ui->theMetadataSwitchesEdit->count(); i++)
   {
      mdList.append(ui->theMetadataSwitchesEdit->item(i)->text());
   }
   theCfg->setMetadataSwitches(mdList);

   QStringList dlList;
   for(int i = 0; i < ui->theDownloadSwitchesEdit->count(); i++)
   {
      dlList.append(ui->theDownloadSwitchesEdit->item(i)->text());
   }
   theCfg->setDownloadSwitches(dlList);

}

void ConfigDialog::selectFileForIaTool()
{

   QString iaPath = QFileDialog::getOpenFileName(this, "Where is the IA tool?");
   if (iaPath != "")
   {
      ui->theToolLocationEdit->setText(iaPath);
   }

}

void ConfigDialog::addMetadataArg()
{
   qDebug() << __PRETTY_FUNCTION__;

   ui->theMetadataSwitchesEdit->addItem(ui->theMdArgLineEdit->text());
}

void ConfigDialog::removeMetadataArg()
{
   qDebug() << __PRETTY_FUNCTION__;

   auto mdArgsToRemove = ui->theMetadataSwitchesEdit->selectedItems();
   foreach(auto curItem, mdArgsToRemove)
   {
      qDebug() << "MD Item to remove" << curItem->text();
      ui->theMetadataSwitchesEdit->removeItemWidget(curItem);
   }
}

void ConfigDialog::addDownloadArg()
{
   qDebug() << __PRETTY_FUNCTION__;
   ui->theDownloadSwitchesEdit->addItem(ui->theDlArgLineEdit->text());
}

void ConfigDialog::removeDownloadArg()
{
   qDebug() << __PRETTY_FUNCTION__;
}
