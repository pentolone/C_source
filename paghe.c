#ifndef LINT
static char sccs_id[] = "Module @(#)paghe.c Version 1.2 Date 00/02/21 Hour 11:53:39";
#endif
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#define S_MOVCON 1
#define S_SIPMOV 2
#define TRPACG   3
#define DBSEP '|'
main(int argc,char *argv[])
{
char i_file[256];
char o_file[256];
char tmp[256];
char c;
signed int fd = 0;
unsigned short tipo = TRPACG,lunghezza = 149;
signed short inx,inx1;
FILE *fdout;

   strcpy(i_file,"/usr/apri/fls/si/1/TRPACG.D");
   strcpy(o_file,"/usr/users/PAG/TRPACG.unl");

   if((fd = open(i_file,O_RDONLY)) == -1) {
     sprintf(tmp,"Can't open file %s reason",i_file);
     perror(tmp);
     exit(1);
     }

   if((fdout = fopen(o_file,"w")) == (FILE *)NULL) {
     sprintf(tmp,"Can't open file %s reason",o_file);
     perror(tmp);
     close(fd);
     exit(1);
     }

   memset(tmp,'\0',sizeof(tmp));
   inx1 = inx = 0;

   while(read(fd,&c,1) > 0) {

         tmp[inx1] = c;

         switch(tipo) {
                case S_MOVCON:
                              if(inx == lunghezza) {
                                 inx1++;
                                 tmp[inx1] = DBSEP;
                                 inx1++;
                                 tmp[inx1] = '\n';
                                 inx1++;
                                 tmp[inx1] = '\0';
                                 fprintf(fdout,"%s",tmp);
                                 inx1 = inx = -1;
                               }
                              if(inx == 4 || inx == 16 || inx == 29 ||
                                 inx == 30 || inx == 42 || inx == 47 ||
                                 inx == 52 || inx == 54 || inx == 58 ||
                                 inx == 59 || inx == 60 || inx == 64 ||
                                 inx == 65 || inx == 68 || inx == 69 ||
                                 inx == 73 || inx == 75 || inx == 76 ||
                                 inx == 79) {
                                 inx1++;
                                 tmp[inx1] = DBSEP;
                                 }
                              break;
                case S_SIPMOV:
                              if(inx == lunghezza) {
                                 inx1++;
                                 tmp[inx1] = DBSEP;
                                 inx1++;
                                 tmp[inx1] = '\n';
                                 inx1++;
                                 tmp[inx1] = '\0';
                                 fprintf(fdout,"%s",tmp);
                                 inx1 = inx = -1;
                               }
                              if(inx == 4 || inx == 16 || inx == 29 ||
                                 inx == 30 || inx == 42 || inx == 47 ||
                                 inx == 52 || inx == 54 || inx == 58 ||
                                 inx == 59 || inx == 60 || inx == 64 ||
                                 inx == 65 || inx == 68 || inx == 69 ||
                                 inx == 73 || inx == 75 || inx == 76 ||
                                 inx == 79) {
                                 inx1++;
                                 tmp[inx1] = DBSEP;
                                 }
                              break;
                case TRPACG:
                              if(inx == lunghezza) {
                                 inx1++;
                                 tmp[inx1] = DBSEP;
                                 inx1++;
                                 tmp[inx1] = '\n';
                                 inx1++;
                                 tmp[inx1] = '\0';
                                 fprintf(fdout,"%s",tmp);
                                 inx1 = inx = -1;
                               }
                              if(inx == 0 || inx == 6 || inx == 8 ||
                                 inx == 12 || inx == 14 || inx == 26 ||
                                 inx == 38 || inx == 51 || inx == 63 ||
                                 inx == 64 || inx == 67 || inx == 72 ||
                                 inx == 92 || inx == 94 || inx == 99 ||
                                 inx == 103 || inx == 109 || inx == 112 ||
                                 inx == 113 || inx == 114 || inx == 133 ) 
				 {
                                 inx1++;
                                 tmp[inx1] = DBSEP;
                                 }
                              break;
                      }
         inx++;
         inx1++;
        }
   close(fd);
   fclose(fdout);
   exit(0);
}
