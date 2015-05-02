#include "variant.h"

namespace SocialKit {

class Variant::PrivateVariant {
public:
  PrivateVariant() {}
  ~PrivateVariant() {}

  std::list<std::function<void(ValueMap)> > m_notify_list;
};

Variant::Variant() : d(new PrivateVariant) {}

Variant::~Variant() { delete d; }

void Variant::setDelegate(std::function<void(ValueMap)> target) {
  d->m_notify_list.push_front(target);
}

Variant::__data__::__data__() : m_variant_type(kUnknownType) {
  m_double_data = 0;
  m_float_data = 0;
  m_int_data = 0;
  m_char_data = 0;
  m_string_data = "";
}

Variant::__data__::~__data__() {}

Variant::__data__::__data__(const __data__ &copy) {
  m_string_data = copy.m_string_data;
  m_variant_type = copy.m_variant_type;
  m_double_data = copy.m_double_data;
  m_int_data = copy.m_int_data;
  m_char_data = copy.m_char_data;
  m_float_data = copy.m_float_data;
}

Variant::__data__ &Variant::__data__::operator=(Variant::__data__ &copy) {
  m_string_data = copy.m_string_data;
  m_variant_type = copy.m_variant_type;
  m_double_data = copy.m_double_data;
  m_int_data = copy.m_int_data;
  m_char_data = copy.m_char_data;
  m_float_data = copy.m_float_data;
  return *this;
}

void Variant::__data__::setValue(float value) {
  std::cout << __FUNCTION__ << " Float " << std::endl;
  m_variant_type = kFloatType;
  m_float_data = value;
}

void Variant::__data__::setValue(double value) {
  std::cout << __FUNCTION__ << " Double " << std::endl;

  m_variant_type = kDoubleType;
  m_double_data = value;
}

void Variant::__data__::setValue(const std::string &value) {
  m_variant_type = kStringType;
  m_string_data = value;
}

bool Variant::__data__::isString() const {
  if (m_variant_type == kStringType) {
    return true;
  }
  return false;
}

bool Variant::__data__::isFloat() {
  if (m_variant_type == kFloatPointType) {
    return true;
  }
  return false;
}

bool Variant::__data__::isDouble() {
  if (m_variant_type == kDoubleType) {
    return true;
  }
  return false;
}

bool Variant::__data__::isFloatRect() {
  if (m_variant_type == kFloatRectType) {
    return true;
  }
  return false;
}

bool Variant::__data__::isFloatPoint() {
  if (m_variant_type == kFloatPointType) {
    return true;
  }

  return false;
}

bool Variant::__data__::isDataMap() {
  if (m_variant_type == kDataMapType) {
    return true;
  }

  return false;
}

bool Variant::__data__::isChar() {
  if (m_variant_type == kDataMapType) {
    return true;
  }

  return false;
}

std::string Variant::__data__::toString() const {
  std::string rv;
  std::ostringstream string_stream;

  switch (m_variant_type) {
    case kStringType:
      rv = m_string_data;
      break;
    case kFloatType:
      string_stream << m_float_data;
      rv = string_stream.str();
      break;
    case kDoubleType:
      string_stream << m_double_data;
      rv = string_stream.str();
      break;
    case kCharType:
      string_stream << m_char_data;
      rv = string_stream.str();
      break;
    default:
      rv = "";
  };

  return rv;
}

float Variant::__data__::toFloat() const {
  if (m_variant_type == kFloatType)
    return m_float_data;

  if (m_variant_type == kDoubleType)
    return m_double_data;

  return 0;
}

double Variant::__data__::toDouble() const {
  if (m_variant_type == kDoubleType)
    return m_double_data;

  if (m_variant_type == kFloatType)
    return m_float_data;

  return 0;
}
}
