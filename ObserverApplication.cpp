#include "ObserverApplication.h"
QMap<QByteArray, QSet<QObject *>> ObserverApplication::mEventPool;
QReadWriteLock ObserverApplication::mRegisterLock;


void typeName(const QByteArray &s, QByteArray &d)
{
    foreach (auto var, s) {
        if((var >= 'A' && var <= 'Z') ||
                (var >= 'a' && var <= 'z') ||
                (var >= '0' && var <= '9') ||
                var == '_')
        {
            d.append(var);
            continue;
        }
        break;
    }
}

bool checkpParameter(QList<QByteArray> s, QList<QByteArray> d)
{
    qDebug() << d;
    qDebug() << s;

    for(int i= 0; i < d.size(); ++ i)
    {
        QByteArray name;
        typeName(d[i], name);
        if(name.isEmpty()) return  false;
        if(!(s[i].startsWith(name) || s[i].startsWith("class " + name))) return false;
    }
    return true;
}
ObserverApplication::ObserverApplication(int &argc, char **argv)
    : QApplication(argc, argv) {}

ObserverApplication::~ObserverApplication() {}

int ObserverApplication::methodIndex(QObject *recv,
                                     QList<QByteArray> &typeNames,
                                     unsigned int argsNum,
                                     QByteArray methodName) {
  if (recv == nullptr)
    return -1;

  auto obj = recv->metaObject();

  for (int i = obj->methodOffset(); i < recv->metaObject()->methodCount();
       ++i) {
    auto method = obj->method(i);

    if (methodName == method.name()) {
      if (static_cast<unsigned int>(method.parameterCount()) > argsNum)
        continue;
      if (checkpParameter(typeNames, method.parameterTypes()))
        return i;
    }
  }

  qDebug() << "Observe Event : Not found "
           << QString(obj->className()) + "::" + methodName;

  return -1;
}

bool ObserverApplication::subscibeEvent(QObject *listener,
                                        QByteArray eventName) {
  QWriteLocker loker(&mRegisterLock);

  if (mEventPool.contains(eventName)) {
    QSet<QObject *> tmpset = mEventPool[eventName];

    tmpset.insert(listener);

    mEventPool.insert(eventName, tmpset);

    return true;
  }

  QSet<QObject *> tmpset;
  tmpset.insert(listener);
  mEventPool.insert(eventName, tmpset);

  return true;
}

void ObserverApplication::logoutEvent(QObject *listener, QByteArray eventName) {
  QWriteLocker loker(&mRegisterLock);

  if (mEventPool.contains(eventName)) {
    QSet<QObject *> tmpset = mEventPool[eventName];

    tmpset.remove(listener);

    mEventPool.insert(eventName, tmpset);
  }
}
