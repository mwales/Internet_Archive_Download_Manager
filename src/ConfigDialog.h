#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "ConfigData.h"

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
   Q_OBJECT

public:
   explicit ConfigDialog(ConfigData* configData, QWidget *parent = nullptr);
   ~ConfigDialog();

protected slots:

   void saveGuiConfiguration();

   void selectFileForIaTool();

protected:

   ConfigData* theCfg;

private:
   Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
