#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QProcess>

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

protected:

   void refreshConfigurationData();

   void processMetadata(QByteArray metadataText);

   void updateTableWidget();

   ConfigData theCfg;

   QProcess theMetadataDownloadProcess;

   FileCollection theMD;

private:
   Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
