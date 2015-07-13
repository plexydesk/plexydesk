#include "ck_data_source.h"
#include <QVariantHash>

namespace cherry_kit {

class data_source::PrivateDataSource {
public:
  PrivateDataSource() {}
  ~PrivateDataSource() {}
};

data_source::data_source(QObject *a_object_ptr)
    : QObject(a_object_ptr), o_data_soure(new PrivateDataSource) {}

data_source::~data_source() { delete o_data_soure; }

void data_source::request_data(QVariant a_args) { set_arguments(a_args); }
}
