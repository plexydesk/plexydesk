#include "disksyncengine.h"

#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>

namespace QuetzalKit {

class DiskSyncEngine::PrivateDiskSyncEngine {
public:
  PrivateDiskSyncEngine() {}
  ~PrivateDiskSyncEngine() {
    if (mFile)
      delete mFile;
  }
  QString mData;
  QFileSystemWatcher *mFileWatch;
  QFile *mFile;
  QString mCurrentEngine;
};

DiskSyncEngine::DiskSyncEngine(QObject *parent)
    : SyncEngineInterface(parent), d(new PrivateDiskSyncEngine) {
  d->mFileWatch = new QFileSystemWatcher(this);

  connect(d->mFileWatch, SIGNAL(fileChanged(QString)), this,
          SIGNAL(modified()));

  // setup file
  d->mFile = new QFile();

  connect(d->mFile, SIGNAL(bytesWritten(qint64)), this,
          SLOT(onBytesWritten(qint64)));
}

DiskSyncEngine::~DiskSyncEngine() { delete d; }

void DiskSyncEngine::setEngineName(const QString &name) {
  d->mCurrentEngine = name;
  QString homePath =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/");
  QFileInfo fileInfo(homePath);

  if (!fileInfo.exists()) {
    QDir::home().mkpath(homePath);
  }

  QString watchFile = QDir::toNativeSeparators(
      QDir::homePath() + "/.quetzal/datastore/" + name + ".xml");
  QFileInfo info(watchFile);
  qDebug() << Q_FUNC_INFO << watchFile;

  if (!info.exists()) {
    qDebug() << Q_FUNC_INFO << "Add watch" << homePath;
    connect(d->mFileWatch, SIGNAL(directoryChanged(QString)), this,
            SLOT(onDirectoryChanged(QString)));
    d->mFileWatch->addPath(homePath);
  }
}

QString DiskSyncEngine::data(const QString &fileName) {
  QFile file(QDir::toNativeSeparators(
      QDir::homePath() + "/.quetzal/datastore/" + fileName + ".xml"));

  // if (!file.exists())
  //   return QString();

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    // qWarning() << Q_FUNC_INFO << file.errorString();
    return QString();
  }

  QString watchFile = QDir::toNativeSeparators(
      QDir::homePath() + "/.quetzal/datastore/" + fileName + ".xml");

  if (!d->mFileWatch->files().contains(watchFile)) {
    d->mFileWatch->addPath(watchFile);
  }

  QTextStream in(&file);
  QString line = in.readLine();
  QString data;
  while (!line.isNull()) {
    // process_line(line);
    data += line;
    line = in.readLine();
  }
  return data;
}

void DiskSyncEngine::sync(const QString &datqstoreName, const QString &data) {
  // qDebug() << Q_FUNC_INFO << data;
  d->mData = data;
  this->saveDataToDisk(datqstoreName, data);
}

bool DiskSyncEngine::hasLock() { return false; }

void DiskSyncEngine::saveDataToDisk(const QString &fileName,
                                    const QString &data) {
  QString homePath =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore");
  QFileInfo fileInfo(homePath);

  if (!fileInfo.exists()) {
    QDir::home().mkpath(homePath);
  }

  QString watchFile = QDir::toNativeSeparators(
      QDir::homePath() + "/.quetzal/datastore/" + fileName + ".xml");
  d->mFile->setFileName(watchFile);

  connect(d->mFile, SIGNAL(bytesWritten(qint64)), this,
          SLOT(onBytesWritten(qint64)));

  if (!d->mFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << Q_FUNC_INFO
             << "Failed to open the file : " << d->mFile->errorString();
    qFatal("no such file");
  }

  QTextStream out(d->mFile);
  out << data;
  out.flush();
  d->mFile->close();

  Q_EMIT modified();
}

void DiskSyncEngine::onBytesWritten(qint64 bytes) {
  qDebug() << Q_FUNC_INFO << "File Writtent to disk:" << bytes;
  qFatal("Done Writing");
}

void DiskSyncEngine::onDirectoryChanged(const QString &name) {
  qDebug() << Q_FUNC_INFO << name;
  QString homePath =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/");
  if (homePath == name) {
    qDebug() << Q_FUNC_INFO << "New Dir Added";
    Q_EMIT modified();
  } else {
    Q_EMIT modified();
  }
}
}
