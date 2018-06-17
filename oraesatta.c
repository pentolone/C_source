#ifndef LINT
static char sccs_ora_id[] = "Module @(#)oraesatta.c Version 1.4 Date 00/10/19 Time 10:08:44";
#endif
/*-----------------------------------------------------------------------------

	oraesatta.c : funzione per la visualizzazione dell'ora corrente

	              Viene richiamata dai programmi Informix 4GL

	              Parametri in input: nessuno

-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
int oraesatta(void)
{
 char   tmp[5];
 struct tm *dls_tm;

 time_t time(time_t *);
 time_t ora = (time_t)NULL;
/*
 struct tm *localtime(time_t *);
*/ 

   ora = time((time_t *)NULL);
   dls_tm = localtime(&ora);
   (void)strftime(tmp,sizeof(tmp),"%H%M",dls_tm);
   retquote(tmp);
   return(1);
}
