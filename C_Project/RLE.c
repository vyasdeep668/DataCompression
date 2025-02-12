#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE 65536
#define FILE_NAME "../Dataset/Data/dataset_65536.bin"

// Function to compress data in-place using RLE
size_t rle_compress_inplace(uint8_t* data_ptr, size_t data_size) {
    if (data_size == 0) {
        return 0; // No data to compress
    }

    size_t read_idx = 0;  // Index for reading the original data
    size_t write_idx = 0; // Index for writing the compressed data

    while (read_idx < data_size) {
        uint8_t current_byte = data_ptr[read_idx];
        size_t count = 1;

        // Count consecutive repetitions of the current byte
        while (read_idx + count < data_size && count < 255 && data_ptr[read_idx + count] == current_byte) {
            count++;
        }

        if (count > 1) {
            // Write the count and the byte value
            data_ptr[write_idx++] = (uint8_t)count;
            data_ptr[write_idx++] = current_byte | 0x80;
        } else {
            // Write the single byte as-is
            data_ptr[write_idx++] = current_byte & 0x7F;
        }
        read_idx += count;
    }

    return write_idx; // Return the size of the compressed data
}

// Function to decompress RLE-compressed data in-place
size_t rle_decompress_inplace(uint8_t* data_ptr, size_t compressed_size) {
    // Create a temporary buffer to hold the compressed data
    uint8_t* temp_buffer = (uint8_t*)malloc(compressed_size * sizeof(uint8_t));
    if (!temp_buffer) {
        printf("Memory allocation failed!\n");
        return 0; // Failed to allocate memory
    }
    memcpy(temp_buffer, data_ptr, compressed_size); // Copy compressed data to temp buffer

    size_t read_idx = 0;  // Index for reading the compressed data
    size_t write_idx = 0; // Index for writing the decompressed data

    while (read_idx < compressed_size) {
        uint8_t byte = temp_buffer[read_idx];

        // Check if the next byte has the MSB set (indicating a run)
        if (read_idx + 1 < compressed_size && (temp_buffer[read_idx + 1] & 0x80)) {
            // This is a run: extract count and value
            uint8_t count = byte; // Count is stored directly
            uint8_t value = temp_buffer[read_idx + 1] & 0x7F; // Extract actual value (clear MSB)

            // Write the value 'count' times to the decompressed data
            memset(&data_ptr[write_idx], value, count);
            write_idx += count;
            read_idx += 2; // Move to the next pair
        } else {
            // This is a single byte: write it as-is
            data_ptr[write_idx++] = byte;
            read_idx++; // Move to the next byte
        }
    }

    free(temp_buffer); // Free the temporary buffer
    return write_idx; // Return the size of the decompressed data
}

// Function to check if two arrays are equal
int check_integrity(const uint8_t* original, const uint8_t* decompressed, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (original[i] != decompressed[i]) {
            return 0; // Arrays are not equal
        }
    }
    return 1; // Arrays are equal
}

int main() {
    // Define the file path for the binary data
    const char* filepath = FILE_NAME;

    // Open the binary file in read-binary mode
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }


    // Define an array to store the 128 bytes from the file
    uint8_t data[DATA_SIZE];

    // Read the binary file into the data array
    size_t bytes_read = fread(data, sizeof(uint8_t), DATA_SIZE, file);
    if (bytes_read != DATA_SIZE) {
        perror("Error reading file");
        fclose(file);
        return -1;
    }

    // Close the file after reading
    fclose(file);

    // Store the original data for integrity check
    uint8_t original_data[DATA_SIZE];
    memcpy(original_data, data, DATA_SIZE);

    printf("Original data (first 20 bytes): ");
    for (size_t i = 0; i < 20; i++) {
        printf("%x ", data[i]);
    }
    printf("\n");

    // Compress the data in-place
    size_t compressed_size = rle_compress_inplace(data, DATA_SIZE);
    printf("Compressed size: %zu\n", compressed_size);

    printf("Compressed data (first 20 bytes): ");
    for (size_t i = 0; i < 20; i++) {
        printf("%x ", data[i]);
    }
    printf("\n");


    // Decompress the data in-place
    size_t decompressed_size = rle_decompress_inplace(data, compressed_size);
    printf("Decompressed size: %zu\n", decompressed_size);

    // Check integrity
    if (check_integrity(original_data, data, DATA_SIZE)) {
        printf("Integrity check: PASSED\n");
    } else {
        printf("Integrity check: FAILED\n");
        return -1;
    }

    // Print Compression Ratio
    printf("Compression Ratio: %.3f\n", (float)compressed_size/(float)decompressed_size);

    return 0;
}