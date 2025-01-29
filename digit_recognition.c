#include "digit_recognition.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TRAINING_SET_LABEL_FILE "training_data/train-labels-idx1-ubyte"
#define TRAINING_SET_IMAGE_FILE "training_data/train-images-idx3-ubyte"
#define TEST_SET_LABEL_FILE "training_data/t10k-labels-idx1-ubyte"
#define TEST_SET_IMAGE_FILE "training_data/t10k-images-idx3-ubyte"
#define IMAGE_SIZE 28

typedef struct {
    uint32_t magicNumber;
    uint32_t numItems;
} MNISTHeader;

int recognizeDigit(const char* imageFilePath) {
    // Load and process the training data
    FILE* labelFile = fopen(TRAINING_SET_LABEL_FILE, "rb");
    FILE* imageFile = fopen(TRAINING_SET_IMAGE_FILE, "rb");
    if (labelFile == NULL || imageFile == NULL) {
        perror("Failed to open training data files");
        return -1;
    }

    MNISTHeader labelHeader, imageHeader;
    fread(&labelHeader, sizeof(MNISTHeader), 1, labelFile);
    fread(&imageHeader, sizeof(MNISTHeader), 1, imageFile);
    labelHeader.numItems = __builtin_bswap32(labelHeader.numItems);
    imageHeader.numItems = __builtin_bswap32(imageHeader.numItems);

    if (labelHeader.numItems != imageHeader.numItems) {
        printf("Error: Number of labels does not match the number of images\n");
        fclose(labelFile);
        fclose(imageFile);
        return -1;
    }

    uint8_t* labels = (uint8_t*)malloc(labelHeader.numItems * sizeof(uint8_t));
    uint8_t* images = (uint8_t*)malloc(imageHeader.numItems * IMAGE_SIZE * IMAGE_SIZE * sizeof(uint8_t));
    if (labels == NULL || images == NULL) {
        perror("Memory allocation failed");
        fclose(labelFile);
        fclose(imageFile);
        return -1;
    }

    fread(labels, sizeof(uint8_t), labelHeader.numItems, labelFile);
    fread(images, sizeof(uint8_t), imageHeader.numItems * IMAGE_SIZE * IMAGE_SIZE, imageFile);
    fclose(labelFile);
    fclose(imageFile);

    // Load the test image
    FILE* testImageFile = fopen(imageFilePath, "rb");
    if (testImageFile == NULL) {
        perror("Failed to open test image file");
        free(labels);
        free(images);
        return -1;
    }

    fseek(testImageFile, 0, SEEK_END);
    long imageSize = ftell(testImageFile);
    rewind(testImageFile);

    uint8_t* testImage = (uint8_t*)malloc(imageSize * sizeof(uint8_t));
    if (testImage == NULL) {
        perror("Memory allocation failed");
        fclose(testImageFile);
        free(labels);
        free(images);
        return -1;
    }

    fread(testImage, sizeof(uint8_t), imageSize, testImageFile);
    fclose(testImageFile);

    // Perform digit recognition
    int recognizedDigit = 0;
    int maxScore = 0;

    for (uint32_t i = 0; i < imageHeader.numItems; i++) {
        int score = 0;
        for (int j = 0; j < IMAGE_SIZE * IMAGE_SIZE; j++) {
            score += abs(images[i * IMAGE_SIZE * IMAGE_SIZE + j] - testImage[j]);
        }

        if (i ==0 || score < maxScore) {
            maxScore = score;
            recognizedDigit = labels[i];
        }
    }
    // Clean up
    free(labels);
    free(images);
    free(testImage);

// Return the recognized digit
    return recognizedDigit;
}

void trainAlgorithm() {
// Load and process the training data
    FILE* labelFile = fopen(TRAINING_SET_LABEL_FILE, "rb");
    FILE* imageFile = fopen(TRAINING_SET_IMAGE_FILE, "rb");
    if (labelFile == NULL || imageFile == NULL) {
        perror("Failed to open training data files");
        return;
    }
    MNISTHeader labelHeader, imageHeader;
    fread(&labelHeader, sizeof(MNISTHeader), 1, labelFile);
    fread(&imageHeader, sizeof(MNISTHeader), 1, imageFile);
    labelHeader.numItems = __builtin_bswap32(labelHeader.numItems);
    imageHeader.numItems = __builtin_bswap32(imageHeader.numItems);

    if (labelHeader.numItems != imageHeader.numItems) {
        printf("Error: Number of labels does not match the number of images\n");
        fclose(labelFile);
        fclose(imageFile);
        return;
    }

    uint8_t* labels = (uint8_t*)malloc(labelHeader.numItems * sizeof(uint8_t));
    uint8_t* images = (uint8_t*)malloc(imageHeader.numItems * IMAGE_SIZE * IMAGE_SIZE * sizeof(uint8_t));
    if (labels == NULL || images == NULL) {
        perror("Memory allocation failed");
        fclose(labelFile);
        fclose(imageFile);
        return;
    }

    fread(labels, sizeof(uint8_t), labelHeader.numItems, labelFile);
    fread(images, sizeof(uint8_t), imageHeader.numItems * IMAGE_SIZE * IMAGE_SIZE, imageFile);
    fclose(labelFile);
    fclose(imageFile);

// Perform training
// You can implement your training algorithm here using the labels and images data

// Clean up
    free(labels);
    free(images);
}
