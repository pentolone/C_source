#include <f2c/fglExt.h>

UsrData usrData[]={
  { 0, 0 }
};

/* Vecchie funzioni */
int err_msg(void);
int oraesatta(void);
int put_env(void);
int read_env(void);
int terminale(void);

/* Nuove funzioni (dal 1994) */
int apri_porta(void);
int blocco(void);
int dls_blocca_db(void);
int dls_read(void);
int dls_sblocca_db(void);
int leggi_coda(void);
int leggi_shm(void);
int sblocco(void);

/* Nuove funzioni (dal 27/09/1994 L.Romano) */

int c_chktel(void);
int leggi_coda1(void);

/* Nuove funzioni (dal 05/06/1995 L.Romano) */

int dls_help(void);

/* Funzioni Registratore di cassa */
int c_bcc(void);
int leggi_porta(void);
int scrivi_porta(void);

/* Funzioni PayTv */
int dls_readq(void);
int dls_setshm(void);
int dls_writeq(void);
int leggi_seq(void);

/* Funzioni calcolo CRC (01/10/1996 L.Romano) */
int dls_crc(void);

/* Funzione gestione lettere di conferma */
int riga_lettera(void);

/* Funzione gestione lettura semaforo badge (A.Giammello) */
int rw_semaphore(void);


UsrFunction usrFunctions[]={
  { "err_msg",err_msg,-1,-1 }, 
  { "oraesatta",oraesatta,-1,-1 },
  { "put_env",put_env,-1,-1 },
  { "read_env",read_env,-1,-1 },
  { "terminale",terminale,-1,-1 },
  { "apri_porta",apri_porta,-1,-1 },
  { "blocco",blocco,-1,-1 },
  { "dls_blocca_db",dls_blocca_db,-1,-1 },
  { "dls_read",dls_read,-1,-1 },
  { "dls_sblocca_db",dls_sblocca_db,-1,-1 },
  { "leggi_coda",leggi_coda,-1,-1 },
  { "leggi_shm",leggi_shm,-1,-1 },
  { "sblocco",sblocco,-1,-1 },
  { "c_chktel",c_chktel,-1,-1 },
  { "leggi_coda1",leggi_coda1,-1,-1 },
  { "dls_help",dls_help,-1,-1 },
  { "c_bcc",c_bcc,-1,-1 },
  { "leggi_porta",leggi_porta,-1,-1 },
  { "scrivi_porta",scrivi_porta,-1,-1 },
  { "dls_readq",dls_readq,-1,-1 },
  { "dls_setshm",dls_setshm,-1,-1 },
  { "leggi_seq",leggi_seq,-1,-1 },
  { "dls_writeq",dls_writeq,-1,-1 },
  { "dls_crc",dls_crc,-1,-1 },
  { "riga_lettera",riga_lettera,-1,-1 },
  { "rw_semaphore",rw_semaphore,-1,-1 },
  { 0,0,0,0 }
};
