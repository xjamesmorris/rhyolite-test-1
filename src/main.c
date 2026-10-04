#include "asn1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int hex_to_bytes(const char *hex_string, uint8_t *buffer, size_t buffer_size, size_t *out_length) {
    size_t i = 0;
    size_t hex_length = strlen(hex_string);
    size_t byte_count = 0;

    if (hex_string == NULL || buffer == NULL || out_length == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    if (hex_length == 0u || (hex_length % 2u) != 0u) {
        return ASN1_ERR_INVALID_INPUT;
    }

    if (hex_length / 2u > buffer_size) {
        return ASN1_ERR_SHORT_BUFFER;
    }

    while (i < hex_length) {
        unsigned int value = 0u;
        if (sscanf(&hex_string[i], "%2x", &value) != 1) {
            return ASN1_ERR_INVALID_INPUT;
        }
        buffer[byte_count++] = (uint8_t)value;
        i += 2u;
    }

    *out_length = byte_count;
    return ASN1_OK;
}

static void dump_hex(const uint8_t *data, size_t length) {
    size_t index = 0u;

    for (index = 0u; index < length; ++index) {
        printf("%02x", data[index]);
        if (index + 1u < length) {
            printf(" ");
        }
    }
}

static void print_node(const asn1_node_t *node, unsigned int depth) {
    const char *class_name = "UNIVERSAL";
    size_t index = 0u;

    if (node == NULL) {
        return;
    }

    switch (node->tag_class) {
        case ASN1_CLASS_APPLICATION:
            class_name = "APPLICATION";
            break;
        case ASN1_CLASS_CONTEXT:
            class_name = "CONTEXT";
            break;
        case ASN1_CLASS_PRIVATE:
            class_name = "PRIVATE";
            break;
        default:
            class_name = "UNIVERSAL";
            break;
    }

    printf("%*sTAG %s [%u] len=%zu class=%s constructed=%s\n",
           (int)(depth * 2u), "",
           node->constructed ? "constructed" : "primitive",
           (unsigned int)node->tag_number,
           node->length,
           class_name,
           node->constructed ? "yes" : "no");

    if (node->value != NULL && node->value_length > 0u && !node->constructed) {
        printf("%*svalue=", (int)((depth + 1u) * 2u), "");
        dump_hex(node->value, node->value_length);
        printf("\n");
    }

    for (index = 0u; index < node->child_count; ++index) {
        print_node(&node->children[index], depth + 1u);
    }
}

static void print_usage(const char *program_name) {
    fprintf(stderr,
            "Usage: %s <hex-encoded-asn1>\n"
            "Example: %s 300c02010102020102\n",
            program_name,
            program_name);
}

int main(int argc, char **argv) {
    uint8_t bytes[256] = {0};
    size_t length = 0u;
    asn1_node_t root;
    int rc = 0;

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    rc = hex_to_bytes(argv[1], bytes, sizeof(bytes), &length);
    if (rc != ASN1_OK) {
        fprintf(stderr, "Failed to decode input as hexadecimal: %d\n", rc);
        return 1;
    }

    asn1_node_init(&root);
    rc = asn1_parse_tree(bytes, length, &root);
    if (rc != ASN1_OK) {
        fprintf(stderr, "Failed to parse ASN.1 data: %d\n", rc);
        asn1_node_free(&root);
        return 1;
    }

    printf("Parsed ASN.1 tree for %s\n", argv[1]);
    print_node(&root, 0u);
    asn1_node_free(&root);
    return 0;
}
