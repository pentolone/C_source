#ifndef LINT
static char sccs_bx_fun_id[] = "@(#)check_shm.c	1.3 Date 00/02/21 Hour 11:47:10";
#endif
/*---------------------------------------------------------------------------

	check_shm.c:  funzione per il controllo dello stato dei processi
		      di documentazione addebiti

	Parametri in input (nessuno)

---------------------------------------------------------------------------*/
#ifdef ULTRIX
#include <cursesX.h>
#else
#include <curses.h>
#endif
#include "dls_include.h"

#ifndef ACS_VLINE
#define ACS_VLINE '*'
#endif

#ifndef ACS_HLINE
#define ACS_HLINE '*'
#endif

int main(void)
{
char tmp[90];
char c;
unsigned int inx = FALSE;
signed int shm_flag;
char stringa[MAXSTRLEN];
char *punta;
WINDOW *my_win,*newwin();

    if(dls_shm_pointer == (DLS_IPC *)NULL) { /* accedo Shared memory */

       if((dls_shmid = shmget((key_t)SHMKEY,sizeof(DLS_IPC),0660)) == ERROR) {
          fprintf(stderr,"Shared Memory NOT OPENED !\n");
	  fprintf(stderr,"Premi return ");
	  getchar();
          return(1);
          }

#ifdef ULTRIX
shm_flag = SM_RDONLY;
#else
shm_flag = SHM_RDONLY;
#endif
       if((shm_pointer = shmat(dls_shmid,(char *)0,shm_flag)) == (char *)ERROR) {
          fprintf(stderr,"Cannot attache shared memory segment to my process\n");
	  fprintf(stderr,"Premi return ");
	  getchar();
          return(1);
          }

       dls_shm_pointer = (DLS_IPC *)shm_pointer;

      /*
       * prima volta: valorizzo Shared Memory
       */

       }

    initscr();
    my_win = newwin(23,77,1,1);

    savetty();
    nocrmode();
    noecho();
    clear();

       while(TRUE) {
            /*
             * Processo di lettura Shared Memory
             */

             if(dls_shm_pointer->dls_pabx1[shm_idx].status1 == FALSE)
	        shm_idx = FALSE;

	     inx = shm_idx + 1;

             wmove(my_win,0,0);
             wattron(my_win,A_REVERSE);
             wprintw(my_win,"               MONITOR STATUS OF PABX NUMBER %02d  MAIN PID = %d          ",inx,dls_shm_pointer->dls_main_pid);
             wattroff(my_win,A_REVERSE);

             if(dls_shm_pointer->dls_pabx1[shm_idx].start1 != (time_t)NULL) {
                dls_tm = localtime(&dls_shm_pointer->dls_pabx1[shm_idx].start1);
                strftime(tmp,sizeof(tmp),"%a %d/%b/%Y %H:%M",dls_tm);
	       }
	     else
	       strcpy(tmp,"Process NOT Running !");

             wmove(my_win,2,2);
             wprintw(my_win,"Process (READTTY) Started at       : %-30s",tmp);
             wmove(my_win,3,2);
             wprintw(my_win,"Process (READTTY) current STATUS   : ");

             wmove(my_win,3,39);
	     wattron(my_win,A_REVERSE);
             switch(dls_shm_pointer->dls_pabx1[shm_idx].status1) {

	            case CONNECTING:
			      wprintw(my_win,"trying connection          ");
			      break;
	            case RUNNING:
			      wprintw(my_win,"RUNNING                    ");
			      break;
	            case NOTRUNNING:
	     		      wattron(my_win,A_BLINK);
			      wprintw(my_win,"WARNING ! NOT RUNNING******");
	     		      wattroff(my_win,A_BLINK);
			      break;
			      }
	     wattroff(my_win,A_REVERSE);
             wmove(my_win,5,2);
             wprintw(my_win,"Process (READTTY)  PID             : %d       ",dls_shm_pointer->dls_pabx1[shm_idx].dls_pid1);
             wmove(my_win,6,2);
             wprintw(my_win,"Number of messages SENT            : %ld       ",dls_shm_pointer->dls_pabx1[shm_idx].msgsend);

             if(dls_shm_pointer->dls_pabx1[shm_idx].lastsend != (time_t)NULL) {
                dls_tm = localtime(&dls_shm_pointer->dls_pabx1[shm_idx].lastsend);
                strftime(tmp,sizeof(tmp),"%a %d/%b/%Y %H:%M",dls_tm);
	       }
	     else
	       strcpy(tmp,"No message SENT !");

             wmove(my_win,7,2);
             wprintw(my_win,"Last message       SENT AT         : %-30s",tmp);
             wmove(my_win,9,2);
             wprintw(my_win,"Last operation done                : %-30s",dls_shm_pointer->dls_pabx1[shm_idx].oper1);
            /*
             * Processo di lettura coda di messaggi
             */

             if(dls_shm_pointer->start2 != (time_t)NULL) {
                dls_tm = localtime(&dls_shm_pointer->start2);
                strftime(tmp,sizeof(tmp),"%a %d/%b/%Y %H:%M",dls_tm);
	       }
	     else
	       strcpy(tmp,"Process NOT Running !");

             wmove(my_win,12,2);
             wprintw(my_win,"Process (READQUEUE) Started at     : %-30s",tmp);
             wmove(my_win,13,2);
             wprintw(my_win,"Process (READQUEUE) current STATUS : ");

             wmove(my_win,13,39);
	     wattron(my_win,A_REVERSE);
             switch(dls_shm_pointer->status2) {
	            case RUNNING:
			      wprintw(my_win,"RUNNING                    ");
			      break;

	            case NOTRUNNING:
	     		      wattron(my_win,A_BLINK);
			      wprintw(my_win,"WARNING ! NOT RUNNING******");
	     		      wattroff(my_win,A_BLINK);
			      break;

	            case DBEXCLUSIVE:
	     		      wattron(my_win,A_BLINK);
			      wprintw(my_win,"LOCK REQUESTED !           ");
	     		      wattroff(my_win,A_BLINK);
			      break;
			      }
             wmove(my_win,15,2);
	     wattroff(my_win,A_REVERSE);
             wprintw(my_win,"Process (READQUEUE) PID            : %d       ",dls_shm_pointer->dls_pid2);
             wmove(my_win,16,2);
             wprintw(my_win,"Number of messages  RECEIVED       : %ld       ",dls_shm_pointer->msgrecv);

             if(dls_shm_pointer->lastrecv != (time_t)NULL) {
                dls_tm = localtime(&dls_shm_pointer->lastrecv);
                strftime(tmp,sizeof(tmp),"%a %d/%b/%Y %H:%M",dls_tm);
	       }
	     else
	       strcpy(tmp,"No message RECEIVED !");
             wmove(my_win,17,2);
             wprintw(my_win,"Last message        RECEIVED AT    : %-30s",tmp);
             wmove(my_win,19,2);
             wprintw(my_win,"Last message received:");
             wmove(my_win,20,0);
	     wattron(my_win,A_BOLD);
             strcpy(stringa,(char *)&dls_shm_pointer->laststr[2]);
	     if((punta=strrchr(stringa,'\r')) != (char *)NULL)
	       *punta = ' ';
             wprintw(my_win,"%-80s",stringa);
	     /*
             wprintw(my_win,"%-80s",&dls_shm_pointer->laststr[2]);
	     */
	     wattroff(my_win,A_BOLD);
             wrefresh(my_win);
	     sleep(1);

	     if(shm_idx < MAXPABX)
	        shm_idx++;
	     else
	        shm_idx = FALSE;
    }
}
