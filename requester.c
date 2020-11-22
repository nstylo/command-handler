#include "protocol.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>


/*
 * Auxiliary function to compute the expected response.
 * Return 0 if command is unknown. Caller is responsible for handling
 * such case correctly.
 */
int compute_response(int x, int y, type_t type) {
    switch (type) {
        case ADD_REQ:
            return plus(x, y);
        case SUB_REQ:
            return minus(x, y);
        default:
            return 0;
    }
}

int main(int argc, char *argv[])
{
    // opening the connection
    int fd_req;
    int fd_res;
    fd_req = open(FIFO_REQ, O_WRONLY);
    fd_res = open(FIFO_RES, O_RDONLY);

    // check if pipes exists
    if (fd_req == -1) {
        printf("\nNo pipe with name '%s'. Exiting.\n", FIFO_REQ);
        exit(EXIT_FAILURE);
    }
    if (fd_res == -1) {
        printf("\nNo pipe with name '%s'. Exiting.\n", FIFO_RES);
        exit(EXIT_FAILURE);
    }

    /***************** EDIT THIS PART OF THE CODE ********************/


    # define NROF_REQ    4
    Req requests[NROF_REQ] = {
        { ADD_REQ, SIZE_REQ, 1, 2 },
        { SUB_REQ, SIZE_REQ, 3, 2 },
        { 'd', SIZE_REQ, 4, 2 },                    // example of invalid command
        { SUB_REQ, SIZE_REQ, INT_MIN, INT_MAX }     // TODO: handle int overflow
    };


    /*****************************************************************/

    // setup and clean response buffer
    Res res;
    memset(&res, 0, sizeof(res));

    for (int i = 0; i < NROF_REQ; i++) {
        // setup and clean request buffer
        Req req;
        memset(&req, 0, sizeof(req));

        // make request
        req = requests[i];
        int expected_response = compute_response(req.x, req.y, req.type);
        ssize_t code = write(fd_req, &req, sizeof(req));

        // read until either we encounter error or we read something from pipe
        while (1) {
            ssize_t code = read(fd_res, &res, sizeof(res));

            if (code == -1) {
                printf("Could not read from pipe.\n");
                break;
            } else if (code != 0) {

                // receiver may send a skip instruction, e.g. when error occured
                if (res.type == SKP) {
                    printf("\nSkipping response.\n");
                    break;
                }

                // report if results did not match
                if (expected_response != res.z) {
                    printf("\nExpected response %d does not match actual response %d.\n", expected_response, res.z);
                    break;
                }

                // print response
                printf("\nReceived:\n\n");
                printf("type:\t\t%c\n", res.type);
                printf("size:\t\t%d\n", res.size);
                printf("z:\t\t%d\n", res.z);

                // At this point you could do anything with the received TLV-encoded data
                // ...

                break;
            }
        }
    }

    // send final request to close connection
    Req req_final = {EXT, SIZE_REQ, 0, 0};
    write(fd_req, &req_final, sizeof(req_final));

    // cleaning up
    printf("\nExiting.\n");
    usleep(1e6);
    close(fd_req);
    close(fd_res);

    return 0;
}
