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
#include <sys/stat.h>
#include <time.h>
#include "client_serv.h"
#include "server_config.h"
#include "server_sync.h"
#include "fwk_datatypes.h"
#include "fwk_queue.h"
#include "encry_buff_struct.h"
#define BUF_LEN 1024

SERVER_CONFIG g_server_config;
extern MSG_QUEUE *sync_proc_msg_q;

extern struct ndir_name *glstDB;

extern struct nfil_name *glstDeleteFileList;
extern NAP_UINT32 guiDeletedListCount;


NAP_BOOL client_serv_post_msg(NAP_UINT32    uiMsgType,
				NAP_UINT32  uiMsgLen,
				NAP_CHAR    *pcFileName)
{
    MSG_Q_MESSAGE *pMsg = NAP_NULL;
    pMsg = (MSG_Q_MESSAGE *)malloc(sizeof(MSG_Q_MESSAGE));
    if(pMsg == NAP_NULL)
	return NAP_FAILURE;
    pMsg->FileSize  = uiMsgType;
    if(pcFileName == NULL)
	strcpy(pMsg->FileName, pcFileName);
    pMsg->FileSize  = uiMsgLen;

    send_message(sync_proc_msg_q, pMsg);
	
    return NAP_SUCCESS;
}

NAP_VOID client_serv_thread_handler(NAP_VOID *thread_hdl)
{
   int server_fd;
   int client_fd;
   struct sockaddr_in sock_server;
   struct sockaddr_in sock_client;
   int client_len;
   int ret;
   int optval;
   char buf[BUF_LEN];
   int len;
   char file_name[256];
   char dir_path[512];
   char fname[512];
   DIR *dirp;
   struct dirent dent;
   struct dirent *next_dent;
   req_t req;
   rsp_t rsp;
   struct stat stat_buf;
   FILE *fp;
   char cmd[256];
    int fsize;

   /*
   if(argc < 4) {
      printf("USAGE %s <ip> <port> <dirpath>\n", argv[0]);
      return 0;
   }*/

   /* daemonize */
   //if(fork()) return 0;

    printf("\n\n* * * * * * * * * * * Server Daemon Program * * * * * * * * * * * * \n\n");
   ret = socket(AF_INET, SOCK_STREAM, 0);
   server_fd = ret;
    if(server_fd > 0){
        printf("Successfully Created Server Socket Fd = %d........\n",server_fd);
    }
    else{
        perror("Error in Creating Server Socket:");
    }
   optval = 1;
   ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

   sock_server.sin_family = AF_INET;
   sock_server.sin_addr.s_addr = inet_addr(&g_server_config.ip_addr);
   sock_server.sin_port = htons(g_server_config.port);

   g_server_config.client_serv_fd = server_fd;

   if(bind(server_fd, (struct sockaddr *)&sock_server, sizeof(struct sockaddr)) < 0)
   { 
      printf("\n %s: Bind Failed", __FUNCTION__);
   }

    ret = listen(server_fd, 5);
    if(ret == 0){
        printf("Server Successfully Created Socekt Queue........\n");
    }
    else{
        perror("Error in Creating Server Queue:");
    }

   do {
        printf("Server Waiting..........\n");
      client_len = sizeof(struct sockaddr_in);
      ret = accept(server_fd, (struct sockaddr *)&sock_client, (socklen_t *) &client_len);
      client_fd = ret;
        if(client_fd > 0){
            printf("Server Accepted Client Connection..........\n\n");
        }
        else{
            perror("Error in Accepting Client Connection:");
        }
      printf("\nAccept ret = %d",ret);
      strcpy(dir_path, g_server_config.db_dir);
      strcat(dir_path, "DB/");

      while(1) {
         /* read client req */
         ret = read(client_fd, &req, sizeof(req_t));
		 //decrypt((char *)&req,sizeof(req));
		 printf("\nDecripting Request");
         if(ret <= 0) {
            /* client closed the connection */
            break;
         }

            if(req.type == REQ_RECOVERDIR) {
                dirp = opendir(dir_path);
                do {
                    ret = readdir_r(dirp, &dent, &next_dent);
                    if(next_dent == NULL) {
                        break;
                    }

                    /* check if file is txt */
                    if((strstr(dent.d_name, FILE_FILTER) != NULL) && 
			(strstr(dent.d_name, "~") == NULL) && 
			(strstr(dent.d_name, ".orig") == NULL)) {
                        printf("Sending File data to Client : %s\n", dent.d_name);
                        
                        /* write rsp header to send file name and mtime */
                        len = strlen(dent.d_name);
                        rsp.type = RSP_FILENAME;
                        rsp.len = len;
						//encrypt((char *)&rsp,sizeof(rsp));
						 printf("\nEncrypting responce:  type = %d",  rsp.type);
                        ret = write(client_fd, &rsp, sizeof(rsp_t));
						//decrypt((char *)&rsp,sizeof(rsp));

						printf("\nDecripting responce: type = %d", rsp.type);
                        /* send file name */
				  strcpy(file_name,dent.d_name);
						printf("\nEncrypting file name %s",file_name);
						//encrypt((char *)&file_name, len);
						printf("\nSending //encrypted file name %s ",file_name);
                        ret = write(client_fd, file_name, sizeof(file_name));
						printf("\nDecrypting file name %s",file_name);
						//decrypt((char *)&file_name,len);

                        sprintf(fname, "%s/%s", dir_path, dent.d_name);

                        fp = fopen(fname, "r");
                        /* calculate file size */
                        fseek(fp, 0, SEEK_END);
                        fsize = ftell(fp);
                        fseek(fp, 0, SEEK_SET);

                        /* send file data */
                        rsp.type = RSP_FILEDATA;
                        rsp.len = fsize;
				  //encrypt((char *)&rsp,sizeof(rsp));
				   printf("\nEncrypting buffer for sending %s",buf);
				   printf("\nEncrypting responce");
                        ret = write(client_fd, &rsp, sizeof(rsp_t));
						 printf("\n//decrypting responce");
				  //decrypt((char *)&rsp,sizeof(rsp));
                        len = 0;
                        do {
                            ret = fread(buf, sizeof(char), sizeof(buf), fp);
                            len += ret;
                            if(ret > 0) {
								 printf("\nSending //encrypted buffer =%s ",buf);
						 //encrypt((char *)&buf,ret);
                                ret = write(client_fd, buf, ret);
								printf("\n//decrypting buffer =%s ",buf);
						//decrypt((char *)&buf,ret);
						printf("\n//decrypted buffer =%s ",buf);
                            }
                        } while(len < fsize);
                        printf("Sent File......\n"); 
                        fclose(fp);
                    }
                } while(next_dent);
                closedir(dirp);
                printf("Sending Done Msg........\n");
                rsp.type = RSP_DONE;
				printf("\nEncrypting responce for sending ");
			//encrypt((char *)&rsp,sizeof(rsp));
                ret = write(client_fd, &rsp, sizeof(rsp));
				printf("\n//decrypting responce");
			//decrypt((char *)&rsp,sizeof(rsp));
            }
         if(req.type == REQ_REMOVEFILE) {
            memset(buf, 0, BUF_LEN);
            ret = read(client_fd, buf, req.len);
			printf("\nEncrypted Buffer Received= %s",buf);
			//decrypt((char *)&buf,ret);
			printf("\nDecripting Buffer= %s",buf);
            sprintf(fname, "%s/%s", dir_path, buf);
            remove(fname);
                printf("Request Received Remove File\n");
                printf("Removing File %s\n", buf);
	    //client_serv_post_msg(eFILE_DELETE_MSG, 0, fname);
	    insert_nfil_name(&(glstDeleteFileList), buf, 0);
	    guiDeletedListCount++;
	    
	    delete_list(&glstDB);
            directorylisting2levels(&glstDB);

         }
         if(req.type == REQ_UPLOADPATCH) {
            memset(buf, 0, BUF_LEN);
            ret = read(client_fd, buf, req.len);
			printf("\nEncrypted Buffer Received= %s",buf);
			//decrypt((char *)&buf, ret);
			printf("\nDecripting Buffer= %s",buf);
            sprintf(fname, "%s/%s", dir_path, ".patch");
                printf("Req Received Patch File Name\n");
                printf("Patching File %s\n", buf);
	    //client_serv_post_msg(eFILE_CREATE_MSG, 0, fname);
         }
         if(req.type == REQ_UPLOADPATCHDATA) {
                printf("Req Received patch file data\n");
            fp = fopen(fname, "w");
            len = 0;
            do {
               ret = read(client_fd, buf, ((req.len - len < BUF_LEN)? (req.len - len): BUF_LEN));
			   printf("\nEncrypted Buffer Received= %s",buf);
			   //decrypt((char *)&buf,ret);
			   printf("\nDecripting Buffer= %s",buf);
               fwrite(buf, sizeof(char), ret, fp);
               len += ret;
            } while(len < req.len);
            fclose(fp);
            chdir(dir_path);
            sprintf(cmd, "patch < %s", fname);
            system(cmd);
            sprintf(cmd, "rm %s", fname);
            system(cmd);
	    //client_serv_post_msg(eFILE_MODIFY_MSG, 0, fname);
         }
         if(req.type == REQ_UPLOADFNAME) {
            memset(buf, 0, BUF_LEN);
            ret = read(client_fd, buf, req.len);
			 printf("\nEncrypted Buffer Received= %s",buf);
			   //decrypt((char *)&buf,ret);
			   printf("\nDecripting Buffer= %s",buf);
			
            sprintf(fname, "%s/%s", dir_path, buf);
                printf("Req Received Upload File\n");
                printf("Receiving File %s\n", buf);
         }
         if(req.type == REQ_UPLOADFDATA) {
                printf("Received File data\n");
                printf("Receiving Data Len = %d\n",req.len);
            fp = fopen(fname, "w");
            len = 0;
            do {
               ret = read(client_fd, buf, ((req.len - len < BUF_LEN)? (req.len - len): BUF_LEN));
		printf("\nEncrypted Buffer Received= %s",buf);
		 if (ret > 0)
		{
		   //decrypt((char *)&buf,ret);
	       		printf("\nDecripting Buffer= %s",buf);
			   
               		if(fp)
	  		   fwrite(buf, sizeof(char), ret, fp);
			else
 		    	printf("no able to open file %s\n",fname);
               	len += ret;
		}
            } while(len < req.len);
            fclose(fp);
	    //client_serv_post_msg(eFILE_CREATE_MSG, req.len, fname);
         }
         if(req.type == REQ_DIRINFO) {
                printf("Req Received Dir Info\n");
            /* loop through dir */
            dirp = opendir(dir_path);
            do {
		printf("\nDir Open success");
               ret = readdir_r(dirp, &dent, &next_dent);
               if(next_dent == NULL) {
                  break;
               }

               /* check if file is txt */
               if((strstr(dent.d_name, FILE_FILTER) != NULL)&&
                        (strstr(dent.d_name, "~") == NULL) &&
                        (strstr(dent.d_name, ".orig") == NULL)) {
                        printf("Sending File Info %s\n", dent.d_name);
                  /* write rsp header to send file name and mtime */
                  len = strlen(dent.d_name);
                  rsp.type = RSP_FILENAME;
                  rsp.len = len;
		    printf("\nEncrypting responce:  type = %d",  rsp.type);
		  //encrypt((char *)&rsp,sizeof(rsp));
                  ret = write(client_fd, &rsp, sizeof(rsp_t));
				  //decrypt((char *)&rsp,sizeof(rsp));
		    printf("\nDecrypting responce:  type = %d",  rsp.type);

                  /* send file name */
				  strcpy(file_name,dent.d_name);
				   printf("\nEncrypting file_name= %s",file_name);
				  //encrypt((char *)&file_name,strlen(file_name));
				  printf("\nEncrypted file_name= %s",file_name);
                  ret = write(client_fd, file_name, len);
				  
				  //decrypt((char *)&file_name,ret);
				  printf("\nDecrypted file_name= %s",file_name);

                  /* send file mtime */
                  sprintf(fname, "%s/%s", dir_path, dent.d_name);
                  ret = stat(fname, &stat_buf);
		  //encrypt((char *)&stat_buf.st_mtime,sizeof(time_t));
                  ret = write(client_fd, &stat_buf.st_mtime, sizeof(time_t));
		  //decrypt((char *)&stat_buf.st_mtime,sizeof(time_t));
               }
            } while(next_dent);
            closedir(dirp);

            rsp.type = RSP_DONE;
	    printf("\nEncrypting responce:  type = %d",  rsp.type);
	    //encrypt((char *)&rsp,sizeof(rsp));
            ret = write(client_fd, &rsp, sizeof(rsp));
	    //decrypt((char *)&rsp,sizeof(rsp));
	    printf("\nDecrypting responce:  type = %d",  rsp.type);
         }
      }

      close(client_fd);
    } while(1);

   sleep(1);
   close(server_fd);

   return 0;
}

