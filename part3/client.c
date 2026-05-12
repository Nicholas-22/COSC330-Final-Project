#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define PORT    9000
#define BUFSIZE 65536

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server-ip> <file-to-send>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    const char *filename  = argv[2];

    /* Stat the file to get its size */
    struct stat st;
    if (stat(filename, &st) < 0) {
        perror("stat");
        return 1;
    }
    uint64_t file_size = (uint64_t)st.st_size;

    FILE *fp = fopen(filename, "rb");
    if (!fp) { perror("fopen"); return 1; }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("socket"); fclose(fp); return 1; }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(PORT),
    };
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid server IP: %s\n", server_ip);
        goto cleanup;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        goto cleanup;
    }

    printf("Connected to %s:%d — sending '%s' (%lu bytes)\n",
           server_ip, PORT, filename, (unsigned long)file_size);

    /* Send file size header first */
    uint64_t file_size_net = htobe64(file_size);
    if (send(sock_fd, &file_size_net, sizeof(file_size_net), 0)
            != sizeof(file_size_net)) {
        perror("send header");
        goto cleanup;
    }

    /* Send file data */
    char    buf[BUFSIZE];
    size_t  n;
    uint64_t sent = 0;
    while ((n = fread(buf, 1, BUFSIZE, fp)) > 0) {
        ssize_t s = send(sock_fd, buf, n, 0);
        if (s < 0) { perror("send"); goto cleanup; }
        sent += s;
    }

    printf("Transfer complete: %lu bytes sent\n", (unsigned long)sent);

cleanup:
    fclose(fp);
    close(sock_fd);
    return 0;
}
