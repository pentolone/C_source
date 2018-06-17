#ifndef LINT
static char sccs_bx_fun_id[] = "@(#)bx_function.c Version 1.10 Date 97/01/15 Hour 09:39:07";
#endif
/*---------------------------------------------------------------------------

	apri_porta.c:  funzione di apertura device e parametrizzazione
		       ambiente per la gestione della documentazione addebiti

	Parametri in input (da programma INFORMIX-4GL):

		  - Numero centralino (1-x); questo per permettere
		    la gestione di piu' centralini collegati all'elaboratore

		  - Device da aprire (ex. /dev/tty12, tty18 etc.)
		  - Velocita' di set porta 9600,1200... (BPS)
		  - Bit di data (7,8)
		  - Bit di stop (1,2)
		  - Parita`     (N = none,
				 E = even,
				 O = odd)
		  - Valorizzazione Shared Memory (1 = Si
						  0 = No)

---------------------------------------------------------------------------*/
#include "dls_include.h"
unsigned short BccNullo = FALSE;

int apri_porta(void)
{
#ifdef ULTRIX	/* Only for Ultrix operating system	*/
int ldisc = TERMIODISC;
#endif

struct termio dls_termio;
unsigned short usa_shm;

char tmp[20],*kk;

   if((dls_device=(IOCONTROL *)malloc(sizeof(IOCONTROL)))==(IOCONTROL *)NULL) {
       dls_error("Can't allocate memory",(char *)NULL);
       retint(ERROR);
       return(1);
       }

   /*
    *	Parametri in input (da programma INFORMIX-4GL RDS)
    */

   popshort(&usa_shm);
   popquote(dls_device->dls_parity,sizeof(dls_device->dls_parity));
   popshort(&dls_device->dls_bitstop);
   popshort(&dls_device->dls_bitdata);
   popint(&dls_device->dls_speed);
   popquote(dls_device->dls_tty,sizeof(dls_device->dls_tty));
   popshort(&dls_device->dls_npabx);

   if(dls_device->dls_npabx > MAXPABX) {
      fprintf(stderr,"Number of pabx %d exceeded MAX(%d)\n",dls_device->dls_npabx,MAXPABX);
      retint(ERROR);
      return(1);
      }
   else
      shm_idx = dls_device->dls_npabx;

   shm_idx--;

  /*
   * Elimino blank nelle stringhe (se presenti)
   */

   if((kk = strchr((char *)dls_device->dls_tty,' ')) != (char *)NULL)
      *kk = '\0';

   if((kk = strchr((char *)dls_device->dls_tty,' ')) != (char *)NULL)
      *kk = '\0';

   (void)strcpy(tmp,(char *)dls_device->dls_tty);

   /*
    * Aggiungo suffisso "/dev/" se non presente nel parametro
    */

   if((strstr((char *)dls_device->dls_tty,"/dev/")) == (char *)NULL)
      (void)sprintf((char *)dls_device->dls_tty,"/dev/%s",tmp);

   /*
    * Apertura porta per lettura sospensiva
    */

   if((dls_fdtty = open((char *)dls_device->dls_tty,O_RDWR)) == ERROR) {
       dls_error("Can't open device",(char *)dls_device->dls_tty);
       retint(ERROR);
       return(1);
       }

#ifdef ULTRIX
if(ioctl(dls_fdtty, TIOCSETD, &ldisc) == ERROR) {
   dls_error("Can't set line discipline (ULTRIX) for",(char *)dls_device->dls_tty);
   retint(ERROR);
   return(1);
   }
#endif

   /*
    * Apertura porta OK !
    * Configuro il device con i parametri ricevuti (velocita', parita', ecc..)
    */

    if ( ioctl(dls_fdtty,TCGETA,&dls_copia) == ERROR ) {
	 dls_error("Can't get terminal characteristics for",(char *)dls_device->dls_tty);
	 retint(ERROR);
	 return(1);
    }

    memcpy((void *)&dls_termio,(void *)&dls_copia,sizeof(struct termio));

    dls_termio.c_cflag &= ~CBAUD;
    dls_termio.c_cflag &= ~CSIZE;
    dls_termio.c_iflag &= ~IXANY;
    dls_termio.c_iflag &= ~IXOFF;
    dls_termio.c_iflag &= ~IXON;
    dls_termio.c_iflag |= IXON | IXOFF;

    switch(dls_device->dls_speed) {

			   case 50:
                                    dls_termio.c_cflag |=  B50;
				    break;
			   case 75:
                                    dls_termio.c_cflag |=  B75;
				    break;
			   case 110:
                                    dls_termio.c_cflag |=  B110;
				    break;
			   case 134:
                                    dls_termio.c_cflag |=  B134;
				    break;
			   case 150:
                                    dls_termio.c_cflag |=  B150;
				    break;
			   case 200:
                                    dls_termio.c_cflag |=  B200;
				    break;
			   case 300:
                                    dls_termio.c_cflag |=  B300;
				    break;
			   case 600:
                                    dls_termio.c_cflag |=  B600;
				    break;
			   case 1200:
                                    dls_termio.c_cflag |=  B1200;
				    break;
			   case 2400:
                                    dls_termio.c_cflag |=  B2400;
				    break;
			   case 4800:
                                    dls_termio.c_cflag |=  B4800;
				    break;
			   case 9600:
                                    dls_termio.c_cflag |=  B9600;
				    break;
			   case 19200:
                                    dls_termio.c_cflag |=  B19200;
				    break;
			   default:
				    fprintf(stderr,"Invalid speed -> %d\n",dls_device->dls_speed);
				    retint(ERROR);
				    return(1);
				    break;
                                  }
   /*
    *	Set parita'
    */

    switch(dls_device->dls_parity[0]) {

			   case 'E':
                                    dls_termio.c_cflag &=  ~PARODD;
                                    dls_termio.c_cflag |=  PARENB;
				    break;

			   case 'N':
                                    dls_termio.c_cflag &=  ~PARENB;
                                    dls_termio.c_cflag &=  ~PARODD;
				    break;

			   case 'O':
                                    dls_termio.c_cflag |=  PARENB;
                                    dls_termio.c_cflag |=  PARODD;
				    break;
			   default:
				    fprintf(stderr,"Invalid parity -> %s\n",dls_device->dls_parity);
				    retint(ERROR);
				    return(1);
				    break;
                                  }

   /*
    *	Set bit di data
    */

    switch(dls_device->dls_bitdata) {

			   case 7:
                                    dls_termio.c_cflag |=  CS7;
				    break;
			   case 8:
                                    dls_termio.c_cflag |=  CS8;
				    break;
			   default:
				    fprintf(stderr,"Invalid bit data -> %d\n",dls_device->dls_bitdata);
				    retint(ERROR);
				    return(1);
				    break;
                                  }
   /*
    *	Set bit di stop
    */

    switch(dls_device->dls_bitstop) {

			   case 1:
                                    dls_termio.c_cflag &=  ~CSTOPB;
				    break;
			   case 2:
                                    dls_termio.c_cflag |=  CSTOPB;
				    break;
			   default:
				    fprintf(stderr,"Invalid bit stop -> %d\n",dls_device->dls_bitstop);
				    retint(ERROR);
				    return(1);
				    break;
                                  }

    dls_termio.c_lflag &= ~ECHO;
    dls_termio.c_lflag &= ~ICANON;
    dls_termio.c_cc[VTIME] = 0;
    dls_termio.c_cc[VMIN] = 1;

    if ( ioctl(dls_fdtty,TCSETA,&dls_termio) == ERROR ) {
	 dls_error("Can't set terminal characteristics for",(char *)dls_device->dls_tty);
	 retint(ERROR);
	 return(1);
    }

    /*
     * Settaggio porta OK !
     * Creo il semaforo per la sincronizzazione dell'accesso alla shared Memory
     * (se utilizzata)
     */

    if(usa_shm == TRUE) {
       semshm = semget((key_t)SEMSHM,1,0660);

       if(semshm == ERROR) {
          dls_error("Cannot get semaphore","SHM");
          retint(ERROR);
          return(1);
         }
       }

   /*
    * Creazione semafori OK !
    * Accedo Shared Memory
    * (se richiesto)
    */

    if(usa_shm == TRUE) {
       if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0660)) == ERROR) {
           dls_error("Cannot access shared memory",(char *)NULL);
           dls_sem_unlock(SEMSHM);
           retint(ERROR);
           return(1);
           }
   /*
    * Carico Shared Memory
    * (se richiesto)
    */

       if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
          dls_error("Cannot attache shared memory segment to my process",(char *)NULL);
          retint(ERROR);
          return(1);
          }

       dls_shm_pointer = (DLS_IPC *)shm_pointer;

       dls_shm_pointer->dls_pabx1[shm_idx].start1  = time((time_t *)NULL);
       dls_shm_pointer->dls_pabx1[shm_idx].status1 = CONNECTING;
       (void)strcpy((char *)dls_shm_pointer->dls_pabx1[shm_idx].oper1,"Creation shared memory");
       dls_shm_pointer->dls_pabx1[shm_idx].lastsend = (time_t)NULL;
       dls_shm_pointer->dls_pabx1[shm_idx].msgsend  = 0L;
       dls_shm_pointer->dls_pabx1[shm_idx].dls_pid1 = getpid();

   /*
    * Creazione Shared Memory OK !
    * Creo Coda di Messaggi
    * (se richiesto)
    */

       dls_sem_unlock(SEMSHM);

       if((dls_msgid = msgget((key_t)MSGKEY,0660)) == ERROR) {
	    dls_error("Can't access message queue ID",(char *)NULL);
	    retint(ERROR);
	    return(1);
	    }

       dls_sem_lock(SEMSHM);
       (void)strcpy((char *)dls_shm_pointer->dls_pabx1[shm_idx].oper1,"Accessing message queue");
       dls_sem_unlock(SEMSHM);
     } /* fine if(usa_shm == TRUE)... */

   /*
    * Tutto OK !
    * Ritorno 0 a 4GL (SUCCESSFUL !)
    */

    retint(0);
    return(1);
}

int dls_read(void)
/*---------------------------------------------------------------------------

	dls_read: routine di gestione della lettura della porta in uso

	Parametri in input: terminatore di riga (di solito '\n')

	Parametri in output: stringa letta

---------------------------------------------------------------------------*/
{
int line_terminator;
char c,ret_buf[MAXSTRLEN];
int  i = FALSE;
struct dls_msg {
		long mtype;
		char mtext[MAXSTRLEN];
	        };
struct dls_msg dls_msgstruct;

  /*
   * Terminatore di stringa passato da programma 4GL
   * DEFAULT = '\n' (LineFeed) ASCII(10)
   */

   popint(&line_terminator);

   dls_setsig(1);

   if(line_terminator == 0)
      line_terminator = 10;	/* valore ASCII di line feed ('\n') */

   memset((void *)ret_buf,'\0',sizeof(ret_buf));

   dls_sem_lock(SEMSHM);
   dls_shm_pointer->dls_pabx1[shm_idx].status1 = RUNNING;
   dls_sem_unlock(SEMSHM);

   (void)strcpy((char *)dls_shm_pointer->dls_pabx1[shm_idx].oper1,"Reading tty");
   while(read(dls_fdtty,&c,1) > 0) {

	 if(c == line_terminator || i == (MAXSTRLEN - 1)) {	
					   /* raggiunto il termine di stringa */
	    if( i <= 10 ) {	/* se ho letto 10 o meno caratteri riciclo */
	        i = FALSE;
		continue;
		}

            if(i == (MAXSTRLEN - 1))
	       ret_buf[i] = '\0';
               
	    dls_msgstruct.mtype = 1L;

	    i = shm_idx + 1;

	    (void)sprintf(dls_msgstruct.mtext,"%02d",i);
	    (void)strcat(dls_msgstruct.mtext,ret_buf);

	   /*
	    * Blocco il semaforo per 1^ aggiornamento Shared Memory
	    */

	    dls_sem_lock(SEMSHM);
            (void)strcpy((char *)dls_shm_pointer->dls_pabx1[shm_idx].oper1,"Wait to send message");
	    dls_sem_unlock(SEMSHM);
	    if(msgsnd(dls_msgid,(void *)&dls_msgstruct,sizeof(ret_buf),0) == ERROR) {
               dls_error("Can't send message queue",(char *)NULL);
	       dls_abort(1);
	       return(0);
	       }

	   /*
	    * Blocco il semaforo per 2^ aggiornamento Shared Memory
	    */
	    dls_sem_lock(SEMSHM);
	    (void)strcpy((char *)dls_shm_pointer->dls_pabx1[shm_idx].oper1,"Message sent !");
            dls_shm_pointer->dls_pabx1[shm_idx].msgsend++;

            dls_shm_pointer->dls_pabx1[shm_idx].lastsend = time((time_t *)NULL);
            memset((void *)ret_buf,'\0',sizeof(ret_buf));
	    i = FALSE;
	    (void)strcpy((char *)dls_shm_pointer->dls_pabx1[shm_idx].oper1,"Reading tty");
	   /*
	    * Libero il semaforo
	    */
	    dls_sem_unlock(SEMSHM);
	    continue;
	    }
	 ret_buf[i] = c;
         if(ret_buf[i] == '\0')
            continue;
	 i++;
	}
   /*
    * Uscito per errore fatale in lettura porta ! (EOF ?)
    * Abort del processo
    */

   (void)fprintf(stderr,"Fatal ERROR while reading: closing device\n");
   dls_abort(1);
   return(0);
}

int leggi_coda(void)
/*---------------------------------------------------------------------------

	leggi_coda: lettura della coda di messaggi

	Parametri in input (da programma INFORMIX-4GL): nessuno

	Parametri ritornati al programma 4GL: stringa letta + return code
					      0 = OK !
					      1 = Database in accesso esclusivo
					     -1 = Errore fatale !

---------------------------------------------------------------------------*/
{
    char ret_buf[MAXSTRLEN];
    char tmp[81];
    struct dls_msg {
		    long mtype;
		    char mtext[MAXSTRLEN];
	            };
    struct dls_msg dls_msgstruct;

    dls_setsig(2);

    if(dls_shm_pointer == (DLS_IPC *)NULL) { /* accedo Shared memory */
					     /* accedo segnali       */
       if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0660)) == ERROR) {
          fprintf(stderr,"Shared Memory NOT OPENED !\n");
          retquote("Shared Memory NOT OPENED !");
          retint(ERROR);
          return(2);
          }
      /*
       * Carico Shared Memory
       */

       if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
          dls_error("Cannot attache shared memory segment to my process",(char *)NULL);
          retquote("Cannot attach shared memory to my process !");
          retint(ERROR);
          return(2);
          }

       dls_shm_pointer = (DLS_IPC *)shm_pointer;

       semshm = semget((key_t)SEMSHM,1,0660);

       if(semshm == ERROR) {
          dls_error("Cannot get semaphore","SHM");
          retquote("Cannot get semaphore SHM");
          retint(ERROR);
          return(2);
          }
       dls_sem_lock(SEMSHM);

      /*
       * prima volta: valorizzo Shared Memory
       */

       dls_shm_pointer->start2 = time((time_t *)NULL);
       dls_shm_pointer->status2 = RUNNING;

       dls_shm_pointer->dls_pid2 = getpid();
       dls_sem_unlock(SEMSHM);

       if((dls_msgid = msgget((key_t)MSGKEY,0660)) == ERROR) {
	    dls_error("Can't access message queue ID",(char *)NULL);
	    retquote("Can't access message queue ID");
	    retint(ERROR);
	    return(2);
	    }

      }

    if(msgrcv(dls_msgid,(void *)&dls_msgstruct,sizeof(ret_buf),1L,0) == ERROR) {
      /*
       * testo se e' stato interrotto da segnale (SIGHUP received)
       */
       dls_sem_lock(SEMSHM);
       dls_shm_pointer->start2 = (time_t)NULL;
       dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
       dls_shm_pointer->status2 = NOTRUNNING;

       if(dls_shm_pointer->db_status == TRUE) { /* Database in accesso esclusivo */
          dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
          dls_shm_pointer->status2 = DBEXCLUSIVE;
          dls_shm_pointer = (DLS_IPC *)NULL;
          dls_sem_unlock(SEMSHM);
          retquote("Database Exclusive Requested");
          retint(TRUE);
          return(2);
	  }
       else { /* Errore fatale */
             dls_shm_pointer = (DLS_IPC *)NULL;
             dls_sem_unlock(SEMSHM);
             dls_error("Can't receive message queue",(char *)NULL);

             ora = time((time_t *)NULL);

             dls_tm = localtime(&ora);
             (void)strftime(tmp,sizeof(tmp),"%A %d %B %Y  %H:%M",dls_tm);

             fprintf(stderr,"Process stopped at %s\n",tmp);

             retquote("Fatal error while receiving !");
             retint(ERROR);
             return(2);
	  }
      }
      /*
       * messaggio ricevuto correttamente !
       */

    /*
     * Test per accesso esclusivo 
     */

    dls_sem_lock(SEMSHM);

    if(dls_shm_pointer->db_status == TRUE) { /* Database in accesso esclusivo */
       dls_shm_pointer->status2 = DBEXCLUSIVE;
       dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
       dls_shm_pointer = (DLS_IPC *)NULL;
       dls_sem_unlock(SEMSHM);
       retquote("Database Exclusive Requested");
       retint(TRUE);
       return(2);
       }

    dls_shm_pointer->msgrecv++;
    dls_shm_pointer->lastrecv = time((time_t *)NULL);

    (void)strcpy((char *)dls_shm_pointer->laststr,dls_msgstruct.mtext);

    strcpy(ret_buf,dls_msgstruct.mtext);
    dls_sem_unlock(SEMSHM);
    retquote(ret_buf);
    retint(FALSE);
    return(2);
}

int leggi_shm(void)
/*---------------------------------------------------------------------------

	leggi_shm: lettura dello stato del Database

	Parametri in input (da programma INFORMIX-4GL): nessuno

	Parametri ritornati al programma 4GL: 0 = Database non esclusivo
	                                      1 = Database esclusivo
					      -1 = Shared memory non aperta

---------------------------------------------------------------------------*/
{

    if(dls_shm_pointer == (DLS_IPC *)NULL) { /* Carico Shared Memory */
       if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0660)) == ERROR) {
           dls_error("Cannot access shared memory",(char *)NULL);
           retint(ERROR);
           return(1);
           }
      /*
       * Carico Shared Memory
       */

       if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
          dls_error("Cannot attache shared memory segment to my process",(char *)NULL);
          retint(ERROR);
          return(1);
          }

       dls_shm_pointer = (DLS_IPC *)shm_pointer;
     }

    dls_sem_lock(SEMSHM);
    if(dls_shm_pointer->db_status == FALSE) { /* Database non in accesso esclusivo */
       dls_sem_unlock(SEMSHM);
       retint(FALSE);
       return(1);
       }
    else { /* Database in accesso esclusivo */
       dls_shm_pointer->start2 = (time_t)NULL;
       dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
       dls_shm_pointer->status2 = DBEXCLUSIVE;
       dls_shm_pointer = (DLS_IPC *)NULL;
       dls_sem_unlock(SEMSHM);
       retint(TRUE);
       return(1);
       }
}

void dls_sem_lock(unsigned int numero)
/*---------------------------------------------------------------------------

	dls_sem_lock: routine per il lock del semaforo

	Parametri in input:  semaforo da trattare

	Parametri in output:  nessuno

---------------------------------------------------------------------------*/
{
    switch(numero) {
	   case SEMSHM:
                       (void)semop(semshm,&locktalk,1);
		       break;
                   }
}

void dls_sem_unlock(unsigned int numero)
/*---------------------------------------------------------------------------

	dls_sem_unlock: routine per unlock del semaforo

	Parametri in input:  semaforo da trattare

	Parametri in output:  nessuno

---------------------------------------------------------------------------*/
{
    switch(numero) {
	   case SEMSHM:
                       (void)semctl(semshm,0,SETVAL,1);
		       break;
                   }
}

void dls_error(char *errore,char *parametro)
/*---------------------------------------------------------------------------

	dls_error: routine di gestione errori

	Parametri in input: descrizione errore
			    parametro aggiuntivo

	Parametri in output: nessuno

---------------------------------------------------------------------------*/
{
char strerr[256];

      (void)sprintf(strerr,"%s <%s> reason",errore,parametro);
      perror(strerr);

}

void dls_abort(int sig)
/*---------------------------------------------------------------------------

	dls_abort: chiusura del processo di lettura tty per errore fatale

	Parametri in input: segnale rilevato

	Parametri in output: nessuno

---------------------------------------------------------------------------*/
{
    char   tmp[81];

    nil(sig);

    if ( ioctl(dls_fdtty,TCSETA,(char *)&dls_copia) == ERROR )
	 dls_error("Can't set <default> terminal characteristics for",(char *)dls_device->dls_tty);

    if(close(dls_fdtty) == ERROR)
	 dls_error("Can't close device for",(char *)dls_device->dls_tty);

    dls_sem_lock(SEMSHM);
    dls_shm_pointer->dls_pabx1[shm_idx].start1 = (time_t)NULL;
    dls_shm_pointer->dls_pabx1[shm_idx].status1 = NOTRUNNING;
    dls_shm_pointer->dls_pabx1[shm_idx].dls_pid1 = (pid_t)FALSE;
    dls_sem_unlock(SEMSHM);

    ora = time((time_t *)NULL);

    dls_tm = localtime(&ora);

    (void)strftime(tmp,sizeof(tmp),"%A %d %B %Y  %H:%M",dls_tm);

    fprintf(stderr,"Process stopped at %s\n",tmp);
    exit(1);
}

void dls_abort1(int sig)
/*---------------------------------------------------------------------------

	dls_abort1: chiusura del processo di lettura coda per errore fatale

	Parametri in input: segnale rilevato

	Parametri in output: nessuno

---------------------------------------------------------------------------*/
{
    char   tmp[81];

    nil(sig);
    dls_sem_lock(SEMSHM);
    dls_shm_pointer->start2   = (time_t)NULL;
    dls_shm_pointer->status2   = NOTRUNNING;
    dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
    dls_sem_unlock(SEMSHM);

    ora = time((time_t *)NULL);

    dls_tm = localtime(&ora);

    (void)strftime(tmp,sizeof(tmp),"%A %d %B %Y  %H:%M",dls_tm);

    fprintf(stderr,"Process stopped at %s\n",tmp);
    exit(1);
}

void nil(int sig)
/*---------------------------------------------------------------------------

	nil:	funzione utilizzata in caso di segnali inviati al processo

---------------------------------------------------------------------------*/
{
char tmp[70];
unsigned int k;

    ora = time((time_t *)NULL);

    dls_tm = localtime(&ora);

    (void)strftime(tmp,sizeof(tmp),"%A %d/%b/%Y  %H:%M",dls_tm);
    k = sig - 1;

    fprintf(stderr,"Got signal number %d <%s> at %s\n",sig,c_segnali[k],tmp);
}

void dls_setsig(short caso)
/*---------------------------------------------------------------------------

	dls_setsig:	funzione per settare il trattamento dei segnali
			ricevuti

---------------------------------------------------------------------------*/
{
    signal(SIGHUP,nil);	/* hangup */
    signal(SIGCONT,nil);/* signale continue */

    switch(caso) {
	   case 1:
                  signal(SIGINT,dls_abort); /* interrupt */
                  signal(SIGQUIT,dls_abort);/* quit */
                  signal(SIGFPE,dls_abort); /* floating point exception */
                  signal(SIGBUS,dls_abort); /* bus error */
                  signal(SIGSEGV,dls_abort);/* segmentation violation */
                  signal(SIGSYS,dls_abort); /* bad argument to system call */
                  signal(SIGTERM,dls_abort);/* software termination signal from kill */
		  break;
	   case 2:	/* processo di lettura coda */
                  signal(SIGINT,dls_abort1); /* interrupt */
                  signal(SIGQUIT,dls_abort1);/* quit */
                  signal(SIGFPE,dls_abort1); /* floating point exception */
                  signal(SIGBUS,dls_abort1); /* bus error */
                  signal(SIGSEGV,dls_abort1);/* segmentation violation */
                  signal(SIGSYS,dls_abort1); /* bad argument to system call */
                  signal(SIGTERM,dls_abort1);/* software termination signal from kill */
		  break;
	  }
}

int dls_blocca_db(void)
/*---------------------------------------------------------------------------

	dls_blocca_db:	funzione richiamata da programma 4GL
			per richiedere il database esclusivo
			al programma di gestione centralini

	parametri in input: nessuno

	parametri in output: 0 OK !
			    -1 KO

---------------------------------------------------------------------------*/
{
struct dls_msg {
		long mtype;
		char mtext[MAXSTRLEN];
	       };
struct dls_msg dls_msgstruct;

    if(dls_shm_pointer == (DLS_IPC *)NULL) {
       if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0660)) == ERROR) {
           dls_error("Cannot access shared memory",(char *)NULL);
           retint(ERROR);
           return(1);
           }
      /*
       * Carico Shared Memory
       */

       if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
          dls_error("Cannot attache shared memory segment to my process",(char *)NULL);
          retint(ERROR);
          return(1);
          }

       dls_shm_pointer = (DLS_IPC *)shm_pointer;
     }

    dls_shm_pointer->db_status = TRUE;

    if(dls_shm_pointer->dls_pid2 == (pid_t)FALSE) { /* processo gia` inattivo */
       retint(0);
       return(1);
       }

    if((dls_msgid = msgget((key_t)MSGKEY,0660)) == ERROR) {
	 dls_error("Can't access message queue ID",(char *)NULL);
	 retint(ERROR);
	 return(1);
	 }

    dls_msgstruct.mtype = 1L;

    (void)strcpy(dls_msgstruct.mtext,"Database exclusive");

    if(msgsnd(dls_msgid,(void *)&dls_msgstruct,sizeof(MAXSTRLEN),0) == ERROR) {
       dls_error("Can't send message queue",(char *)NULL);
       retint(ERROR);
       return(1);
       }

    retint(0);
    return(1);
}

int dls_sblocca_db(void)
/*---------------------------------------------------------------------------

	dls_sblocca_db:	funzione richiamata da programma 4GL
			per riattivare il programma di gestione centralini

	parametri in input: nessuno

	parametri in output: 0 OK !
			    -1 KO

---------------------------------------------------------------------------*/
{

    if(dls_shm_pointer == (DLS_IPC *)NULL) {
       if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0660)) == ERROR) {
           dls_error("Cannot access shared memory",(char *)NULL);
           retint(ERROR);
           return(1);
           }
      /*
       * Carico Shared Memory
       */

       if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
          dls_error("Cannot attache shared memory segment to my process",(char *)NULL);
          retint(ERROR);
          return(1);
          }

       dls_shm_pointer = (DLS_IPC *)shm_pointer;
      }

    dls_shm_pointer->db_status = FALSE;

    retint(0);
    return(1);
}

/*******************************************************************************

   blocco - Funzione di gestione accesso alla procedura

*******************************************************************************/
int blocco()
{
int	ind;
int	esco;
void    uscita();
char	*getenv();
char	*comodo;

   if(shm_utenti_pointer == (char *)NULL) {
      /************************
        Apro la share memory
      ************************/
   
      if((shm_ute_id=shmget(CHKSHM,sizeof(USER_ON),0660)) == -1) {
         retint(10);
         return(1);
      }
   
      if((shm_utenti_pointer=shmat(shm_ute_id,(char *)0,0)) == (char *)-1) {
         perror("Errore nel prendere la porzione di Share Memory !\nCausa ");
         exit(-1);
      }

      /*********************************
        Creo semaforo di controllo SHM
      *********************************/
   
      if((sem_ute_id=semget(CHKSEM,1,0660)) == -1) {
         perror("Errore di apertura semaf. di controllo Share Memory\nCausa ");
         exit(-1);
      }
   }

   shm_utenti = (USER_ON *)shm_utenti_pointer;

   (void)semop(sem_ute_id,&sem_operation,1);

   /*******************************************
     Controllo il numero di utenti collegati
   *******************************************/

   if(shm_utenti->attivi + 1 > shm_utenti->licenza) {
      /***************************
	Troppi utenti collegati
      ***************************/
      retint(0);
      return(1);
   }

   for(ind=0,esco=0;ind < MAXUSER && !esco;ind++) {
       /**********************
	 Inserisco l'utente
       **********************/
       if(shm_utenti->user_on[ind].id_proc == 0) {
	  shm_utenti->user_on[ind].id_proc = (int)getpid();
	  comodo = getenv("USER");
	  sprintf(shm_utenti->user_on[ind].utente,"%s",comodo);
	  esco = 1;
       }
   }

   /**********************************************
     Incremento il numero degli utenti collegati
   **********************************************/

   shm_utenti->attivi++;

   /***********************
     Sblocco il semaforo
   ***********************/

   (void)semctl(sem_ute_id,0,SETVAL,1);

   retint(1);
   return(1);
}

/*******************************************************************************

   sblocco - Funzione di gestione uscita dalla procedura

*******************************************************************************/
int sblocco()
{
int	ind;
int	esco;
void    uscita();
pid_t   processo;

   if(shm_utenti_pointer == (char *)NULL) {
      /************************
        Apro la share memory
      ************************/
   
      if((shm_ute_id=shmget(CHKSHM,sizeof(USER_ON),0660)) == -1) {
         retint(10);
         return(1);
      }
   
      if((shm_utenti_pointer=shmat(shm_ute_id,(char *)0,0)) == (char *)-1) {
         perror("Errore nel prendere la porzione di Share Memory !\nCausa ");
         exit(-1);
      }

      /*********************************
        Creo semaforo di controllo SHM
      *********************************/
   
      if((sem_ute_id=semget(CHKSEM,1,0660)) == -1) {
         perror("Errore di apertura semaf. di controllo Share Memory\nCausa ");
         exit(-1);
      }
   }

   shm_utenti = (USER_ON *)shm_utenti_pointer;

   (void)semop(sem_ute_id,&sem_operation,1);

   processo = getpid();

   for(ind=0,esco=0;ind < MAXUSER && !esco;ind++) {
       /**********************
	 Scollego l'utente
       **********************/
       if(shm_utenti->user_on[ind].id_proc == processo) {
	  shm_utenti->user_on[ind].id_proc = 0;
	  strcpy(shm_utenti->user_on[ind].utente,"");
	  esco = 1;
       }
   }

   /**********************************************
     Decremento il numero degli utenti collegati
   **********************************************/

   shm_utenti->attivi--;

   /***********************
     Sblocco il semaforo
   ***********************/

   (void)semctl(sem_ute_id,0,SETVAL,1);

   retint(1);
   return(1);
}


/******************************************************************************

	PROCEDURA DI DIALOGO DELLA PROCEDURA INFORMIX COL CENTRALINO

		Copyright (C) Delos s.r.l. 1989-1994

******************************************************************************/

int c_chktel(void)
/*---------------------------------------------------------------------------

	c_chktel:  funzione per la gestione automatica del dialogo
		   tra sistema Unix <-> Pabx

	Parametri in input (stringa,lunghezza)

---------------------------------------------------------------------------*/
{
/* parametri da programma Informix 4GL */
char i_str[MAXSTRLEN];
unsigned short lun;

/* variabili locali */
char ret_buf[MAXSTRLEN];
struct dls_msg {
		long mtype;
		char mtext[MAXSTRLEN];
	        };
struct dls_msg dls_msgstruct;

  popshort(&lun);
  popquote(i_str,sizeof(i_str));

  i_str[lun] = '\0';

  if(dls_msgid == FALSE) { /* accedo MESSAGE QUEUE */
    if((dls_msgid = msgget((key_t)MSGKEY,0660)) == ERROR) {
	 /*--------------------------------

		GESTIONE ERRORI

	 --------------------------------*/
	 perror("Can't access message queue ID");
	 retint(ERROR);
	 return(1);
	 }
     }
  dls_msgstruct.mtype = 2L;

  (void)strcpy(dls_msgstruct.mtext,i_str);

  if(msgsnd(dls_msgid,(void *)&dls_msgstruct,sizeof(i_str),0) == ERROR) {
    /*--------------------------------

		GESTIONE ERRORI

    --------------------------------*/
    perror("Can't send message");
    retint(ERROR);
    return(1);
    }
  /* Tutto OK ! */

  retint(0);
  return(1);
}

int leggi_coda1(void)
/*---------------------------------------------------------------------------

	leggi_coda1: lettura della coda di messaggi (check_in/out)

	Parametri in input (da programma INFORMIX-4GL): nessuno

	Parametri ritornati al programma 4GL: stringa letta + return code
					      0 = OK !
					     -1 = Errore fatale !

---------------------------------------------------------------------------*/
{
    char ret_buf[MAXSTRLEN];
    char tmp[81];
    struct dls_msg {
		    long mtype;
		    char mtext[MAXSTRLEN];
		   };
    struct dls_msg dls_msgstruct;

    if(dls_msgid == FALSE) {
      if((dls_msgid = msgget((key_t)MSGKEY,0660)) == ERROR) {
	 dls_error("Can't access message queue ID",(char *)NULL);
	 retquote("Can't access message queue ID");
	 retint(ERROR);
	 return(2);
	 }
     }

    if(msgrcv(dls_msgid,(void *)&dls_msgstruct,sizeof(ret_buf),2L,0) == ERROR) { /* Errore fatale */
       retquote("Fatal error while receiving !");
       retint(ERROR);
       return(2);
      }
   else {
      /*
       * messaggio ricevuto correttamente !
       */
         strcpy(ret_buf,dls_msgstruct.mtext);
         retquote(ret_buf);
         retint(FALSE);
         return(2);
       }
}

void dialog_abort(unsigned int sig)
/*---------------------------------------------------------------------------

	dialog_abort:	funzione utilizzata in caso di Timeout sulla
			connessione Unix <-> Pabx

---------------------------------------------------------------------------*/
{
	(void)close(dls_fdtty);
}

/*******************************************************************************

        DA QUI IN AVANTI SONO LE FUNZIONI PER LA GESTIONE DELLE CASSE

*******************************************************************************/
/*---------------------------------------------------------------------------

	leggi_porta:	funzione utilizzata per la lettura del device

---------------------------------------------------------------------------*/
int leggi_porta() {
unsigned short oltre_fine;
unsigned short quanti_char;
int      fine_riga;
char     c;
char     ret_buf[MAXSTRLEN];
int      i=0;
char     *scarto;

   popshort(&oltre_fine);
   popshort(&quanti_char);
   popint(&fine_riga);

   for(i = 0; i< MAXSTRLEN;i++)
       ret_buf[i] = ' ';
   i=0;
   while(read(dls_fdtty,&c,1) > 0) {
         if(c ==  '\0')
            continue;

	 if(quanti_char == 0) {
	    if(c == fine_riga ) {
	       if ( oltre_fine > 0 ) {
		    scarto=(char *)malloc((unsigned)oltre_fine);
                    read(dls_fdtty,scarto,oltre_fine);
		    free(scarto);
	       }
	       ret_buf[i] = c;
	       i++;
	       ret_buf[i] = '\0';
               break;
	     }
	  }
	 if(quanti_char > 0) {
	    if(i == (quanti_char - 1)) {	
	      if ( oltre_fine > 0 ) {
		   scarto=(char *)malloc((unsigned)oltre_fine);
                   read(dls_fdtty,scarto,oltre_fine);
		   free(scarto);
	        }
	      ret_buf[i] = c;
	      i++;
	      ret_buf[i] = '\0';
              break;
	   }
	 }
               
	 ret_buf[i] = c;
	 i++;
    }

    retquote(ret_buf);
    return(1);
}

int c_bcc()
/*------------------------------------------------------------------------------

     c_bcc - Calcola il codice di controllo di una stringa generica 
             null-terminata per RCHG600T

------------------------------------------------------------------------------*/
{
	register char bcc;
	char ret_bcc[2];
	char s[MAXSTRLEN];
        unsigned short len;
#ifdef ULTRIX
	char *t;
#else
	const char *t;
#endif

        popshort(&len);
        popquote(s,sizeof(s));

        s[len] = '\0';
        t = s;

	bcc = 0;
	while (*t) { bcc = bcc ^ *t++; }

        ret_bcc[0] = bcc;

        if(ret_bcc[0] == NULL)
           BccNullo = TRUE;
        else
           BccNullo = FALSE;

        ret_bcc[1] = '\0';
        retquote(ret_bcc);
	return(1);
}

/*---------------------------------------------------------------------------

	scrivi_porta:	funzione per la scrittura del device

---------------------------------------------------------------------------*/
int scrivi_porta() {
char str[MAXSTRLEN];
int  lunghezza;
int  ritorno=0;

     popint(&lunghezza);
     popquote(str,sizeof(str));

     if(BccNullo == TRUE) {
        str[lunghezza-1]='\0';
        BccNullo = FALSE;
       }

     str[lunghezza]='\0';

     if ( write(dls_fdtty,str,lunghezza) != lunghezza ) {
          dls_error("Can't write to device",(char *)dls_device->dls_tty);
          ritorno = -1;
     }
     retint(ritorno);
     return(1);
}

/******************************************************************************
*
*   DA QUI IN AVANTI FUNZIONI PER LA GESTIONE DELLE LETTERE DI CONFERMA
*   DELLA PRENOTAZIONE
*
*******************************************************************************/

/*******************************************************************************
*
*   riga_lettera - Funzione di ritorno di una riga di una lettera tipo
*
*******************************************************************************/
int riga_lettera(void)
{
typedef struct variable { char dove[80];
			  char nome[80];
		          int  numero;
		          int  riga;
		          int  crt; } VARIABILE;

static FILE *pointer=(FILE *)NULL;
static long variabili=0;
static long fine_testata=0;
static long fine_dettaglio=0;
static long fine_fondo=0;
static long testata=0;
static long dettaglio=0;
static long fondo=0;
static int  linee_testata=0;
static int  linee_dettaglio=0;
static int  linee_fondo=0;
static int  n_var=0;
static VARIABILE var[50];
char   nomefile[80];
char   tipo_estrazione[80];
char   parametri[50][80];
char   ricerca[81];
char   stringa[257];
char   stringa1[257];
char   stringa2[257];
int    ind=0;
int    ret=0;
int    ind1=0;
char   davanti[81];
char   dietro[81];
char   *punta;
char   ritorno[257];
char   *debug;
void   cut_rem();
void   cut_nl();
char   *getenv();

     debug = getenv("DBDBG");

     for (ind=49; ind >= 0; ind--) {

	  popquote(parametri[ind],80);

	  if(debug != NULL )
	     fprintf(stderr,"ind %d <%s>\n",ind,parametri[ind]);
     }

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

     if ( strcmp(tipo_estrazione,"RESET") == 0 ) {
	  pointer = (FILE *)NULL;
          variabili = 0;
	  testata = 0;
	  dettaglio = 0;
	  fondo = 0;
	  linee_testata = 0;
	  linee_dettaglio = 0;
	  linee_fondo = 0;
	  fine_testata = 0;
	  fine_dettaglio = 0;
	  fine_fondo = 0;
     }

     if ( pointer == (FILE *)NULL) {
          if ((pointer = fopen(nomefile,"r")) == (FILE *)NULL) {
	       if ( debug != NULL )
	            perror("Non riesco ad aprire il file causa ");

	       retquote("");
	       return(1);
	  }

	  if ( debug != NULL )
	       fprintf(stderr,"Ho aperto il file %s\n",nomefile);
     }

     if ( fgets(stringa,256,pointer) == (char *)NULL ) {
	  retquote("");
	  return(1);
     }

     cut_rem(stringa);

     if ( debug != NULL )
	  fprintf(stderr,"Prima lettura %s\n",stringa);

     while (strcmp("{VARIABILI}\n",stringa) != 0 &&
	    variabili                       == 0    ) {

            if ( fgets(stringa,256,pointer) == (char *)NULL ) {
		 retquote("");
	         return(1);
            }

            cut_rem(stringa);

            if ( debug != NULL )
	         fprintf(stderr,"Ricerco variabili %s\n",stringa);
     }

     if ( strcmp("{VARIABILI}\n",stringa) == 0 ) {
          sscanf(stringa,"%[^\n]\n",stringa1);
          strcpy(stringa,stringa1);
	  variabili = ftell(pointer);
	  n_var = 0;
	  while ( fgets(stringa,256,pointer) != (char *)NULL ) {

                  cut_rem(stringa);
                  sscanf(stringa,"%[^\n]\n",stringa1);
                  strcpy(stringa,stringa1);

		  ret = strcmp("{FINE VARIABILI}",stringa);
		  if ( strcmp("{FINE VARIABILI}",stringa) != 0) {
		       if ( sscanf(stringa,"%[^:]:%[^:]:%d:%d:%d",
			                   var[n_var].dove,
			                   var[n_var].nome,
		                           &var[n_var].numero,
			                   &var[n_var].riga,
			                   &var[n_var].crt) == 5 ) {

			    if ( debug != NULL )
		                 fprintf(stderr,
				 "dove %s nome %s numero %d riga %d crt %d\n",
				 var[n_var].dove,
				 var[n_var].nome,
		                 var[n_var].numero,
				 var[n_var].riga,
				 var[n_var].crt);

			    n_var++;
		       }
		  } else
		    break;
	  }

	  if ( debug != NULL )
	       fprintf(stderr,"N_VAR = %d\n",n_var);
     }

     if ( strcmp("TESTATA",tipo_estrazione) == 0) {
	  if ( fine_testata ) {
	       fseek(pointer,testata,SEEK_SET);
               fgets(stringa,256,pointer);
               cut_rem(stringa);
	       fine_testata = 0;
	       linee_testata = 0;
	  }

	  if ( !testata ) {
	       if ( debug != NULL )
		    fprintf(stderr,"Cerco la TESTATA\n");

	       do {
                       cut_rem(stringa);
		       sscanf(stringa,"%[^\n]\n",stringa1);
		       strcpy(stringa,stringa1);

		       if ( strcmp("{TESTATA}",stringa) == 0) {
			    testata = ftell(pointer);
			    break;
		       }
	       } while ( fgets(stringa,256,pointer) != (char *)NULL );

               if ( fgets(stringa,256,pointer) == (char *)NULL )  {
		    retquote("");
	            return(1);
	       }
	  }

	  if ( debug != NULL )
	       fprintf(stderr,"Testata TROVATA\n");

	  if ( strcmp("{FINE TESTATA}\n",stringa) == 0 ) {
	       if ( debug != NULL )
	            fprintf(stderr,"FINE TESTATA");
	       fine_testata = 1;
	       retquote("");
               return(1);
	  }

	  linee_testata++;

	  if ( debug != NULL )
	       fprintf(stderr,"Ciclo variabili\n");

          for ( ind=0;ind < n_var; ind++) {

		if ( strcmp(var[ind].dove,"TESTATA") == 0 ) {

		     if ( var[ind].riga == linee_testata ) {
			  sprintf(ricerca,"$%s",var[ind].nome);
			  strcpy(stringa2,stringa);
			  punta = strstr(stringa2,ricerca);
			  if ( punta != NULL ) {
			       *punta = '\0';
			       strcpy(davanti,stringa2);
			       for ( ind1=0; ind1 < var[ind].crt; ind1++) {
				     punta++;
			       }
			       strcpy(dietro,punta);
			       parametri[var[ind].numero-1][var[ind].crt]='\0';
			       sprintf(stringa,"%s%s%s",davanti,
						   parametri[var[ind].numero-1],
						   dietro);
			  }
		     }
		}
	  }

	  if ( debug != NULL )
	       fprintf(stderr,"Prima cut_nl %s\n",stringa);

	  cut_nl(stringa);

	  if ( debug != NULL )
	       fprintf(stderr,"Dopo cut_nl %s\n",stringa);

	  strcpy(ritorno,stringa);
	  retquote(ritorno);
	  return(1);
     }

     if ( strcmp("DETTAGLIO",tipo_estrazione) == 0) {
	  if ( fine_dettaglio ) {
	       fseek(pointer,dettaglio,SEEK_SET);
               fgets(stringa,256,pointer);
               cut_rem(stringa);
	       fine_dettaglio = 0;
	       linee_dettaglio = 0;
	  }

	  if ( !dettaglio ) {
	       do {
                       cut_rem(stringa);
		       sscanf(stringa,"%[^\n]\n",stringa1);
		       strcpy(stringa,stringa1);

		       if ( strcmp("{DETTAGLIO}",stringa) == 0) {
			    dettaglio = ftell(pointer);
			    break;
		       }
	       } while ( fgets(stringa,256,pointer) != (char *)NULL );

               if ( fgets(stringa,256,pointer) == (char *)NULL ) {
		    retquote("");
	            return(1);
               }
	  }

	  ret=strcmp("{FINE DETTAGLIO}\n",stringa);
	  if ( strcmp("{FINE DETTAGLIO}\n",stringa) == 0 ) {
	       fine_dettaglio = 1;
	       retquote("");
	       return(1);
	  }

	  linee_dettaglio++;

          for ( ind=0;ind < n_var; ind++) {

		if ( strcmp(var[ind].dove,"DETTAGLIO") == 0 ) {

		     if ( var[ind].riga == linee_dettaglio ) {
			  sprintf(ricerca,"$%s",var[ind].nome);
			  strcpy(stringa2,stringa);
			  punta = strstr(stringa2,ricerca);
			  if ( punta != NULL ) {
			       *punta = '\0';
			       strcpy(davanti,stringa2);
			       for ( ind1=0; ind1 < var[ind].crt; ind1++) {
				     punta++;
			       }
			       strcpy(dietro,punta);
			       parametri[var[ind].numero-1][var[ind].crt]='\0';
			       sprintf(stringa,"%s%s%s",davanti,
						   parametri[var[ind].numero-1],
						   dietro);
			  }
		     }
		}
	  }

	  cut_nl(stringa);
	  strcpy(ritorno,stringa);
	  retquote(ritorno);
	  return(1);
     }

     if ( strcmp("FONDO",tipo_estrazione) == 0) {
	  if ( fine_fondo ) {
	       fseek(pointer,fondo,SEEK_SET);
               fgets(stringa,256,pointer);
               cut_rem(stringa);
	       fine_fondo = 0;
	       linee_fondo = 0;
	  }

	  if ( !fondo ) {
	       do {
                       cut_rem(stringa);
		       sscanf(stringa,"%[^\n]\n",stringa1);
		       strcpy(stringa,stringa1);

		       if ( strcmp("{FONDO}",stringa) == 0) {
			    fondo = ftell(pointer);
			    break;
		       }
	       } while ( fgets(stringa,256,pointer) != (char *)NULL );

               if ( fgets(stringa,256,pointer) == (char *)NULL ) {
		    retquote("");
	            return(1);
               }
	  }

	  ret=strcmp("{FINE FONDO}\n",stringa);
	  if ( strcmp("{FINE FONDO}\n",stringa) == 0 ) {
	       fine_fondo = 1;
	       retquote("");
	       return(1);
	  }

	  linee_fondo++;

          for ( ind=0;ind < n_var; ind++) {

		if ( strcmp(var[ind].dove,"FONDO") == 0 ) {

		     if ( var[ind].riga == linee_fondo ) {
			  sprintf(ricerca,"$%s",var[ind].nome);
			  strcpy(stringa2,stringa);
			  punta = strstr(stringa2,ricerca);
			  if ( punta != NULL ) {
			       *punta = '\0';
			       strcpy(davanti,stringa2);
			       for ( ind1=0; ind1 < var[ind].crt; ind1++) {
				     punta++;
			       }
			       strcpy(dietro,punta);
			       parametri[var[ind].numero-1][var[ind].crt]='\0';
			       sprintf(stringa,"%s%s%s",davanti,
						   parametri[var[ind].numero-1],
						   dietro);
			  }
		     }
		}
	  }

	  cut_nl(stringa);
	  strcpy(ritorno,stringa);
	  retquote(ritorno);
	  return(1);
     }

     retquote("");
     return(1);

}

void cut_rem(char *stringa)
/*******************************************************************************
*
*   cut_rem - Taglia il commento da una stringa.
*             (Il commento e' qualsiasi cosa preceduta dal simbolo #
*
*******************************************************************************/
{
char *commento;
char *newline;
int  crt;

     crt = '#';

     commento = strchr(stringa,crt);
     newline  = strchr(stringa,'\n');

     if ( commento != NULL  ) {
	  *commento = '\0';

          if ( newline != NULL )
	       strcat(stringa,"\n");
     }
}

void cut_nl(char *stringa)
/*******************************************************************************
*
*   cut_nl - Toglia i NewLine da una stringa.
*
*******************************************************************************/
{
char *newline;
int  crt;

     newline  = strchr(stringa,'\n');

     if ( newline != NULL )
	  *newline = ' ';
}

/*---------------------------------------------------------------------------

Nome Funzione: rw_semaphore
Funzionalitaà: Lettura e scrittura semafori
			 
Parametri di I.: path_file = Nome del file del semaforo
                 operazione = lettura o scrittura del semaforo
			      read = lettura
		              write = scrittura
		 variabile = stato del semaforo da impostare (solo per la scrittura)

Valori di ritorno: Stato del semaforo: 0 = SBLOCCATO
			               1 = BLOCCATO
Data di creazione: 7/2/2000               Autore: Alessio Giammello
Data ultima mod.:                         Autore:
------------------------------------------------------------------------------*/

int rw_semaphore(void)
{
	int file_sem;                             
	char valore; 
	int valore_ritorno=0;
	char comando[512];
        int conta;


	/*variabili passate dal programma I4gl*/
	char path_file[101];
	char operazione[6];
	char variabile[2];

	/*carico i dati in arrivo dal 4gl*/
	popquote(variabile,sizeof(variabile));		  /*CONTROLLARE*/
	popquote(operazione,sizeof(operazione));	  /*CONTROLLARE*/
	popquote(path_file,sizeof(path_file));            /*CONTROLLARE*/

        conta = 0;
        while( conta < 100 && *( path_file + conta ) != ' ' )
                conta++;
        *( path_file + conta ) = '\0';

        conta = 0;
        while( conta < 5 && *( operazione + conta ) != ' ' )
                conta++;
        *( operazione + conta ) = '\0';

        conta = 0;
        while( conta < 1 && *( variabile + conta ) != ' ' )
                conta++;
        *( variabile + conta ) = '\0';


  	/*fprintf (stderr, "%s %s %s\n", path_file, operazione, variabile);*/ 
        /*getchar();*/

	/*eseguo la lettura del semaforo*/
	if (operazione[0] == 'r' || operazione[0] == 'R')
	{
		if ( (file_sem = open(path_file, O_RDONLY|O_NDELAY) ) != -1)
		{
			read(file_sem, &valore, 1);
			
			/*trasformo il carattere letto nel valore intero 0,1 del semaforo*/

			valore_ritorno = valore - 48;
/*
			if (valore =='0') 
				valore_ritorno = 0;
			else 
				valore_ritorno = 1;
*/
		}
		else
			valore_ritorno = -1;

		close(file_sem);

		retint(valore_ritorno);          /*CONTROLLA RITORNO*/
		return(1);
	}
	/*eseguo la scrittura del semaforo*/
	else
	{
	
		sprintf (comando, "echo %s > %s", variabile, path_file);
		system (comando);
		
		retint(0);			/*CONTROLLA RITORNO*/
		return(1);
	}


}                   
