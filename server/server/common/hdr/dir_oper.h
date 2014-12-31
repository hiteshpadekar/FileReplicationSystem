#ifndef __DIR_OPER_H__
#define __DIR_OPER_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#include<time.h>


//#define DIR_SERVER_PATH "/home/hitesh/208/File_Replicate/"
#define MAX_FILE_SIZE 1024
#define SIZE_FILE_NAME 128

//Node for file name
struct nfil_name
{
   char 	fil_name[SIZE_FILE_NAME];
   int 		updateflag;
   time_t 	mtime;
   struct nfil_name *next;
};

//Node for directory name
typedef struct ndir_name
{
   char dir_name[SIZE_FILE_NAME];
   int updateflag;
   int FileCount;
   struct ndir_name *next;
   struct nfil_name *filenext;
}DIR_NAME;


int dir_lookup(struct ndir_name **headref,struct ndir_name **headptr, struct ndir_name **retPtr,char name[]);

//Function takes pointerof dir type,name of dir, filename-> look for file return 1 else 0
int file_lookup(struct ndir_name **headref, struct nfil_name **retPtr,char name[], char filename[] );

//Insert function for directories
void insert_ndir_name(struct ndir_name **refhead,char dir_name[SIZE_FILE_NAME]);

//Insert function for files
void insert_nfil_name(struct nfil_name **refhead,char fil_name[SIZE_FILE_NAME], time_t mtime);

//Print function for printing the whole list Takes pointer referance to main pointer
void print_ndir_name(struct ndir_name **refhead);


//Delets the whole list
void delete_list(struct ndir_name** headref);

///Return if link list exsists
int check_linklist(struct ndir_name** headref);

//Funtion to list directories in give path "DIR_SERVER_PATH" 
void directorylisting2levels(struct ndir_name** ptr);

#endif //__DIR_OPER_H__
