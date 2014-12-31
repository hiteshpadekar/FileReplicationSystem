#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include "proj_commons.h"
#include "encry_buff_struct.h"

#define BUF_LEN 1024

struct finfo {
    char fname[512];
    time_t mtime;
    struct finfo *next;
};

struct finfo *add_to_list(struct finfo *head, char *fname, time_t mtime)
{
    struct finfo *temp, *new_node;

    new_node = malloc(sizeof(struct finfo));
    strcpy(new_node->fname, fname);
    new_node->mtime = mtime;
    new_node->next = NULL;

    if(head == NULL) {
        head = new_node;
    }
    else {
        temp = head;
        while(temp->next) {
            temp = temp->next;
        }
        temp->next = new_node;
    }

    return head;
}

struct finfo *remove_from_list(struct finfo *head, char *fname)
{
    struct finfo *temp, *prev, *del_node;

    if(head != NULL) {
        if(strcmp(head->fname, fname) == 0) {
            /* node to be deleted is the first node of the linked list */
            del_node = head;
            head = head->next;
            free(del_node);
        }
        else {
            /* node to be deleted is not the first node */
            temp = head;
            while(temp) {
                if(strcmp(temp->fname, fname) == 0) {
                    del_node = temp;
                    prev->next = temp->next;
                    free(del_node);
                    break;
                }
                prev = temp;
                temp = temp->next;
            }
        }
    }

    return head;
}

void print_list(struct finfo *head)
{
    while(head) {
        printf("[%u] %s [ modified %s", (unsigned)head->mtime, head->fname, ctime(&(head->mtime)));
        head = head->next;
    }
}

int send_delete_file(int server_fd, char *dir_path, char *fil_nam)
{
    req_t req;
    int len;
    int ret;
    char file_name[256];

    strcpy(file_name, fil_nam);

    printf("sending req to delete file %s\n", file_name);
    /* send req header */
    len = strlen(file_name);
    req.type = REQ_REMOVEFILE;
    req.len = len;
    printf("\nEncrypting request for sending ");
    //encrypt((char *)&req,sizeof(req));
    printf("\nEncrypted Buffer ");

    ret = write(server_fd, &req, sizeof(req_t));
    
    //decrypt((char *)&req,sizeof(req));
    printf("\nDecrypting request");
    /* send file name */
		printf("\nEncrypting file name for sending %s",file_name);
	//encrypt((char *)&file_name,len);
			printf("\nEncrypted file_name for sending %s",file_name);
    ret = write(server_fd, file_name, len);
	//decrypt((char *)&file_name,len);
				  printf("\nDecrypting file name after sent= %s",file_name);
    if(ret < 0) {
    }

    return 0;
}

int send_upload_file(int server_fd, char *dir_path, char *fil_nam)
{
    char fname[80];
    FILE *fp;
    int fsize;
    req_t req;
    int len;
    int ret;
    char buf[BUF_LEN];
    char file_name[256];
    char backdir[80];
    struct stat st;
    FILE *fpw;

    sprintf(backdir, "%s/%s", dir_path, ".projback");
    ret = stat(backdir, &st);
    if(ret < 0) {
        mkdir(backdir, 0777);
    }

    printf("uploading file %s\n", fil_nam);
    sprintf(fname, "%s/%s", dir_path, fil_nam);

    fp = fopen(fname, "r");
    /* calculate file size */
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* send req header */
    strcpy(file_name,fil_nam);
    len = strlen(file_name);
    req.type = REQ_UPLOADFNAME;
    req.len = len;
		printf("\nEncrypting request for sending ");
			//encrypt((char *)&req,sizeof(req));
    ret = write(server_fd, &req, sizeof(req_t));
			printf("\nDecrypting request");
			//decrypt((char *)&req,sizeof(req));
    /* send file name */
			printf("\nEncrypting file_name for sending %s",file_name);
			//encrypt((char *)&file_name,len);
			printf("\nEncrypted file_name for sending %s",file_name);
    ret = write(server_fd, file_name, len);
			//decrypt((char *)&file_name,len);
			printf("\nDecrypting file_name= %s",file_name);
    
			/* send req header */
    req.type = REQ_UPLOADFDATA;
    req.len = fsize;
	printf("\nEncrypting request for sending ");
			//encrypt((char *)&req,sizeof(req));
    ret = write(server_fd, &req, sizeof(req_t));
	printf("\nDecrypting request");
			//decrypt((char *)&req,sizeof(req));
    /* send file data */
    sprintf(fname, "%s/.projback/%s", dir_path, file_name);
    fpw = fopen(fname, "w");
    len = 0;
    do {
        ret = fread(buf, sizeof(char), sizeof(buf), fp);
        len += ret;
        if(ret > 0) {
			printf("\nEncrypting buffer for sending %s",buf);
			//encrypt((char *)&buf, ret);
			printf("\nEncrypted bufer for sending %s",buf);
            ret = write(server_fd, buf, ret);
			//decrypt((char *)buf,ret);
				  printf("\nDecrypted buffer= %s",buf);
            ret = fwrite(buf, sizeof(char), ret, fpw);
        }
    } while(len < fsize);

    fclose(fp);
    fclose(fpw);

    return 0;
}

int send_file_modification(int server_fd, char *dir_path, char *fil_nam)
{
    char fname[80];
    FILE *fp;
    int fsize;
    req_t req;
    int len;
    int ret;
    char file_name[256];
    char buf[BUF_LEN];
    char backdir[80];
    struct stat st;
    char cmd[256];

    strcpy(file_name,fil_nam);
    /* if database is not present, server might not have earlier copy, so send entire file */
    sprintf(backdir, "%s/%s", dir_path, ".projback");
    ret = stat(backdir, &st);
    if(ret < 0) {
        return send_upload_file(server_fd, dir_path, file_name);
    }

    /* if old version is not present, server might not have earlier copy, so send entire file */
    sprintf(fname, "%s/.projback/%s", dir_path, file_name);
    ret = stat(fname, &st);
    if(ret < 0) {
        return send_upload_file(server_fd, dir_path, file_name);
    }

    /* create patch */
    printf("creating patch for %s\n", file_name);
    sprintf(cmd, "diff -u %s/.projback/%s %s/%s > %s/.projback/.patch", 
            dir_path, file_name, dir_path, file_name, dir_path);
    system(cmd);

    /* send patch file */
    sprintf(fname, "%s/.projback/.patch", dir_path);
    fp = fopen(fname, "r");
    /* calculate file size */
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* send req header */
    len = strlen(file_name);
    req.type = REQ_UPLOADPATCH;
    req.len = len;
	printf("\nEncrypting request for sending ");
	//encrypt((char *)&req,sizeof(req));
    ret = write(server_fd, &req, sizeof(req_t));
	printf("\nDecrypting request");
	//decrypt((char *)&req,sizeof(req));
    /* send patch file name */
    printf("\nEncrypting file name for sending %s",file_name);
    //encrypt((char *)&file_name,len);
    printf("\nEncrypted file name for sending %s",file_name);
    ret = write(server_fd, file_name, len);
	//decrypt((char *)&file_name,len);
	printf("\nDecrypted file name after sent %s",file_name);
    /* send req header */
    req.type = REQ_UPLOADPATCHDATA;
    req.len = fsize;
	printf("\nEncrypting request for sending ");
			//encrypt((char *)&req,sizeof(req));
    ret = write(server_fd, &req, sizeof(req_t));
	printf("\nDecrypting request");
			//decrypt((char *)&req,sizeof(req));
    /* send patch file data */
    printf("sending patch file\n");
    len = 0;
    do {
        ret = fread(buf, sizeof(char), sizeof(buf), fp);
        len += ret;
        if(ret > 0) {
			printf("\nEncrypting bufer for sending %s",buf);
			//encrypt((char *)&buf, ret);
			printf("\nEncrypted bufer for sending %s",buf);
            ret = write(server_fd, buf, ret);
			//decrypt((char *)&buf,ret);
	        printf("\nDecrypted buf after sent %s",buf);
        }
    } while(len < fsize);

    fclose(fp);

    /* copy new version to database so as to create patch next time */
    sprintf(cmd, "cp %s/%s %s/.projback/", dir_path, file_name, dir_path);
    system(cmd);
    /* remove patch file */
    sprintf(cmd, "rm %s/.projback/.patch", dir_path);
    system(cmd);

    return 0;
}

int folder_sync(char *server_ip, char *server_port, char *dir_path)
{
    int server_fd;
    struct sockaddr_in sock_server;
    int ret;
    char buf[BUF_LEN];
    DIR *dirp;
    struct dirent dent;
    struct dirent *next_dent;
    req_t req;
    rsp_t rsp;
    time_t mtime;
    struct finfo *upload_list = NULL;
    struct finfo *delete_list = NULL;
    struct finfo *modification_list = NULL;
    struct stat stat_buf;
    char fname[512];
    char found;
    struct finfo *temp;

    /* create socket */
    ret = socket(AF_INET, SOCK_STREAM, 0);
    server_fd = ret;

    if(server_fd > 0){
        printf("CLient Successfully Created Socket Fd = %d\n",server_fd);
    }
    else{
        perror("Error in Creating Socket\n");
    }
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = inet_addr(server_ip);
    sock_server.sin_port = htons(atoi(server_port));

    ret = connect(server_fd, (struct sockaddr *)&sock_server, sizeof(struct sockaddr));
    if(ret == 0){
        printf("Client Successfully Connected to Server.......\n");
    }
    else{
        perror("Error in Client Socket\n");
        return 0;
    }

    /* loop through dir, populate list of text files */
    printf("text files in client dir:\n");
    dirp = opendir(dir_path);
    do {
        ret = readdir_r(dirp, &dent, &next_dent);
        if(next_dent == NULL) {
            break;
        }

        /* check if file is txt */
        if(strstr(dent.d_name, FILE_FILTER) != NULL &&  (strstr(dent.d_name, "~") == NULL) && 
                        (strstr(dent.d_name, ".orig") == NULL)) {
            sprintf(fname, "%s/%s", dir_path, dent.d_name);
            ret = stat(fname, &stat_buf);
            upload_list = add_to_list(upload_list, dent.d_name, stat_buf.st_mtime);
            printf("[%u] %s [ modified %s", (unsigned)stat_buf.st_mtime, dent.d_name, ctime(&(stat_buf.st_mtime)));
        }
    } while(next_dent);
    closedir(dirp);

    /* write req to get dir info */
    req.type = REQ_DIRINFO;
	printf("\nEncrypting request for sending ");
			//encrypt((char *)&req,sizeof(req));

    ret = write(server_fd, &req, sizeof(req_t));
	printf("\nDecrypting request");
			//decrypt((char *)&req,sizeof(req));
    do {
        /* read rsp header */
        ret = read(server_fd, &rsp, sizeof(rsp_t));
		 //decrypt((char *)&rsp,sizeof(rsp));
		 printf("\nDecripting Response = %d", rsp.type );
        if(rsp.type == RSP_DONE) {
            break;
        }

        if(rsp.type == RSP_FILENAME) {
            /* read file name */
            memset(buf, 0, BUF_LEN);
            ret = read(server_fd, buf, rsp.len);
			printf("\nEncrypted Buffer Received= %s",buf);
			//decrypt((char *)&buf, rsp.len);
			printf("\nDecripting Buffer= %s",buf);
            /* read file mtime */
            ret = read(server_fd, &mtime, sizeof(time_t));
	    //decrypt((char *)&mtime,sizeof(time_t));
            /* check if this file is present in client dir */
            temp = upload_list;
            found = 0;
            while(temp) {
                if(strcmp(temp->fname, buf) == 0) {
                    if(temp->mtime > mtime) {
                        /* client has the latest copy, send modifications */
                        upload_list = remove_from_list(upload_list, buf);
                        modification_list = add_to_list(modification_list, buf, mtime);
                    }
                    else {
                        /* latest copy is already saved on server */
                        upload_list = remove_from_list(upload_list, buf);
                    }
                    found = 1;
                    break;
                }
                temp = temp->next;
            }
            if(!found) {
                /* file is removed on client, remove it from server */
                upload_list = remove_from_list(upload_list, buf);
                delete_list = add_to_list(delete_list, buf, mtime);
            }
        }
	else
	   printf("ERROR : unknown response");
    } while(1);

    printf("\n");
    printf("upload_list:\n");
    print_list(upload_list);

    printf("\n");
    printf("delete_list:\n");
    print_list(delete_list);

    printf("\n");
    printf("modification_list:\n");
    print_list(modification_list);

    /* upload new files to server */
    temp = upload_list;
    while(temp) {
        send_upload_file(server_fd, dir_path, temp->fname);
        temp = temp->next;
    }

    /* delete files from server which are already deleted from client */
    temp = delete_list;
    while(temp) {
        send_delete_file(server_fd, dir_path, temp->fname);
        temp = temp->next;
    }

    /* send patches for modified files */
    temp = modification_list;
    while(temp) {
        send_file_modification(server_fd, dir_path, temp->fname);
        temp = temp->next;
    }

    close(server_fd);

    return 1;
}

int folder_recover(char *server_ip, char *server_port, char *dir_path)
{
    int server_fd;
    struct sockaddr_in sock_server;
    int ret;
    char buf[BUF_LEN];
    req_t req;
    rsp_t rsp;
    char fname[512];
    FILE *fp;
    int len;

    /* create socket */
    ret = socket(AF_INET, SOCK_STREAM, 0);
    server_fd = ret;
    if(server_fd > 0){
        printf("CLient Successfully Created Socket Fd = %d\n",server_fd);
    }
    else{
        perror("Error in Creating Socket\n");
    }
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = inet_addr(server_ip);
    sock_server.sin_port = htons(atoi(server_port));

    ret = connect(server_fd, (struct sockaddr *)&sock_server, sizeof(struct sockaddr));
    if(ret == 0){
        printf("Client Successfully Connected to Server.......\n");
    }
    else{
        perror("Error in Client Socket\n");
        return 0;
    }

    /* write req to get dir info */
    req.type = REQ_RECOVERDIR;
	printf("\nEncrypting request for sending ");
			//encrypt((char *)&req,sizeof(req));
    ret = write(server_fd, &req, sizeof(req_t));
	printf("\nDecrypting request");
			//decrypt((char *)&req,sizeof(req));
    do {
        /* read rsp header */
        ret = read(server_fd, &rsp, sizeof(rsp_t));
		 //decrypt((char *)&rsp,sizeof(rsp));
		 printf("\nDecripting Response");
        if(rsp.type == RSP_DONE) {
            break;
        }

        if(rsp.type == RSP_FILENAME) {
            /* read file name */
            memset(buf, 0, BUF_LEN);
            ret = read(server_fd, buf, rsp.len);
			printf("\nEncrypted Buffer Received= %s",buf);
			//decrypt((char *)&buf,rsp.len);
			printf("\nDecripting Buffer= %s",buf);
            /* read file data */
            ret = read(server_fd, &rsp, sizeof(rsp_t));
			 //decrypt((char *)&rsp,sizeof(rsp));
		 printf("\nDecripting Response");
            if(rsp.type != RSP_FILEDATA) {
                /* this should not get executed */
            }
            sprintf(fname, "%s/%s", dir_path, buf);
            printf("writing file %s, len %d\n", buf, rsp.len);
            fp = fopen(fname, "w");
            len = 0;
            do {
                ret = read(server_fd, buf, ((rsp.len - len < BUF_LEN)? (rsp.len - len): BUF_LEN));
				printf("\nEncrypted Buffer Received= %s",buf);
			//decrypt((char *)&buf,ret);
			printf("\nDecripting Buffer= %s",buf);
                fwrite(buf, sizeof(char), ret, fp);
                len += ret;
            } while(len < rsp.len);
            fclose(fp);
        }
    } while(1);

    close(server_fd);

    return 1;
}

int main(int argc, char *argv[])
{
    char server1_ip[80];
    char server2_ip[80];
    char server1_port[80];
    char server2_port[80];
    char dir_path[256];
    FILE *fp;

    if(argc < 3) {
        printf("USAGE %s <config_file> <dirpath> [-recover]\n", argv[0]);
        return 0;
    }
    printf("\n\n* * * * * * * * Client Daemon * * * * * * * * \n\n");
    /* daemonize */
    //if(fork()) return 0;

    strcpy(dir_path, argv[2]);

    fp = fopen(argv[1], "r");
    fscanf(fp, "%s%s", server1_ip, server1_port);
    fscanf(fp, "%s%s", server2_ip, server2_port);
   
    fclose(fp);
    
    if(argc > 3 && strcmp(argv[3], "-recover") == 0) {
        /* recover from the first server in config */
        if(!folder_recover(server1_ip, server1_port, dir_path))
	{
            printf("Unable to recover from Server : %s : %s",server1_ip, server1_port);
            if(!folder_recover(server2_ip, server2_port, dir_path))
            {
                printf("Unable to recover from Server : %s : %s",server2_ip, server2_port);
                printf("Both Replication Servers are Down, we can not recover");
            }
        }
    }

    do 
    {
       printf("syncing folder with server %s:%s\n", server1_ip, server1_port);
       folder_sync(server1_ip, server1_port, dir_path);
       printf("syncing folder with server %s:%s\n", server2_ip, server2_port);
       folder_sync(server2_ip, server2_port, dir_path);
       sleep(5); // check every 10 seconds
    } while(1);

    return 0;
}

