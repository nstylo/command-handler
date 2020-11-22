#define FIFO_REQ "/tmp/fifo_req"
#define FIFO_RES "/tmp/fifo_res"

#define ADD_REQ         'a'
#define ADD_RES         'A'
#define SUB_REQ         's'
#define SUB_RES         'S'
#define EXT             'x'
#define SKP             'K'
#define SIZE_REQ         8
#define SIZE_RES         4

#ifndef PROTOCOL_H
#define PROTOCOL_H

int plus(int x, int y);

int minus(int x, int y);

typedef unsigned char type_t;
typedef unsigned char length_t;

typedef struct {
    type_t      type;
    length_t    size;
    int         x, y;
} Req;

typedef struct {
    type_t      type;
    length_t    size;
    int         z;
} Res;

#endif
