#ifndef OBSERVERAPPLICATION_H
#define OBSERVERAPPLICATION_H

#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QMap>
#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
#include <QReadWriteLock>
#include <QSet>
#include <QSharedPointer>
#include <QThread>
#include <QVariant>

#if defined(Q_OS_LINUX)
#include <cxxabi.h>
#endif

#include <QEvent>
#include <QMetaMethod>
#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#define METHODHEAD "handle_"

class ObserverApplication : public QApplication {
public:
  ObserverApplication(int &argc, char **argv);
  virtual ~ObserverApplication();

  template <class T>
  static void getList(QList<QByteArray> &typeNames,
                      QList<QGenericArgument> &list, T &&t) {
#if defined(Q_OS_LINUX)
    char *type =
        abi::__cxa_demangle(typeid(t).name(), nullptr, nullptr, nullptr);
    typeNames << QByteArray(type);
    list << Q_ARG(T, t);
    free(type);
#elif defined(Q_OS_WIN)
    typeNames << QByteArray(typeid(t).name());
    list << Q_ARG(T, t);
#endif
  }

  template <class T>
  static void getList(QList<QByteArray> &typeNames,
                      QList<QSharedPointer<QVariant>> &list, T &&t) {
#if defined(Q_OS_LINUX)
    char *type =
        abi::__cxa_demangle(typeid(t).name(), nullptr, nullptr, nullptr);
    typeNames << QByteArray(type);
    QSharedPointer<QVariant> ptr(new QVariant());
    ptr->setValue(t);
    list << ptr;
    free(type);
#elif defined(Q_OS_WIN)
    typeNames << QByteArray(typeid(t).name());
    QSharedPointer<QVariant> ptr(new QVariant());
    ptr->setValue(t);
    list << ptr;
#endif
  }

  template <class... Args>
  static void publishEvent(QByteArray eventName, Qt::ConnectionType type,
                           Args &&... args) {
    if (eventName.isEmpty())
      return;

    auto argsNum = sizeof...(args);
    Q_ASSERT_X(argsNum <= 10, "QuickEvent",
               "publishEvent argv number not greater than 10");

    QReadLocker loker(&mRegisterLock);

    if (mEventPool.contains(eventName)) {
      auto set = mEventPool[eventName];
      auto methodName = QByteArray(METHODHEAD) + eventName;

      foreach (auto var, set) {
        QList<QGenericArgument> list;
        QList<QByteArray> typeNames;
        int arr[] = {(getList(typeNames, list, args), 0)...};
        Q_UNUSED(arr)
        while (list.size() < 10)
          list << QGenericArgument();
        auto index = -1;

        if ((index = methodIndex(var, typeNames, argsNum, methodName)) == -1)
          continue;
        var->metaObject()->method(index).invoke(
            var, type, list[0], list[1], list[2], list[3], list[4], list[5],
            list[6], list[7], list[8], list[9]);
        //        } /*else {
        //          QList<QGenericArgument> list;
        //          QList<QByteArray> typeNames;
        //          int arr[] = {(getList(typeNames, list, args), 0)...};
        //          Q_UNUSED(arr)
        //          while (list.size() < 10)
        //            list << QGenericArgument();

        //          QMetaObject::invokeMethod(
        //              var, methodName, type, list[0], list[1], list[2],
        //              list[3], list[4], list[5], list[6], list[7], list[8],
        //              list[9]);
        //        }*/
      }
    }
  }

  static int methodIndex(QObject *recv, QList<QByteArray> &typeNames,
                         unsigned int argsNum, QByteArray methodName);

  static bool subscibeEvent(QObject *listener, QByteArray eventName);

  static void logoutEvent(QObject *listener, QByteArray eventName);

private:
  static QMap<QByteArray, QSet<QObject *>> mEventPool;

  static QReadWriteLock mRegisterLock;
};

#endif // OBSERVERAPPLICATION_H
