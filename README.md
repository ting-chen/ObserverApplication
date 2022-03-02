# ObserverApplication
 realize publish and subscribe patten in qt


## Feature
### 1. <b>Easy use</b>


#### import


 ``` c++
 include "ObserverApplication.h"
 int main(int argc, char *argv[]) {
  ObserverApplication a(argc, argv);
  return a.exec();
}

 ```
 
 #### subscribe event
 
 
 ``` C++
 ObserverApplication::subscibeEvent(subscibe_event_obj, "triggered_event_named");
```

#### register event handler
<br>implement slots in subscibe_event_obj class, and function name start with "handle_"


``` C++
class subscibeEventObjClass
{
...
slots:
T handle_triggered_event_named(arg1...arg10);
}
```

#### publish event
<br>declare event name , connection type and transport parameter，note that Parameters are limited to 10

```c++

  ObserverApplication::publishEvent("triggered_event_named",
                                    Qt::UniqueConnection, arg1..arg10);
              
```

<br>ObserverApplication will invoking event handler in specified thread by declare connection type after register event been published


### 2. Thread safe
<br> using Qt invoke framework,this is thread safe


### 3. support for specifying subscription function threads by connection parameters
<br> for more detail about connect parameters [Qt ConnectionType](https://doc.qt.io/qt-5.15/qt.html#ConnectionType-enum)


### 4. Support multi platform 


* window
* linux
* mac


### 5. Support Custom type
<br>support for custom data types registered with qt metasystems
<br>[qRegisterMetaTyp](https://doc.qt.io/qt-5.15/qmetatype.html#qRegisterMetaType-1)
<br>[Q_DECLARE_METATYPE](https://doc.qt.io/qt-5.15/qmetatype.html#Q_DECLARE_METATYPE)


 
