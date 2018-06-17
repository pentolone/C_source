#ifndef LINT
static char sccs_ter_id[] = "Module @(#)terminale.c Version 1.3 Date 94/01/31 Time 14:52:20";
#endif
/*-----------------------------------------------------------------------------

	terminale.c : funzione che ritorna il nome del terminale

	              Viene richiamata dai programmi Informix 4GL

	              Parametri in input: nessuno

-----------------------------------------------------------------------------*/
#include <string.h>
#include <sys/ioctl.h>

int terminale(void)
{
 char str1[80];
 char str2[80];
 char *punta;
 int  i = 0;
 char *ttyname(int);  
 void *memset(void *,int,size_t);

   memset(str1,'\0',sizeof(str1));
   memset(str2,'\0',sizeof(str2));

   strcpy(str1,ttyname(0));

   punta = strrchr(str1,'/');
   punta++;

   while(*punta != '\0' && *punta != ' ') {
	 str2[i] = *punta;
	 i++;
	 punta++;
	 }
   retquote(str2);
   return(1); 
}
