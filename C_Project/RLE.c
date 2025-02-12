#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE 65536

#if DATA_SIZE == 128
    #define FILE_PATH "../Dataset/Data/dataset_128.bin"
#elif DATA_SIZE == 256
    #define FILE_PATH "../Dataset/Data/dataset_256.bin"
#elif DATA_SIZE == 512
    #define FILE_PATH "../Dataset/Data/dataset_512.bin"
#elif DATA_SIZE == 1024
    #define FILE_PATH "../Dataset/Data/dataset_1024.bin"
#elif DATA_SIZE == 2048
    #define FILE_PATH "../Dataset/Data/dataset_2048.bin"
#elif DATA_SIZE == 4096
    #define FILE_PATH "../Dataset/Data/dataset_4096.bin"
#elif DATA_SIZE == 8192
    #define FILE_PATH "../Dataset/Data/dataset_8192.bin"
#elif DATA_SIZE == 16384
    #define FILE_PATH "../Dataset/Data/dataset_16384.bin"
#elif DATA_SIZE == 32768
    #define FILE_PATH "../Dataset/Data/dataset_32768.bin"
#elif DATA_SIZE == 65536
    #define FILE_PATH "../Dataset/Data/dataset_65536.bin"
#endif

// Function Prototypes
size_t byte_compress(uint8_t* data_ptr, size_t data_size);              // Improved RLE Compression
size_t byte_decompress(uint8_t* data_ptr, size_t compressed_size);      // Improved RLE Decompression

/**
 * @brief Compress data in-place using Run-Length Encoding (RLE).
 * 
 * @param data_ptr Pointer to the data to be compressed.
 * @param data_size Size of the data to be compressed.
 * @return size_t Size of the compressed data.
 */
size_t byte_compress(uint8_t* data_ptr, size_t data_size) 
{
    if (data_size == 0) 
    {
        return 0; 
    }

    size_t read_idx = 0;  
    size_t write_idx = 0; 

    while (read_idx < data_size) 
    {
        uint8_t current_byte = data_ptr[read_idx];
        size_t count = 1;

        // Count consecutive repetitions of the current byte
        while (read_idx + count < data_size && count < 255 && 
            data_ptr[read_idx + count] == current_byte) 
        {
            count++;
        }

        if (count > 1) 
        {
            data_ptr[write_idx++] = (uint8_t)count;
            data_ptr[write_idx++] = current_byte | 0x80;        // Since Data is 7-bit, MSB is set to 1 to indicate a run
        } else 
        {
            data_ptr[write_idx++] = current_byte & 0x7F;
        }
        read_idx += count;
    }

    return write_idx; 
}

/**
 * @brief Decompress RLE-compressed data in-place.
 * 
 * @param data_ptr Pointer to the compressed data.
 * @param compressed_size Size of the compressed data.
 * @return size_t Size of the decompressed data.
 */
size_t byte_decompress(uint8_t* data_ptr, size_t compressed_size) 
{
    uint8_t* temp_buffer = (uint8_t*)malloc(compressed_size * sizeof(uint8_t));
    if (!temp_buffer) 
    {
        printf("Memory allocation failed!\n");
        return 0; // Failed to allocate memory
    }
    memcpy(temp_buffer, data_ptr, compressed_size); // Copy compressed data to temp buffer

    size_t read_idx = 0; 
    size_t write_idx = 0; 

    while (read_idx < compressed_size) {
        uint8_t byte = temp_buffer[read_idx];

        // Check if the next byte has the MSB set (indicating a run)
        if (read_idx + 1 < compressed_size && (temp_buffer[read_idx + 1] & 0x80)) 
        {
            uint8_t count = byte; 
            uint8_t value = temp_buffer[read_idx + 1] & 0x7F; 
            memset(&data_ptr[write_idx], value, count);
            write_idx += count;
            read_idx += 2; 
        } else 
        {
            data_ptr[write_idx++] = byte;
            read_idx++; 
        }
    }

    free(temp_buffer); // Free the temporary buffer
    return write_idx; 
}

/**
 * @brief Check if two arrays are equal.
 * 
 * @param original Pointer to the original data array.
 * @param decompressed Pointer to the decompressed data array.
 * @param size Size of the arrays to be compared.
 * @return int 1 if arrays are equal, 0 otherwise.
 */
int check_integrity(const uint8_t* original, const uint8_t* decompressed, size_t size) 
{
    for (size_t i = 0; i < size; i++) 
    {
        if (original[i] != decompressed[i]) 
        {
            return 0; // Arrays are not equal
        }
    }
    return 1; // Arrays are equal
}

int main() 
{
    // Define the file path for the binary data
    const char *filepath = FILE_PATH;

    // Open the binary file in read-binary mode
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) 
    {
        printf("Failed to open file");
        return -1;
    }


    // Define an array to store the 128 bytes from the file
    uint8_t data[DATA_SIZE];

    // Read the binary file into the data array
    size_t bytes_read = fread(data, sizeof(uint8_t), DATA_SIZE, file);
    if (bytes_read != DATA_SIZE) 
    {
        printf("Error reading file");
        fclose(file);
        return -1;
    }
    printf("Original size: %zu\n", bytes_read);

    // Close the file after reading
    fclose(file);

    // Store the original data for integrity check
    uint8_t original_data[DATA_SIZE];
    memcpy(original_data, data, DATA_SIZE);

    printf("Original data (first 20 bytes): ");
    for (size_t i = 0; i < 20; i++) 
    {
        printf("%x ", data[i]);
    }
    printf("\n");

    // Compress the data in-place
    size_t compressed_size = byte_compress(data, DATA_SIZE);
    printf("Compressed size: %zu\n", compressed_size);

    printf("Compressed data (first 20 bytes): ");
    for (size_t i = 0; i < 20; i++) 
    {
        printf("%x ", data[i]);
    }
    printf("\n");


    // Decompress the data in-place
    size_t decompressed_size = byte_decompress(data, compressed_size);
    printf("Decompressed size: %zu\n", decompressed_size);

    // Check integrity
    if (check_integrity(original_data, data, DATA_SIZE)) 
    {
        printf("Integrity check: PASSED\n");
    } 
    else 
    {
        printf("Integrity check: FAILED\n");
        return -1;
    }

    // Print Compression Ratio
    printf("Compression Ratio: %.3f\n", (float)compressed_size/(float)decompressed_size);

    return 0;
}