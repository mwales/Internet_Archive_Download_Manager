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
      qDebug() << "Config IA Path=" << theCfgPathToIaTool;
   }

   if (configKeys.contains(METADATA_SWITCHES_SETTING_NAME))
   {
      QString value = theSettings->value(METADATA_SWITCHES_SETTING_NAME, "error_with_config").toString();
      qDebug() << "Config Metadata Arg =" << value;
      theCfgExtraMetadataSwitches =  value.split("&");
   }

   if (configKeys.contains(DOWNLOAD_SWITCHES_SETTING_NAME))
   {
      QString value = theSettings->value(DOWNLOAD_SWITCHES_SETTING_NAME, "error_with_config").toString();
      qDebug() << "Config Download Arg =" << value;
      theCfgExtraDownloadSwitches =  value.split("&");
   }
}

QString ConfigData::getPathToIaTool()
{
   return theCfgPathToIaTool;
}

QStringList ConfigData::getMetadataSwitches()
{
   return theCfgExtraMetadataSwitches;
}

QStringList ConfigData::getDownloadSwitches()
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

void ConfigData::setMetadataSwitches(QStringList argList)
{
   if (argList != theCfgExtraMetadataSwitches)
   {
      qDebug() << "Metadata configuration updated to: " << argList;
      theCfgExtraMetadataSwitches = argList;
      theSettings->setValue(METADATA_SWITCHES_SETTING_NAME, theCfgExtraMetadataSwitches.join("&"));
      theSettings->sync();
   }
}

void ConfigData::setDownloadSwitches(QStringList argList)
{
   if (argList != theCfgExtraDownloadSwitches)
   {
      qDebug() << "Download switches updated to: " << argList;
      theCfgExtraDownloadSwitches = argList;
      theSettings->setValue(DOWNLOAD_SWITCHES_SETTING_NAME, theCfgExtraDownloadSwitches.join("&"));
      theSettings->sync();
   }
}
