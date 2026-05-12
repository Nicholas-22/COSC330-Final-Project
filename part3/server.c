#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT        9000
#define BUFSIZE     65536
#define SAVE_NAME   "received_file1.txt"

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    /* Allow quick restart without waiting for TIME_WAIT */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(PORT),
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(server_fd); return 1;
    }
    if (listen(server_fd, 1) < 0) {
        perror("listen"); close(server_fd); return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int conn_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (conn_fd < 0) { perror("accept"); close(server_fd); return 1; }

    printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

    /* First 8 bytes: file size as a big-endian uint64 */
    uint64_t file_size_net;
    if (recv(conn_fd, &file_size_net, sizeof(file_size_net), MSG_WAITALL)
            != sizeof(file_size_net)) {
        fprintf(stderr, "Failed to receive file size\n");
        goto cleanup;
    }
    uint64_t file_size = be64toh(file_size_net);
    printf("Expecting %lu bytes\n", (unsigned long)file_size);

    FILE *fp = fopen(SAVE_NAME, "wb");
    if (!fp) { perror("fopen"); goto cleanup; }

    char buf[BUFSIZE];
    uint64_t received = 0;
    ssize_t  n;
    while (received < file_size) {
        uint64_t remaining = file_size - received;
        size_t   to_read   = remaining < BUFSIZE ? (size_t)remaining : BUFSIZE;
        n = recv(conn_fd, buf, to_read, 0);
        if (n <= 0) break;
        fwrite(buf, 1, n, fp);
        received += n;
    }
    fclose(fp);

    if (received == file_size)
        printf("File saved as '%s' (%lu bytes)\n", SAVE_NAME, (unsigned long)received);
    else
        fprintf(stderr, "Warning: expected %lu bytes, got %lu\n",
                (unsigned long)file_size, (unsigned long)received);

cleanup:
    close(conn_fd);
    close(server_fd);
    return 0;
}
