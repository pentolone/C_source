#ifndef LINT
static char sccs_tableauid[] = "@(#)tableau.c Version 1.1 Date 00/02/21 Hour 12:07:17";
#endif
/*---------------------------------------------------------------------------

	pt_attiva.c:  funzione per la gestione dell'interfacciamento
		      tra il S.I.A. e il sistema TV GuestLink

	Parametri in input - tty, speed, parity, databit, stopbit

---------------------------------------------------------------------------*/
#include "dls_include.h"
#include "paytv.h"
#include <sys/wait.h>
#include <pwd.h>

pid_t dbpid;
unsigned int time_to_sleep=80;
int *sia_status;

int main(int argc, char *argv[])
{
int i,lun,dls_qr,dls_qw;
char pt_bcc(const char*);
char c;
char *scarto;
char pt_stringa[MAXSTRLEN];
struct passwd *dls_passwd;
struct dls_msg {
		long mtype;
		char mtext[MAXSTRLEN];
	        };
struct dls_msg dls_msgstruct;

    if(argc < 6) {
       fprintf(stderr,"Usage: %s tty, speed, parity, databit, stopbit\n",argv[0]);
       exit(1);
       }

    if((dls_shmid = shmget((key_t)PT_SHM,sizeof(sia_status),IPC_EXCL | IPC_CREAT | 0666)) == ERROR) {
       if(errno != EEXIST) {
          perror("Cannot create Shared Memory ! reason");
          exit(1);
	  }
       else {
          if((dls_shmid = shmget((key_t)PT_SHM,sizeof(sia_status),0666)) == ERROR) {
              perror("Cannot get shared memory identifier reason");
              exit(1);
              }
           }
       }

    if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
       perror("Cannot attache shared memory segment to my process reason");
       exit(1);
       }

    sia_status = (int *)shm_pointer;
    *sia_status = FALSE;

   /*
    * Apro il device e ne setto le caratteristiche
    */
    if(dls_opentty(argv) == ERROR)
       exit(1);

    dls_passwd = getpwnam("informix");

    if(setgid(dls_passwd->pw_gid) == ERROR) {
       perror("Can't set group id (informix) reason");
       exit(1);
      }

    if(setuid(dls_passwd->pw_uid) == ERROR) {
       perror("Can't set user id (informix) reason");
       exit(1);
      }

   /*
    * Creo semaforo e setto valido
    */

    semshm = semget((key_t)PT_SEM,1,IPC_CREAT|0660);

    if(semshm == ERROR) {
       perror("Cannot create semaphore");
       exit(1);
       }

    (void)semctl(semshm,0,SETVAL,1);

   /*
    * Creo coda di messaggi READ
    */

    if((dls_qr = msgget((key_t)PT_MSGQR,IPC_CREAT | 0660)) == ERROR) {
	perror("Can't create message queue ID (READ)");
	exit(1);
	}

   /*
    * Creo coda di messaggi WRITE
    */

    if((dls_qw = msgget((key_t)PT_MSGQW,IPC_CREAT | 0660)) == ERROR) {
	perror("Can't create message queue ID (WRITE)");
	exit(1);
	}

   /*
    * Forko e Leggo/Scrivo
    */

    if((dbpid = fork()) == 0) {
      /*
       * Leggo il device e valorizzo la coda di messaggi
       */
       i = 0;
       while(read(dls_fdtty,&c,1) > 0) {
             if(c == PT_ACK) {
                pt_stringa[0] = c;
                pt_stringa[1] = '\0';
                dls_msgstruct.mtype = 1L;
                (void)strcpy(dls_msgstruct.mtext,pt_stringa);
	        if(msgsnd(dls_qr,(void *)&dls_msgstruct,sizeof(pt_stringa),0) == ERROR) {
                   perror("Can't send message queue");
	           exit(1);
	          }
                i = 0;
                continue;
               }

             if(c == PT_ENQ) {
                pt_stringa[0] = c;
                pt_stringa[1] = '\0';
                dls_msgstruct.mtype = 1L;
                (void)strcpy(dls_msgstruct.mtext,pt_stringa);
	        if(msgsnd(dls_qr,(void *)&dls_msgstruct,sizeof(pt_stringa),0) == ERROR) {
                   perror("Can't send message queue");
	           exit(1);
	          }
                i = 0;
                continue;
               }

             if(c == PT_NCK) {
                pt_stringa[0] = c;
                pt_stringa[1] = '\0';
                dls_msgstruct.mtype = 1L;
                (void)strcpy(dls_msgstruct.mtext,pt_stringa);
	        if(msgsnd(dls_qr,(void *)&dls_msgstruct,sizeof(pt_stringa),0) == ERROR) {
                   perror("Can't send message queue");
	           exit(1);
	          }
                i = 0;
                continue;
               }

             if(c == PT_ETX) {
                pt_stringa[i] = '\0';
	        scarto = (char *)malloc(sizeof(char));
                read(dls_fdtty,scarto,1);
		free(scarto);
                dls_msgstruct.mtype = 1L;
                (void)strcpy(dls_msgstruct.mtext,pt_stringa);
	        if(msgsnd(dls_qr,(void *)&dls_msgstruct,sizeof(pt_stringa),0) == ERROR) {
                   perror("Can't send message queue");
	           exit(1);
	          }
                i = 0;
                continue;
               }
              pt_stringa[i] = c;
              i++;
            }
       }
    else {	/* Processo Padre */
          while(TRUE) {
          if(msgrcv(dls_qw,(void *)&dls_msgstruct,sizeof(pt_stringa),1L,0) == ERROR) { /* Errore fatale */
             perror("Fatal error while receiving !");
	     exit(1);
	    }

          lun = strlen(dls_msgstruct.mtext);
          dls_msgstruct.mtext[lun] = pt_bcc(&dls_msgstruct.mtext[1]);
          lun++;

          dls_msgstruct.mtext[lun] = '\0';

          if(write(dls_fdtty,dls_msgstruct.mtext,lun) == ERROR ) {
             perror("Can't write to device");
	     exit(1);
	    }
        }
      }
}

int dls_opentty(char *argv[])
/*---------------------------------------------------------------------------

	dls_opentty:	funzione che apre e setta il device

---------------------------------------------------------------------------*/
{
char tmp[20];
struct termio dls_termio;

   if((dls_device=(IOCONTROL *)malloc(sizeof(IOCONTROL)))==(IOCONTROL *)NULL) {
       perror("Can't allocate memory");
       return(ERROR);
       }

   strcpy(dls_device->dls_tty,argv[1]);
   dls_device->dls_speed = atoi(argv[2]);
   strcpy(dls_device->dls_parity,argv[3]);
   dls_device->dls_bitdata = atoi(argv[4]);
   dls_device->dls_bitstop = atoi(argv[5]);

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
       perror("Can't open device");
       return(ERROR);
       }

   /*
    * Apertura porta OK !
    * Configuro il device con i parametri ricevuti (velocita', parita', ecc..)
    */

    if ( ioctl(dls_fdtty,TCGETA,&dls_copia) == ERROR ) {
	 perror("Can't get terminal characteristics");
	 return(ERROR);
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
				    return(ERROR);
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
				    return(ERROR);
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
				    return(ERROR);
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
				    return(ERROR);
				    break;
                                  }

    dls_termio.c_lflag &= ~ECHO;
    dls_termio.c_lflag &= ~ICANON;
    dls_termio.c_cc[VTIME] = 0;
    dls_termio.c_cc[VMIN] = 1;

    if ( ioctl(dls_fdtty,TCSETA,&dls_termio) == ERROR ) {
	 perror("Can't set terminal characteristics");
	 return(ERROR);
    }
    return(0);
}

char pt_bcc(const char *t)
/*------------------------------------------------------------------------------

     pt_bcc - Calcola il codice di controllo di una stringa generica 
              null-terminata per NOKIA GuestLink

------------------------------------------------------------------------------*/
{
	register char bcc;
  
	bcc = 0;
	while (*t) { bcc = bcc ^ *t++; }

        return(bcc);
}
