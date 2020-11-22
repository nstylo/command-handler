#include "protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


int main(int argc, char *argv[])
{
    int fd_req;
    int fd_res;
    int fifo_req_created = mkfifo(FIFO_REQ, 0666);
    int fifo_res_created = mkfifo(FIFO_RES, 0666);

    // in which case it is assumed to be -1
    if (fifo_req_created != 0) {
        printf("Failed with [Errno %d]\n", errno);
        exit(EXIT_FAILURE);
    }

    // in which case it is assumed to be -1
    if (fifo_res_created != 0) {
        printf("Failed with [Errno %d]\n", errno);
        unlink(FIFO_REQ);
        exit(EXIT_FAILURE);
    }

    // open blocks until the pipe is opened from the other end
    fd_req = open(FIFO_REQ, O_RDONLY);
    fd_res = open(FIFO_RES, O_WRONLY);

    // setup and clean buffers
    Req req;
    memset(&req, 0, sizeof(req));
    Res res;
    memset(&res, 0, sizeof(res));

    // read until either we encounter error or we read something from pipe
    while (1) {
        ssize_t code = read(fd_req, &req, sizeof(req));

        if (code == -1) {
            printf("Could not read from pipe.\n");
            break;
        } else if (code != 0) {
            printf("\nReceived:\n\n");
            printf("type:\t\t%c\n", req.type);
            printf("size:\t\t%d\n", req.size);
            printf("x:\t\t%d\n", req.x);
            printf("y:\t\t%d\n", req.y);

            // here the actual computation happens
            // TODO: perhaps compose into seperate function
            switch (req.type) {
                case EXT:
                    goto finished;
                case ADD_REQ:
                    res.type = ADD_RES;
                    res.z = plus(req.x, req.y);
                    break;
                case SUB_REQ:
                    res.type = SUB_RES;
                    res.z = minus(req.x, req.y);
                    break;
                default:
                    res.type = SKP;
                    res.z = 0;
                    printf("\nUnknown operation: %c. Skipping.\n", req.type);
            }

            res.size = SIZE_RES;

            // TODO: this might fail, so handle
            write(fd_res, &res, sizeof(res));
        }

        usleep(1e6);
    }

    finished:
        // clean up
        printf("\nExiting.\n");
        close(fd_req);
        close(fd_res);
        unlink(FIFO_REQ);
        unlink(FIFO_RES);
        exit(0);

    return 0;
}
