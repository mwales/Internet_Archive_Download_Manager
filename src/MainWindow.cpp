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
#include <QDir>
#include <QFileInfo>

#include "ConfigDialog.h"

// Sample ia commands
// ./ia download "nintendo-64-romset-usa/Tony Hawk's Pro Skater 3 (U).zip"
// ./ia metadata "nintendo-64-romset-usa"


MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent),
     theMetadataDownloadProcess(this),
     theNumberBytesCompletedFiles(0),
     theNumberBytesCurrentFile(0),
     theCurrentFileDownloading(nullptr),
     theFileDownloadUpdateTimer(this),
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
   connect(ui->theCollectionName, &QLineEdit::returnPressed,
           this, &MainWindow::startMetadataDownload);
   connect(ui->theStartDlFilesButton, &QPushButton::pressed,
           this, &MainWindow::startDownloading);

   connect(&theMetadataDownloadProcess, &QProcess::errorOccurred,
           this, &MainWindow::metadataDownloadCompleteError);

   connect(&theMetadataDownloadProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
           this, SLOT(metadataDownloadComplete(int, QProcess::ExitStatus)));

   connect(ui->theMDTable, &QTableWidget::itemChanged,
           this, &MainWindow::metadataCellItemChanged);

   theFileDownloadUpdateTimer.setInterval(1000);
   connect(&theFileDownloadUpdateTimer, &QTimer::timeout,
           this, &MainWindow::fileDownloadStatusUpdate);

   connect(&theFileDownloadProcess, &QProcess::errorOccurred,
           this, &MainWindow::fileDownloadCompleteError);

   connect(&theFileDownloadProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
           this, SLOT(fileDownloadComplete(int, QProcess::ExitStatus)));

   theDestDir = "./";

   return;
   }

void MainWindow::showAboutQt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::showAboutTool()
{
   QMessageBox::about(this, "About I.A.D.M.",
                      "<h2>Internet Archive Download Manager</h2>\n\n"
                      "<p>v0.1 - Lets download some ROMs</p>\n"
                      "<p>By: boot_force</p>");
}

void MainWindow::showConfigureWindow()
{
   qDebug() << "Open configuration screen";

   ConfigDialog cd(&theCfg, this);
   cd.exec();
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::startMetadataDownload()
{
   qDebug() << "Start metadata download for " << ui->theCollectionName->text();

   theMD.clearCollection();
   updateTableWidget();

   QStringList args;
   args.append("metadata");
   args.append(ui->theCollectionName->text());

   theMetadataDownloadProcess.setProgram(theCfg.getPathToIaTool());
   theMetadataDownloadProcess.setArguments(args);

   qDebug() << "Starting metadata process:" << theCfg.getPathToIaTool() << args.join(" ");

   theMetadataDownloadProcess.start();

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
   QMessageBox::critical(this, "Metadata Download Error", processErrorString(error));
   ui->theDlMetadataButton->setEnabled(true);
}

void MainWindow::processMetadata(QByteArray metadataText)
{
   // Metadata output should be a JSON structure. Want to look for the member in the structure called files, should be an array.
   QJsonDocument loadDoc(QJsonDocument::fromJson(metadataText));

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
      checkItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      ui->theMDTable->setItem(i, 0, checkItem);

      QTableWidgetItem* formatItem = new QTableWidgetItem(fmd.format);
      formatItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      ui->theMDTable->setItem(i, 1, formatItem);

      QTableWidgetItem* filenameItem = new QTableWidgetItem(fmd.filename);
      filenameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      ui->theMDTable->setItem(i, 2, filenameItem);

      QTableWidgetItem* sizeItem = new QTableWidgetItem(QString::number(fmd.filesize));
      sizeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      ui->theMDTable->setItem(i, 3, sizeItem);

      ui->theMDTable->resizeColumnsToContents();
      ui->theMDTable->setSelectionBehavior(QAbstractItemView::SelectRows);
   }
}

void MainWindow::metadataCellItemChanged(QTableWidgetItem* item)
{
   qDebug() << "Item Change: " << item->column() << ", " << item->row();

   Qt::CheckState state = item->checkState();

   qDebug() << "Items that are currently selected";
   foreach(auto curSelectedItem, ui->theMDTable->selectedItems())
   {
      //qDebug() << "  Item at " << curSelectedItem->column() << ", " << curSelectedItem->row() << " is selected!";

      // Unselect the items that are selected, makes things weird later if we don't
      curSelectedItem->setSelected(false);

      if (curSelectedItem->column() == 0)
      {
         curSelectedItem->setCheckState(state);

         if (state == Qt::Checked)
         {
            theMD.setCheckedState(true, curSelectedItem->row());
         }
         else
         {
            theMD.setCheckedState(false, curSelectedItem->row());
         }
      }
   }

   // Always update the check state of the current item (cause it may not be selected)
   if (state == Qt::Checked)
   {
      theMD.setCheckedState(true, item->row());
   }
   else
   {
      theMD.setCheckedState(false, item->row());
   }

   updateCheckedSizes();

}

void MainWindow::updateCheckedSizes()
{
   QLocale locale = this->locale();
   int64_t numBytesReq = theMD.sizeOfAllMarkedItems();
   QString valueText = locale.formattedDataSize(numBytesReq);
   ui->theSpaceReqd->setText(valueText);

   if (numBytesReq > 0)
   {
      ui->theStartDlFilesButton->setEnabled(true);
   }
   else
   {
      ui->theStartDlFilesButton->setEnabled(false);
   }
}

void MainWindow::startDownloading()
{
   qDebug() << "Start Downloading pressed!";

   int64_t totalBytesToDownload = theMD.sizeOfAllMarkedItems();
   theNumberBytesCompletedFiles = 0;
   theNumberBytesCurrentFile = 0;
   ui->theTotalProgress->setMaximum(totalBytesToDownload);

   updateProgressBars();

   startDownloadNextFile();

   // Don't let user spam the button for downloading
   ui->theStartDlFilesButton->setEnabled(false);

   theFileDownloadUpdateTimer.start();
}

void MainWindow::updateProgressBars()
{
   int curTotalProgress = theNumberBytesCompletedFiles + theNumberBytesCurrentFile;
   ui->theTotalProgress->setValue(curTotalProgress);
   ui->theFileProgress->setValue(theNumberBytesCurrentFile);

   QLocale locale = this->locale();
   ui->theBytesDownloaded->setText(locale.formattedDataSize(curTotalProgress));
}

void MainWindow::startDownloadNextFile()
{
   theCurrentFileDownloading = theMD.getNextFileToDownload();

   if(theCurrentFileDownloading == nullptr)
   {
      qDebug() << "Stop all the downloading!";
      //ui->theStartDlFilesButton->setEnabled(true);
      QMessageBox::information(this, "Download Complete",
                               QString("Downloaded %1 files successfully").arg(theMD.getNumberOfDownloadedFiles()));
      return;
   }

   qDebug() << "Name of the next file " << theCurrentFileDownloading->filename;
   ui->theCurrentFileDlName->setText(theCurrentFileDownloading->filename);

   ui->theFileProgress->setMaximum(theCurrentFileDownloading->filesize);

   theFileDownloadProcess.setProgram(theCfg.getPathToIaTool());
   QStringList args;
   args.append("download");
   args.append(QString("%1%2%3").arg(ui->theCollectionName->text())
                                .arg(QDir::separator())
                                .arg(theCurrentFileDownloading->filename));
   args.append(QString("--destdir=%1").arg(theDestDir));
   args.append("-C"); // checksum

   theFileDownloadProcess.setArguments(args);
   theFileDownloadProcess.start();

   qDebug() << "Proces Started: " << theFileDownloadProcess.program() << " " << args.join(" ");

}

void MainWindow::fileDownloadStatusUpdate()
{
   if (!theCurrentFileDownloading)
   {
      // Not downloading anything, just exit
      return;
   }

   qDebug() << __PRETTY_FUNCTION__;

   QFileInfo fi(theDestDir + QDir::separator() + ui->theCollectionName->text() +
                QDir::separator() + theCurrentFileDownloading->filename);
   theNumberBytesCurrentFile = fi.size();

   ui->theFileProgress->setValue(theNumberBytesCurrentFile);

   qDebug() << "Size of" << fi.filePath() << "is" << theNumberBytesCurrentFile;

}

void MainWindow::fileDownloadComplete(int exitCode, QProcess::ExitStatus exitStatus)
{
   qDebug() << "File Download complete. ExitCode=" << exitCode;
   qDebug() << "File Download StdOut = ";
   qDebug() << theFileDownloadProcess.readAllStandardOutput();
   qDebug() << "File Download StdErr = ";
   qDebug() << theFileDownloadProcess.readAllStandardError();

   QFileInfo fi(theDestDir + QDir::separator() + ui->theCollectionName->text() +
                QDir::separator() + theCurrentFileDownloading->filename);
   theNumberBytesCurrentFile = fi.size();
   ui->theFileProgress->setValue(theNumberBytesCurrentFile);

   theNumberBytesCompletedFiles += theNumberBytesCurrentFile;

   QLocale locale = this->locale();
   ui->theBytesDownloaded->setText(locale.formattedDataSize(theNumberBytesCompletedFiles));
   ui->theTotalProgress->setValue(theNumberBytesCompletedFiles);

   theMD.setDownloadComplete(theCurrentFileDownloading);
   theCurrentFileDownloading = nullptr;

   startDownloadNextFile();
}

void MainWindow::fileDownloadCompleteError(QProcess::ProcessError error)
{
   qDebug() << "File Download complete with an error";

   qDebug() << "File Download StdOut = ";
   qDebug() << theFileDownloadProcess.readAllStandardOutput();
   qDebug() << "File Download StdErr = ";
   qDebug() << theFileDownloadProcess.readAllStandardError();

   QMessageBox::critical(this, "File Download Error", processErrorString(error));
}

QString MainWindow::processErrorString(QProcess::ProcessError err)
{
   QMap<QProcess::ProcessError, QString> errStrings;
   errStrings[QProcess::FailedToStart] = "The process failed to start.  Program missing or bad permission";
   errStrings[QProcess::Crashed] = "The process crashed after it started";
   errStrings[QProcess::Timedout] = "Timed out for waiting for the process";
   errStrings[QProcess::WriteError] = "Error writing to the process";
   errStrings[QProcess::ReadError] = "Error reading from the process";
   errStrings[QProcess::UnknownError] = "Unknown error occurred";

   return errStrings.value(err, "Error lookup failed");
}
