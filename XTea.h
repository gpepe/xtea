/*
 * xtea.h
 *
 *  Created on: 2016 jún. 7
 *      Author: Peter gp@iothome.hu
 */

#ifndef SRC_XTEA_H_
#define SRC_XTEA_H_

#include <Arduino.h>

#include <WString.h>
#include "libb64/cdecode.h"
#include "libb64/cencode.h"
#include <limits.h>



typedef struct t_ch_node
{
    uint32_t y;
    uint32_t z;
} t_ch_node;


class XTea {
 private:
  uint32_t* key = nullptr;
  void keySetup(const String& key);
  uint32_t* str2long(const String& data);
  t_ch_node blockEncrypt(uint32_t y, uint32_t z);
  t_ch_node blockDecrypt(uint32_t y, uint32_t z);
 public:
  XTea(const String& key);
  String encrypt(String& text);
  String decrypt(const String& source);
  ~XTea();
};

#endif /* SRC_XTEA_H_ */
