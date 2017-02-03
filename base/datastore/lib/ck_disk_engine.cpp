/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "ck_disk_engine.h"

#include <QDomDocument>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>

namespace cherry_kit {

class disk_engine::Privatedisk_engine {
public:
  Privatedisk_engine() {}
  ~Privatedisk_engine() {
    if (mFile) {
      delete mFile;
    }
  }
  QString mData;
  //QFileSystemWatcher *mFileWatch;
  QFile *mFile;
  QString mCurrentEngine;
  std::string m_app_name;
};

disk_engine::disk_engine(QObject *parent)
    : sync_engine_interface(parent), d(new Privatedisk_engine) {
  //d->mFileWatch = new QFileSystemWatcher(this);

  //connect(d->mFileWatch, SIGNAL(fileChanged(QString)), this,
   //       SIGNAL(modified()));

  // setup file
  d->mFile = new QFile();

  connect(d->mFile, SIGNAL(bytesWritten(qint64)), this,
          SLOT(onBytesWritten(qint64)));
}

disk_engine::~disk_engine() { delete d; }

void disk_engine::setEngineName(const QString &name) {
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
    /*
    connect(d->mFileWatch, SIGNAL(directoryChanged(QString)), this,
            SLOT(onDirectoryChanged(QString)));
    d->mFileWatch->addPath(homePath);
    */
  }
}

void disk_engine::set_app_name(const std::string &a_app_name) {
  d->m_app_name = a_app_name;
}

void disk_engine::insert_request(const sync_object &a_obj) {
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

    ck_string_list ck_prop_list = a_obj.property_list();

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
      "xml", "version=\"1.0\" encoding=\"iso-8859-1\""));
  QDomElement root_element =
      dom_doc.createElement(QString::fromStdString(d->m_app_name));
  dom_doc.appendChild(root_element);

  QDomElement main_element = dom_doc.createElement(a_obj.name().c_str());
  ck_string_list prop_list = a_obj.property_list();

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

QString disk_engine::db_home_path() {
  QString home_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/");

  return home_path;
}

QString disk_engine::db_app_path() {
  QString db_file_path =
      QDir::toNativeSeparators(QDir::homePath() + "/.quetzal/datastore/" +
                               QString::fromStdString(d->m_app_name) + "/");

  return db_file_path;
}

void disk_engine::update_request(const sync_object &a_obj) {
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

            ck_string_list ck_prop_list = a_obj.property_list();

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

      ck_string_list ck_prop_list = a_obj.property_list();

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
      "xml", "version=\"1.0\" encoding=\"iso-8859-1\""));
  QDomElement root_element =
      dom_doc.createElement(QString::fromStdString(d->m_app_name));
  dom_doc.appendChild(root_element);

  QDomElement main_element = dom_doc.createElement(a_obj.name().c_str());

  ck_string_list ck_prop_list = a_obj.property_list();

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

void disk_engine::delete_request(const std::string &a_object_name,
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

QString disk_engine::data(const QString &fileName) {
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

  /*
  if (!d->mFileWatch->files().contains(watchFile)) {
    d->mFileWatch->addPath(watchFile);
  }
  */

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

void disk_engine::find(const std::string &a_object_name,
                       const std::string &a_attrib,
                       const std::string &a_value) {
  bool match_found = false;
  QString home_path = db_home_path();
  QFileInfo fileInfo(home_path);

  if (!fileInfo.exists()) {
    qDebug() << Q_FUNC_INFO << "DB File Not found:" << home_path;

    sync_object null_obj;
    search_request_complete(null_obj, d->m_app_name, 0);
    return;
  }

  QString db_file_path = db_app_path();
  QFileInfo db_file_path_info(db_file_path);

  if (!db_file_path_info.exists()) {
    qDebug() << Q_FUNC_INFO << "DB File Not Found :" << db_file_path;
    ;

    sync_object null_obj;
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
      sync_object null_obj;
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

      sync_object null_obj;
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
          cherry_kit::sync_object obj;
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
                obj.set_property(attr.name().toStdString(),
                                 attr.value().toStdString());
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
    sync_object null_obj;
    search_request_complete(null_obj, d->m_app_name, match_found);
  }
}

void disk_engine::sync(const QString &datqstoreName, const QString &data) {
  // qDebug() << Q_FUNC_INFO << data;
  d->mData = data;
  this->saveDataToDisk(datqstoreName, data);
}

bool disk_engine::hasLock() { return false; }

void disk_engine::saveDataToDisk(const QString &fileName, const QString &data) {
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

void disk_engine::onBytesWritten(qint64 bytes) {
  qDebug() << Q_FUNC_INFO << "File Writtent to disk:" << bytes;
  qFatal("Done Writing");
}

void disk_engine::onDirectoryChanged(const QString &name) {
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
