#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#include "dir_oper.h"
#include "server_config.h"

extern SERVER_CONFIG g_server_config;

////Function takes pointerof dir structure,NULL, dirname-> look for dir return 1 else 0
int dir_lookup(struct ndir_name **headref,
	       struct ndir_name **headptr, 
	       struct ndir_name **retPtr, 
	       char name[])
{
	if(headptr==NULL)
	{
	struct ndir_name *findptr= *headref;
		if(findptr == NULL)
		{
			return 0;
		}	
		while(findptr != NULL)
		{
			if(strcmp(name,findptr->dir_name)==0)
                        {
                           *retPtr = findptr;
			   return 1;
                        }
			findptr = findptr->next;
		}
		return 0;
	}
	else{
		if((*headptr) == NULL)
		{
			return 0;
		} 
		while((*headptr) != NULL)
		{
			if(strcmp(name,(*headptr)->dir_name)==0)
                        {
                           retPtr = headptr;
			   return 1;
                        }
			(*headptr) = (*headptr)->next;
		}
		return 0;
	}
}
//Function takes pointerof dir type,name of dir, filename-> look for file return 1 else 0
int file_lookup(struct ndir_name **headref,
	        struct nfil_name **retPtr,
		char name[], 
		char filename[] )
{
	struct ndir_name *findptr = *headref;
	struct nfil_name *findfil;
	if(!(dir_lookup(NULL, (&findptr), (&findptr), name)))
	{
		return 0;
	}
	else
	{
		findfil = findptr->filenext;
		if(findfil == NULL)
        	{
			return 0;
        	} 
        	while(findfil != NULL)
        	{
			if(strcmp(filename,findfil->fil_name)==0)
			{
				*retPtr = findfil;
				return 1;
			}
			findfil = findfil->next;
       	 	}
        	return 0;

	}

}
//Insert function for directories
void insert_ndir_name(struct ndir_name **refhead,char dir_name[SIZE_FILE_NAME])
{
	struct ndir_name* newnode;
	newnode =(struct ndir_name*)malloc(sizeof(struct ndir_name));
	strcpy(newnode->dir_name,dir_name);
	newnode->next = *refhead;
	*refhead = newnode;
	newnode->filenext=NULL;
}	
//Insert function for files
void insert_nfil_name(struct nfil_name **refhead, char fil_name[SIZE_FILE_NAME], time_t mtime)
{
	struct nfil_name* newnode;
	newnode =(struct nfil_name*)malloc(sizeof(struct nfil_name));
	strcpy(newnode->fil_name, fil_name);
	newnode->mtime = mtime;
	newnode->next = *refhead;
	*refhead = newnode;
}

//Print function for printing the whole list Takes pointer referance to main pointer
void print_ndir_name(struct ndir_name **refhead)
{
	struct ndir_name* printptr = *refhead;
	struct nfil_name* nextfileheadref=NULL;
	printf("\nPrinting from linked list\n");
	while(printptr != NULL)
	{
		nextfileheadref = printptr->filenext;
		printf("\n%s",printptr->dir_name);
		printf("\t%d",printptr->FileCount);
		while(nextfileheadref!=NULL)
		{
			printf("\n\t|-%s\t%s\n",nextfileheadref->fil_name, ctime(&nextfileheadref->mtime));
			nextfileheadref = nextfileheadref->next;
		}
		printptr = printptr->next;
	}
}
//Delets the whole list
void delete_list(struct ndir_name** headref)
{
	struct ndir_name* freedir=NULL;
	struct nfil_name* freefile=NULL;
	struct nfil_name* fileheadref=NULL;
	while(*headref != NULL)
	{
		freedir = *headref;
		fileheadref =freedir->filenext;
		while(fileheadref!=NULL)
		{
			freefile = fileheadref;
			fileheadref =fileheadref->next;
			free(freefile);
		}
		*headref = (*headref)->next;
		free(freedir);
	}


}
///Return if link list exsists
int check_linklist(struct ndir_name** headref)
{
	return((*headref!=NULL));
}
//Funtion to list directories in give path "DIR_SERVER_PATH" 
void directorylisting2levels(struct ndir_name** ptr)
{
   struct dirent* dir,*file;
   DIR *db_dir,*client;
   int ret;
   char filepath[MAX_FILE_SIZE];
   char temp_filepath[MAX_FILE_SIZE];
   struct stat stat_buf;
   db_dir = opendir(g_server_config.db_dir);//DIR_SERVER_PATH);
   if (db_dir != NULL)
   {
	while(dir = readdir(db_dir))
	{
	   memset(filepath, 0, sizeof(filepath));
           if(dir->d_name[0]!='.')
	   {
		strcpy(filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
		strcat(filepath, dir->d_name);

		///GOT DIRECTORY NAME OR FOLDER NAME
		printf("%s\n",dir->d_name);
		
		insert_ndir_name(&(*ptr),dir->d_name);
		(*ptr)->FileCount = 0;
		
		client = opendir(filepath);
		if(client != NULL)
		{
		   while(file = readdir(client))
		   {
			memset(temp_filepath, 0, sizeof(temp_filepath));
			if((file->d_name[0]!='.') && 
				(strstr(file->d_name, ".txt") != NULL) &&
				(strstr(file->d_name, "~") == NULL))
			{									
			   strcpy(temp_filepath, g_server_config.db_dir);//DIR_SERVER_PATH);
			   strcat(temp_filepath, dir->d_name);
			   strcat(temp_filepath, "/");
			   strcat(temp_filepath, file->d_name);
			   //printf("\n%s", temp_filepath);
			   ret = stat(temp_filepath, &stat_buf);
			   if(ret == 0)
			   {
				printf("\t");
				insert_nfil_name(&((*ptr)->filenext), 
						file->d_name, 
						stat_buf.st_mtime);
                                (*ptr)->FileCount++;
				///GOT FILE NAME IN THAT FOLDER
				printf("%s\t%s\n",file->d_name, ctime(&stat_buf.st_mtime) );
			   }
			}	
		   }
		   closedir(client);
		}
		printf("\n");
	   }	
	}
	closedir(db_dir);
   }
   else
	printf("\n ERROR :: Invalid DB path");
}
#if 0
//Main function
void main()
{
int var=0;
struct ndir_name *ptr = NULL;
directorylisting2levels(&ptr);
printf("\nLinklist exsists?  %d\n",check_linklist(&ptr));
printf("\nlinklist done\n");
print_ndir_name(&ptr);
printf("\nlookup dir %d   %s",dir_lookup(&ptr,NULL,"sahil22"),"sahil22");
printf("\nlookup file %d",file_lookup(&ptr,"sahil22","file3"));
        printf("\ndelete linklist?");
        scanf("%d",&var);
        if(var==1)
        delete_list(&ptr);
        printf("\nLinklist exsists?  %d\n",check_linklist(&ptr));

}
#endif
