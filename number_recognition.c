#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "number_recognition.h"

void load_MNIST_dataset(char* image_file_path, char* label_file_path, MNIST_Image** dataset, int* dataset_size) {
    // Open image file
    FILE* image_file = fopen(image_file_path, "rb");
    if (!image_file) {
        printf("Failed to open the MNIST image file.\n");
        exit(1);
    }

    // Open label file
    FILE* label_file = fopen(label_file_path, "rb");
    if (!label_file) {
        printf("Failed to open the MNIST label file.\n");
        fclose(image_file);
        exit(1);
    }

    // Read image file header
    uint32_t image_magic_number, image_num_images, image_num_rows, image_num_cols;
    fread(&image_magic_number, sizeof(image_magic_number), 1, image_file);
    fread(&image_num_images, sizeof(image_num_images), 1, image_file);
    fread(&image_num_rows, sizeof(image_num_rows), 1, image_file);
    fread(&image_num_cols, sizeof(image_num_cols), 1, image_file);

    // Read label file header
    uint32_t label_magic_number, label_num_items;
    fread(&label_magic_number, sizeof(label_magic_number), 1, label_file);
    fread(&label_num_items, sizeof(label_num_items), 1, label_file);

    // Convert header values from big-endian to host byte order if necessary
    image_magic_number = be32toh(image_magic_number);
    image_num_images = be32toh(image_num_images);
    image_num_rows = be32toh(image_num_rows);
    image_num_cols = be32toh(image_num_cols);
    label_magic_number = be32toh(label_magic_number);
    label_num_items = be32toh(label_num_items);

    // Check if the number of images and labels match
    if (image_num_images != label_num_items) {
        printf("Mismatch between the number of images and labels.\n");
        fclose(image_file);
        fclose(label_file);
        exit(1);
    }

    // Allocate memory for the dataset
    *dataset_size = image_num_images;
    *dataset = malloc(image_num_images * sizeof(MNIST_Image));
    if (!*dataset) {
        printf("Memory allocation failed.\n");
        fclose(image_file);
        fclose(label_file);
        exit(1);
    }

    // Read images
    // Read images and labels
    for (int i = 0; i < image_num_images; i++) {
        MNIST_Image* image = &(*dataset)[i];

        // Read pixel data of the current image
        fread(image->pixels, sizeof(unsigned char), image_num_rows * image_num_cols, image_file);

        // Read label of the current image
        fread(&image->label, sizeof(unsigned char), 1, label_file);
    }

    // Close the files
    fclose(image_file);
    fclose(label_file);
}

int recognize_digit(const char* image_path, MNIST_Image* dataset, int dataset_size) {
    // Load the input image
    FILE* image_file = fopen(image_path, "rb");
    if (!image_file) {
        printf("Failed to open the input image file.\n");
        exit(1);
    }

    // Read image file header
    uint32_t image_magic_number, image_num_images, image_num_rows, image_num_cols;
    fread(&image_magic_number, sizeof(image_magic_number), 1, image_file);
    fread(&image_num_images, sizeof(image_num_images), 1, image_file);
    fread(&image_num_rows, sizeof(image_num_rows), 1, image_file);
    fread(&image_num_cols, sizeof(image_num_cols), 1, image_file);

    // Convert header values from big-endian to host byte order if necessary
    image_magic_number = be32toh(image_magic_number);
    image_num_images = be32toh(image_num_images);
    image_num_rows = be32toh(image_num_rows);
    image_num_cols = be32toh(image_num_cols);

    // Check if the image size matches MNIST images
    if (image_num_rows != 28 || image_num_cols != 28) {
        printf("Invalid image dimensions. Expected 28x28.\n");
        fclose(image_file);
        exit(1);
    }

    // Read pixel data of the input image
    unsigned char pixels[28][28];
    fread(pixels, sizeof(unsigned char), 28 * 28, image_file);

    // Close the input image file
    fclose(image_file);

    // Compare the input image with the MNIST dataset
    int recognized_digit = -1;
    int best_match = -1;

    for (int i = 0; i < dataset_size; i++) {
        MNIST_Image* image = &dataset[i];
        int match = 0;

        // Compare pixels of the current MNIST image with the input image
        for (int row = 0; row < 28; row++) {
            for (int col = 0; col < 28; col++) {
                if (image->pixels[row][col] != pixels[row][col]) {
                    match = 0;
                    break;
                }
                match = 1;
            }
            if (!match) {
                break;
            }
        }

        // Update the recognized digit if it is a better match
        if (match && image->label > best_match) {
            best_match = image->label;
            recognized_digit = best_match;
        }
    }

    return recognized_digit;
}
