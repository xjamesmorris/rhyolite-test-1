#include "asn1.h"

#include <stdlib.h>
#include <string.h>

void asn1_node_init(asn1_node_t *node) {
    if (node == NULL) {
        return;
    }

    memset(node, 0, sizeof(*node));
}

void asn1_node_free(asn1_node_t *node) {
    size_t index = 0;

    if (node == NULL) {
        return;
    }

    if (node->children != NULL) {
        for (index = 0; index < node->child_count; ++index) {
            asn1_node_free(&node->children[index]);
        }

        free(node->children);
        node->children = NULL;
        node->child_count = 0;
    }

    memset(node, 0, sizeof(*node));
}

int asn1_read_tag(const uint8_t *buffer, size_t buffer_length, size_t *offset, uint8_t *tag_class, bool *constructed, uint32_t *tag_number, size_t *header_length) {
    size_t index = 0;
    uint8_t first_octet = 0;
    uint32_t number = 0;

    if (buffer == NULL || offset == NULL || tag_class == NULL || constructed == NULL || tag_number == NULL || header_length == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    index = *offset;
    if (index >= buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    first_octet = buffer[index++];
    *tag_class = (uint8_t)((first_octet >> 6) & 0x03u);
    *constructed = ((first_octet & 0x20u) != 0u);
    number = (uint32_t)(first_octet & 0x1Fu);

    if (number == 0x1Fu) {
        uint8_t next_octet = 0;

        while (index < buffer_length) {
            next_octet = buffer[index++];
            number = (number << 7) | (uint32_t)(next_octet & 0x7Fu);
            if ((next_octet & 0x80u) == 0u) {
                break;
            }
        }

        if (index > buffer_length || (index == buffer_length && (next_octet & 0x80u) != 0u)) {
            return ASN1_ERR_BAD_TAG;
        }
    }

    *tag_number = number;
    *header_length = index - *offset;
    *offset = index;
    return ASN1_OK;
}

int asn1_read_length(const uint8_t *buffer, size_t buffer_length, size_t *offset, size_t *length) {
    size_t index = 0;
    uint8_t first_octet = 0;
    size_t length_octets = 0;
    size_t value_length = 0;
    size_t i = 0;

    if (buffer == NULL || offset == NULL || length == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    index = *offset;
    if (index >= buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    first_octet = buffer[index++];
    if ((first_octet & 0x80u) == 0u) {
        *length = (size_t)first_octet;
        *offset = index;
        return ASN1_OK;
    }

    if (first_octet == 0x80u) {
        return ASN1_ERR_INDEFINITE_LENGTH;
    }

    length_octets = (size_t)(first_octet & 0x7Fu);
    if (length_octets == 0u || length_octets > 4u) {
        return ASN1_ERR_BAD_LENGTH;
    }

    if (index + length_octets > buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    value_length = 0u;
    for (i = 0u; i < length_octets; ++i) {
        value_length = (value_length << 8) | (size_t)buffer[index + i];
    }

    *length = value_length;
    *offset = index + length_octets;
    return ASN1_OK;
}

static int parse_constructed_node(const uint8_t *buffer, size_t buffer_length, size_t content_start, size_t content_end, asn1_node_t *node) {
    size_t cursor = content_start;
    size_t child_capacity = 4u;
    size_t child_count = 0u;
    asn1_node_t *children = NULL;

    if (content_start > content_end || content_end > buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    children = (asn1_node_t *)calloc(child_capacity, sizeof(*children));
    if (children == NULL) {
        return ASN1_ERR_MEMORY;
    }

    while (cursor < content_end) {
        size_t child_offset = cursor;
        asn1_node_t child;
        int rc = 0;

        asn1_node_init(&child);
        rc = asn1_parse_tlv(buffer, buffer_length, &child_offset, &child);
        if (rc != ASN1_OK) {
            asn1_node_free(&child);
            free(children);
            return rc;
        }

        if (child_count == child_capacity) {
            asn1_node_t *resized = NULL;
            size_t new_capacity = child_capacity * 2u;
            resized = (asn1_node_t *)realloc(children, new_capacity * sizeof(*children));
            if (resized == NULL) {
                asn1_node_free(&child);
                free(children);
                return ASN1_ERR_MEMORY;
            }
            children = resized;
            child_capacity = new_capacity;
        }

        children[child_count++] = child;
        cursor = child_offset;
    }

    if (cursor != content_end) {
        free(children);
        return ASN1_ERR_PARSE_ERROR;
    }

    node->children = children;
    node->child_count = child_count;
    node->value = &buffer[content_start];
    node->value_length = content_end - content_start;
    return ASN1_OK;
}

int asn1_parse_tlv(const uint8_t *buffer, size_t buffer_length, size_t *offset, asn1_node_t *node) {
    size_t index = 0;
    size_t length = 0;
    size_t start = 0;
    size_t content_start = 0;
    size_t content_end = 0;
    uint8_t tag_class = 0;
    bool constructed = false;
    uint32_t tag_number = 0;
    size_t tag_header_length = 0;
    int rc = ASN1_OK;

    if (buffer == NULL || offset == NULL || node == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    asn1_node_init(node);
    index = *offset;
    if (index >= buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    start = index;
    rc = asn1_read_tag(buffer, buffer_length, &index, &tag_class, &constructed, &tag_number, &tag_header_length);
    if (rc != ASN1_OK) {
        return rc;
    }

    rc = asn1_read_length(buffer, buffer_length, &index, &length);
    if (rc != ASN1_OK) {
        return rc;
    }

    content_start = index;
    content_end = index + length;
    if (content_end > buffer_length) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    node->tag = (uint8_t)((tag_class << 6) | (constructed ? 0x20u : 0x00u) | (tag_number < 31u ? tag_number : 0x1Fu));
    node->tag_class = tag_class;
    node->constructed = constructed;
    node->tag_number = tag_number;
    node->length = length;
    node->header_length = (content_start - start);
    node->value = &buffer[content_start];
    node->value_length = length;

    if (constructed) {
        rc = parse_constructed_node(buffer, buffer_length, content_start, content_end, node);
        if (rc != ASN1_OK) {
            return rc;
        }
    }

    *offset = content_end;
    return ASN1_OK;
}

int asn1_parse_tree(const uint8_t *buffer, size_t buffer_length, asn1_node_t *root) {
    size_t offset = 0;
    int rc = 0;

    if (buffer == NULL || root == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    asn1_node_init(root);
    rc = asn1_parse_tlv(buffer, buffer_length, &offset, root);
    if (rc != ASN1_OK) {
        return rc;
    }

    if (offset != buffer_length) {
        asn1_node_free(root);
        return ASN1_ERR_PARSE_ERROR;
    }

    return ASN1_OK;
}

int asn1_parse_buffer(const uint8_t *buffer, size_t buffer_length, asn1_node_t *node) {
    size_t offset = 0;
    int rc = 0;

    if (buffer == NULL || node == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    asn1_node_init(node);
    rc = asn1_parse_tlv(buffer, buffer_length, &offset, node);
    if (rc != ASN1_OK) {
        return rc;
    }

    if (offset != buffer_length) {
        asn1_node_free(node);
        return ASN1_ERR_PARSE_ERROR;
    }

    return ASN1_OK;
}
