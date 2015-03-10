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

DataSource::DataSource(QObject *a_object_ptr)
  : QObject(a_object_ptr), d(new PrivateDataSource) {}

DataSource::~DataSource() { delete d; }

void DataSource::request_data(QVariant a_args) { set_arguments(a_args); }
}
