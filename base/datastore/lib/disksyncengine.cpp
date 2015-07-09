#include "disksyncengine.h"

#include <QDomDocument>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>

namespace ck {

class DiskSyncEngine::PrivateDiskSyncEngine {
public:
  PrivateDiskSyncEngine() {}
  ~PrivateDiskSyncEngine() {
    if (mFile) {
      delete mFile;
    }
  }
  QString mData;
  QFileSystemWatcher *mFileWatch;
  QFile *mFile;
  QString mCurrentEngine;
  std::string m_app_name;
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

void DiskSyncEngine::set_app_name(const std::string &a_app_name) {
  d->m_app_name = a_app_name;
}

void DiskSyncEngine::insert_request(const SyncObject &a_obj) {
  if (a_obj.name().empty())
    return;

  QString home_path = db_home_path();
  QFileInfo fileInfo(home_path);

  if (!fileInfo.exists()) {
    qDebug() << Q_FUNC_INFO << "Create New Dir" << home_path;
    QDir::home().mkpath(home_path);
  }

  QString db_file_path = db_app_path();
  QFileInfo db_file_path_info(db_file_path);

  if (!db_file_path_info.exists()) {
    QDir::home().mkpath(db_file_path);
  }

  QFile object_file;
  QString object_file_name =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/" +
                               QString::fromStdString(d->m_app_name) + "/" +
                               QString::fromStdString(d->m_app_name) + ".xml");

  object_file.setFileName(object_file_name);

  if (object_file.exists()) {
    if (!object_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return;
    }
    QTextStream in(&object_file);
    QString data = in.readAll();
    object_file.close();

    QDomDocument dom_doc;
    QString error_msg;
    int line;
    int column;
    if (!dom_doc.setContent(data, &error_msg, &line, &column)) {
      qDebug() << Q_FUNC_INFO << "Error :" << error_msg << " Line : " << line
               << " Column : " << column;
      return;
    }

    QDomElement root =
        dom_doc.firstChildElement(QString::fromStdString(d->m_app_name));
    QDomElement main_element = dom_doc.createElement(a_obj.name().c_str());

    /*
    foreach(const QString & key, a_obj.property_list()) {
      main_element.setAttribute(key, a_obj.property(key).toString());
    }
    */

    CkStringList ck_prop_list = a_obj.property_list();

    std::for_each(std::begin(ck_prop_list), std::end(ck_prop_list),
                  [&](const std::string &a_prop) {
      main_element.setAttribute(a_prop.c_str(), a_obj.property(a_prop).c_str());
    });

    int index = root.childNodes().count();

    main_element.setAttribute("db_key", index);

    root.appendChild(main_element);

    if (object_file.isOpen())
      object_file.close();

    if (!object_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << Q_FUNC_INFO
               << "Failed to open the file : " << object_file.errorString();
      return;
    }

    QTextStream out(&object_file);
    out << dom_doc.toString();
    out.flush();

    object_file.close();

    return;
  }

  // create xml
  QDomDocument dom_doc(QString::fromStdString(d->m_app_name));
  dom_doc.appendChild(dom_doc.createProcessingInstruction(
      "xml", "version=\"1.0\" encoding=\"utf-8\""));
  QDomElement root_element =
      dom_doc.createElement(QString::fromStdString(d->m_app_name));
  dom_doc.appendChild(root_element);

  QDomElement main_element = dom_doc.createElement(a_obj.name().c_str());
  CkStringList prop_list = a_obj.property_list();

  std::for_each(std::begin(prop_list), std::end(prop_list),
                [&](const std::string &a_key) {
    main_element.setAttribute(a_key.c_str(), a_obj.property(a_key).c_str());
  });

  int index = root_element.childNodes().count();
  main_element.setAttribute("db_key", index);

  root_element.appendChild(main_element);

  if (!object_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << Q_FUNC_INFO
             << "Failed to open the file : " << object_file.errorString();
    return;
  }

  QTextStream out(&object_file);
  out << dom_doc.toString();
  out.flush();

  if (object_file.isOpen())
    object_file.close();

  qDebug() << Q_FUNC_INFO << "Done";
}

QString DiskSyncEngine::db_home_path() {
  QString home_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/");

  return home_path;
}

QString DiskSyncEngine::db_app_path() {
  QString db_file_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/" +
                               QString::fromStdString(d->m_app_name) + "/");

  return db_file_path;
}

void DiskSyncEngine::update_request(const SyncObject &a_obj) {
  if (a_obj.name().empty())
    return;

  QString home_path = db_home_path();
  QFileInfo fileInfo(home_path);

  if (!fileInfo.exists()) {
    qDebug() << Q_FUNC_INFO << "Create New Dir" << home_path;
    QDir::home().mkpath(home_path);
  }

  QString db_file_path = db_app_path();
  QFileInfo db_file_path_info(db_file_path);

  if (!db_file_path_info.exists()) {
    QDir::home().mkpath(db_file_path);
  }

  QFile object_file;
  QString object_file_name =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/" +
                               QString::fromStdString(d->m_app_name) + "/" +
                               QString::fromStdString(d->m_app_name) + ".xml");

  object_file.setFileName(object_file_name);

  if (object_file.exists()) {
    if (!object_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return;
    }
    QTextStream in(&object_file);
    QString data = in.readAll();
    object_file.close();

    QDomDocument dom_doc;
    QString error_msg;
    int line;
    int column;
    if (!dom_doc.setContent(data, &error_msg, &line, &column)) {
      qDebug() << Q_FUNC_INFO << "Error :" << error_msg << " Line : " << line
               << " Column : " << column;
      return;
    }

    QDomElement root =
        dom_doc.firstChildElement(QString::fromStdString(d->m_app_name));
    if (root.hasChildNodes()) {
      QDomNodeList node_list = root.childNodes();
      for (int i = 0; i <= node_list.count(); i++) {
        QDomNode child_node = node_list.at(i);

        QDomElement child_element = child_node.toElement();

        if (a_obj.name().compare(child_element.tagName().toLatin1()) == 0) {
          int db_key = child_element.attribute("db_key").toInt();

          if (a_obj.key() == db_key) {
            /*
            foreach(const QString & key, a_obj.property_list()) {
              child_element.setAttribute(key, a_obj.property(key).toString());
            }
            */

            CkStringList ck_prop_list = a_obj.property_list();

            std::for_each(std::begin(ck_prop_list), std::end(ck_prop_list),
                          [&](const std::string &a_prop) {
              child_element.setAttribute(a_prop.c_str(),
                                         a_obj.property(a_prop).c_str());
            });
          }
        }
      }
    } else {
      QDomElement main_element = dom_doc.createElement(a_obj.name().c_str());

      CkStringList ck_prop_list = a_obj.property_list();

      std::for_each(std::begin(ck_prop_list), std::end(ck_prop_list),
                    [&](const std::string &a_prop) {
        main_element.setAttribute(a_prop.c_str(),
                                  a_obj.property(a_prop).c_str());
      });

      int index = root.childNodes().count();
      main_element.setAttribute("db_key", index);
      root.appendChild(main_element);
    }

    if (object_file.isOpen())
      object_file.close();

    if (!object_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << Q_FUNC_INFO
               << "Failed to open the file : " << object_file.errorString();
      return;
    }

    QTextStream out(&object_file);
    out << dom_doc.toString();
    out.flush();

    object_file.close();

    return;
  }

  // create xml
  QDomDocument dom_doc(QString::fromStdString(d->m_app_name));
  dom_doc.appendChild(dom_doc.createProcessingInstruction(
      "xml", "version=\"1.0\" encoding=\"utf-8\""));
  QDomElement root_element =
      dom_doc.createElement(QString::fromStdString(d->m_app_name));
  dom_doc.appendChild(root_element);

  QDomElement main_element = dom_doc.createElement(a_obj.name().c_str());

  CkStringList ck_prop_list = a_obj.property_list();

  std::for_each(std::begin(ck_prop_list), std::end(ck_prop_list),
                [&](const std::string &a_prop) {
    main_element.setAttribute(a_prop.c_str(), a_obj.property(a_prop).c_str());
  });

  int index = root_element.childNodes().count();
  main_element.setAttribute("db_key", index);

  root_element.appendChild(main_element);

  if (object_file.isOpen())
    object_file.close();

  if (!object_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << Q_FUNC_INFO
             << "Failed to open the file : " << object_file.errorString();
    return;
  }

  QTextStream out(&object_file);
  out << dom_doc.toString();
  out.flush();

  if (object_file.isOpen())
    object_file.close();

  qDebug() << Q_FUNC_INFO << "Done";
}

void DiskSyncEngine::delete_request(const std::string &a_object_name,
                                    const std::string &a_key,
                                    const std::string &a_value) {
  QString home_path = db_home_path();
  QFileInfo fileInfo(home_path);

  if (!fileInfo.exists()) {
    QDir::home().mkpath(home_path);
  }

  QString db_file_path = db_app_path();
  QFileInfo db_file_path_info(db_file_path);

  if (!db_file_path_info.exists()) {
    QDir::home().mkpath(db_file_path);
  }

  QFile object_file;
  QString object_file_name =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/" +
                               QString::fromStdString(d->m_app_name) + "/" +
                               QString::fromStdString(d->m_app_name) + ".xml");

  object_file.setFileName(object_file_name);

  if (object_file.exists()) {
    if (!object_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return;
    }
    QTextStream in(&object_file);
    QString data = in.readAll();
    object_file.close();

    QDomDocument dom_doc;
    QString error_msg;
    int line;
    int column;
    if (!dom_doc.setContent(data, &error_msg, &line, &column)) {
      qDebug() << Q_FUNC_INFO << "Error :" << error_msg << " Line : " << line
               << " Column : " << column;
      return;
    }

    object_file.close();

    QDomElement root =
        dom_doc.firstChildElement(QString::fromStdString(d->m_app_name));
    if (root.hasChildNodes()) {
      QDomNode i = root.firstChild();
      QString tag_name = QString::fromStdString(a_object_name);
      QString match_key = QString::fromStdString(a_key);
      QString match_value = QString::fromStdString(a_value);

      QList<QDomNode> matching_node_list;

      while (!i.isNull()) {
        if (i.nodeName() == tag_name) {
          if (!a_key.empty() && !a_value.empty()) {
            QDomNamedNodeMap attr_map = i.attributes();

            if (!attr_map.contains(match_key)) {
              i = i.nextSibling();
              continue;
            }

            if (attr_map.namedItem(match_key).nodeValue() != match_value) {
              i = i.nextSibling();
              continue;
            }
          }

          matching_node_list << i;
        }
        i = i.nextSibling();
      }

      foreach(QDomNode node, matching_node_list) { root.removeChild(node); }
    }

    if (!object_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << Q_FUNC_INFO
               << "Failed to open the file : " << object_file.errorString();
      return;
    }

    QTextStream out(&object_file);
    out << dom_doc.toString();
    out.flush();

    object_file.close();

    return;
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

void DiskSyncEngine::find(const std::string &a_object_name,
                          const std::string &a_attrib,
                          const std::string &a_value) {
  bool match_found = false;
  QString home_path = db_home_path();
  QFileInfo fileInfo(home_path);

  if (!fileInfo.exists()) {
    qDebug() << Q_FUNC_INFO << "DB File Not found:" << home_path;

    SyncObject null_obj;
    search_request_complete(null_obj, d->m_app_name, 0);
    return;
  }

  QString db_file_path = db_app_path();
  QFileInfo db_file_path_info(db_file_path);

  if (!db_file_path_info.exists()) {
    qDebug() << Q_FUNC_INFO << "DB File Not Found :" << db_file_path;
    ;

    SyncObject null_obj;
    search_request_complete(null_obj, d->m_app_name, 0);
    return;
  }

  QFile object_file;
  QString object_file_name =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/" +
                               QString::fromStdString(d->m_app_name) + "/" +
                               QString::fromStdString(d->m_app_name) + ".xml");

  object_file.setFileName(object_file_name);

  if (object_file.exists()) {
    if (!object_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << Q_FUNC_INFO << "Failed to open device read-only";
      SyncObject null_obj;
      search_request_complete(null_obj, d->m_app_name, 0);
      return;
    }

    QTextStream in(&object_file);
    QString data = in.readAll();
    object_file.close();

    QDomDocument dom_doc;
    QString error_msg;
    int line;
    int column;
    if (!dom_doc.setContent(data, &error_msg, &line, &column)) {
      qDebug() << Q_FUNC_INFO << "Error :" << error_msg << " Line : " << line
               << " Column : " << column;

      SyncObject null_obj;
      search_request_complete(null_obj, d->m_app_name, 0);
      return;
    }

    QDomElement root =
        dom_doc.firstChildElement(QString::fromStdString(d->m_app_name));
    if (root.hasChildNodes()) {
      QDomNodeList node_list = root.childNodes();
      for (int i = 0; i < node_list.count(); i++) {
        QDomNode child_node = node_list.at(i);

        QDomElement child_element = child_node.toElement();

        if (QString::fromStdString(a_object_name) == child_element.tagName()) {
          ck::SyncObject obj;
          obj.set_name(a_object_name);

          QDomNamedNodeMap attrMap = child_node.attributes();

          if (!a_attrib.empty()) {
            if (!attrMap.contains(QString::fromStdString(a_attrib)))
              continue;

            QDomNode value_node =
                attrMap.namedItem(QString::fromStdString(a_attrib));
            std::string node_value_str = value_node.nodeValue().toStdString();

            if (node_value_str.compare(a_value) != 0)
              continue;
          }

          for (int i = 0; i < attrMap.count(); i++) {
            QDomNode attrNode = attrMap.item(i);
            if (attrNode.isAttr()) {
              QDomAttr attr = attrNode.toAttr();

              if (!attr.isNull()) {
                if (attr.name() == "db_key") {
                  obj.set_key(attr.value().toInt());
                  continue;
                }
                obj.set_property(attr.name().toStdString(), attr.value().toStdString());
              }
            }
          }

          match_found = 1;
          search_request_complete(obj, d->m_app_name, match_found);
        }
      }
    }
  }

  if (!match_found) {
    SyncObject null_obj;
    search_request_complete(null_obj, d->m_app_name, match_found);
  }
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
