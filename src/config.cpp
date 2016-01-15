#include "config.h"

Config::~Config() {
  if(buf != NULL) delete buf;
}

Config::Config():json(NULL) {}

int Config::load(String filename) {
  if(!filename.startsWith("/")) filename = '/' + filename;

  SPIFFS.begin();

  configFile = SPIFFS.open(filename, "rw");
  if (!configFile) return 1;

  size_t size = configFile.size();
  if (size > 1024) return 2;

  buf = new char[size];

  configFile.readBytes(buf.get(), size);
  json = jsonBuffer.parseObject(buf.get());
  if (!json.success()) return 3;
  return 0;
}


int Config::save() {
  if (!configFile) return 1;
  json.printTo(configFile);
  return 0;
}
