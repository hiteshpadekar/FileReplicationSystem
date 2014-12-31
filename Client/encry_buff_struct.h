#ifndef encry_buff_struct
#define encry_buff_struct
#define KEY 0xFACA

void encrypt(char *ptr,int size)/*encrypt((char *)&req,sizeof(req));*/;
void decrypt(char *ptr,int size)/*decrypt((char *)&req,sizeof(req));*/;
void encrypt_buff(char *ptr)/*encrypt_buff((char *)&name);*/;
void decrypt_buff(char *ptr)/*decrypt_buff((char *)&name);*/;

void encrypt(char *ptr,int size)//encrypt((char *)&req,sizeof(req));
{
char ptrval;
int index =0;
while(size!=0)
{
ptrval=*ptr;
ptrval=ptrval - KEY;
*ptr =ptrval;
ptr++;
size--;
}

}

void decrypt(char *ptr,int size)//decrypt((char *)&req,sizeof(req));
{
char ptrval;
int index =0;
while(size!=0)
{
ptrval=*ptr;
ptrval=ptrval + KEY;
*ptr =ptrval;
ptr++;
size--;
}

}

void encrypt_buff(char *ptr)//encrypt_buff((char *)&name);
{
char ptrval;
while(*ptr!='\0')
{
ptrval=*ptr;
ptrval=ptrval - KEY;
*ptr =ptrval;
ptr++;
}

}
void decrypt_buff(char *ptr)//decrypt_buff((char *)&name);
{
char ptrval;
while(*ptr!='\0')
{
ptrval=*ptr;
ptrval=ptrval + KEY;
*ptr =ptrval;
ptr++;
}

}

/*   ==================Uses===============
///please test it and check 
void main()
{
char name[] = "sahil";
req_t req;
req.type = REQ_INVALID;
req.len = 40;
printf("Size of req before = %d\n",sizeof(req));
 
printf("Before encryption values are \ntype=%d \nlen=%d\n",req.type,req.len);
encrypt((char *)&req,sizeof(req));//encryption call for post and get structures of VINAY
printf("After encryption values are \ntype=%d \nlen=%d\n",req.type,req.len);
printf("Size of req after = %d\n",sizeof(req));
decrypt((char *)&req,sizeof(req));//decrypt call for post and get structure of VINAY
printf("After decryption values are \ntype=%d \nlen=%d\n",req.type,req.len);
printf(" value before = %s\n",name);
encrypt_buff((char *)&name);//encrypt any buffer 
printf("Encrypted value = %s\n",name);
decrypt_buff((char *)&name);//decrypt any buffer
printf("Decrypted value = %s\n",name);

}*/
#endif // End of ency_buff_struct.h
