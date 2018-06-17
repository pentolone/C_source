#ifndef LINT
static char sccs_bx_attiva_id[] = "@(#)bx_attiva.c Version 1.6 Date 96/06/18 Hour 16:38:47";
#endif
/*---------------------------------------------------------------------------

	bx_attiva.c:  funzione per la gestione automatica dei processi
		      di documentazione addebiti

	Parametri in input - numero di centralini connessi al sistema

---------------------------------------------------------------------------*/
#include "dls_include.h"
#include <sys/wait.h>
#include <pwd.h>
#define STRWARNING    "bx_avvisa.sh"

void dls_start(void);
void controlla_pabx(char *);
pid_t dbpid;
unsigned int time_to_sleep;

int main(int argc, char *argv[])
{
int jj,i,k;
char numero[3];
pid_t devpid[MAXPABX];
pid_t died_pid;
#ifdef ULTRIX
union wait *dls_status;
#else
int dls_status = 0;
#endif
struct passwd *dls_passwd;

    if(argc < 4) {
       fprintf(stderr,"Usage: %s nr. pabx (MAX=20),time warning,device\n",argv[0]);
       exit(1);
       }
    jj = atoi(argv[1]);

    if(jj == 0) {
       fprintf(stderr,"Number of pabx %d INVALID !\n",jj);
       exit(1);
       }

    if(jj > 20) {
       fprintf(stderr,"Number of pabx %d EXCEEDED (MAX=20)!\n",jj);
       exit(1);
       }

    time_to_sleep = atoi(argv[2]);

    if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),IPC_EXCL | IPC_CREAT | 0666)) == ERROR) {
       if(errno != EEXIST) {
          perror("Cannot create Shared Memory ! reason");
          exit(1);
	  }
       else {
          if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0666)) == ERROR) {
              perror("Cannot get shared memory identifier reason");
              exit(1);
              }
           }
       }

    if((shm_pointer = shmat(dls_shmid,(char *)0,0)) == (char *)ERROR) {
       perror("Cannot attache shared memory segment to my process reason");
       exit(1);
       }

    dls_shm_pointer = (DLS_IPC *)shm_pointer;

   /*
    * creo processo per il controllo della corretta acquisizione 
    * delle telefonate (L. Romano 12/09/1994)
    */

    if(fork() == 0) {
       controlla_pabx(argv[3]);
       perror("Function controlla_pabx() died reason");
       exit(1);
       }

    dls_passwd = getpwnam("informix");

    if(setuid(dls_passwd->pw_uid) == ERROR) {
       perror("Can't set user id (informix) reason");
       exit(1);
      }
   /*
    * Inizializzo Shared Memory
    */

    for(i = FALSE;i < MAXPABX;i++)
	dls_shm_pointer->dls_pabx1[i].status1 = 0;

    i = jj - 1;
    dls_shm_pointer->db_status = FALSE;
    dls_shm_pointer->dls_main_pid = getpid();

    for(;i >= 0;i--) {
        dls_shm_pointer->dls_pabx1[i].start1  = (time_t)NULL;
        dls_shm_pointer->dls_pabx1[i].status1 = NOTRUNNING;
        (void)strcpy((char *)dls_shm_pointer->dls_pabx1[i].oper1,"Starting");
        dls_shm_pointer->dls_pabx1[i].lastsend = (time_t)NULL;
        dls_shm_pointer->dls_pabx1[i].msgsend  = 0L;
        dls_shm_pointer->dls_pabx1[i].dls_pid1 = (pid_t)FALSE;
	}

    dls_shm_pointer->start2  = (time_t)NULL;
    dls_shm_pointer->status2 = NOTRUNNING;
    dls_shm_pointer->lastrecv = (time_t)NULL;
    dls_shm_pointer->msgrecv  = 0L;
    dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
    (void)strcpy((char *)dls_shm_pointer->laststr,"  None");

   /*
    * Creo semaforo e setto valido
    */

    semshm = semget((key_t)SEMSHM,1,IPC_CREAT|0666);

    if(semshm == ERROR) {
       perror("Cannot create semaphore");
       exit(1);
       }

    (void)semctl(semshm,0,SETVAL,1);

   /*
    * Creo coda di messaggi
    */

    if((dls_msgid = msgget((key_t)69,IPC_CREAT | 0666)) == ERROR) {
	 perror("Can't create message queue ID");
	 exit(1);
	 }


   /*
    * Lancio i programmi per la lettura del tty (1 x centralino)
    */

    for(i = 1,k = 0;i <= jj;i++,k++) {
	sprintf(numero,"%d",i);
	if((devpid[k] = fork()) == 0) {
	   execl("fglgo","fglgo","bx_device",numero,(char *)NULL);
	   perror("Execl <tty> error");
	   exit(1);
	   }
    }

   /*
    * Lancio il programma di aggiornamento del DataBase (unico)
    */

    if((dbpid = fork()) == 0) {
       execl("fglgo","fglgo","bx_leggi",(char *)NULL);
       perror("Execl <DataBase> error");
       exit(1);
       }

    while(TRUE) {
#ifdef ULTRIX
          died_pid = wait(dls_status);
#else
          died_pid = wait(&dls_status);
#endif
	  if(died_pid == ERROR) {

	     i = jj - 1;
             for(;i >= 0;i--) {
                 dls_shm_pointer->dls_pabx1[i].start1  = (time_t)NULL;
                 dls_shm_pointer->dls_pabx1[i].status1 = NOTRUNNING;
                 (void)strcpy((char *)dls_shm_pointer->dls_pabx1[i].oper1,"Fatal error");
                 dls_shm_pointer->dls_pabx1[i].dls_pid1 = (pid_t)FALSE;
	        }
             dls_shm_pointer->start2  = (time_t)NULL;
             dls_shm_pointer->status2 = NOTRUNNING;
             dls_shm_pointer->dls_pid2 = (pid_t)FALSE;
	     dls_shm_pointer->dls_main_pid = (pid_t)FALSE;
	     break;
	     }

           
	  if(died_pid != dbpid) {
             for(i = 1,k = 0;i <= jj;i++,k++) {
                 if(died_pid == devpid[k]) {
                    dls_shm_pointer->dls_pabx1[k].start1  = (time_t)NULL;
                    dls_shm_pointer->dls_pabx1[k].status1 = NOTRUNNING;
                    dls_shm_pointer->dls_pabx1[k].dls_pid1 = (pid_t)FALSE;
                    (void)strcpy((char *)dls_shm_pointer->dls_pabx1[k].oper1,"Retrying");
                    sleep(10);
	            sprintf(numero,"%d",i);
                    if((devpid[k] = fork()) == 0) {
	               execl("fglgo","fglgo","bx_device",numero,(char *)NULL);
	               perror("Execl <tty> error");
	               exit(1);
                       }
                    break;
	          }
	      }
            continue;
	   }

	  while(TRUE) {
                if(dls_shm_pointer->db_status == FALSE) {
		   dls_start();
		   break;
		   }
		else
		   sleep(10);
		}
	  }

}

void dls_start(void)
/*---------------------------------------------------------------------------

	dls_start:	funzione che riattiva il processo di acquisizione
			terminata la fase di accesso esclusivo

---------------------------------------------------------------------------*/
{

    if((dbpid = fork()) == 0) {
       execl("fglgo","fglgo","bx_leggi",(char *)NULL);
       perror("Execl <DataBase> error");
       }
}

void controlla_pabx(char *devwarning)
/*---------------------------------------------------------------------------

	controlla_pabx:	funzione che controlla (ogni time_to_sleep secondi)
			che l'acquisizione dei messaggi sia corretta

---------------------------------------------------------------------------*/
{
    time_t current_time = (time_t)NULL;
    int    diffe;
    char   tmp[MAXSTRLEN];
    char   tmp1[MAXSTRLEN];
    char   unix_cmd[MAXSTRLEN];

    while(TRUE) {
                 sleep(time_to_sleep);
                 current_time = time((time_t *)NULL);

                 diffe = current_time - dls_shm_pointer->lastrecv;

                 if(diffe > time_to_sleep) {

                   /*
                    * Data/ora ultimo messaggio ricevuto
                    */
                   dls_tm = localtime(&dls_shm_pointer->lastrecv);
                   (void)strftime(tmp,sizeof(tmp),"%A %d %B %Y  %H:%M",dls_tm);

                   /*
                    * Data/ora attuale
                    */

                   dls_tm = localtime(&current_time);
                   (void)strftime(tmp1,sizeof(tmp),"%A %d %B %Y  %H:%M",dls_tm);
                   sprintf(unix_cmd,"%s '%s' '%s' >%s &",STRWARNING,tmp,tmp1,devwarning);
                   system(unix_cmd);
                  }
                }
}
