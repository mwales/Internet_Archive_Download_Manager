#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFile>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QStringList>
#include <QIODevice>


#include "ConfigDialog.h"

// Sample ia commands
// ./ia download "nintendo-64-romset-usa/Tony Hawk's Pro Skater 3 (U).zip"
// ./ia metadata "nintendo-64-romset-usa"


MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent),
     theMetadataDownloadProcess(this),
     ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   // Plumb the menu bar actions
   connect(ui->theAboutQtAction, &QAction::triggered,
           this, &MainWindow::showAboutQt);
   connect(ui->theAboutToolAction, &QAction::triggered,
           this, &MainWindow::showAboutTool);
   connect(ui->theConfigureAction, &QAction::triggered,
           this, &MainWindow::showConfigureWindow);

   connect(ui->theDlMetadataButton, &QPushButton::pressed,
           this, &MainWindow::startMetadataDownload);

   connect(&theMetadataDownloadProcess, &QProcess::errorOccurred,
           this, &MainWindow::metadataDownloadCompleteError);

   connect(&theMetadataDownloadProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
           this, SLOT(metadataDownloadComplete(int, QProcess::ExitStatus)));
   return;
   }

void MainWindow::showAboutQt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::showAboutTool()
{
   QMessageBox::about(this, "About Internet Archive Download Manager",
                      "v0.1 - Lets download some ROMs\n\nBy: boot_force");
}

void MainWindow::showConfigureWindow()
{
   qDebug() << "Open configuration screen";

   ConfigDialog cd(&theCfg, this);
   cd.exec();

   refreshConfigurationData();
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::refreshConfigurationData()
{

}

void MainWindow::startMetadataDownload()
{
   qDebug() << "Start metadata download for " << ui->theCollectionName->text();

   QStringList args;

   theMetadataDownloadProcess.start(theCfg.getPathToIaTool(), args, QIODevice::ReadOnly);

   ui->theDlMetadataButton->setEnabled(false);
}

void MainWindow::metadataDownloadTimeout()
{
   qDebug() << "The metadata download has timed out";
   ui->theDlMetadataButton->setEnabled(true);
}

void MainWindow::metadataDownloadComplete(int exitCode, QProcess::ExitStatus exitStatus)
{
   qDebug() << "The metadata download is complete!";

   QByteArray metadatatext = theMetadataDownloadProcess.readAllStandardOutput();

   processMetadata(metadatatext);



   ui->theDlMetadataButton->setEnabled(true);
}

void MainWindow::metadataDownloadCompleteError(QProcess::ProcessError error)
{
   qDebug() << "The metadata download is complete with error!";
   ui->theDlMetadataButton->setEnabled(true);
}

void MainWindow::processMetadata(QByteArray metadataText)
{
   // Metadata output should be a JSON structure. Want to look for the member in the structure called files, should be an array.
   QJsonDocument loadDoc(QJsonDocument::fromJson(metadataText));

   //qDebug() << "is array" << loadDoc.isArray();
   //qDebug() << "is object" << loadDoc.isObject();

   if (!loadDoc.isObject())
   {
      QMessageBox::critical(this, "Metadata format error", "Can't load root JSON object");
      return;
   }

   QJsonObject od = loadDoc.object();

   // Do we have the files key?
   const QString FILES_KEYWORD = "files";
   if (!od.contains(FILES_KEYWORD))
   {
      QMessageBox::critical(this, "Metadata format error", "Can't find the files member in the JSON object");
      return;
   }

   QJsonValue metadataFileList = od.value(FILES_KEYWORD);
   if (!metadataFileList.isArray())
   {
      QMessageBox::critical(this, "Metadata format error", "We found the files struct member, but its not an array");
      return;
   }

   QJsonArray fa = metadataFileList.toArray();

   foreach(auto ai, fa)
   {
      if (ai.isObject())
      {
         theMD.addItemFromJson(ai.toObject());
      }
   }

   QLocale locale = this->locale();
   QString valueText = locale.formattedDataSize(theMD.sizeOfAllItems());

   updateTableWidget();

   QMessageBox::information(this, "File Metadata Received",
                            "Size of all files in metadata = " + valueText);


}

void MainWindow::updateTableWidget()
{
   ui->theMDTable->setColumnCount(4);
   ui->theMDTable->setRowCount(theMD.getNumFiles());

   QStringList labels;
   labels.append("DL?");
   labels.append("Format");
   labels.append("File Name");
   labels.append("Size");
   ui->theMDTable->setHorizontalHeaderLabels(labels);

   for(int i = 0; i < theMD.getNumFiles(); i++)
   {
      FileMetaData fmd = theMD.getMetaData(i);

      QTableWidgetItem* checkItem = new QTableWidgetItem();
      checkItem->setCheckState(fmd.markedForDownload ? Qt::Checked : Qt::Unchecked);
      checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      ui->theMDTable->setItem(i, 0, checkItem);

      QTableWidgetItem* formatItem = new QTableWidgetItem(fmd.format);
      ui->theMDTable->setItem(i, 1, formatItem);

      QTableWidgetItem* filenameItem = new QTableWidgetItem(fmd.filename);
      ui->theMDTable->setItem(i, 2, filenameItem);

      QTableWidgetItem* sizeItem = new QTableWidgetItem(QString::number(fmd.filesize));
      ui->theMDTable->setItem(i, 3, sizeItem);

      ui->theMDTable->resizeColumnsToContents();
      ui->theMDTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   }
}

