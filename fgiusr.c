static char sccs_fgiusr_id[] = "%Z%%M% Version %I% Date %D% Hour %T%";
/***************************************************************************
 *
 *			   INFORMIX SOFTWARE, INC.
 *
 *			      PROPRIETARY DATA
 *
 *	THIS DOCUMENT CONTAINS TRADE SECRET DATA WHICH IS THE PROPERTY OF 
 *	INFORMIX SOFTWARE, INC.  THIS DOCUMENT IS SUBMITTED TO RECIPIENT IN
 *	CONFIDENCE.  INFORMATION CONTAINED HEREIN MAY NOT BE USED, COPIED OR 
 *	DISCLOSED IN WHOLE OR IN PART EXCEPT AS PERMITTED BY WRITTEN AGREEMENT 
 *	SIGNED BY AN OFFICER OF INFORMIX SOFTWARE, INC.
 *
 *	THIS MATERIAL IS ALSO COPYRIGHTED AS AN UNPUBLISHED WORK UNDER
 *	SECTIONS 104 AND 408 OF TITLE 17 OF THE UNITED STATES CODE. 
 *	UNAUTHORIZED USE, COPYING OR OTHER REPRODUCTION IS PROHIBITED BY LAW.
 *
 *
 *  Title:	fgiusr.c
 *  Sccsid:	@(#)fgiusr.c	7.2	7/8/90  13:50:19
 *  Description:
 *		definition of user C functions
 *
 ***************************************************************************
 */

/***************************************************************************
 *
 * This table is for user-defined C functions.
 *
 * Each initializer has the form:
 *
 *	"name", name, nargs,
 *
 * Variable # of arguments:
 *
 *	set nargs to -(maximum # args)
 *
 * Be sure to declare name before the table and to leave the
 * line of 0's at the end of the table.
 *
 * Example:
 *
 *	You want to call your C function named "mycfunc" and it expects
 *	2 arguments.  You must declare it:
 *
 *		int mycfunc();
 *
 *	and then insert an initializer for it in the table:
 *
 *		"mycfunc", mycfunc, 2,
 *
 ***************************************************************************
 */

#include "fgicfunc.h"

/* Vecchie funzioni */
int err_msg(void);
int oraesatta(void);
int terminale(void);
int read_env(void);
int put_env(void);

/* Nuove funzioni (dal 1994) */
int apri_porta(void);
int dls_read(void);
int leggi_shm(void);
int leggi_coda(void);
int dls_blocca_db(void);
int dls_sblocca_db(void);
int blocco(void);
int sblocco(void);

/*
 * Nuove funzioni dal 27/09/1994 (L. Romano)
 */

int c_chktel(void);
int leggi_coda1(void);

/*
 * Nuove funzioni dal 05/06/1995 (L. Romano)
 */

int dls_help(void);

/*
 * Funzioni per la cassa
 */

int leggi_porta(void);
int scrivi_porta(void);
int c_bcc(void);

/*
 * Funzioni per la PAY TV
 */

int dls_writeq(void);
int dls_readq(void);
int dls_setshm(void);
int leggi_seq(void);

/*
 * Funzione per il calcolo del CRC  01/10/1996 (L. Romano)
 */
int dls_crc(void);

/*
 *  Funzione per la gestione delle lettere di conferma
 */

int riga_lettera(void);

/*
 *  Funzione per la gestione dei semafori badge 07/02/2000 (A. Giammello)
 */

int rw_semaphore(void);

cfunc_t usrcfuncs[] = 
    {
    "err_msg", err_msg, 2,
    "oraesatta", oraesatta, 0,
    "terminale", terminale, 0,
    "read_env", read_env, 1,
    "put_env", put_env, 2,
    "apri_porta", apri_porta, 7,
    "dls_read", dls_read, 1,
    "leggi_shm", leggi_shm, 0,
    "leggi_coda", leggi_coda, 0,
    "dls_blocca_db", dls_blocca_db, 0,
    "dls_sblocca_db", dls_sblocca_db, 0,
    "blocco", blocco, 0,
    "sblocco", sblocco, 0,

/*
 * Nuove funzioni dal 27/09/1994 (L. Romano)
 */

    "c_chktel", c_chktel, 2,
    "leggi_coda1", leggi_coda1, 0,

/*
 * Nuove funzioni dal 05/06/1995 (L. Romano)
 */
    "dls_help", dls_help, 1,

/*
 * Funzioni per la cassa
 */

    "scrivi_porta", scrivi_porta, 2,
    "leggi_porta", leggi_porta, 3,
    "c_bcc", c_bcc, 2,

/*
 * Funzioni per la PAY TV 08/07/1996 (L. Romano)
 */
    "dls_readq", dls_readq, 0,
    "dls_writeq", dls_writeq, 2,
    "dls_setshm", dls_setshm, 1,
    "leggi_seq", leggi_seq, 2,

/*
 * Funzione per il calcolo del CRC  01/10/1996 (L. Romano)
 */
    "dls_crc", dls_crc, 3,

/*
 * Funzione per la gestione delle lettere conferma 
 */
    "riga_lettera", riga_lettera, 52,

/*
 * Funzione per la gestione dei semafori badge
 */
    "rw_semaphore", rw_semaphore, 3,

/*
 * Fine dichiarazione funzioni 
 */
    0, 0, 0
    };
