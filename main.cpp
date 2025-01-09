#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>

// Function to read a file into a buffer
void readFile(const std::string& filename, std::vector<char>& buffer) {
    std::ifstream file(filename, std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        buffer.resize(size);
        file.read(buffer.data(), size);
    }
}

// Function to write a buffer to a file
void writeFile(const std::string& filename, const std::vector<char>& buffer) {
    std::ofstream file(filename, std::ios::binary);
    file.write(buffer.data(), buffer.size());
}

// Function to compress data using Run-Length Encoding (RLE)
std::unique_ptr<char[]> compressData(const std::vector<char>& data, size_t& compressedSize) {
    std::vector<char> compressed;
    for (size_t i = 0; i < data.size(); ++i) {
        size_t runLength = 1;
        while (i + 1 < data.size() && data[i] == data[i + 1]) {
            ++runLength;
            ++i;
        }
        compressed.push_back(data[i]);
        compressed.push_back(static_cast<char>(runLength));
    }
    compressedSize = compressed.size();
    auto compressedData = std::make_unique<char[]>(compressedSize);
    std::copy(compressed.begin(), compressed.end(), compressedData.get());
    return compressedData;
}

// Function to compress a single file
void compressFile(const std::string& filename) {
    std::vector<char> buffer;
    readFile(filename, buffer);
    size_t compressedSize;
    auto compressedData = compressData(buffer, compressedSize);
    writeFile(filename + ".compressed", std::vector<char>(compressedData.get(), compressedData.get() + compressedSize));
}

// Function to compress multiple files using multithreading
void compressFiles(const std::vector<std::string>& filenames) {
    std::vector<std::thread> threads;
    for (const auto& filename : filenames) {
        threads.emplace_back(compressFile, filename);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    std::vector<std::string> filenames = {"file1.txt", "file2.txt", "file3.txt"};
    compressFiles(filenames);
    return 0;
}