/*
 * xtea.cpp
 *
 *  Created on: 2016 jún. 7
 *      Author: Peter gp@iotgome.hu
 */

#include "XTea.h"

XTea::XTea(const String& key) { keySetup(key); }

void XTea::keySetup(const String& key) {
  String k = key;
  while (k.length() < 16) {
    k += k.substring(0, key.length());
    if (k.length() > 16) {
      k = k.substring(0, 16);
      break;
    }
  }
  this->key = str2long(k);
}

uint32_t* XTea::str2long(const String& data) {
  int size = data.length() / 4;
  uint32_t* res = (uint32_t*)malloc(size * 4);
  int pos = 0;
  uint32_t v = 0;
  for (int i = 1; i <= size * 4; i++) {
    byte c = data.charAt(i - 1);
    v |= c;
    if (i % 4 == 0) {
      res[pos++] = v;
      v = 0;
    } else {
      v <<= 8;
    }
  }
  return (res);
}

String XTea::encrypt(String& input) {
  while (input.length() % sizeof(t_ch_node) != 0) {
    input.concat(' ');
  }
  const int size = input.length() / sizeof(t_ch_node) + 1;
  t_ch_node* cipher = (t_ch_node*)malloc(size * sizeof(t_ch_node));
  if (cipher == nullptr) return ("");
  cipher[0].y = random(LONG_MAX);
  cipher[0].z = random(LONG_MAX);
  int a = 1;
  int idx = 0;
  for (int i = 0; i < size - 1; i++) {
    uint32_t y = input.charAt(idx++) << 24;
    y += input.charAt(idx++) << 16;
    y += input.charAt(idx++) << 8;
    y += input.charAt(idx++);
    y ^= cipher[a - 1].y;
    uint32_t z = input.charAt(idx++) << 24;
    z += input.charAt(idx++) << 16;
    z += input.charAt(idx++) << 8;
    z += input.charAt(idx++);
    z ^= cipher[a - 1].z;
    cipher[a++] = blockEncrypt(y, z);
  }
  uint8_t* p = (uint8_t*)cipher;
  for (int i = 0; i < size; i++) {
    uint32_t y = *p++ << 24;
    y += *p++ << 16;
    y += *p++ << 8;
    y += *p++;
    uint32_t z = *p++ << 24;
    z += *p++ << 16;
    z += *p++ << 8;
    z += *p++;
    cipher[i].y = y;
    cipher[i].z = z;
  }

  char* data = (char*)malloc(size * sizeof(t_ch_node) * 2);
  if (data == nullptr) {
    free(cipher);
    return ("");
  }

  int len = base64_encode_chars((char*)&cipher[0], size * sizeof(t_ch_node),
                                &data[0]);
  data[len] = '\0';
  data[len] = '\0';
  String out = String(data);
  free(data);
  free(cipher);
  return (out);
}

String XTea::decrypt(const String& source) {
  char* data = (char*)malloc(source.length());
  if (data == nullptr) return ("");
  const int size =
      base64_decode_chars(&source.c_str()[0], source.length() , &data[0]) /
      8;
  t_ch_node* cipher = (t_ch_node*)realloc(data, size * 8);
  if (cipher == nullptr) {
    free(data);
    return ("");
  }

  uint8_t* p = (uint8_t*)cipher;
  for (int i = 0; i < size; i++) {
    uint32_t y = *p++ << 24;
    y += *p++ << 16;
    y += *p++ << 8;
    y += *p++;
    uint32_t z = *p++ << 24;
    z += *p++ << 16;
    z += *p++ << 8;
    z += *p++;
    cipher[i].y = y;
    cipher[i].z = z;
  }

  for (int i = 1; i < size; i++) {
    uint32_t y = cipher[i].y;
    uint32_t z = cipher[i].z;
    t_ch_node ret = blockDecrypt(y, z);
    y = ret.y ^ cipher[i - 1].y;
    z = ret.z ^ cipher[i - 1].z;
    cipher[i - 1].y = y;
    cipher[i - 1].z = z;
  }

  p = (uint8_t*)cipher;
  for (int i = 0; i < size - 1; i++) {
    uint32_t y = *p++ << 24;
    y += *p++ << 16;
    y += *p++ << 8;
    y += *p++;
    uint32_t z = *p++ << 24;
    z += *p++ << 16;
    z += *p++ << 8;
    z += *p++;
    cipher[i].y = y;
    cipher[i].z = z;
  }

  cipher[size - 1].y = 0;
  cipher[size - 1].z = 0;
  String plain = String((char*)&cipher[0]);
  free(cipher);
  return (plain);
}

t_ch_node XTea::blockEncrypt(uint32_t y, uint32_t z) {
  uint32_t sum = 0;
  uint32_t delta = 0x9e3779b9;
  t_ch_node ret;
  for (int i = 0; i < 32; i++) {
    y += (((z << 4) ^ (z >> 5)) + z) ^ (sum + key[sum & 3]);
    sum += delta;
    z += (((y << 4) ^ (y >> 5)) + y) ^ (sum + key[(sum >> 11) & 3]);
  }
  ret.y = y;
  ret.z = z;
  return (ret);
}

t_ch_node XTea::blockDecrypt(uint32_t y, uint32_t z) {
  uint32_t delta = 0x9E3779B9, sum = delta * 32;
  t_ch_node ret;
  for (int i = 0; i < 32; i++) {
    z -= (((y << 4) ^ (y >> 5)) + y) ^ (sum + key[(sum >> 11) & 3]);
    sum -= delta;
    y -= (((z << 4) ^ (z >> 5)) + z) ^ (sum + key[sum & 3]);
  }
  ret.y = y;
  ret.z = z;
  return (ret);
}

XTea::~XTea() {
  if (key != nullptr) {
    free(key);
  }
}
