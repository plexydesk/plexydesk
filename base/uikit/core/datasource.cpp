#include "datasource.h"
#include <QVariantHash>

namespace CherryKit {

class DataSource::PrivateDataSource {
public:
  PrivateDataSource() {}
  ~PrivateDataSource() {}
};

DataSource::DataSource(QObject *a_object_ptr)
    : QObject(a_object_ptr), o_data_soure(new PrivateDataSource) {}

DataSource::~DataSource() { delete o_data_soure; }

void DataSource::request_data(QVariant a_args) { set_arguments(a_args); }
}
