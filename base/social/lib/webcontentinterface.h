#ifndef WEBCONTENTINTERFACE_H
#define WEBCONTENTINTERFACE_H

#include <QTextStream>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit
{

class QuetzalSocialKit_EXPORT WebContentInterface
{
public:
  WebContentInterface();

  virtual QString content();
};
}

#endif // WEBCONTENTINTERFACE_H
