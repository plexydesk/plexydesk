#include "datasource.h"
#include <QVariantHash>

namespace UIKit
{

class DataSource::PrivateDataSource
{
public:
  PrivateDataSource() {}
  ~PrivateDataSource() {}
};

DataSource::DataSource(QObject *object)
  : QObject(object), d(new PrivateDataSource) {}

DataSource::~DataSource() { delete d; }

void DataSource::requestData(QVariant args) { setArguments(args); }
}
