# Data Compression Project

## **Overview**

### **Step1**: Dataset Generation ### 
- The important initial part of this project is to generates synthetic dataset with assumption that each byte will contain a number from 0 to 127 (0x00 to 0x7F). It is common for the data in the buffer to have the same value repeated in the series.

### **Step2**: Choosing right algorithm ###
- The another aspect of this project is to choose right alogorithm by comparing all industry standard algorithm on the generated data based on compression ratio, Time required for compression decompression, Memory usage(Flash and RAM).
- Since in this project we are aiming for this compression algorithm to run on small embedded device with much less computing power and far less Flash and RAM memory above step becomes very important

### **Step3**: Implement compression algorithm in C ###
- This last part of the project is to implement selected algorithm in step2 in C language.

--- 

## Dataset Generation
   - Datasets are generated with sizes ranging from 128 bytes to 65,536 bytes (in powers of 2).
   - Each dataset consists of byte values (0–127) repeated in sequences. The length of these sequences is determined by a geometric distribution, introducing variability in how many times a byte is repeated consecutively.
   - The generated datasets are saved as binary files (`.bin`) in the `Data` directory.
   - Files are named based on their size (e.g., [dataset_128.bin](/Dataset/Data/dataset_128.bin), [dataset_256.bin](/Dataset/Data/dataset_256.bin), etc.).
   - **Sequence Analysis**: Counts how many times each byte value starts a new sequence (a run of consecutive identical bytes).
   - The results of the sequence analysis are plotted as bar charts and saved as images in the `Images` directory.
   - Each plot shows the number of sequences for each byte value in the dataset.
   - For each dataset, the program ( [Dataset Generation Notebook File](/Dataset/Dataset_Generation.ipynb) )
   ![alt text](/Dataset/Images/sequences_65536.png)

---

## Choosing right algorithm
- We start by getting a list of dataset sizes (like 128, 256, 512, etc.) that we generated in step 1.
- For each size, we load a dataset from a binary file stored in the Data folder.
- Next, we compress the dataset using different compression algorithms, including standard ones like zlib, bz2, and lzma, as well as custom ones like Run-Length Encoding (RLE) and Huffman Coding. 
- We measure how much time each algorithm takes to compress the data and calculate the compression ratio (how much smaller the data becomes after compression). 
- After compression, we decompress the data to ensure it matches the original, and we measure the time taken for decompression too. 
- All the results, including compression ratios and timings, are saved in a file called [compression_results.csv](/Algorithm_Comparison/compression_results.csv). 
- Finally, we create a plot comparing the performance of all the algorithms and save it as [compression_comparison.png](/Algorithm_Comparison/compression_comparison.png). 
- The plot shows how compression ratios and timings change as the dataset size increases.
![alt text](/Algorithm_Comparison/compression_comparison.png)

    ### **Compression Algorithms Comparison Results**

    | Algorithm       | Compression Ratio | Compression Time | Decompression Time | Best Use Case                     |
    |-----------------|-------------------|------------------|--------------------|-----------------------------------|
    | **`zlib`**      | ~0.50             | ~0.001s          | ~0.001s            | General-purpose compression       |
    | **`bz2`**       | ~0.45             | ~0.001s          | ~0.001s            | General-purpose compression       |
    | **`lzma`**      | ~0.38             | ~0.015s          | ~0.002s            | High-compression needs            |
    | **`rle`**       | ~0.60             | ~0.001s          | ~0.001s            | Repetitive data                   |
    | **`improved_rle`** | ~0.50         | ~0.001s          | ~0.001s            | Repetitive data                   |
    | **`huffman`**   | ~1.50             | ~0.010s          | ~0.020s            | Specific use cases (e.g., text)   |
- For **microcontroller software**, **Improved RLE** is an excellent choice due to its **speed**, **simplicity**, and **efficiency** for repetitive data. It balances compression performance and resource usage, making it ideal for embedded systems.

--- 

## Implement compression algorithm in C
- We start by defining the paths to binary files containing datasets of sizes 128, 256, and 65,536 bytes. 
- The program reads one of these files (in this case, the 65,536-byte file) into memory. It then compresses the data in-place using a modified Run-Length Encoding (RLE) algorithm
- This **Improved RLE** algorithm works by counting consecutive repeated bytes and storing the count and byte value in a compact format. 
- **How it Works**: 
    - In normal RLE, every byte is treated the same, and runs of repeated bytes are stored as a count followed by the byte value. This works well for repeated bytes but wastes space for single bytes, as they still require a count (even if it’s just 1).
    - By using the MSB, the modified RLE distinguishes between single bytes and runs:
    - If the MSB is set (1), it indicates a run, and the previous byte stores the count.
    - If the MSB is not set (0), it indicates a single byte, which is stored as-is without needing a count.
    - This optimization reduces overhead for single bytes, making the compression more efficient, especially for data with fewer repeated sequences.
    - This improvement ensures that compression ratio will not go over 1 even in worst case where no sequance is present in file/array.
- After compression, the program decompresses the data back to its original form, also in-place. It then checks if the decompressed data matches the original data to ensure no data was lost or corrupted during the process.
- Finally, it prints the compression ratio (how much smaller the compressed data is compared to the original) and confirms whether the integrity check passed or failed.
![alt text](/C_Project/Results/Dataset_65536_results.png)
![alt text](/C_Project/Results/Dataset_32768_results.png)
![alt text](/C_Project/Results/Dataset_16384_results.png)
![alt text](/C_Project/Results/Dataset_8192_results.png)
![alt text](/C_Project/Results/Dataset_4096_results.png)
![alt text](/C_Project/Results/Dataset_2048_results.png)
![alt text](/C_Project/Results/Dataset_1024_results.png)
![alt text](/C_Project/Results/Dataset_512_results.png)
![alt text](/C_Project/Results/Dataset_256_results.png)
![alt text](/C_Project/Results/Dataset_128_results.png)
