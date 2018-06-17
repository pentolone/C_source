#ifndef LINT
static char sccs_put_id[] = "Module @(#)put_env.c Version 1.6 Date 00/11/08 Time 11:10:00";
#endif
/*-----------------------------------------------------------------------------

	put_env.c : funzione per il cambiamento delle variabili di ambiente

	            Viene richiamata dai programmi Informix 4GL

	            Parametri in input: stringa (ex. DBSOC='0379'),
			                lunghezza della stringa
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
int put_env(void)
{
 long i;
 int  j;
 int  conta;
 char p_ambiente[256];
 char *punta,b[128],c[128];
 int stato = 0;
#ifdef ULTRIX
 int setenv(char *,char *,int);
#else
 int putenv(char *);
#endif

 void *memset(void *,int,size_t);

   memset((void *)p_ambiente,'\0',sizeof(p_ambiente));
   poplong(&i);
#ifdef ULTRIX
   popquote(p_ambiente,i);
#else
   punta = malloc(sizeof(char) * i);
   popquote(punta,i);
#endif

#ifdef ULTRIX
   sscanf(p_ambiente,"%[^=]=%s",b,c);

   if((punta = strchr(p_ambiente,'=')) != (char *)NULL) {
      punta++;
      stato = setenv(b,punta,1);
     }
   else
     stato = -1;
#else
   j = strlen(punta);

   if (punta[j]==' '){ 
       punta[j]='\0';}

   stato = putenv(punta);
#endif
   retint(stato);
   return(1);
}
