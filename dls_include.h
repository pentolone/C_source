#ifndef LINT
static char sccs_dls_include_h_id[] = "@(#)dls_include.h Version 1.4 Date 00/02/24 Hour 18:22:43";
#endif
/*---------------------------------------------------------------------------

	dls_include.h:  include Delos per la gestione dei centralini
	Copyright (C) Delos s.r.l. 1993-1994

---------------------------------------------------------------------------*/
#ifndef _DLS_INCLUDE_H	/* protezione per doppia inclusione */
#define _DLS_INCLUDE_H
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <termio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ERROR
#define ERROR (-1)
#endif

/************************************************
  Strutture usate per il controllo degli utenti
************************************************/

#define CHKSHM   257   	/* Shared memory address CHK-USER */
#define CHKSEM   12	/* Semaforo per accesso a Shared Memory CHK-USER */
#define MAXUSER  200	/* Numero massimo di utenti gestibili */

typedef struct def_utente {
			    char	utente[31];
			    pid_t	id_proc;
			  } UTENTE;

typedef struct user_attivi {
			    UTENTE	user_on[MAXUSER];
			    int		attivi;
			    int		licenza;
			   } USER_ON;

char    *shm_utenti_pointer = (char *)NULL;
USER_ON	*shm_utenti         = (USER_ON *) NULL;
int	shm_ute_id          = FALSE;
int     sem_ute_id   	    = FALSE;

struct  sembuf sem_operation={0, -1, SEM_UNDO};

/*************************************************
  Strutture usate per la gestione dei centralini
*************************************************/

#define SHMKEY 268   	/* Shared memory address CENTRALINI */
#define MSGKEY 69   	/* Message queue address CENTRALINI (1) */
#define MSGKEY1 70   	/* Message queue address CENTRALINI (2) */

#define SEMSHM 11	/* Semaforo per accesso a Shared Memory CENTRALINI */

#define MAXPABX 20	/* massimo numero di centralini connessi */

#define MAXSTRLEN 255   /* massimo numero di caratteri per stringa letta */

/*
* Possibili stati del processo di lettura porta
*/
#define CONNECTING  1
#define RUNNING     2
#define NOTRUNNING  3
#define DBEXCLUSIVE 4

signed int dls_fdtty = FALSE;	/* tty file descriptor pointer */
signed int dls_msgid  = FALSE;	/* Message Queue ID */
signed int semshm = FALSE;	/* semaphore identifier (SHM)*/

typedef struct iocontrol {
			  unsigned short dls_npabx;
			  unsigned char  dls_tty[51];
			  unsigned int   dls_speed;
			  unsigned short dls_bitdata;
			  unsigned short dls_bitstop;
			  unsigned char  dls_parity[2];
			  } IOCONTROL;
IOCONTROL *dls_device;

/*
 *	Struttura dati da caricare in Shared Memory
 *	per la comunicazione tra piu' processi
 */

typedef struct dls_pabx {
			 /*
			  * Dati relativi al processo di lettura porta
			  */
			  time_t   start1;
			  unsigned short status1;
			  unsigned char  oper1[80];
			  time_t   lastsend;
			  unsigned long int msgsend;
			  pid_t    dls_pid1;
			  } DLS_PABX;

typedef struct dls_ipc {
			  unsigned short db_status;	/* stato DB */
			  pid_t          dls_main_pid;	/* pid bx_attiva */
			  DLS_PABX       dls_pabx1[MAXPABX];
			 /*
			  * Dati relativi al processo di lettura coda
			  * e aggiornamento database
			  */
			  time_t   start2;
			  unsigned short status2;
			  time_t   lastrecv;
			  unsigned long int msgrecv;
			  pid_t    dls_pid2;
			  unsigned char laststr[MAXSTRLEN];
			} DLS_IPC;

char *shm_pointer = (char *)NULL;
DLS_IPC *dls_shm_pointer = (DLS_IPC *)NULL;

signed int dls_shmid = FALSE;	/* Shared Memory pointer */

struct sembuf locktalk = {0,-1,SEM_UNDO};       /* struct semafori per I P C
								       n r o
								       t o m
								       e c m
								       r e u
								         s n
								         s i
								           c
								           a
								           t
								           i
								           o
								           n */

/* Descrizione dei segnali */
char *c_segnali[] =      {"SIGHUP",
                          "SIGINT",
                          "SIGQUIT",
                          "SIGILL",
                          "SIGTRAP",
                          "SIGIOT",
                          "SIGEMT",
                          "SIGFPE",
                          "SIGKILL",
                          "SIGBUS",
                          "SIGSEGV",
                          "SIGSYS",
                          "SIGPIPE",
                          "SIGALRM",
                          "SIGTERM",
                          "SIGURG",
                          "SIGSTOP",
                          "SIGTSTP",
                          "SIGCONT",
                          "SIGCHLD",
                          "SIGTTIN",
                          "SIGTTOU",
                          "SIGIO",
                          "SIGXCPU",
                          "SIGXFSZ",
                          "SIGVTALRM",
                          "SIGPROF",
                          "SIGWINCH",
                          "SIGLOST",
                          "0"};

time_t ora = (time_t)NULL;

void dls_error(char *,char *);
void dls_sem_lock(unsigned int);
void dls_sem_unlock(unsigned int);
void dls_abort(int);
void dls_abort1(int);
void dls_setsig(short);
void nil(int);

unsigned short shm_idx = FALSE;
struct termio dls_copia;
struct tm *dls_tm;

/*
 * Definizione parametri utilizzati dal check-in/check_out del telefono
 * eliminata in data 19/09/1994 per problemi di occupazione memoria
 * (L. Romano)
 */

#endif /* fine ifndef _DLS_INCLUDE_H */
