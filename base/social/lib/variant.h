/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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

#ifndef DATA_KIT_VARIANT_H
#define DATA_KIT_VARIANT_H

#include <map>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>
#include <list>
#include <iostream>
#include <cmath>
#include <typeinfo>
#include <string>

namespace SocialKit {
class Variant {
public:
  typedef struct __data__ {
    enum {
      kUnknownType = 1 << 0,
      kCharType = 1 << 1,
      kIntType = 1 << 2,
      kFloatType = 1 << 3,
      kDoubleType = 1 << 4,
      kDataMapType = 1 << 5,
      kFloatRectType = 1 << 6,
      kFloatPointType = 1 << 7,
      kStringType = 1 << 8
    } m_variant_type;

  private:
    union {
      std::string m_string_data;
      std::map<std::string, __data__> m_dict_data;
      char m_char_data;
      float m_float_data;
      double m_double_data;
      int m_int_data;
    };

  public:
    __data__();
    ~__data__();
    __data__(const __data__ &copy);

    __data__ &operator=(__data__ &copy);

    void setValue(float value);
    void setValue(double value);
    void setValue(const std::string &value);

    bool isString() const;
    bool isFloat();
    bool isDouble();
    bool isFloatRect();
    bool isFloatPoint();
    bool isDataMap();
    bool isChar();

    std::string toString() const;
    float toFloat() const;
    double toDouble() const;
  } Data;

  typedef std::map<std::string, std::vector<Data> > ValueMap;
  typedef std::map<std::string, Data> DataMap;

  Variant();
  virtual ~Variant();

  virtual void setDelegate(std::function<void(ValueMap)> target);

private:
  class PrivateVariant;
  PrivateVariant *const d;
};
}
#endif
