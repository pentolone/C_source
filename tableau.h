#ifndef LINT
static char sccs_tableau_h[] = "@(#)tableau.h Version 1.1 Date 00/02/21 Hour 15:09:34";
#endif
/*---------------------------------------------------------------------------

	tableau.h:  include per le definizioni usate per tableau X/Window

---------------------------------------------------------------------------*/
#ifndef _TABLEAU_H
#define _TABLEAU_H
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

#define TBL_MSGQR 101	/* Message queue (READ) */
#define TBL_MSGQW 102	/* Message queue (WRITE) */
#endif
