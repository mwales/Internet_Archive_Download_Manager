#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <QString>
#include <QSettings>


class ConfigData
{
public:
   ConfigData();

   void refreshConfigurationData();

   QString getPathToIaTool();

   QString getMetadataSwitches();

   QString getDownloadSwitches();

   void setPathToIaTool(QString path);

   void setMetadataSwitches(QString text);

   void setDownloadSwitches(QString text);

protected:

   QString theCfgPathToIaTool;
   QString theCfgExtraMetadataSwitches;
   QString theCfgExtraDownloadSwitches;

   QSettings* theSettings;
};

#endif // CONFIGDATA_H
