/*
 * File: server.c
 * Author: Hitesh Padekar
 * email id: hitesh.padekar@sjsu.edu
 */


#include "fwk_datatypes.h"
#include <pthread.h>
#include "server_config.h"
#include "dir_oper.h"


SERVER_CONFIG g_server_config;

extern NAP_VOID sync_master_thread_handler(NAP_VOID *thread_hdl);
extern NAP_VOID sync_slave_thread_handler(NAP_VOID *thread_hdl);
extern NAP_VOID client_serv_thread_handler(NAP_VOID *thread_hdl);
extern NAP_VOID sync_proc_thread_handler(NAP_VOID *thread_hdl);
extern struct ndir_name *glstDB;

NAP_BOOL server_spawn_thread(NAP_VOID *(*func)(NAP_VOID *), 
				pthread_t     *pThreadId, 
				NAP_VOID      *arg)
{
   NAP_UINT32 thread_rc;

   printf("\n%s: Entering ", __FUNCTION__);

   thread_rc = pthread_create(pThreadId, NAP_NULL, (NAP_VOID *)func, arg);
   if (thread_rc) {
	printf("%s: Thread creation error ", __FUNCTION__);
	return NAP_FAILURE;
   }
   printf("\n%s: Exit ", __FUNCTION__);
   return NAP_SUCCESS;

}

/*
 * Function: main
 * 
 * Description: Main entry point. Simply perform initialization and pass
 *              control to the queue processing thread. 
 */
int
main(int argc, char * argv[])
{
   NAP_UINT32    rc;
   NAP_UINT32    s;
   NAP_VOID      *res;
   NAP_UINT32    sub_rc;
   NAP_UINT32    iOption;
   NAP_UCHAR     isExit;
   NAP_UCHAR     isCompress;
   NAP_UCHAR     isEncrypt;

   /* Read the config */
   printf("\n######### Enter Server Configuration ######### \n");
   printf("\nEnter Server IP Address: ");
   scanf("%s", &g_server_config.ip_addr);
   printf("\nEnter Server port for client handling: ");
   scanf("%d", &g_server_config.port);
   printf("\nEnter Server DB directory: ");
   scanf("%s", &g_server_config.db_dir);
   //printf("\nEnter sync Master server IP address: ");
   //scanf("%s", &g_server_config.sync.master_cfg.sync_ip_addr);
   printf("\nEnter sync Master server port: ");
   scanf("%d", &g_server_config.sync.master_cfg.sync_port);
   //printf("\nEnter peer sync Slave server IP address: ");
   //scanf("%s", &g_server_config.sync.slave_cfg.sync_ip_addr);
   printf("\nEnter peer sync slave server port: ");
   scanf("%d", &g_server_config.sync.slave_cfg.sync_port);
   memcpy(&g_server_config.sync.master_cfg.sync_ip_addr,
	   &g_server_config.ip_addr,
	   sizeof(g_server_config.sync.master_cfg.sync_ip_addr));

   memcpy(&g_server_config.sync.slave_cfg.sync_ip_addr,
	   &g_server_config.ip_addr,
	   sizeof(g_server_config.sync.slave_cfg.sync_ip_addr));
#if 0
    printf("Enable File Compression: (Y/N):\n");
    scanf("%c",&isCompress);
    if((isCompress == 'y') || (isCompress == 'Y'))
    {
	g_server_config.bCompression = NAP_TRUE;
	printf("File Compression Enabled!\n");
    } 
    if((isCompress == 'n') || (isCompress == 'N'))
    {
	g_server_config.bCompression = NAP_FALSE;
	printf("File Compression Disabled!\n");
    }	
    printf("Enable File Encryption: (Y/N): \n");
    scanf("%c",&isEncrypt);
    if(isEncrypt == 'y' || isEncrypt == 'Y')
    {
      g_server_config.bEncryption = NAP_TRUE;
      printf("\n File Encryption Enabled!");
    } 
    if(isEncrypt == 'n' || isEncrypt == 'N')
    {
      g_server_config.bEncryption = NAP_FALSE;
      printf("\n File Encryption Disabled!");
    }
	
#endif
    printf("\nSpawning Client Serving thread");
   if(NAP_SUCCESS == server_spawn_thread((NAP_VOID *)client_serv_thread_handler, 
					&g_server_config.client_thrd_id,
					NAP_NULL))
   { 
      printf("\nSpawned Client Serving thread");
   }
#if 1
    printf("\nspawning sync Master thread");
   if(NAP_SUCCESS == server_spawn_thread((NAP_VOID *)sync_master_thread_handler,
					&g_server_config.sync_master_thrd_id,
					NAP_NULL))
   {
      printf("\nspawned sync Master thread");
   }

   sleep(2);
  
       printf("\nspawning sync - up  Slave thread");
       if(NAP_SUCCESS == server_spawn_thread((NAP_VOID *)sync_slave_thread_handler, 
					    &g_server_config.sync_slave_thrd_id,
					    NAP_NULL))
       { 
	  printf("\nspawned sync Slave thread");
       }
#endif
#if 0     
       sleep(10);
       printf("main(): sending cancellation request\n");
	s = pthread_cancel(g_server_config.sync_slave_thrd_id);
	if (s != 0)
	    printf("pthread_cancel");

       /* Join with thread to see what its exit status was */

       s = pthread_join(g_server_config.sync_slave_thrd_id, &res);
	if (s != 0)
	    printf("pthread_join");

       if (res == PTHREAD_CANCELED)
	    printf("main(): thread was canceled\n");
	else
	    printf("main(): thread wasn't canceled (shouldn't happen!)\n");
   }while(1);
#endif
  
#if 0
   sleep(2);

   printf("\nspawning sync Processing thread");
   if(NAP_SUCCESS == server_spawn_thread((NAP_VOID *)sync_proc_thread_handler, 
					&g_server_config.sync_proc_thrd_id,
					NAP_NULL))
   { 
      printf("\nspawned sync Processing thread");
   }

   /* Spawn Client serving thread */
OPTIONS:
   {
      printf("\nChoose Options(O/o) / Quit(Q/q):");
      scanf("%c", &isExit);
      if((isExit == 'Q') || (isExit == 'q'))
          goto EXIT;
      if((isExit == 'O') || (isExit == 'o'))
      {
          printf("\nCompression settings (C/c)\nEncryption Settings (E/e) \nPrint Server DB File list(P/p)");
          printf("\nChoose your option:\n");
          scanf("%u",&iOption);
          switch(iOption)
	  {
	      case 1:
		  printf("Enable File Compression: (Y/N):\n");
		  scanf("%c",&isCompress);
		  if((isCompress == 'y') || (isCompress == 'Y'))
		  {
		      g_server_config.bCompression = NAP_TRUE;
		      printf("File Compression Enabled!\n");
		  } 
		  if((isCompress == 'n') || (isCompress == 'N'))
		  {
		      g_server_config.bCompression = NAP_FALSE;
		      printf("File Compression Disabled!\n");
		  }	
		  break;
	      case 2:
		  printf("Enable File Encryption: (Y/N): \n");
		  scanf("%c",&isEncrypt);
		  if(isEncrypt == 'y' || isEncrypt == 'Y')
		  {
		      g_server_config.bEncryption = NAP_TRUE;
		      printf("\n File Encryption Enabled!");
		  } 
		  if(isEncrypt == 'n' || isEncrypt == 'N')
		  {
		      g_server_config.bEncryption = NAP_FALSE;
		      printf("\n File Encryption Disabled!");
		  }	
		  break;
	      case 3:
		  print_ndir_name(&glstDB);
		  break;
	      default:
		  printf("\n Invalid Option, Try Again !!!");
		  break;
	    }
	}
      fflush(stdin);
      fflush(stdout);
    }
	goto OPTIONS;

#endif
EXIT:
   pthread_join(g_server_config.sync_master_thrd_id, NAP_NULL);
   pthread_join(g_server_config.sync_slave_thrd_id, NAP_NULL);
   pthread_join(g_server_config.client_thrd_id, NAP_NULL);
   printf("\nAll Threads Died");
   exit(0);
}
