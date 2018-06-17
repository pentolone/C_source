static char sccs_crcinf_id[] = "@(#)crcinf.c Version 1.1 Date 00/02/21 Hour 12:05:54";
/*
	Function that return the character control code.

	Included in fglgo (INFORMIX runtime)

	Input - String to be checked
	      - String length
	      - number of character control code (1,2,3)

	(C) Copyright by Delos s.r.l. 1989-1996

*/
#define tochar(ch)  (((ch) + SP ) & 0xFF )	/* Number to character */
#define SP 0x21

int parity = 0;
int dls_crc(void)
{
unsigned short bctu,len,i=0;
char a[133],b[4];
unsigned int conta=0,chk2(char *),chk3(char *);
unsigned crc;

    popshort(&bctu);
    popshort(&len);
    popquote(a,sizeof(a));

    a[len] = '\0';

    switch (bctu) {			/* Block check */
	case 1:				/* 1 = 6-bit chksum */
	    b[i++] = tochar(chk1(a));
	    break;

	case 2:				/* 2 = 12-bit chksum */
	    conta = chk2(a);
            sprintf(b,"%x",conta);
            i = 2;
	    break;

        case 3:				/* 3 = 16-bit CRC */
	    crc = chk3(a);
	    b[i++] = (unsigned)tochar(((crc & 0170000)) >> 12);
	    b[i++] = (unsigned)tochar((crc >> 6) & 077);
	    b[i++] = (unsigned)tochar(crc & 077);
	    break;
    }
    b[i] = '\0';
    retquote(b);
    return(1);
   }
/*  C H K 1  --  Compute a type-1 6-bit checksum.  */

int
chk1(pkt) register char *pkt; {
    register unsigned int chk;
    chk = chk2(pkt);
    chk = (((chk & 0300) >> 6) + chk) & 077;
    return((int) chk);
}
/*  C H K 2  --  Compute the numeric sum of all the bytes in the packet.  */

unsigned int
chk2(t) register char *t; {
	register unsigned int bcc;
  
	bcc = 0;
	while (*t) {
               bcc = bcc ^ *t++;
              }
        return(bcc);
}

unsigned int
chk3(pkt) register char *pkt; {
/* CRC generation tables */

static long crcta[16] = { 0L, 010201L, 020402L, 030603L, 041004L,
  051205L, 061406L, 071607L, 0102010L, 0112211L, 0122412L, 0132613L, 0143014L,
  0153215L, 0163416L, 0173617L };

static long crctb[16] = { 0L, 010611L, 021422L, 031233L, 043044L,
  053655L, 062466L, 072277L, 0106110L, 0116701L, 0127532L, 0137323L, 0145154L,
  0155745L, 0164576L, 0174367L };
    register long c, crc;
    register unsigned int m;
    m = (parity) ? 0177 : 0377;
    for (crc = 0; *pkt != '\0'; pkt++) {
	c = crc ^ (long)(*pkt & m);
	crc = (crc >> 8) ^ (crcta[(c & 0xF0) >> 4] ^ crctb[c & 0x0F]);
    }
    return((unsigned int) (crc & 0xFFFF));
}
