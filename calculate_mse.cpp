#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

struct BitMapFileHeader {
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
};

struct BitMapInfoHeader {
    unsigned int biSize;
    unsigned int biWidth;
    unsigned int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
};

unsigned char* readBmpImage(const string& filename, int& width, int& height, int& imageSize) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Cannot open file: " << filename << endl;
        return nullptr;
    }

    unsigned short bfType;
    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    file.read((char*)&bfType, sizeof(bfType));
    if (bfType != 0x4D42) {
        cerr << "Not a BMP file: " << filename << endl;
        return nullptr;
    }

    file.read((char*)&fileHeader, sizeof(fileHeader));
    file.read((char*)&infoHeader, sizeof(infoHeader));

    width = infoHeader.biWidth;
    height = infoHeader.biHeight;
    imageSize = infoHeader.biSizeImage;

    unsigned char* imageData = new unsigned char[imageSize];
    file.seekg(fileHeader.bfOffBits, ios::beg);
    file.read((char*)imageData, imageSize);
    file.close();

    return imageData;
}

double calculateMSE(const unsigned char* img1, const unsigned char* img2, int imageSize) {
    double mse = 0.0;
    for (int i = 0; i < imageSize; ++i) {
        double diff = static_cast<double>(img1[i]) - static_cast<double>(img2[i]);
        mse += diff * diff;
    }
    return mse / imageSize;
}

int main() {
    string file1 = "lenna.bmp";
    string file2 = "lenna_r.bmp";
    int width1, height1, imageSize1;
    int width2, height2, imageSize2;

    unsigned char* img1 = readBmpImage(file1, width1, height1, imageSize1);
    unsigned char* img2 = readBmpImage(file2, width2, height2, imageSize2);

    if (!img1 || !img2) {
        cerr << "Error loading images!" << endl;
        return -1;
    }

    if (width1 != width2 || height1 != height2 || imageSize1 != imageSize2) {
        cerr << "Images dimensions do not match!" << endl;
        delete[] img1;
        delete[] img2;
        return -1;
    }

    double mse = calculateMSE(img1, img2, imageSize1);
    cout << "Mean Squared Error (MSE): " << mse << endl;

    delete[] img1;
    delete[] img2;
    return 0;
}
