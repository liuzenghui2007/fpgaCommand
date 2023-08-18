#ifndef COMMON_H
#define COMMON_H
#include <cstdint> // 包含cstdint头文件

uint16_t read_uint16(const unsigned char *c, const int pt);
uint32_t read_uint32(const unsigned char *c, const int pt);
void write_uint32(unsigned char* buf, uint32_t ui);
void write_uint16(unsigned char* buf, uint16_t ui);

#endif // COMMON_H
