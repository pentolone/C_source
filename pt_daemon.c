#ifndef LINT
static char sccs_pt_daemon_id[] = "%Z%%M% Version %I% Date %D% Hour %T%";
#endif
/*---------------------------------------------------------------------------

	pt_daemon.c:  funzioni di lettura/scrittura delle code di messaggi
		      gestite dal pt_attiva.c per Link S.I.A. <-> GuestLink

	(incluse nel programma fglgo di Informix)

---------------------------------------------------------------------------*/
#include "paytv.h"
#include <sys/wait.h>
#include <pwd.h>

pid_t dbpid;
unsigned int time_to_sleep=80;
int *sia_status;

int dls_readq(void)
/*---------------------------------------------------------------------------

	dls_readq: funzione che, leggendo la coda di messaggi ritorna
                   la stringa e lo stato di ritorno al programma 4GL

---------------------------------------------------------------------------*/
{
int pt_qr;
char pt_stringa[MAXPTLEN];
struct pt_msg {
		long mtype;
		char mtext[MAXPTLEN];
	        };
struct pt_msg pt_msgstruct;


   if((pt_qr = msgget((key_t)PT_MSGQR,0660)) == ERROR) {
      perror("Can't access message queue ID (READ)");
      retquote("Can't access message queue ID (READ)");
      retint(ERROR);
      return(2);
    }

    if(msgrcv(pt_qr,(void *)&pt_msgstruct,sizeof(pt_stringa),0L,0) == ERROR) {
       perror("Can't receive message queue (READ)");
       retquote("Fatal error while receiving !");
       retint(ERROR);
       return(2);
      }
      /*
       * messaggio ricevuto correttamente !
       */

    strcpy(pt_stringa,pt_msgstruct.mtext);
    retquote(pt_stringa);
    retint(FALSE);
    return(2);
}

int dls_writeq(void)
/*---------------------------------------------------------------------------

	dls_writeq: funzione che, prendendo in input la stringa e
                    la sua lunghezza, spedisce in message queue
                    Ritorna lo stato dell'operazione

---------------------------------------------------------------------------*/
{
int pt_qw,lunghezza;
char pt_stringa[MAXPTLEN];
struct pt_msg {
		long mtype;
		char mtext[MAXPTLEN];
	        };
struct pt_msg pt_msgstruct;

   popint(&lunghezza);
   popquote(pt_stringa,sizeof(pt_stringa));

   pt_stringa[lunghezza]='\0';

   if((pt_qw = msgget((key_t)PT_MSGQW,0660)) == ERROR) {
      perror("Can't access message queue ID (WRITE)");
      retint(ERROR);
      return(1);
    }
    pt_msgstruct.mtype = 1L;
    (void)strcpy(pt_msgstruct.mtext,pt_stringa);

    if(msgsnd(pt_qw,(void *)&pt_msgstruct,sizeof(pt_stringa),0) == ERROR) {
       perror("Can't send message queue (WRITE)");
       retint(ERROR);
       return(1);
      }
      /*
       * messaggio spedito correttamente !
       */

    retint(FALSE);
    return(1);
}

int dls_setshm(void)
/*---------------------------------------------------------------------------

	dls_setshm: funzione che, setta la Shared Memory a TRUE o FALSE
                    per disattivazione/attivazione processo PAY TV

        Input: valore (TRUE o FALSE)

---------------------------------------------------------------------------*/
{
int stato,*sia_status,pt_shmid, pt_qr;
struct pt_msg {
		long mtype;
		char mtext[MAXPTLEN];
	       };
struct pt_msg pt_msgstruct;
char *pt_pointer;
int ptshm,pt_ute_id;
struct sembuf pt_operation={0, ERROR, SEM_UNDO};

   popint(&stato);

   if((pt_shmid = shmget((key_t)PT_SHM,sizeof(int),0660)) == ERROR) {
      perror("Cannot access shared memory (LOCK)");
      return(0);
      }
      /*
       * Carico Shared Memory
       */

   if((pt_pointer = shmat(pt_shmid,(char *)0,0)) == (char *)ERROR) {
      perror("Cannot attache shared memory segment to my process (LOCK)");
      return(0);
      }

    ptshm = semget((key_t)PT_SEM,1,0660);
   (void)semop(pt_ute_id,&pt_operation,1);
    sia_status = (int *)pt_pointer;
    *sia_status = stato;
   (void)semctl(ptshm,0,SETVAL,1);

    if((pt_qr = msgget((key_t)PT_MSGQR,0660)) == ERROR) {
       perror("Can't access message queue ID (LOCK)");
       return(0);
       }

    pt_msgstruct.mtype = 1L;

    if(stato == TRUE)
       (void)strcpy(pt_msgstruct.mtext,"LOCK BY SIA");
    else
       (void)strcpy(pt_msgstruct.mtext,"UNLOCK BY SIA");

    if(msgsnd(pt_qr,(void *)&pt_msgstruct,sizeof(pt_msgstruct.mtext),0) == ERROR) {
       perror("Can't send message queue (LOCK)");
       return(0);
      }
    return(0);
}

/*---------------------------------------------------------------------------

	leggi_seq: lettura di una singola riga da un file line-sequential

	Parametri in input (da programma INFORMIX-4GL): PathFile,
                                                        OperationCode

	Parametri ritornati al programma 4GL: stringa letta + return code
					      0 = OK !
					      1 = Fine file
					     -1 = Errore fatale !

---------------------------------------------------------------------------*/
int leggi_seq(void)
{
static FILE *pointer=(FILE *)NULL;
char   nomefile[80];
char   tipo_estrazione[80];
char   stringa[257];
int    ind=0;
char   *debug;
char   *getenv();

     debug = getenv("DBDBG");

     popquote(tipo_estrazione,sizeof(tipo_estrazione));

     if(debug != NULL )
        fprintf(stderr,"tipo_estrazione %s\n",tipo_estrazione);

     popquote(nomefile,sizeof(nomefile));

     for ( ind=strlen(nomefile)-1; ind > 0 ; ind--) {
	   if ( nomefile[ind] == ' ' )
		nomefile[ind]='\0';
	   else
		break;
     }

     for ( ind=strlen(tipo_estrazione)-1; ind > 0 ; ind--) {
	   if ( tipo_estrazione[ind] == ' ' )
		tipo_estrazione[ind]='\0';
	   else
		break;
     }

     if(debug != NULL )
        fprintf(stderr,"nome_file <%s>\n",nomefile);

     if ( strcmp(tipo_estrazione, "OPEN") == 0) {
	  if ( pointer != (FILE *)NULL)
	       fclose(pointer);
          if ((pointer = fopen(nomefile,"r")) == (FILE *)NULL) {
	       if ( debug != NULL )
	            perror("Non riesco ad aprire il file causa ");

	       retquote("");
	       retint(-1);
	       return(2);
	  }

	  if ( debug != NULL )
	       fprintf(stderr,"Ho aperto il file %s\n",nomefile);

	  retquote("");
	  retint(0);
	  return(2);
     }

     if ( strcmp(tipo_estrazione, "READ") == 0) {
	  if ( pointer == (FILE *)NULL) {
	       if ( debug != NULL )
		    fprintf(stderr,"Comando errato: file non aperto\n");
	       retquote("");
	       retint(-1);
	       return(2);
	  }
	  if ( fgets(stringa,256,pointer) == (char *)NULL ) {
	       if ( debug != NULL )
		    fprintf(stderr,"EOF od ERRORE: chiudo il file\n");
	       fclose(pointer);
	       retquote("");
	       retint(1);
	       return(2);
	  }
	  else {
	       if ( debug != NULL )
		    fprintf(stderr,"Stringa letta:.%s.\n",stringa);
	       retquote(stringa);
	       retint(0);
	       return(2);
	  }
     }

     if ( debug != NULL )
	  fprintf(stderr,"Comando NON riconosciuto!\n");
     retquote("");
     retint(-1);
     return(2);

}
