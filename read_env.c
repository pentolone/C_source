#ifndef LINT
static char sccs_read_id[] = "Module @(#)read_env.c Version 1.3 Date 94/01/31 Time 14:50:23";
#endif
/*-----------------------------------------------------------------------------

	read_env.c : funzione per la lettura delle variabili di ambiente

	             Viene richiamata dai programmi Informix 4GL

	             Parametri in input: variabile da leggere (ex. DBSOC)

-----------------------------------------------------------------------------*/
#include <string.h>
int read_env(void)
{
 char g_ambiente[256];
 char *EnvVar;
 int  i = 254;
 char *getenv(char *);
 void *memset(void *,int,size_t);

 memset((void *)g_ambiente,'\0',sizeof(g_ambiente));
 popquote(g_ambiente,sizeof(g_ambiente));

 while(g_ambiente[i] == ' ' && i >= 0)
       i--;
 i++;
 g_ambiente[i] = '\0';
 EnvVar = getenv(g_ambiente);

 if(EnvVar == (char *)NULL)
    g_ambiente[0] = '\0';
 else
    strcpy(g_ambiente,EnvVar);

 retquote(g_ambiente);
 return(1);
}
