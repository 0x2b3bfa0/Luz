#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_

#include <Arduino.h>

#include <ArduinoJson.h>
#include <FS.h>

class Config {
  public:
    Config();
    ~Config();
    int load(String filename = "/config.json");
    int save();
    JsonObject& json;
  private:
    File configFile;
    char *buf;
    StaticJsonBuffer<200> jsonBuffer;
};

#endif
