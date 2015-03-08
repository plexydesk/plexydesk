#ifndef EXT_LOADERTESTRUNNER_H
#define EXT_LOADERTESTRUNNER_H

#include <QObject>
#include <QVariantMap>

class ExtLaoderTestRunner : public QObject
{
  Q_OBJECT
public:
  explicit ExtLaoderTestRunner(QObject *a_parent_ptr = 0);

  virtual ~ExtLaoderTestRunner();

  bool connectToDataSource(const QString &source);

  void loadtest(const QString &source);

public slots:
  void onSourceUpdated(const QVariantMap &data);

private:
  class PrivateExtLaoderTestRunner;
  PrivateExtLaoderTestRunner *const d;
};

#endif // EXT_LOADERTESTRUNNER_H
