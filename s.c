//SENDER
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/msg.h>
#define MAX_TEXT 512
struct my_msg_st{
long int my_msg_type;
char some_text[MAX_TEXT];
};
int main()
{
int running=1;
struct my_msg_st some_data_bin;
struct my_msg_st some_data_oct;
struct my_msg_st some_data_hex;
int msgid;
char buffer[BUFSIZ];
msgid=msgget((key_t)1234,0666|IPC_CREAT);
if (msgid==-1) {
fprintf(stderr,"msgget failed with error: %d\n",errno);
exit(EXIT_FAILURE);
}
while(running){
printf("Enter the decimal no.: ");
fgets(buffer,BUFSIZ,stdin);
some_data_bin.my_msg_type=2;
some_data_oct.my_msg_type=8;
some_data_hex.my_msg_type=16;
if(strncmp(buffer,"end",3)!=0)
{
long long int bin;
//DEC TO BIN
int num=atoi(buffer);
int i=1;
while(num!=0) {
int rem=num%2;
num/=2;
bin+=rem*i;
i*=10;
}
sprintf(some_data_bin.some_text,"%lld",bin);
bin=0;long long int octal;
//DEC TO OCTAL
num=atoi(buffer);
i=1;
while(num!=0) {
int rem=num%8;
num/=8;
octal+=rem*i;
i*=10;
}
sprintf(some_data_oct.some_text,"%lld",octal);
octal=0;
//DEC TO HEX
char hexadecimalnum[100];
int decimalnum, quotient,remainder;
int k=0,j=0, l = 0;
char str[100];
quotient=atoi(buffer);
while(quotient!=0)
{
remainder=quotient%16;
if(remainder<10)
hexadecimalnum[j++]=48+remainder;
else
hexadecimalnum[j++]=55+remainder;
quotient=quotient/16;
}
int len = strlen(hexadecimalnum);
l = len;
for(k = 0; k <len ; k++)
{
str[l-1] = hexadecimalnum[k];
l--;
}
sprintf(some_data_hex.some_text,"%s",str);
}
else{
strcpy(some_data_bin.some_text,buffer);
strcpy(some_data_oct.some_text,buffer);
strcpy(some_data_hex.some_text,buffer);
}
if(msgsnd(msgid,(void*)&some_data_bin,MAX_TEXT,0)==-1){
fprintf(stderr,"msgsnd failed\n");
exit(EXIT_FAILURE);
}if(msgsnd(msgid,(void*)&some_data_oct,MAX_TEXT,0)==-1){
fprintf(stderr,"msgsnd failed\n");
exit(EXIT_FAILURE);
}
if(msgsnd(msgid,(void*)&some_data_hex,MAX_TEXT,0)==-1){
fprintf(stderr,"msgsnd failed\n");
exit(EXIT_FAILURE);
}
if(strncmp(buffer,"end",3)==0){
running=0;
}
}
exit(EXIT_SUCCESS);
}
