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

   QStringList getMetadataSwitches();

   QStringList getDownloadSwitches();

   void setPathToIaTool(QString path);

   void setMetadataSwitches(QStringList argList);

   void setDownloadSwitches(QStringList argList);

protected:

   QString theCfgPathToIaTool;
   QStringList theCfgExtraMetadataSwitches;
   QStringList theCfgExtraDownloadSwitches;

   QSettings* theSettings;
};

#endif // CONFIGDATA_H
