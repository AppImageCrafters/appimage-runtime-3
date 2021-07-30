#include "commands.h"

#include <bson/bson.h>

void show_metadata(const char* target, const appimage_header_t* header) {
    FILE* f = fopen(target, "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to read: %s", target);
        return;
    }

    fseek(f, (long) header->metadata_offset, SEEK_SET);
    size_t resources_len = header->signature_offset - header->metadata_offset;

    uint8_t* data = malloc(sizeof(uint8_t) * resources_len);
    if (fread(data, sizeof(uint8_t), resources_len, f) != resources_len) {
        fprintf(stderr, "Unable to read resources bson structure at: %zu with length %zu",
                header->signature_offset, resources_len);
        return;
    }

    bson_t b;
    bool bson_init_res = bson_init_static(&b, data, resources_len);
    if (bson_init_res == false) {
        fprintf(stderr, "Unable to parse resources bson structure at: %zu with length %zu",
                header->signature_offset, resources_len);
        return;
    }

    char* json = bson_as_canonical_extended_json(&b, NULL);
    if (json != NULL) {
        fprintf(stdout, "%s", json);
        free(json);
    }
}
