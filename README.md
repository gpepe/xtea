XTea encrypt / dercpt esp8266

  XTea x("secret key");
  String e = x.encrypt("data");
  String d = x.decrypt(e);

