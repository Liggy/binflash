/**
**  winsg.h
**  Saturday. September 21, 2002
**
**  Pretend to link with Linux SG_IO when compiled for Windows.
**/

#ifdef _WIN32

#include <fcntl.h>
#include <io.h>

extern int ioctl(int, int, void *);

typedef struct sg_io_hdr /* copied from a Linux */
{
    int interface_id;           /* [i] 'S' (required) */
    int dxfer_direction;        /* [i] */
    unsigned char cmd_len;      /* [i] */
    unsigned char mx_sb_len;    /* [i] */
    unsigned short iovec_count; /* [i] */
    unsigned int dxfer_len;     /* [i] */
    void * dxferp;              /* [i], [*io] */
    unsigned char * cmdp;       /* [i], [*i]  */
    unsigned char * sbp;        /* [i], [*o]  */
    unsigned int timeout;       /* [i] unit: millisecs */
    unsigned int flags;         /* [i] */
    int pack_id;                /* [i->o] */
    void * usr_ptr;             /* [i->o] */
    unsigned char status;       /* [o] */
    unsigned char masked_status;/* [o] */
    unsigned char msg_status;   /* [o] */
    unsigned char sb_len_wr;    /* [o] */
    unsigned short host_status; /* [o] */
    unsigned short driver_status;/* [o] */
    int resid;                  /* [o] */
    unsigned int duration;      /* [o] */
    unsigned int info;          /* [o] */
} sg_io_hdr_t;  /* 64 bytes long (on i386) */

#define SG_INFO_OK_MASK 1 /* != Linux ? */
#define SG_INFO_OK 0 /* != Linux ? */

#define SG_DXFER_NONE 0x00 /* != Linux ? */
#define SG_DXFER_FROM_DEV 0x08 /* != Linux ? */
#define SG_DXFER_TO_DEV 0x10 /* != Linux ? */

#define SG_IO 0x1234 /* != Linux ? */
#define SG_GET_VERSION_NUM 0x1235 /* != Linux ? */

int ioctl(int fd, int op, void * data)
    {
    fd = fd;
    if (op == SG_GET_VERSION_NUM)
        {
        int * pi = (int *) data;
        *pi = 30000;
        return 0;
        }
    return 0;
    }

#endif /* _WIN32 */

/* end of file */
