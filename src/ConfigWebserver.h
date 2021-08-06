#ifndef ConfigWebserver_H
#define ConfigWebserver_H

#include "Arduino.h"
#include <ESPAsyncWebServer.h>


class ConfigWebserver
{

public:
  ConfigWebserver();
  ~ConfigWebserver();
  void Serve();


};

#endif