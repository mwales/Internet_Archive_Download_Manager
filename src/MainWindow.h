#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QProcess>
#include <QTableWidgetItem>
#include <QTimer>

#include "ConfigData.h"
#include "FileCollection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();

private slots:

   void showAboutQt();

   void showAboutTool();

   void showConfigureWindow();

   void startMetadataDownload();

   void metadataDownloadTimeout();

   void metadataDownloadComplete(int exitCode, QProcess::ExitStatus exitStatus);

   void metadataDownloadCompleteError(QProcess::ProcessError error);

   void metadataCellItemChanged(QTableWidgetItem* item);

   void startDownloading();

   void fileDownloadStatusUpdate();

   void fileDownloadComplete(int exitCode, QProcess::ExitStatus exitStatus);

   void fileDownloadCompleteError(QProcess::ProcessError error);

protected:

   void updateCheckedSizes();

   void processMetadata(QByteArray metadataText);

   void updateTableWidget();

   void updateProgressBars();

   void startDownloadNextFile();

   QString processErrorString(QProcess::ProcessError err);

   ConfigData theCfg;

   QProcess theMetadataDownloadProcess;

   FileCollection theMD;

   QSet<int> theMDItemsChecked;

   int64_t theNumberBytesCompletedFiles;
   int64_t theNumberBytesCurrentFile;

   FileMetaData const * theCurrentFileDownloading;

   // Timer used to call fileDownloadStatusUpdate periodically
   QTimer theFileDownloadUpdateTimer;

   QProcess theFileDownloadProcess;

   QString theDestDir;

private:
   Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
