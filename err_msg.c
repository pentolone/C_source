#ifndef LINT
static char sccs_err_id[] = "Module @(#)err_msg.c Version 1.5 Date 94/10/14 Time 17:03:56";
#endif
/*-----------------------------------------------------------------------------

	err_msg.c : funzione per la lettura del file di messaggi

	            Viene richiamata dai programmi Informix 4GL

	            Parametri in input: nome programma,
					stringa di errore

-----------------------------------------------------------------------------*/
#include <stdio.h>
#define FILE_ERR_DEF  "file_msg.iem"  /* file messaggi/errori di default     */
#define MAXLINEA 256
int err_msg(void)
{
 int i,x;
 int fine=0;
 char np[81];                /* parametro in input: nome del programma   */
 char par1[81];              /* parametro in input: codice d'errore      */
 char mess[81];              /* parametro di output : messaggio d'errore */
 char linea_letta[MAXLINEA]; /* linea letta dal file di messaggi/errori  */
 char nome_file[15];         /* nome del file messaggi/errori            */
 char opn_fal[30];           /* messaggio di fallimento apertura file    */
 char nome_prog[81];
 char messaggio[81];
 char *punta;
 char *EnvVar;
/*
 FILE *fopen(char *,char *),*fp;
*/
 FILE *fp;
 char *getenv(char *);
 char *strrchr(char *,int);
 char *fgets(char *,int,FILE *);
 void *memset(void *,int,size_t);

    memset((void *)par1,'\0',sizeof(par1));
    memset((void *)np,'\0',sizeof(np));

    /***********************************
     lettura dei parametri dallo stack
    ***********************************/

    popquote(par1,sizeof(par1));
    popquote(np,sizeof(np));
    memset((void *)mess,' ',sizeof(mess));

    /*****************************************
     definizione del file di errori/messaggi 
    *****************************************/

    EnvVar = getenv("DBERR");

    if(EnvVar == (char *)NULL)
       strcpy(nome_file,FILE_ERR_DEF);
    else
       strcpy(nome_file,EnvVar);

    /********************************************
      eliminazione dei blanks non significativi 
    ********************************************/

    if( np[strlen(np) -1] == ' ') {
        punta = &np[strlen(np) - 1];
        while(*punta == ' ')
	      punta--;
        punta++;
        *punta = '\0';
    }

    if( par1[strlen(par1) -1] == ' ') {
        punta = &par1[strlen(par1) - 1];
        while(*punta == ' ')
	      punta--;
        punta++;
        *punta = '\0';
    }

    fine=0;
    i=0;


    /************************************
      apertura del file errori/messaggi
    ************************************/

    if((fp = fopen(nome_file,"r")) != (FILE *)NULL) {
        memset((void *)linea_letta,'\0',sizeof(linea_letta));
        while((fgets(linea_letta,MAXLINEA,fp) != (char *)NULL) && 
               fine                           == 0               ) {
	       i=0;

               if (sscanf(linea_letta,"%[^|]|%[^|]|%[^|]|\n",
                                      nome_prog,messaggio,mess) == 3) {

                   /**************************************************
                     confronto tra parametri e stringhe identificate
                   **************************************************/

	           if((strcmp(np  ,nome_prog) == 0 ) && 
                      (strcmp(par1,messaggio) == 0 )    ) {
                       /********************
                         messaggio Trovato
                       ********************/

                       fine = 1;

                   }
	           else { 
                       /**********************************************
                         pulizia stringhe nel caso di esito negativo
                         dal confronto precedente
                       **********************************************/
    
		       memset((void *)linea_letta,' ',sizeof(linea_letta));
		       memset((void *)mess,' ',sizeof(mess));
                       memset((void *)nome_prog,' ',sizeof(nome_prog));
                       memset((void *)messaggio,' ',sizeof(messaggio));
	           }
               }
	}
	fclose(fp);
    }
    else {
        /*******************************************************************
          assegnazione del messaggio di errore in caso di fallita apertura
          file errori/messaggi
        *******************************************************************/
	strcpy(opn_fal,"Error opening ");
	strcat(opn_fal,nome_file);
	strcpy(mess,opn_fal);
    }

    /**************************
      restituzione parametro
    **************************/
    retquote(mess);
    return(1);
}
