#ifndef FLASH_OPS_H
#define FLASH_OPS_H

#include <stddef.h>
#include <stdint.h>

void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len);
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len);
void flash_erase_safe(uint32_t offset);

#endif // FLASH_OPS_H
