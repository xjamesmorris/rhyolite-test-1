#include <stddef.h>
#include <stdint.h>

int checkpoint_parse(const uint8_t *buf, size_t len, size_t *off, uint32_t *status) {
    (void)buf; (void)len; (void)off;
    if (status != NULL) *status = 0u;
    return 0;
}
