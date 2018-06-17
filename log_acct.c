#ifndef LINT
static char sccs_id[]="Module @(#)log_acct.c	1.2 Date 00/02/21 Hour 11:51:04";
#endif
/*******************************************************************************

   log_acct - Programma che mediante utilizzo file di sistema wtmp
	      elabora un report di statistica

*******************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <utmp.h>
#include <time.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_HOSTS 100
#define MAX_RIGHE 500
#define TRUE      1
#define FALSE     0
#define NOTFOUND  "---------------"
#define BLANK     "               "

typedef struct {
		char user[10];			/* Utente		      */
		char line[10];			/* Linea		      */
		char host[16];          	/* Host		              */
		long inizio;		        /* Ora inizio collegamento    */
	       } ACCT;

typedef struct {
		char user[10];			/* Utente		      */
		char data[15];			/* Data		              */
		char host[16];          	/* Host		              */
		long tempo;		        /* Ora inizio collegamento    */
	       } SUMM;

ACCT        acct[MAX_USERS];		/* Struct con record memorizzati      */
SUMM        summ[MAX_RIGHE];		/* Struct con record x somma          */
char	    hosts[MAX_HOSTS][50];	/* Hosts da controllare	              */
char	    users[MAX_USERS][50];	/* Users da controllare	              */
int	    ok_ck_hosts;		/* Flag di controllo su alcuni hosts  */
int	    ok_ck_users;		/* Flag di controllo su alcuni users  */
FILE        *fp;			/* Puntatore al file wtmp	      */
int         n_byte;			/* N^ di record letti		      */
int         ind;                        /* Comodo			      */
int         ind1;                       /* Comodo			      */
struct utmp record;			/* Record wtmp letto		      */
struct tm   *ora_locale;		/* Ora locale			      */
time_t	    ora_corrente;		/* Ora corrente			      */
time_t	    delta;			/* Ora corrente			      */
int	    delta_H;			/* Ore di collegamento		      */
int	    delta_M;			/* Minuti di collegamento	      */
int         delta_S;			/* Secondi di collegamento	      */
int	    ok_debug;			/* Flag OK per debug		      */
int	    video;			/* Flag OK per video		      */
char        *pointer;			/* Comodo			      */
char	    orarioi[256];		/* Stringa con l'ora da stampare      */
char	    orariof[256];		/* Stringa con l'ora da stampare      */
char        ck_line[50];		/* linea da controllare	              */
char	    ck_data[50];		/* Data da controllare		      */
char	    file_to_open[50];		/* File da aprire		      */
int	    sumdate;			/* Group by linea		      */
int	    sumhost;			/* Group by host		      */
int 	    sumuser;			/* Group by utente		      */
char	    machine[50];		/* Nome dell'Host di provenienza      */
char        *strstr();
struct tm   *localtime();
size_t      fread();
void        *memset();
void        *memcpy();
void	    exit();
void	    perror();
void        tratto();

main(int argc,char *argv[])
{

   gethostname(machine,sizeof(machine));

   /************************************
     Valorizzazione a NULL della struct
   ************************************/

   for(ind=0;ind < MAX_USERS;ind++) {
       memset((void *)&acct[ind],0,sizeof(ACCT));
   }

   for(ind=0;ind < MAX_RIGHE;ind++) {
       memset((void *)&acct[ind],0,sizeof(ACCT));
   }

   memset((void *)hosts,0,sizeof(hosts));
   memset((void *)file_to_open,0,sizeof(file_to_open));

   ck_line[0] = 0;
   ck_data[0] = 0;
   sumdate    = 0;
   sumhost    = 0;
   sumuser    = 0;
   video      = 0;

   argc--;
   while(argc > 0) {
         /**************************************
           Controllo se devo visualizzare HELP
         **************************************/
       
         if(strcmp(argv[argc],"-h") == 0) {
	    printf("%s: usage:\n",argv[0]);
	    printf("	%s [-fFILE_NAME] Legge dati dal file dato\n",argv[0]);
	    printf("	%s [-uUSER_NAME] Login dell'utente digitato\n",argv[0]);

#ifndef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      NON SIAMO SU DPX
*******************************************************************************/

	    printf("	%s [-hHOST_NAME] Login dell'host   digitato\n",argv[0]);

/******************************************************************************/
#endif

	    printf("	%s [-dYYMMDD]    Login nella data  digitata\n",argv[0]);
	    printf("	%s [-sumuser]    Somma i tempi di Login per utente\n",
		    argv[0]);

#ifndef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      NON SIAMO SU DPX
*******************************************************************************/

	    printf("	%s [-sumhost]    Somma i tempi di Login per host\n",
		    argv[0]);

/******************************************************************************/
#endif
	    printf("	%s [-sumdate]    Somma i tempi di Login per data\n",
		    argv[0]);
	    exit(0);
	 }

         /**************************************
           Controllo se output come file o video 
         **************************************/
       
         if(strcmp(argv[argc],"video") == 0) {
            video = TRUE;
	    argc--;
	    continue;
	 }
 
         /**************************************
           Controllo se devo attivare il DEBUG
         **************************************/
       
         if(strcmp(argv[argc],"debug") == 0) {
            ok_debug = TRUE;
	    argc--;
	    continue;
	 }
 
         /*****************************************
           Controllo se e' stato dato nome utente
         *****************************************/
       
         if((pointer=strstr(argv[argc],"-u")) != (char *)NULL) {
	    pointer = pointer+2;
	    ok_ck_users = TRUE;
	    for(ind=0; ind < MAX_HOSTS; ind++)
		if(strlen(users[ind]) == 0) {
	           strcpy(users[ind],pointer);
		   break;
		}
            argc--;
	    continue;
	 }
            
#ifndef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      NON SIAMO SU DPX
*******************************************************************************/

         /****************************************************
           Controllo se e' stata data rete da non controllare
         ****************************************************/
       
         if((pointer=strstr(argv[argc],"-h")) != (char *)NULL) {
	    pointer = pointer + 2;
	    ok_ck_hosts = TRUE;
	    for(ind=0; ind < MAX_HOSTS; ind++)
		if(strlen(hosts[ind]) == 0) {
	           strcpy(hosts[ind],pointer);
		   break;
		}
            argc--;
	    continue;
	 }

/******************************************************************************/
#endif
 
         /*************************************
           Controllo se e' stata data una linea
         *************************************/
       
         if((pointer=strstr(argv[argc],"-l")) != (char *)NULL) {
	    pointer = pointer + 2;
	    strcpy(ck_line,pointer);
            argc--;
	    continue;
	 }

         /*************************************
           Controllo se e' stato data un file
         *************************************/
       
         if((pointer=strstr(argv[argc],"-f")) != (char *)NULL) {
	    pointer = pointer + 2;
	    strcpy(file_to_open,pointer);
	    printf("file_to_open %s\n",file_to_open);
            argc--;
	    continue;
	 }

         /*************************************
           Controllo se e' stato data una data
         *************************************/
       
         if((pointer=strstr(argv[argc],"-d")) != (char *)NULL) {
	    pointer = pointer + 2;
	    strcpy(ck_data,pointer);
            argc--;
	    continue;
	 }

         /*************************************
           Controllo se devo sommare x utente
         *************************************/
       
         if((pointer=strstr(argv[argc],"-sumuser")) != (char *)NULL) {
	    sumuser = TRUE;
            argc--;
	    continue;
	 }

         /*************************************
           Controllo se devo sommare x date
         *************************************/
       
         if((pointer=strstr(argv[argc],"-sumdate")) != (char *)NULL) {
	    sumdate = TRUE;
            argc--;
	    continue;
	 }

#ifndef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      NON SIAMO SU DPX
*******************************************************************************/

         /*************************************
           Controllo se devo sommare x host
         *************************************/
       
         if((pointer=strstr(argv[argc],"-sumhost")) != (char *)NULL) {
	    sumhost = TRUE;
            argc--;
	    continue;
	 }
/******************************************************************************/
#endif

	 argc--;
      
   }

   /************************
     Se non ho dato file
     prendo wtmp standard
   ************************/

   if(file_to_open[0] == '\0')
      strcpy(file_to_open,WTMP_FILE);

   /*************************
     Apertura del file wtmp
   *************************/

   if((fp=fopen(file_to_open,"r")) == (FILE *)NULL) {
      perror("Errore open");
      exit(-1);
   }

   /***************
     Leggo il file
   ***************/

   tratto();

   fclose(fp);
}

/*******************************************************************************

   tratto - Routine di gestione lettura e print dati 

*******************************************************************************/
void tratto()
{
time_t time();
char   orario[256];
char   como[256];
char   line[20];
char   name[10];
char   host[20];
long   tempo;
int    type_of_sum;
int    found;


   /*********************************************
     Costruisco la variabile che mi permette di 
     identificare il raggruppamento di somma
   *********************************************/

   type_of_sum = 0;

   if(sumuser == TRUE )
      type_of_sum = type_of_sum + 100;

   if(sumdate == TRUE )
      type_of_sum = type_of_sum + 10;

   if(sumhost == TRUE )
      type_of_sum = type_of_sum + 1;

   /*******************
     Ciclo di lettura
   *******************/

   while((n_byte = fread((void *)&record,sizeof(struct utmp),1,fp)) == 1) {

	 memset((void *)line,0,sizeof(line));
	 memset((void *)name,0,sizeof(name));
	 memset((void *)host,0,sizeof(host));
	 memset((void *)&tempo,0,sizeof(tempo));

	 memcpy((void *)line,(void *)record.ut_line,sizeof(record.ut_line));
	 memcpy((void *)name,(void *)record.ut_name,sizeof(record.ut_name));

#ifndef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      NON SIAMO SU DPX
*******************************************************************************/

	 memcpy((void *)host,(void *)record.ut_host,sizeof(record.ut_host));

/******************************************************************************/
#endif

	 memcpy((void *)&tempo,(void *)&record.ut_time,sizeof(record.ut_time));

	 /***************************************
	   Scarto messaggi che non interessano
	 ***************************************/

         if(strcmp(line,OTIME_MSG) == 0 ||
	    strcmp(line,NTIME_MSG) == 0   )
	    continue;


#ifndef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      NON SIAMO SU DPX
*******************************************************************************/

         if(strlen(name)          != 0 &&
	    strcmp(line,BOOT_MSG) != 0   ) {

/******************************************************************************/
#else
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      SIAMO SU DPX
*******************************************************************************/

	 if(strlen(name)           != 0 &&
	    record.ut_type         == 7 && 
	    strcmp(line,BOOT_MSG)  != 0   ) {
	    
/******************************************************************************/
#endif

	    /********************************
	      Se ricerco utente particolare 
	      scarto gli altri
	    ********************************/

	    found = TRUE;
	    if(ok_ck_users == TRUE) {
	       found = FALSE;
	       for(ind1=0,found=FALSE;ind1 < MAX_USERS && !found;ind1++) {
		   if(strlen(users[ind1]) == 0)
		      break;

		   if(strcmp(users[ind1],name) == 0) {
		      found = TRUE;
		   }
	       }
	    }

	    if(found == FALSE)
	       continue;

	    /********************************
	      Se ricerco hosts particolari 
	      scarto gli altri
	    ********************************/

	    found = TRUE;
	    if(ok_ck_hosts == TRUE) {
	       found = FALSE;
	       for(ind1=0,found=FALSE;ind1 < MAX_HOSTS && !found;ind1++) {
		   if(strlen(hosts[ind1]) == 0)
		      break;

		   if(strcmp(hosts[ind1],host) == 0) {
		      found = TRUE;
		   }
	       }
	    }

	    if(found == FALSE)
	       continue;

	    /***********************************
	      Se ricerco line/lines particolare 
	      scarto le altre
	    ***********************************/

	    if(strlen(ck_line)                 > 0 ) {
	       if(strstr(line,ck_line) == (char *)NULL)
	          continue;
	    }

	    /********************************
	      Se ricerco data particolare 
	      scarto le altre
	    ********************************/

	    if(strlen(ck_data)                 > 0 ) {
               ora_locale   = localtime((time_t *)&tempo);
               strftime(como,256,"%y%m%d",ora_locale);
	       if(strcmp(como,ck_data) != 0   )
	          continue;
	    }

	    /*******************************************
	      Memorizzo record di inizio collegamento
	    *******************************************/

	    found = FALSE;

            /***************************************************************
	      Se ho gia' la struttura in memoria aggiorno data collegamento
            ***************************************************************/

	    for(ind=0;ind < MAX_USERS; ind++) {
	        if(strcmp(acct[ind].user,name) == 0 &&
		   strcmp(acct[ind].line,line) == 0 &&
		   strcmp(acct[ind].host,host) == 0   ) {
		   acct[ind].inizio = tempo;
		   found = TRUE;
		   break;
	        }
	    }

	    if(found == FALSE) {
	       for(ind=0;ind < MAX_USERS; ind++) {
	           if(strlen(acct[ind].user) == 0) {
		      strcpy(acct[ind].user,name);
		      strcpy(acct[ind].line,line);
		      strcpy(acct[ind].host,host);
		      acct[ind].inizio = tempo;
		      break;
	           }
	       }
	    }
         } else {

	    if(strcmp(line,BOOT_MSG)  == 0 ) {
	      for(ind=0;ind < MAX_USERS; ind++) {
                 if(strlen(acct[ind].user) > 0) {

		   if(video) {
		      /************************************
		        Riempio a spazio i campi di print
		      ************************************/
   
		      while(strlen(acct[ind].user) < 8)
		            strcat(acct[ind].user," ");
   
		      while(strlen(acct[ind].line) < 8)
		            strcat(acct[ind].line," ");
   
		      while(strlen(acct[ind].host) < 15)
		            strcat(acct[ind].host," ");
		   }
   
		   /*******************
		     Tronco alla fine
		   *******************/

		   acct[ind].user[8] = 0;
		   acct[ind].line[8] = 0;
		   acct[ind].host[15] = 0;

                   /*******************************
		     Mi proteggo su orario errato
                   *******************************/

		   if(tempo < acct[ind].inizio)
		      tempo = acct[ind].inizio;

                   /********************************************
		     Rendo printabile le date di login e logout
		   ********************************************/

                   ora_locale   = localtime((time_t *)&acct[ind].inizio);

		   if(video)
                      strftime(orarioi,256,"%d/%m %H:%M",ora_locale);
		   else
                      strftime(orarioi,256,"%d/%m/%y|%H:%M",ora_locale);

                   ora_locale   = localtime((time_t *)&tempo);

		   if(video)
                      strftime(orariof,256,"%d/%m %H:%M",ora_locale);
		   else
                      strftime(orariof,256,"%d/%m/%y|%H:%M",ora_locale);

                   if(strcmp(acct[ind].host,BLANK) == 0)
		      strcpy(acct[ind].host,NOTFOUND);

                   /********************************
		     Calcolo tempo di collegamento
		   ********************************/

		   delta = tempo - acct[ind].inizio;
		   delta_H = delta / 3600;
		   delta_M = (delta % 3600) / 60;
		   delta_S = ((delta % 3600) % 60);

		   /************************************************
		     Non stampo dettaglio se ho richiesto la somma
		   ************************************************/

		   if(type_of_sum == 0) {
		      if (video)
                          printf("%s %s %s Login %s Logout %s %02d:%02d Reboot\n",
			         acct[ind].user,
			         acct[ind].line,
			         acct[ind].host,
			         orarioi,
			         orariof,
			         delta_H,
			         delta_M);
		      else
                          printf("%s|%s|%s|%s|%s|%02d:%02d|%06ld|%s|\n",
			         acct[ind].user,
			         acct[ind].line,
			         acct[ind].host,
			         orarioi,
			         orariof,
			         delta_H,
			         delta_M,
				 delta,
				 machine);
		   } else {
		     /****************
		       SOMMO I TEMPI
		     ****************/

		     for(ind1=0,found=0;ind1 < MAX_RIGHE && !found;ind1++) {

                         ora_locale   = localtime((time_t *)&acct[ind].inizio);
                         strftime(como,256,"%d/%m/%y",ora_locale);

			 if(strlen(summ[ind1].user) == 0) {
			    strcpy(summ[ind1].user,acct[ind].user);
			    strcpy(summ[ind1].data,como);
			    strcpy(summ[ind1].host,acct[ind].host);
			    summ[ind1].tempo = delta;
			    break;
			 }
			 /******************************************
			   Controllo il tipo di somma da effettuare
			 ******************************************/

			 switch(type_of_sum) {
				case 1:
				/*****************
				  Host
				*****************/

				if(strcmp(summ[ind1].host,acct[ind].host) == 0){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 10:
				/*****************
				  Date 
				*****************/

				if(strcmp(summ[ind1].data,como) == 0 ) {
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 11:
				/*****************
				  Date Host
				*****************/

				if(strcmp(summ[ind1].data,como)          ==0 &&
				   strcmp(summ[ind1].host,acct[ind].host)==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 100:
				/*****************
				  User 
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user) == 0){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 101:
				/*****************
				  User Host
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user)==0 &&
				   strcmp(summ[ind1].host,acct[ind].host)==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;
		      
				case 110:
				/*****************
				  User Date 
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user)==0 &&
				   strcmp(summ[ind1].data,como)          ==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 111:
				/*****************
				  User Date Host
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user)==0 &&
				   strcmp(summ[ind1].data,como)          ==0 &&
				   strcmp(summ[ind1].host,acct[ind].host)==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;
			 }

		     }
		   } 

		   /************************
		     Azzero record trattato
		   ************************/

		   acct[ind].user[0] = 0;
		   acct[ind].line[0] = 0;
		   acct[ind].host[0] = 0;
		   acct[ind].inizio  = 0;
                }
	      }
	    }

#ifdef DPX
/*******************************************************************************
      LA SEGUENTE PORZIONE DI CODICE E'INCLUSA NELL'ESEGUIBILE SOLO SE
      SIAMO SU DPX
*******************************************************************************/

	    if(record.ut_type != 8 &&
	       record.ut_type != 7   )
	       continue;
	    
	    if(strlen(line) < 1)
	       continue;

/******************************************************************************/
#endif

	    /********************************************
	      Cerco in struct l'inizio del collegamento
	    ********************************************/

	    for(ind=0;ind < MAX_USERS; ind++) {
	        if(strcmp(acct[ind].line,line) == 0) {
		   if(video) {
		      /************************************
		        Riempio a spazio i campi di print
		      ************************************/
   
		      while(strlen(acct[ind].user) < 8)
		            strcat(acct[ind].user," ");
   
		      while(strlen(acct[ind].line) < 8)
		            strcat(acct[ind].line," ");
   
		      while(strlen(acct[ind].host) < 15)
		            strcat(acct[ind].host," ");
		   }
   
		   /*******************
		     Tronco alla fine
		   *******************/

		   acct[ind].user[8] = 0;
		   acct[ind].line[8] = 0;
		   acct[ind].host[15] = 0;

                   /********************************************
		     Rendo printabile le date di login e logout
		   ********************************************/

                   ora_locale   = localtime((time_t *)&acct[ind].inizio);

		   if(video)
                      strftime(orarioi,256,"%d/%m %H:%M",ora_locale);
		   else
                      strftime(orarioi,256,"%d/%m/%y|%H:%M",ora_locale);

                   ora_locale   = localtime((time_t *)&tempo);

		   if(video)
                      strftime(orariof,256,"%d/%m %H:%M",ora_locale);
		   else
                      strftime(orariof,256,"%d/%m/%y|%H:%M",ora_locale);

                   if(strcmp(acct[ind].host,BLANK) == 0)
		      strcpy(acct[ind].host,NOTFOUND);

                   /********************************
		     Calcolo tempo di collegamento
		   ********************************/

		   delta = tempo - acct[ind].inizio;
		   delta_H = delta / 3600;
		   delta_M = (delta % 3600) / 60;
		   delta_S = ((delta % 3600) % 60);

		   /************************************************
		     Non stampo dettaglio se ho richiesto la somma
		   ************************************************/

		   if(type_of_sum == 0) {
		      if (video)
                          printf("%s %s %s Login %s Logout %s %02d:%02d\n",
			         acct[ind].user,
			         acct[ind].line,
			         acct[ind].host,
			         orarioi,
			         orariof,
			         delta_H,
			         delta_M);
		      else
                          printf("%s|%s|%s|%s|%s|%02d:%02d|%06ld|%s|\n",
			         acct[ind].user,
			         acct[ind].line,
			         acct[ind].host,
			         orarioi,
			         orariof,
			         delta_H,
			         delta_M,
				 delta,
				 machine);
		   } else {
		     /****************
		       SOMMO I TEMPI
		     ****************/

		     for(ind1=0,found=0;ind1 < MAX_RIGHE && !found;ind1++) {

                         ora_locale   = localtime((time_t *)&acct[ind].inizio);
                         strftime(como,256,"%d/%m/%y",ora_locale);

			 if(strlen(summ[ind1].user) == 0) {
			    strcpy(summ[ind1].user,acct[ind].user);
			    strcpy(summ[ind1].data,como);
			    strcpy(summ[ind1].host,acct[ind].host);
			    summ[ind1].tempo = delta;
			    break;
			 }
			 /******************************************
			   Controllo il tipo di somma da effettuare
			 ******************************************/

			 switch(type_of_sum) {
				case 1:
				/*****************
				  Host
				*****************/

				if(strcmp(summ[ind1].host,acct[ind].host) == 0){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 10:
				/*****************
				  Date 
				*****************/

				if(strcmp(summ[ind1].data,como) == 0 ) {
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 11:
				/*****************
				  Date Host
				*****************/

				if(strcmp(summ[ind1].data,como)          ==0 &&
				   strcmp(summ[ind1].host,acct[ind].host)==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 100:
				/*****************
				  User 
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user) == 0){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 101:
				/*****************
				  User Host
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user)==0 &&
				   strcmp(summ[ind1].host,acct[ind].host)==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;
		      
				case 110:
				/*****************
				  User Date 
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user)==0 &&
				   strcmp(summ[ind1].data,como)          ==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;

				case 111:
				/*****************
				  User Date Host
				*****************/

				if(strcmp(summ[ind1].user,acct[ind].user)==0 &&
				   strcmp(summ[ind1].data,como)          ==0 &&
				   strcmp(summ[ind1].host,acct[ind].host)==0  ){
				   summ[ind1].tempo = summ[ind1].tempo+delta;
				   found = TRUE;
				}
				break;
			 }

		     }
		   } 

		   /************************
		     Azzero record trattato
		   ************************/

		   acct[ind].user[0] = 0;
		   acct[ind].line[0] = 0;
		   acct[ind].host[0] = 0;
		   acct[ind].inizio  = 0;
		   break;
                }
            } 
         }
   }

   if(type_of_sum == 0) {
      /***********************
        Utenti ancora logati
      ***********************/
   
      for(ind=0;ind < MAX_USERS; ind++) {
          if(strlen(acct[ind].user) > 0) {
	     if(video) {
	        /************************************
	          Riempio a spazio i campi di print
	        ************************************/
      
	        while(strlen(acct[ind].user) < 8)
		      strcat(acct[ind].user," ");
      
	        while(strlen(acct[ind].line) < 8)
		      strcat(acct[ind].line," ");
      
	        while(strlen(acct[ind].host) < 15)
		      strcat(acct[ind].host," ");
	     }
   
	     /*******************
	       Tronco alla fine
	     *******************/
   
	     acct[ind].user[8] = 0;
	     acct[ind].line[8] = 0;
	     acct[ind].host[15] = 0;
   
             /***********************************
	       Rendo printabile le date di login 
	     ***********************************/
   
             ora_corrente = time((time_t *)NULL);
             ora_locale   = localtime(&ora_corrente);
   
             strftime(orario,256,"%D",ora_locale);
   
             ora_locale   = localtime((time_t *)&acct[ind].inizio);
             strftime(orarioi,256,"%D",ora_locale);
   
	     /***********************
	       Scarto record appesi
	     ***********************/
   
	     if(strcmp(orario,orarioi) != 0)
	        continue;
   
             ora_locale   = localtime((time_t *)&acct[ind].inizio);
             strftime(orarioi,256,"%d/%m %H:%M",ora_locale);
   
	     if(video)
                printf("%s %s %s Login %s Ancora collegato\n",
			        acct[ind].user,
			        acct[ind].line,
			        acct[ind].host,
			        orarioi);
          }
      }
   } else {
     if(video) {
        for(ind =0;ind < MAX_RIGHE;ind ++) {
             if(strlen(summ[ind].user) > 0) {
	        /************************************
	          Riempio a spazio i campi di print
	        ************************************/
      
	        while(strlen(summ[ind].user) < 8)
		      strcat(summ[ind].user," ");
      
	        while(strlen(summ[ind].host) < 15)
		      strcat(summ[ind].host," ");
      
	        /*******************
	          Tronco alla fine
	        *******************/
      
	        summ[ind].user[8] = 0;
	        summ[ind].host[15] = 0;
      
	        if(sumuser)
		   printf("%s ",summ[ind].user);
   
	        if(sumhost)
		   printf("%s ",summ[ind].host);
   
	        if(sumdate)
		   printf("%s ",summ[ind].data);
   
                /********************************
	           Calcolo tempo di collegamento
	        ********************************/
   
	        delta = summ[ind].tempo;
	        delta_H = delta / 3600;
	        delta_M = (delta % 3600) / 60;
	        delta_S = ((delta % 3600) % 60);
   
	        printf("%03d:%02d:%02d\n",delta_H,delta_M,delta_S);
             }
        }
     }
   }
}
