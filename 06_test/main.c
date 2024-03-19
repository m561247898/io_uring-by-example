#include <stdio.h>
#include <stdlib.h>
#include <liburing.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define FILENAME "output.txt"
#define MESSAGE "Hello, io_uring!\n"
#define QUEUE_DEPTH 1

int main() {
    struct io_uring ring;
    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;
    int fd, ret;

    // Open file
    fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // Initialize io_uring
    ret = io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
    if (ret < 0) {
        fprintf(stderr, "io_uring_queue_init failed: %d\n", ret);
        close(fd);
        return 1;
    }

    // Get a submission queue entry
    sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        fprintf(stderr, "io_uring_get_sqe failed\n");
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Prepare the write operation
    io_uring_prep_write(sqe, fd, MESSAGE, strlen(MESSAGE), 0);

    // Submit the write request
    ret = io_uring_submit(&ring);
    if (ret < 0) {
        fprintf(stderr, "io_uring_submit failed: %d\n", ret);
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Wait for completion
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
        fprintf(stderr, "io_uring_wait_cqe failed: %d\n", ret);
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Check the result of the write operation
    if (cqe->res < 0) {
        fprintf(stderr, "Write failed: %d\n", cqe->res);
    } else {
        printf("Write completed successfully\n");
    }

    // Mark this cqe as seen
    io_uring_cqe_seen(&ring, cqe);

    // Cleanup
    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}

