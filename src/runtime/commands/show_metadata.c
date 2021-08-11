#include "commands.h"

#include <bson/bson.h>

char* read_resources(const char* target, const appimage_header_t* header) {
    FILE* f = fopen(target, "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to read: %s", target);
        return NULL;
    }

    fseek(f, (long) header->metadata_offset, SEEK_SET);
    size_t resources_len = header->signature_offset - header->metadata_offset;

    uint8_t* data = malloc(sizeof(uint8_t) * resources_len);
    if (fread(data, sizeof(uint8_t), resources_len, f) != resources_len) {
        fprintf(stderr, "Unable to read resources bson structure at: %zu with length %zu",
                header->signature_offset, resources_len);
        return NULL;
    }

    bson_t b;
    bool bson_init_res = bson_init_static(&b, data, resources_len);
    if (bson_init_res == false) {
        fprintf(stderr, "Unable to parse resources bson structure at: %zu with length %zu",
                header->signature_offset, resources_len);
        return NULL;
    }

    char* json = bson_as_canonical_extended_json(&b, NULL);
    return json;
}


char* read_signatures(const char* target, const appimage_header_t* header) {
    FILE* f = fopen(target, "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to read: %s", target);
        return NULL;
    }

    fseek(f, 0L, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, header->signature_offset, SEEK_SET);

    size_t raw_bson_len = file_size - header->signature_offset;

    uint8_t* raw_bson = malloc(sizeof(uint8_t) * raw_bson_len);
    if (fread(raw_bson, sizeof(uint8_t), raw_bson_len, f) != raw_bson_len) {
        fprintf(stderr, "Unable to read signatures bson structure at: %zu with length %zu",
                header->signature_offset, raw_bson_len);
        return NULL;
    }

    bson_t b;
    bool bson_init_res = bson_init_static(&b, raw_bson, raw_bson_len);
    if (bson_init_res == false) {
        fprintf(stderr, "Unable to parse signatures bson structure at: %zu with length %zu",
                header->signature_offset, raw_bson_len);
        return NULL;
    }

    char* json = bson_as_canonical_extended_json(&b, NULL);
    return json;
}

void show_metadata(const char* target, const appimage_header_t* header) {
    char* resources = read_resources(target, header);
    char* signatures = read_signatures(target, header);

    // join json objects for output
    if (resources != NULL && signatures != NULL) {
        resources[strlen(resources) - 1] = 0;
        signatures[0] = ',';
    }
    printf("%s%s\n", resources, signatures);

    if (resources)
        free(resources);

    if (signatures)
        free(signatures);
}