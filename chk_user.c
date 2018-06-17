#ifndef LINT
static char sccs_id[]="Module @(#)chk_user.c Version 1.1 Date 00/02/21 Hour 11:47:33";
#endif
/*******************************************************************************

   chk_user - Demone di controllo accessi alla procedura Informix 4gl

   Copyright(C) Delos srl 1994

*******************************************************************************/
#include "dls_include.h"

#ifndef MAX_DLS_USER
#define MAX_DLS_USER 4
#endif

int main(int argc,char *argv[])
{
int	ind;
void    uscita();

   /************************
     Se gia' esiste la SHM
     controllo sul doppio
     lancio
   ************************/

   if(shmget(CHKSHM,sizeof(USER_ON),0660) != -1) {
      fprintf(stderr,"Errore : Demone gia' attivo !!!\n");
      exit(-1);
   }

   /****************************
     Get della porzione di SHM
   ****************************/

   if((shm_ute_id=shmget(CHKSHM,sizeof(USER_ON),IPC_EXCL|IPC_CREAT|0666))== -1){
      perror("Errore nella creazione della Share Memory !!\nCausa ");
      exit(-1);
   }

   if((shm_utenti_pointer=shmat(shm_ute_id,(char *)0,0)) == (char *)-1) {
      perror("Errore nel prendere la porzione di Share Memory !\nCausa ");
      exit(-1);
   }

   shm_utenti = (USER_ON *)shm_utenti_pointer;

   /********************
     Normalizzo la SHM
   ********************/

   for(ind=0;ind < MAXUSER;ind++) {
       memset((void *)&(shm_utenti->user_on[ind]),0,sizeof(UTENTE));
       shm_utenti->user_on[ind].id_proc = 0;
   }
      
   /*********************************
     Aggiorno i parametri di utenza
     della share memory
   *********************************/

   shm_utenti->attivi  = 0;
   shm_utenti->licenza = MAX_DLS_USER;

   /*********************************
     Creo semaforo di controllo SHM
   *********************************/

   if((sem_ute_id=semget(CHKSEM,1,IPC_CREAT | IPC_EXCL | 0660)) == -1) {
      perror("Errore di creazione semaforo di controllo Share Memory\nCausa ");
      exit(-1);
   }

   /**********************
     Sblocco il semaforo
   **********************/

   (void)semctl(sem_ute_id,0,SETVAL,1);

   signal(SIGHUP,SIG_IGN);
   signal(SIGQUIT,SIG_IGN);

   signal(SIGTERM,uscita);
   pause();
}

/*******************************************************************************

   uscita - Funzione di cattura segnale SIGTERM

*******************************************************************************/
void uscita(unsigned int my_segnale)
{
     (void)semctl(sem_ute_id,0,IPC_RMID,0);
     (void)shmctl(shm_ute_id,IPC_RMID,(struct shmid_ds *)NULL);
     fprintf(stderr,"ATTENZIONE : Abort del demone di gestione login\n");
}
