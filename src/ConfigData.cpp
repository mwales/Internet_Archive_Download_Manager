#include "ConfigData.h"

#include <QSettings>
#include <QtDebug>

const QString ORG_NAME = "mwalesnet";
const QString APP_NAME = "internet_archive_download_manager";

const QString TOOL_LOCATION_SETTING_NAME = "ia_tool_location";
const QString METADATA_SWITCHES_SETTING_NAME = "metadata_extra_switches";
const QString DOWNLOAD_SWITCHES_SETTING_NAME = "download_extra_switches";


ConfigData::ConfigData()
{
   theCfgPathToIaTool = "./ia";
   theCfgExtraMetadataSwitches = "";
   theCfgExtraDownloadSwitches = "";

   theSettings = new QSettings(ORG_NAME, APP_NAME);

   refreshConfigurationData();
}

void ConfigData::refreshConfigurationData()
{
   qDebug() << "Configuration File Path: " << theSettings->fileName();
   qDebug() << "Config Keys: " << theSettings->allKeys();

   QStringList configKeys = theSettings->allKeys();

   if (configKeys.contains(TOOL_LOCATION_SETTING_NAME))
   {
      theCfgPathToIaTool = theSettings->value(TOOL_LOCATION_SETTING_NAME, "error_with_config").toString();
   }

   if (configKeys.contains(METADATA_SWITCHES_SETTING_NAME))
   {
      theCfgExtraMetadataSwitches = theSettings->value(METADATA_SWITCHES_SETTING_NAME, "error_with_config").toString();
   }

   if (configKeys.contains(DOWNLOAD_SWITCHES_SETTING_NAME))
   {
      theCfgExtraDownloadSwitches = theSettings->value(DOWNLOAD_SWITCHES_SETTING_NAME, "error_with_config").toString();
   }
}

QString ConfigData::getPathToIaTool()
{
   return theCfgPathToIaTool;
}

QString ConfigData::getMetadataSwitches()
{
   return theCfgExtraMetadataSwitches;
}

QString ConfigData::getDownloadSwitches()
{
   return theCfgExtraDownloadSwitches;
}

void ConfigData::setPathToIaTool(QString path)
{
   if (path != theCfgPathToIaTool)
   {
      qDebug() << "IA Tool Location updated to: " << path;
      theCfgPathToIaTool = path;
      theSettings->setValue(TOOL_LOCATION_SETTING_NAME, theCfgPathToIaTool);
      theSettings->sync();
   }
}

void ConfigData::setMetadataSwitches(QString text)
{
   if (text != theCfgExtraMetadataSwitches)
   {
      qDebug() << "Metadata configuration updated to: " << text;
      theCfgExtraMetadataSwitches = text;
      theSettings->setValue(METADATA_SWITCHES_SETTING_NAME, theCfgExtraMetadataSwitches);
      theSettings->sync();
   }
}

void ConfigData::setDownloadSwitches(QString text)
{
   if (text != theCfgExtraDownloadSwitches)
   {
      qDebug() << "Download switches updated to: " << text;
      theCfgExtraDownloadSwitches = text;
      theSettings->setValue(DOWNLOAD_SWITCHES_SETTING_NAME, theCfgExtraDownloadSwitches);
      theSettings->sync();
   }
}
