#ifndef ASN1_AMD64_H
#define ASN1_AMD64_H

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(_M_X64)

static inline int asn1_read_length_amd64(const uint8_t *buffer, size_t buffer_length, size_t *offset, size_t *length) {
    size_t i = 0u;
    size_t n = 0u;
    size_t value = 0u;
    uint8_t first = 0u;

    if (buffer == NULL || offset == NULL || length == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    i = *offset;
    if (i >= buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    first = buffer[i++];
    if ((first & 0x80u) == 0u) {
        *length = (size_t)first;
        *offset = i;
        return ASN1_OK;
    }

    if (first == 0x80u) {
        return ASN1_ERR_INDEFINITE_LENGTH;
    }

    n = (size_t)(first & 0x7Fu);
    if (n == 0u || n > 4u) {
        return ASN1_ERR_BAD_LENGTH;
    }

    if (i + n > buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    __asm__ volatile(
        "xor %%rax, %%rax\n\t"
        "mov %[count], %%rcx\n\t"
        "lea (%[base], %[index]), %%rsi\n\t"
        "1:\n\t"
        "movzx (%%rsi), %%r8\n\t"
        "shl $8, %%rax\n\t"
        "or %%r8, %%rax\n\t"
        "inc %%rsi\n\t"
        "dec %%rcx\n\t"
        "jnz 1b\n\t"
        "mov %%rax, %[value]\n\t"
        : [value] "=r" (value)
        : [base] "r" (buffer), [index] "r" (i), [count] "r" (n)
        : "rax", "rcx", "rsi", "r8", "cc", "memory"
    );

    *length = value;
    *offset = i + n;
    return ASN1_OK;
}

#endif

#endif
