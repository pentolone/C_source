#ifndef LINT
static char sccs_help_id[] = "Module @(#)dls_help.c Version 1.2 Date 00/02/21 Time 11:48:09";
#endif
/*-----------------------------------------------------------------------------

	dls_help.c : funzione per la lettura degli help dei programmi

	             Viene richiamata dai programmi Informix 4GL

	             Parametri in input : codice di help
	             Parametri in output:  0 = OK
                                           1 = Fine file
	                                  -1 = KO

-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

 typedef struct aiuto {
		       char testo[81];
                       unsigned short riga;
		       struct aiuto *indietro;
		       struct aiuto *avanti;
		      } AIUTO;
 AIUTO *primo = (AIUTO *)NULL;
 AIUTO *pp;
 AIUTO *tmp;

int dls_help(void)
{
 int codice_help;
 char g_ambiente[256];
 char g_file[256];
 char *EnvVar,*getenv(char *);
 FILE *fd = (FILE *)NULL;

 popint(&codice_help);

 if(primo == (AIUTO *)NULL) {
    memset((void *)g_ambiente,'\0',sizeof(g_ambiente));

    EnvVar = getenv("DBHELP");

    if(EnvVar == (char *)NULL)
       strcpy(g_ambiente,".");
    else
       strcpy(g_ambiente,EnvVar);

    sprintf(g_file,"%s/%d",g_ambiente,codice_help);

    if((fd = fopen(g_file,"r")) == (FILE *)NULL) {
       retquote(" ");
       retint(-1);
       return(2);
       }

    if((pp = (AIUTO *)malloc(sizeof(AIUTO))) == (AIUTO *)NULL) {
       retquote(" ");
       retint(-1);
       return(2);
       }
    primo = pp;
    tmp   = pp;
    pp->riga     = 1;
    pp->avanti   = (AIUTO *)NULL;
    pp->indietro = (AIUTO *)NULL;
    while(fgets(pp->testo,80,fd) != (char *)NULL) {
          if((pp = (AIUTO *)malloc(sizeof(AIUTO))) == (AIUTO *)NULL) {
             (void)dls_libera();
             fclose(fd);
             retquote(" ");
             retint(-1);
             return(2);
             }
          pp->riga = tmp->riga + 1;
          pp->indietro = tmp;
          pp->avanti   = (AIUTO *)NULL;
          tmp->avanti  = pp;
          tmp = pp;
        }
    fclose(fd);
    pp = primo;
   }
 while(pp->avanti != (AIUTO *)NULL) {
       retquote(pp->testo);
       pp = pp->avanti;
       retint(0);
       return(2);
      }
 (void)dls_libera();
 retquote(" ");
 retint(1);
 return(2);
}

int dls_libera(void)
/*-----------------------------------------------------------------------------

	dls_libera: funzione che libera la memoria allocata


-----------------------------------------------------------------------------*/
{
 AIUTO *tmp;
 pp  = primo;
 tmp = primo;
 while((tmp = pp->avanti) != (AIUTO *)NULL) {
       free(pp);
       pp = tmp;
       }
 free(pp);
 primo = (AIUTO *)NULL;
 return(0);
}

