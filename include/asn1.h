#ifndef ASN1_H
#define ASN1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(_M_X64)
#include "asn1_amd64.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ASN1_CLASS_UNIVERSAL = 0,
    ASN1_CLASS_APPLICATION = 1,
    ASN1_CLASS_CONTEXT = 2,
    ASN1_CLASS_PRIVATE = 3
} asn1_class_t;

typedef enum {
    ASN1_TAG_BOOLEAN = 0x01,
    ASN1_TAG_INTEGER = 0x02,
    ASN1_TAG_BIT_STRING = 0x03,
    ASN1_TAG_OCTET_STRING = 0x04,
    ASN1_TAG_NULL = 0x05,
    ASN1_TAG_OBJECT_IDENTIFIER = 0x06,
    ASN1_TAG_SEQUENCE = 0x10,
    ASN1_TAG_SET = 0x11,
    ASN1_TAG_PRINTABLE_STRING = 0x13,
    ASN1_TAG_UTF8_STRING = 0x0C,
    ASN1_TAG_IA5_STRING = 0x22,
    ASN1_TAG_UTC_TIME = 0x17,
    ASN1_TAG_GENERAL_TIME = 0x18,
    ASN1_TAG_ENUMERATED = 0x0A
} asn1_tag_t;

typedef enum {
    ASN1_OK = 0,
    ASN1_ERR_INVALID_INPUT = -1,
    ASN1_ERR_SHORT_BUFFER = -2,
    ASN1_ERR_UNSUPPORTED_TAG = -3,
    ASN1_ERR_BAD_LENGTH = -4,
    ASN1_ERR_PARSE_ERROR = -5,
    ASN1_ERR_BAD_TAG = -6,
    ASN1_ERR_INDEFINITE_LENGTH = -7,
    ASN1_ERR_MEMORY = -8
} asn1_error_t;

typedef struct asn1_node {
    uint8_t tag;
    uint8_t tag_class;
    bool constructed;
    uint32_t tag_number;
    size_t length;
    size_t header_length;
    const uint8_t *value;
    size_t value_length;
    size_t child_count;
    struct asn1_node *children;
} asn1_node_t;

typedef struct {
    const uint8_t *data;
    size_t length;
    size_t offset;
} asn1_reader_t;

void asn1_node_init(asn1_node_t *node);
void asn1_node_free(asn1_node_t *node);
int asn1_read_length(const uint8_t *buffer, size_t buffer_length, size_t *offset, size_t *length);
int asn1_read_length_amd64(const uint8_t *buffer, size_t buffer_length, size_t *offset, size_t *length);
int asn1_read_tag(const uint8_t *buffer, size_t buffer_length, size_t *offset, uint8_t *tag_class, bool *constructed, uint32_t *tag_number, size_t *header_length);
int asn1_parse_tlv(const uint8_t *buffer, size_t buffer_length, size_t *offset, asn1_node_t *node);
int asn1_parse_tree(const uint8_t *buffer, size_t buffer_length, asn1_node_t *root);
int asn1_parse_buffer(const uint8_t *buffer, size_t buffer_length, asn1_node_t *node);

#ifdef __cplusplus
}
#endif

#endif
