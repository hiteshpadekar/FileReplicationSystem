#ifndef __PROJ_COMMONS_H__
#define __PROJ_COMMONS_H__

#define FILE_FILTER ".txt"

enum req_type_e {
   REQ_INVALID,
   REQ_DONE,
   REQ_DIRINFO,
   REQ_UPLOADFNAME,
   REQ_UPLOADFDATA,
   REQ_REMOVEFILE,
   REQ_UPLOADPATCH,
   REQ_UPLOADPATCHDATA,
   REQ_RECOVERDIR
};
typedef enum req_type_e req_type_t;

enum rsp_type_e {
   RSP_INVALID,
   RSP_DONE,
   RSP_FILENAME,
   RSP_FILEDATA
};
typedef enum rsp_type_e rsp_type_t;

struct req_s {
   req_type_t type;
   int len;
};
typedef struct req_s req_t;

struct rsp_s {
   rsp_type_t type;
   int len;
};
typedef struct rsp_s rsp_t;

#endif

