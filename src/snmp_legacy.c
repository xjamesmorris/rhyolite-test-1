#include "asn1.h"

#include <stddef.h>
#include <stdint.h>

int snmp_parse_cisco_subset(const uint8_t *buf, size_t len, size_t *off, uint32_t *version, uint32_t *community_len, const uint8_t **community, uint32_t *oid_len, uint32_t *oid_value, uint32_t *status) {
    asn1_node_t root;
    asn1_node_t *vnode = NULL;
    asn1_node_t *cnode = NULL;
    asn1_node_t *onode = NULL;
    asn1_node_t *sub = NULL;
    size_t cursor = 0;
    size_t idx = 0;
    uint32_t value = 0;
    uint32_t acc = 0;
    int rc = ASN1_OK;

    if (buf == NULL || off == NULL || version == NULL || community_len == NULL || community == NULL || oid_len == NULL || oid_value == NULL || status == NULL) {
        return ASN1_ERR_INVALID_INPUT;
    }

    cursor = *off;
    asn1_node_init(&root);
    rc = asn1_parse_tlv(buf, len, &cursor, &root);
    if (rc != ASN1_OK) {
        goto fail;
    }
    if (root.tag != ASN1_TAG_SEQUENCE || !root.constructed || root.child_count < 3u) {
        rc = ASN1_ERR_UNSUPPORTED_TAG;
        goto fail;
    }

    vnode = &root.children[0];
    cnode = &root.children[1];
    onode = &root.children[2];
    if (vnode->tag_class != ASN1_CLASS_UNIVERSAL || vnode->tag != ASN1_TAG_INTEGER || vnode->value_length == 0u) {
        rc = ASN1_ERR_UNSUPPORTED_TAG;
        goto fail;
    }
    if (cnode->tag_class != ASN1_CLASS_UNIVERSAL || cnode->tag != ASN1_TAG_OCTET_STRING || cnode->value == NULL) {
        rc = ASN1_ERR_UNSUPPORTED_TAG;
        goto fail;
    }
    if (onode->tag_class != ASN1_CLASS_CONTEXT || !onode->constructed || onode->child_count == 0u) {
        rc = ASN1_ERR_UNSUPPORTED_TAG;
        goto fail;
    }

    value = 0u;
    for (idx = 0; idx < vnode->value_length; ++idx) {
        value = (value << 8) | (uint32_t)vnode->value[idx];
    }
    *version = value;
    *community_len = (uint32_t)cnode->value_length;
    *community = cnode->value;
    sub = &onode->children[0];
    if (sub->tag_class != ASN1_CLASS_UNIVERSAL || sub->tag != ASN1_TAG_OBJECT_IDENTIFIER || sub->value == NULL || sub->value_length == 0u) {
        rc = ASN1_ERR_UNSUPPORTED_TAG;
        goto fail;
    }

    acc = 0u;
    for (idx = 0; idx < sub->value_length; ++idx) {
        acc = (acc << 8) | (uint32_t)sub->value[idx];
    }
    *oid_len = (uint32_t)sub->value_length;
    *oid_value = acc;
    *status = 0u;
    *off = cursor;
    asn1_node_free(&root);
    return ASN1_OK;

fail:
    if (version != NULL) *version = 0u;
    if (community_len != NULL) *community_len = 0u;
    if (community != NULL) *community = NULL;
    if (oid_len != NULL) *oid_len = 0u;
    if (oid_value != NULL) *oid_value = 0u;
    if (status != NULL) *status = (uint32_t)(rc & 0xFFFFFFFFu);
    asn1_node_free(&root);
    return rc;
}
