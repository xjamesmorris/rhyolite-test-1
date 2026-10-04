#ifndef FIREWALL_PLUGINS_H
#define FIREWALL_PLUGINS_H

#include <stddef.h>
#include <stdint.h>

int panos_parse(const uint8_t *buf, size_t len, size_t *off, uint32_t *status);
int checkpoint_parse(const uint8_t *buf, size_t len, size_t *off, uint32_t *status);
int fortinet_parse(const uint8_t *buf, size_t len, size_t *off, uint32_t *status);
int juniper_parse(const uint8_t *buf, size_t len, size_t *off, uint32_t *status);
int sonicwall_parse(const uint8_t *buf, size_t len, size_t *off, uint32_t *status);

#endif
