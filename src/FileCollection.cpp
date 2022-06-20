#include "FileCollection.h"
#include <QtDebug>

FileCollection::FileCollection()
{

}

void FileCollection::addItemFromJson(QJsonObject const & jo)
{
   FileMetaData fmd;

   if (!jo.contains("name"))
   {
      qDebug() << "Can't add any objects that don't have names";
      return;
   }

   fmd.filename = jo.value("name").toString();
   fmd.md5hash = jo.value("md5").toString("");
   fmd.sha1hash = jo.value("sha1").toString("");
   fmd.format = jo.value("format").toString("");
   fmd.filesize = jo.value("size").toString("-1").toLongLong();

   fmd.markedForDownload = false;
   fmd.alreadyDownloaded = false;

   theMDList.append(fmd);

   qDebug() << "Added " << itemToString(fmd);
}

QString FileCollection::itemToString(FileMetaData const & fmd)
{
   QString retVal;
   retVal = fmd.filename;
   retVal += " [fmt=";
   retVal += fmd.format;
   retVal += ", md5=";
   retVal += fmd.md5hash;
   retVal += ", sha1=";
   retVal += fmd.sha1hash;
   retVal += ", size=";
   retVal += QString::number(fmd.filesize);
   retVal += ", markedForDL=";
   retVal += (fmd.markedForDownload ? "YES" : "NO");
   retVal += ", alreadyDL=";
   retVal += (fmd.alreadyDownloaded ? "YES" : "NO");
   retVal += "]";
   return retVal;
}

int64_t FileCollection::sizeOfAllItems()
{
   int64_t retVal = 0;
   foreach(auto iter, theMDList)
   {
      if (iter.filesize >= 0)
      {
         retVal += iter.filesize;
      }
   }

   return retVal;
}

int64_t FileCollection::sizeOfAllMarkedItems()
{
   int64_t retVal = 0;
   foreach(auto iter, theMDList)
   {
      if ( (iter.filesize >= 0) && (iter.markedForDownload) )
      {
         retVal += iter.filesize;
      }
   }

   return retVal;
}

int FileCollection::getNumFiles()
{
   return theMDList.size();
}

FileMetaData FileCollection::getMetaData(int n)
{
   Q_ASSERT(n >= 0);
   Q_ASSERT(n < theMDList.size());

   return theMDList.at(n);
}

void FileCollection::setCheckedState(bool state, int n)
{
   Q_ASSERT(n >= 0);
   Q_ASSERT(n < theMDList.size());

   theMDList[n].markedForDownload = state;
}


void FileCollection::setAlreadyDownloaded(bool state, int n)
{
   Q_ASSERT(n >= 0);
   Q_ASSERT(n < theMDList.size());

   theMDList[n].alreadyDownloaded = state;

}
