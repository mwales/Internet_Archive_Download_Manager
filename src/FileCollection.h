#ifndef FILECOLLECTION_H
#define FILECOLLECTION_H

#include <stdint.h>
#include <QString>
#include <QJsonObject>
#include <QVector>


/*
  Array Item:
   crc32" :  "690ec837"
   format" : "ZIP"
   md5" :    "9c304f5459a1f9aa2246be05c2b9dc99"
   mtime" :  "1622849751"
   name" :   "Yoshi's Story (U) (M2) [!].zip"
   sha1" :   "ad0bb391574eb697bf54fa7d2b3580f487164fb1"
   size" :   "9645654"
   source" : "original"
*/

typedef struct FileMetaDataStruct
{
   QString format;
   QString md5hash;
   QString sha1hash;
   QString filename;
   int64_t filesize;
   bool markedForDownload;
   bool alreadyDownloaded;
} FileMetaData;

class FileCollection
{
public:
   FileCollection();

   void addItemFromJson(QJsonObject const & jo);

   static QString itemToString(FileMetaData const & fmd);

   int64_t sizeOfAllItems();

   int64_t sizeOfAllMarkedItems();

   int getNumFiles();

   FileMetaData getMetaData(int n);

   void setCheckedState(bool state, int n);

   void setAlreadyDownloaded(bool state, int n);

   void setDownloadComplete(FileMetaData const * md);

   FileMetaData const * getNextFileToDownload();

   int getNumberOfDownloadedFiles();

   void clearCollection();

protected:

   QVector<FileMetaData> theMDList;


};

#endif // FILECOLLECTION_H
