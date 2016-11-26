#ifndef __CC936_H
#define __CC936_H


typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef char			CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;



WCHAR ff_convert (WCHAR, UINT);
WCHAR ff_wtoupper (	/* Upper converted character */
	WCHAR chr		/* Input character */
);

#endif
