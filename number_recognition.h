#ifndef NUMBER_RECOGNITION_H
#define NUMBER_RECOGNITION_H

typedef struct {
    unsigned char pixels[28][28];
    unsigned char label;
} MNIST_Image;

void load_MNIST_dataset(char* image_file_path, char* label_file_path, MNIST_Image** dataset, int* dataset_size);

int recognize_digit(const char* image_path, MNIST_Image* dataset, int dataset_size);

#endif /* NUMBER_RECOGNITION_H */
