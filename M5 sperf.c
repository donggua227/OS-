#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/sha.h>

#define BMP_HEADER_SIZE 2
#define BUFFER_SIZE 512

void calculate_sha1(unsigned char *data, size_t size, char *output) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(data, size, hash);
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[SHA_DIGEST_LENGTH * 2] = 0;
}

void recover_bmp_files(char *file_path) {
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Failed to get file stats");
        close(fd);
        return;
    }

    unsigned char *map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        return;
    }

    // Scanning the file for BMP headers
    for (size_t i = 0; i < sb.st_size - BMP_HEADER_SIZE; i++) {
        if (map[i] == 'B' && map[i + 1] == 'M') {
            // Potential BMP header found
            printf("Found BMP header at offset: %zu\n", i);

            // Calculate SHA1 for this segment
            char sha1_output[SHA_DIGEST_LENGTH * 2 + 1];
            calculate_sha1(&map[i], BUFFER_SIZE, sha1_output);
            
            // Output the SHA1 and a filename placeholder
            printf("%s file_%zu.bmp\n", sha1_output, i);
        }
    }

    munmap(map, sb.st_size);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <FAT32 image file>\n", argv[0]);
        return 1;
    }

    recover_bmp_files(argv[1]);
    return 0;
}
