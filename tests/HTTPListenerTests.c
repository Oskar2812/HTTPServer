#include <stdio.h>

#include "../src/HTTPListener.c"

int ParseSizeT_ValidDigits_ParseSuccesful() {
    size_t result = 0;

    if (ParseSizeT("12345", 5, &result) == -1) {
        printf("Failed to parse valid digits\n");
        return -1;
    }
    if (result != 12345) {
        printf("Incorrect parsed value\n");
        return -1;
    }

    return 0;
}

int ParseSizeT_NonDigitCharacter_ParseUnsuccesful() {
    size_t result = 0;

    if (ParseSizeT("12a45", 5, &result) != -1) {
        printf("Failed to catch non-digit character\n");
        return -1;
    }

    return 0;
}

int AllocateBufferMemory_WithinLimit_AllocatesSuccesfully() {
    TextBuffer buffer = {0};

    if (AllocateBufferMemory(&buffer, 100) == -1) {
        printf("Failed to allocate within limit\n");
        return -1;
    }
    if (buffer.Capacity != 100) {
        printf("Incorrect capacity after allocation\n");
        free(buffer.Content);
        return -1;
    }

    free(buffer.Content);

    return 0;
}

int AllocateBufferMemory_ExceedsMaxBufferSize_AllocationUnsuccesful() {
    TextBuffer buffer = {0};
    buffer.Capacity = MAX_BUFFER_SIZE - 10;

    if (AllocateBufferMemory(&buffer, 100) != -1) {
        printf("Failed to catch buffer allocation over the maximum size\n");
        free(buffer.Content);
        return -1;
    }

    return 0;
}
