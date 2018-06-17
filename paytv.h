#ifndef LINT
static char sccs_paytv[] = "@(#)paytv.h Version 1.2 Date 02/04/08 Hour 17:32:09";
#endif
/*---------------------------------------------------------------------------

	paytv.h:  include per le definizioni usate dai daemons per PAY TV

---------------------------------------------------------------------------*/
#ifndef _PAYTV_H_
#define _PAYTV_H_
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

#define PT_SHM 90	/* Shared memory address Pay TV */
#define PT_SEM 91	/* Semaphore for shared memory access */
#define PT_MSGQR 92	/* Message queue (READ) */
#define PT_MSGQW 93	/* Message queue (WRITE) */

#define MAXPTLEN 255

#define PT_STX 0x2	/* Start Of Text */
#define PT_ETX 0x3	/* End Of Text */
#define PT_EOT 0x4	/* End Of Transmission */
#define PT_ENQ 0x5	/* Enquiry */
#define PT_ACK 0x6	/* Aknowledge */
#define PT_NCK 0x15	/* Not Aknowledge */
#define PT_ETB 0x17	/* End Of Block */
#endif
