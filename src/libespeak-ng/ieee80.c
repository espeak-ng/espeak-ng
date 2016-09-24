
//#define TEST_FP

/* Copyright (C) 1989-1991 Apple Computer, Inc.
 *
 * All rights reserved.
 *
 * Warranty Information
 *  Even though Apple has reviewed this software, Apple makes no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the above warranty information.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *	Apple Macintosh, MPW 3.1 C compiler
 *	Apple Macintosh, THINK C compiler
 *	Silicon Graphics IRIS, MIPS compiler
 *	Cray X/MP and Y/MP
 *	Digital Equipment VAX
 *	Sequent Balance (Multiprocesor 386)
 *	NeXT
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */


#include	<stdio.h>
#include	<math.h>

typedef float Single;

#ifndef applec
 typedef double defdouble;
#else applec
 typedef long double defdouble;
#endif applec

#ifndef THINK_C
 typedef double Double;
#else THINK_C
 typedef short double Double;
#endif THINK_C

#ifndef HUGE_VAL
# define HUGE_VAL HUGE
#endif HUGE_VAL


/****************************************************************
 * The following two routines make up for deficiencies in many
 * compilers to convert properly between unsigned integers and
 * floating-point.  Some compilers which have this bug are the
 * THINK_C compiler for the Macintosh and the C compiler for the
 * Silicon Graphics MIPS-based Iris.
 ****************************************************************/

#ifdef applec	/* The Apple C compiler works */
# define FloatToUnsigned(f)	((unsigned long)(f))
# define UnsignedToFloat(u)	((defdouble)(u))
#else applec
# define FloatToUnsigned(f)	((unsigned long)(((long)((f) - 2147483648.0)) + 2147483647L + 1))
# define UnsignedToFloat(u)	(((defdouble)((long)((u) - 2147483647L - 1))) + 2147483648.0)
#endif applec


/****************************************************************
 * Single precision IEEE floating-point conversion routines
 ****************************************************************/

#define SEXP_MAX		255
#define SEXP_OFFSET		127
#define SEXP_SIZE		8
#define SEXP_POSITION	(32-SEXP_SIZE-1)


defdouble
ConvertFromIeeeSingle(bytes)
char* bytes;
{
	defdouble	f;
	long	mantissa, expon;
	long	bits;

	bits =	((unsigned long)(bytes[0] & 0xFF) << 24)
		|	((unsigned long)(bytes[1] & 0xFF) << 16)
		|	((unsigned long)(bytes[2] & 0xFF) << 8)
		|	 (unsigned long)(bytes[3] & 0xFF);		/* Assemble bytes into a long */

	if ((bits & 0x7FFFFFFF) == 0) {
		f = 0;
	}

	else {
		expon = (bits & 0x7F800000) >> SEXP_POSITION;
		if (expon == SEXP_MAX) {		/* Infinity or NaN */
			f = HUGE_VAL;		/* Map NaN's to infinity */
		}
		else {
			if (expon == 0) {	/* Denormalized number */
				mantissa = (bits & 0x7fffff);
				f = ldexp((defdouble)mantissa, expon - SEXP_OFFSET - SEXP_POSITION + 1);
			}
			else {				/* Normalized number */
				mantissa = (bits & 0x7fffff) + 0x800000;	/* Insert hidden bit */
				f = ldexp((defdouble)mantissa, expon - SEXP_OFFSET - SEXP_POSITION);
			}
		}
	}

	if (bits & 0x80000000)
		return -f;
	else
		return f;
}


/****************************************************************/


void
ConvertToIeeeSingle(num, bytes)
defdouble num;
char* bytes;
{
	long	sign;
	register long bits;

	if (num < 0) {	/* Can't distinguish a negative zero */
		sign = 0x80000000;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		bits = 0;
	}

	else {
		defdouble fMant;
		int expon;

		fMant = frexp(num, &expon);

		if ((expon > (SEXP_MAX-SEXP_OFFSET+1)) || !(fMant < 1)) {
			/* NaN's and infinities fail second test */
			bits = sign | 0x7F800000;		/* +/- infinity */
		}

		else {
			long mantissa;

			if (expon < -(SEXP_OFFSET-2)) {	/* Smaller than normalized */
				int shift = (SEXP_POSITION+1) + (SEXP_OFFSET-2) + expon;
				if (shift < 0) {	/* Way too small: flush to zero */
					bits = sign;
				}
				else {			/* Nonzero denormalized number */
					mantissa = (long)(fMant * (1L << shift));
					bits = sign | mantissa;
				}
			}

			else {				/* Normalized number */
				mantissa = (long)floor(fMant * (1L << (SEXP_POSITION+1)));
				mantissa -= (1L << SEXP_POSITION);			/* Hide MSB */
				bits = sign | ((long)((expon + SEXP_OFFSET - 1)) << SEXP_POSITION) | mantissa;
			}
		}
	}

	bytes[0] = bits >> 24;	/* Copy to byte string */
	bytes[1] = bits >> 16;
	bytes[2] = bits >> 8;
	bytes[3] = bits;
}


/****************************************************************
 * Double precision IEEE floating-point conversion routines
 ****************************************************************/

#define DEXP_MAX		2047
#define DEXP_OFFSET		1023
#define DEXP_SIZE		11
#define DEXP_POSITION	(32-DEXP_SIZE-1)


defdouble
ConvertFromIeeeDouble(bytes)
char* bytes;
{
	defdouble	f;
	long	mantissa, expon;
	unsigned long first, second;

	first = ((unsigned long)(bytes[0] & 0xFF) << 24)
		|	((unsigned long)(bytes[1] & 0xFF) << 16)
		|	((unsigned long)(bytes[2] & 0xFF) << 8)
		|	 (unsigned long)(bytes[3] & 0xFF);
	second= ((unsigned long)(bytes[4] & 0xFF) << 24)
		|	((unsigned long)(bytes[5] & 0xFF) << 16)
		|	((unsigned long)(bytes[6] & 0xFF) << 8)
		|	 (unsigned long)(bytes[7] & 0xFF);
	
	if (first == 0 && second == 0) {
		f = 0;
	}

	else {
		expon = (first & 0x7FF00000) >> DEXP_POSITION;
		if (expon == DEXP_MAX) {		/* Infinity or NaN */
			f = HUGE_VAL;		/* Map NaN's to infinity */
		}
		else {
			if (expon == 0) {	/* Denormalized number */
				mantissa = (first & 0x000FFFFF);
				f = ldexp((defdouble)mantissa, expon - DEXP_OFFSET - DEXP_POSITION + 1);
				f += ldexp(UnsignedToFloat(second), expon - DEXP_OFFSET - DEXP_POSITION + 1 - 32);
			}
			else {				/* Normalized number */
				mantissa = (first & 0x000FFFFF) + 0x00100000;	/* Insert hidden bit */
				f = ldexp((defdouble)mantissa, expon - DEXP_OFFSET - DEXP_POSITION);
				f += ldexp(UnsignedToFloat(second), expon - DEXP_OFFSET - DEXP_POSITION - 32);
			}
		}
	}

	if (first & 0x80000000)
		return -f;
	else
		return f;
}


/****************************************************************/


void
ConvertToIeeeDouble(num, bytes)
defdouble num;
char *bytes;
{
	long	sign;
	long	first, second;

	if (num < 0) {	/* Can't distinguish a negative zero */
		sign = 0x80000000;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		first = 0;
		second = 0;
	}

	else {
		defdouble fMant, fsMant;
		int expon;

		fMant = frexp(num, &expon);

		if ((expon > (DEXP_MAX-DEXP_OFFSET+1)) || !(fMant < 1)) {
			/* NaN's and infinities fail second test */
			first = sign | 0x7FF00000;		/* +/- infinity */
			second = 0;
		}

		else {
			long mantissa;

			if (expon < -(DEXP_OFFSET-2)) {	/* Smaller than normalized */
				int shift = (DEXP_POSITION+1) + (DEXP_OFFSET-2) + expon;
				if (shift < 0) {	/* Too small for something in the MS word */
					first = sign;
					shift += 32;
					if (shift < 0) {	/* Way too small: flush to zero */
						second = 0;
					}
					else {			/* Pretty small demorn */
						second = FloatToUnsigned(floor(ldexp(fMant, shift)));
					}
				}
				else {			/* Nonzero denormalized number */
					fsMant = ldexp(fMant, shift);
					mantissa = (long)floor(fsMant);
					first = sign | mantissa;
					second = FloatToUnsigned(floor(ldexp(fsMant - mantissa, 32)));
				}
			}

			else {				/* Normalized number */
				fsMant = ldexp(fMant, DEXP_POSITION+1);
				mantissa = (long)floor(fsMant);
				mantissa -= (1L << DEXP_POSITION);			/* Hide MSB */
				fsMant -= (1L << DEXP_POSITION);
				first = sign | ((long)((expon + DEXP_OFFSET - 1)) << DEXP_POSITION) | mantissa;
				second = FloatToUnsigned(floor(ldexp(fsMant - mantissa, 32)));
			}
		}
	}
	
	bytes[0] = first >> 24;
	bytes[1] = first >> 16;
	bytes[2] = first >> 8;
	bytes[3] = first;
	bytes[4] = second >> 24;
	bytes[5] = second >> 16;
	bytes[6] = second >> 8;
	bytes[7] = second;
}


/****************************************************************
 * Extended precision IEEE floating-point conversion routines.
 * Extended is an 80-bit number as defined by Motorola,
 * with a sign bit, 15 bits of exponent (offset 16383?),
 * and a 64-bit mantissa, with no hidden bit.
 ****************************************************************/

defdouble
ConvertFromIeeeExtended(bytes)
char* bytes;
{
	defdouble	f;
	long	expon;
	unsigned long hiMant, loMant;
	
	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
	hiMant	=	((unsigned long)(bytes[2] & 0xFF) << 24)
			|	((unsigned long)(bytes[3] & 0xFF) << 16)
			|	((unsigned long)(bytes[4] & 0xFF) << 8)
			|	((unsigned long)(bytes[5] & 0xFF));
	loMant	=	((unsigned long)(bytes[6] & 0xFF) << 24)
			|	((unsigned long)(bytes[7] & 0xFF) << 16)
			|	((unsigned long)(bytes[8] & 0xFF) << 8)
			|	((unsigned long)(bytes[9] & 0xFF));

	if (expon == 0 && hiMant == 0 && loMant == 0) {
		f = 0;
	}
	else {
		if (expon == 0x7FFF) {	/* Infinity or NaN */
			f = HUGE_VAL;
		}
		else {
			expon -= 16383;
			f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
			f += ldexp(UnsignedToFloat(loMant), expon-=32);
		}
	}

	if (bytes[0] & 0x80)
		return -f;
	else
		return f;
}


/****************************************************************/


void
ConvertToIeeeExtended(num, bytes)
defdouble num;
char *bytes;
{
	int	sign;
	int expon;
	defdouble fMant, fsMant;
	unsigned long hiMant, loMant;

	if (num < 0) {
		sign = 0x8000;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		expon = 0; hiMant = 0; loMant = 0;
	}
	else {
		fMant = frexp(num, &expon);
		if ((expon > 16384) || !(fMant < 1)) {	/* Infinity or NaN */
			expon = sign|0x7FFF; hiMant = 0; loMant = 0; /* infinity */
		}
		else {	/* Finite */
			expon += 16382;
			if (expon < 0) {	/* denormalized */
				fMant = ldexp(fMant, expon);
				expon = 0;
			}
			expon |= sign;
			fMant = ldexp(fMant, 32);          fsMant = floor(fMant); hiMant = FloatToUnsigned(fsMant);
			fMant = ldexp(fMant - fsMant, 32); fsMant = floor(fMant); loMant = FloatToUnsigned(fsMant);
		}
	}
	
	bytes[0] = expon >> 8;
	bytes[1] = expon;
	bytes[2] = hiMant >> 24;
	bytes[3] = hiMant >> 16;
	bytes[4] = hiMant >> 8;
	bytes[5] = hiMant;
	bytes[6] = loMant >> 24;
	bytes[7] = loMant >> 16;
	bytes[8] = loMant >> 8;
	bytes[9] = loMant;
}

/****************************************************************
 * Testing routines for the floating-point conversions.
 ****************************************************************/


#ifdef applec
# define IEEE
#endif applec
#ifdef THINK_C
# define IEEE
#endif THINK_C
#ifdef sgi
# define IEEE
#endif sgi
#ifdef sequent
# define IEEE
# define LITTLE_ENDIAN
#endif sequent
#ifdef sun
# define IEEE
#endif sun
#ifdef NeXT
# define IEEE
#endif NeXT

#ifdef TEST_FP

union SParts {
	Single s;
	long i;
};
union DParts {
	Double d;
	long i[2];
};
union EParts {
	defdouble e;
	short i[6];
};


int
GetHexValue(x)
register int x;
{
	x &= 0x7F;
	
	if ('0' <= x && x <= '9')
		x -= '0';
	else if ('a' <= x && x <= 'f')
		x = x - 'a' + 0xA;
	else if ('A' <= x && x <= 'F')
		x = x - 'A' + 0xA;
	else
		x = 0;
	
	return(x);
}


void
Hex2Bytes(hex, bytes)
register char *hex, *bytes;
{
	for ( ; *hex; hex += 2) {
		*bytes++ = (GetHexValue(hex[0]) << 4) | GetHexValue(hex[1]);
		if (hex[1] == 0)
			break;	/* Guard against odd bytes */
	}
}


int
GetHexSymbol(x)
register int x;
{
	x &= 0xF;
	if (x <= 9)
		x += '0';
	else
		x += 'A' - 0xA;
	return(x);
}


void
Bytes2Hex(bytes, hex, nBytes)
register char *bytes, *hex;
register int nBytes;
{
	for ( ; nBytes--; bytes++) {
		*hex++ = GetHexSymbol(*bytes >> 4);
		*hex++ = GetHexSymbol(*bytes);
	}
	*hex = 0;
}


void
MaybeSwapBytes(bytes, nBytes)
char* bytes;
int nBytes;
{
#ifdef LITTLE_ENDIAN
	register char *p, *q, t;
	for (p = bytes, q = bytes+nBytes-1; p < q; p++, q--) {
		t = *p;
		*p = *q;
		*q = t;
	}
#else
	if (bytes, nBytes);		/* Just so it's used, to avoid warnings */
#endif LITTLE_ENDIAN
	
}


float
MachineIEEESingle(bytes)
char* bytes;
{
	float t;
	MaybeSwapBytes(bytes, 4);
	t = *((float*)(bytes));
	MaybeSwapBytes(bytes, 4);
	return (t);
}


Double
MachineIEEEDouble(bytes)
char* bytes;
{
	Double t;
	MaybeSwapBytes(bytes, 8);
	t = *((Double*)(bytes));
	MaybeSwapBytes(bytes, 8);
	return (t);
}


void
TestFromIeeeSingle(hex)
char *hex;
{
	defdouble f;
	union SParts p;
	char bytes[4];

	Hex2Bytes(hex, bytes);
	f = ConvertFromIeeeSingle(bytes);
	p.s = f;

#ifdef IEEE
	printf("IEEE(%g) [%s] --> float(%g) [%08lX]\n",
	MachineIEEESingle(bytes),
	hex, f, p.i);
#else IEEE
	printf("IEEE[%s] --> float(%g) [%08lX]\n", hex, f, p.i);
#endif IEEE
}


void
TestToIeeeSingle(f)
defdouble f;
{
	union SParts p;
	char bytes[4];
	char hex[8+1];

	p.s = f;

	ConvertToIeeeSingle(f, bytes);
	Bytes2Hex(bytes, hex, 4);
#ifdef IEEE
	printf("float(%g) [%08lX] --> IEEE(%g) [%s]\n",
		f, p.i,
		MachineIEEESingle(bytes),
		hex
	);
#else IEEE
	printf("float(%g) [%08lX] --> IEEE[%s]\n", f, p.i, hex);
#endif IEEE
}


void
TestFromIeeeDouble(hex)
char *hex;
{
	defdouble f;
	union DParts p;
	char bytes[8];
	
	Hex2Bytes(hex, bytes);
	f = ConvertFromIeeeDouble(bytes);
	p.d = f;

#ifdef IEEE
	printf("IEEE(%g) [%.8s %.8s] --> double(%g) [%08lX %08lX]\n",
	MachineIEEEDouble(bytes),
	hex, hex+8, f, p.i[0], p.i[1]);
#else IEEE
	printf("IEEE[%.8s %.8s] --> double(%g) [%08lX %08lX]\n",
		hex, hex+8, f, p.i[0], p.i[1]);
#endif IEEE

}

void
TestToIeeeDouble(f)
defdouble f;
{
	union DParts p;
	char bytes[8];
	char hex[16+1];

	p.d = f;

	ConvertToIeeeDouble(f, bytes);
	Bytes2Hex(bytes, hex, 8);
#ifdef IEEE
	printf("double(%g) [%08lX %08lX] --> IEEE(%g) [%.8s %.8s]\n",
		f, p.i[0], p.i[1],
		MachineIEEEDouble(bytes),
		hex, hex+8
	);
#else IEEE
	printf("double(%g) [%08lX %08lX] --> IEEE[%.8s %.8s]\n",
		f, p.i[0], p.i[1], hex, hex+8
	);
#endif IEEE

}


void
TestFromIeeeExtended(hex)
char *hex;
{
	defdouble f;
	union EParts p;
	char bytes[12];

	Hex2Bytes(hex, bytes);
	f = ConvertFromIeeeExtended(bytes);
	p.e = f;

	bytes[11] = bytes[9];
	bytes[10] = bytes[8];
	bytes[9] = bytes[7];
	bytes[8] = bytes[6];
	bytes[7] = bytes[5];
	bytes[6] = bytes[4];
	bytes[5] = bytes[3];
	bytes[4] = bytes[2];
	bytes[3] = 0;
	bytes[2] = 0;

#if defined(applec) || defined(THINK_C)
	printf("IEEE(%g) [%.4s %.8s %.8s] --> extended(%g) [%04X %04X%04X %04X%04X]\n",
		*((defdouble*)(bytes)),
		hex, hex+4, hex+12, f,
		p.i[0]&0xFFFF, p.i[2]&0xFFFF, p.i[3]&0xFFFF, p.i[4]&0xFFFF, p.i[5]&0xFFFF
	);
#else /* !Macintosh */
	printf("IEEE[%.4s %.8s %.8s] --> extended(%g) [%04X %04X%04X %04X%04X]\n",
		hex, hex+4, hex+12, f,
		p.i[0]&0xFFFF, p.i[2]&0xFFFF, p.i[3]&0xFFFF, p.i[4]&0xFFFF, p.i[5]&0xFFFF
	);
#endif /* Macintosh */
}


void
TestToIeeeExtended(f)
defdouble f;
{
	char bytes[12];
	char hex[24+1];

	ConvertToIeeeExtended(f, bytes);
	Bytes2Hex(bytes, hex, 10);

	bytes[11] = bytes[9];
	bytes[10] = bytes[8];
	bytes[9] = bytes[7];
	bytes[8] = bytes[6];
	bytes[7] = bytes[5];
	bytes[6] = bytes[4];
	bytes[5] = bytes[3];
	bytes[4] = bytes[2];
	bytes[3] = 0;
	bytes[2] = 0;

#if defined(applec) || defined(THINK_C)
	printf("extended(%g) --> IEEE(%g) [%.4s %.8s %.8s]\n",
		f, *((defdouble*)(bytes)),
		hex, hex+4, hex+12
	);
#else /* !Macintosh */
	printf("extended(%g) --> IEEE[%.4s %.8s %.8s]\n",
		f,
		hex, hex+4, hex+12
	);
#endif /* Macintosh */
}

#include	<signal.h>

void SignalFPE(i, j)
int	i;
void (*j)();
{
	printf("[Floating Point Interrupt Caught.]\n", i, j);
	signal(SIGFPE, SignalFPE);
}
	
void
main()
{
	long d[3];
	char bytes[12];

	signal(SIGFPE, SignalFPE);

	TestFromIeeeSingle("00000000");
	TestFromIeeeSingle("80000000");
	TestFromIeeeSingle("3F800000");
	TestFromIeeeSingle("BF800000");
	TestFromIeeeSingle("40000000");
	TestFromIeeeSingle("C0000000");
	TestFromIeeeSingle("7F800000");
	TestFromIeeeSingle("FF800000");
	TestFromIeeeSingle("00800000");
	TestFromIeeeSingle("00400000");
	TestFromIeeeSingle("00000001");
	TestFromIeeeSingle("80000001");
	TestFromIeeeSingle("3F8FEDCB");
	TestFromIeeeSingle("7FC00100");	/* Quiet NaN(1) */
	TestFromIeeeSingle("7F800100");	/* Signalling NaN(1) */

	TestToIeeeSingle(0.0);
	TestToIeeeSingle(-0.0);
	TestToIeeeSingle(1.0);
	TestToIeeeSingle(-1.0);
	TestToIeeeSingle(2.0);
	TestToIeeeSingle(-2.0);
	TestToIeeeSingle(3.0);
	TestToIeeeSingle(-3.0);
#if !(defined(sgi) || defined(NeXT))
	TestToIeeeSingle(HUGE_VAL);
	TestToIeeeSingle(-HUGE_VAL);
#endif /* !sgi, !NeXT */

#ifdef IEEE
	/* These only work on big-endian IEEE machines */
	d[0] = 0x00800000L; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])));		/* Smallest normalized */
	d[0] = 0x00400000L; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])));		/* Almost largest denormalized */
	d[0] = 0x00000001L; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])));		/* Smallest denormalized */
	d[0] = 0x00000001L; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])) * 0.5);	/* Smaller than smallest denorm */
	d[0] = 0x3F8FEDCBL; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])));
#if !(defined(sgi) || defined(NeXT))
	d[0] = 0x7FC00100L; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])));		/* Quiet NaN(1) */
	d[0] = 0x7F800100L; MaybeSwapBytes((char*)d,4); TestToIeeeSingle(*((float*)(&d[0])));		/* Signalling NaN(1) */
#endif /* !sgi, !NeXT */
#endif IEEE



	TestFromIeeeDouble("0000000000000000");
	TestFromIeeeDouble("8000000000000000");
	TestFromIeeeDouble("3FF0000000000000");
	TestFromIeeeDouble("BFF0000000000000");
	TestFromIeeeDouble("4000000000000000");
	TestFromIeeeDouble("C000000000000000");
	TestFromIeeeDouble("7FF0000000000000");
	TestFromIeeeDouble("FFF0000000000000");
	TestFromIeeeDouble("0010000000000000");
	TestFromIeeeDouble("0008000000000000");
	TestFromIeeeDouble("0000000000000001");
	TestFromIeeeDouble("8000000000000001");
	TestFromIeeeDouble("3FFFEDCBA9876543");
	TestFromIeeeDouble("7FF8002000000000");	/* Quiet NaN(1) */
	TestFromIeeeDouble("7FF0002000000000");	/* Signalling NaN(1) */

	TestToIeeeDouble(0.0);
	TestToIeeeDouble(-0.0);
	TestToIeeeDouble(1.0);
	TestToIeeeDouble(-1.0);
	TestToIeeeDouble(2.0);
	TestToIeeeDouble(-2.0);
	TestToIeeeDouble(3.0);
	TestToIeeeDouble(-3.0);
#if !(defined(sgi) || defined(NeXT))
	TestToIeeeDouble(HUGE_VAL);
	TestToIeeeDouble(-HUGE_VAL);
#endif /* !sgi, !NeXT */

#ifdef IEEE
	/* These only work on IEEE machines */
	Hex2Bytes("0010000000000000", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Smallest normalized */
	Hex2Bytes("0010000080000000", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Normalized, problem with unsigned */
	Hex2Bytes("0008000000000000", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Almost largest denormalized */
	Hex2Bytes("0000000080000000", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Denorm problem with unsigned */
	Hex2Bytes("0000000000000001", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Smallest denormalized */
	Hex2Bytes("0000000000000001", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)) * 0.5);	/* Smaller than smallest denorm */
	Hex2Bytes("3FFFEDCBA9876543", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* accuracy test */
#if !(defined(sgi) || defined(NeXT))
	Hex2Bytes("7FF8002000000000", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Quiet NaN(1) */
	Hex2Bytes("7FF0002000000000", bytes); MaybeSwapBytes(bytes,8); TestToIeeeDouble(*((Double*)(bytes)));	/* Signalling NaN(1) */
#endif /* !sgi, !NeXT */
#endif IEEE

	TestFromIeeeExtended("00000000000000000000");	/* +0 */
	TestFromIeeeExtended("80000000000000000000");	/* -0 */
	TestFromIeeeExtended("3FFF8000000000000000");	/* +1 */
	TestFromIeeeExtended("BFFF8000000000000000");	/* -1 */
	TestFromIeeeExtended("40008000000000000000");	/* +2 */
	TestFromIeeeExtended("C0008000000000000000");	/* -2 */
	TestFromIeeeExtended("7FFF0000000000000000");	/* +infinity */
	TestFromIeeeExtended("FFFF0000000000000000");	/* -infinity */
	TestFromIeeeExtended("7FFF8001000000000000");	/* Quiet NaN(1) */
	TestFromIeeeExtended("7FFF0001000000000000");	/* Signalling NaN(1) */
	TestFromIeeeExtended("3FFFFEDCBA9876543210");	/* accuracy test */

	TestToIeeeExtended(0.0);
	TestToIeeeExtended(-0.0);
	TestToIeeeExtended(1.0);
	TestToIeeeExtended(-1.0);
	TestToIeeeExtended(2.0);
	TestToIeeeExtended(-2.0);
#if !(defined(sgi) || defined(NeXT))
	TestToIeeeExtended(HUGE_VAL);
	TestToIeeeExtended(-HUGE_VAL);
#endif /* !sgi, !NeXT */

#if defined(applec) || defined(THINK_C)
	Hex2Bytes("7FFF00008001000000000000", bytes); TestToIeeeExtended(*((long double*)(bytes)));	/* Quiet NaN(1) */
	Hex2Bytes("7FFF00000001000000000000", bytes); TestToIeeeExtended(*((long double*)(bytes)));	/* Signalling NaN(1) */
	Hex2Bytes("7FFE00008000000000000000", bytes); TestToIeeeExtended(*((long double*)(bytes)));
	Hex2Bytes("000000008000000000000000", bytes); TestToIeeeExtended(*((long double*)(bytes)));
	Hex2Bytes("000000000000000000000001", bytes); TestToIeeeExtended(*((long double*)(bytes)));
	Hex2Bytes("3FFF0000FEDCBA9876543210", bytes); TestToIeeeExtended(*((long double*)(bytes)));
#endif /* applec, THINK_C */
}


/* This is the output of the test program on a big-endian IEEE machine:
IEEE(0) [00000000] --> float(0) [00000000]
IEEE(-0) [80000000] --> float(-0) [80000000]
IEEE(1) [3F800000] --> float(1) [3F800000]
IEEE(-1) [BF800000] --> float(-1) [BF800000]
IEEE(2) [40000000] --> float(2) [40000000]
IEEE(-2) [C0000000] --> float(-2) [C0000000]
IEEE(INF) [7F800000] --> float(INF) [7F800000]
IEEE(-INF) [FF800000] --> float(-INF) [FF800000]
IEEE(1.17549e-38) [00800000] --> float(1.17549e-38) [00800000]
IEEE(5.87747e-39) [00400000] --> float(5.87747e-39) [00400000]
IEEE(1.4013e-45) [00000001] --> float(1.4013e-45) [00000001]
IEEE(-1.4013e-45) [80000001] --> float(-1.4013e-45) [80000001]
IEEE(1.12444) [3F8FEDCB] --> float(1.12444) [3F8FEDCB]
IEEE(NAN(001)) [7FC00100] --> float(INF) [7F800000]
IEEE(NAN(001)) [7F800100] --> float(INF) [7F800000]
float(0) [00000000] --> IEEE(0) [00000000]
float(-0) [80000000] --> IEEE(0) [00000000]
float(1) [3F800000] --> IEEE(1) [3F800000]
float(-1) [BF800000] --> IEEE(-1) [BF800000]
float(2) [40000000] --> IEEE(2) [40000000]
float(-2) [C0000000] --> IEEE(-2) [C0000000]
float(3) [40400000] --> IEEE(3) [40400000]
float(-3) [C0400000] --> IEEE(-3) [C0400000]
float(INF) [7F800000] --> IEEE(INF) [7F800000]
float(-INF) [FF800000] --> IEEE(-INF) [FF800000]
float(1.17549e-38) [00800000] --> IEEE(1.17549e-38) [00800000]
float(5.87747e-39) [00400000] --> IEEE(5.87747e-39) [00400000]
float(1.4013e-45) [00000001] --> IEEE(1.4013e-45) [00000001]
float(7.00649e-46) [00000000] --> IEEE(0) [00000000]
float(1.12444) [3F8FEDCB] --> IEEE(1.12444) [3F8FEDCB]
float(NAN(001)) [7FC00100] --> IEEE(INF) [7F800000]
float(NAN(001)) [7FC00100] --> IEEE(INF) [7F800000]
IEEE(0) [00000000 00000000] --> double(0) [00000000 00000000]
IEEE(-0) [80000000 00000000] --> double(-0) [80000000 00000000]
IEEE(1) [3FF00000 00000000] --> double(1) [3FF00000 00000000]
IEEE(-1) [BFF00000 00000000] --> double(-1) [BFF00000 00000000]
IEEE(2) [40000000 00000000] --> double(2) [40000000 00000000]
IEEE(-2) [C0000000 00000000] --> double(-2) [C0000000 00000000]
IEEE(INF) [7FF00000 00000000] --> double(INF) [7FF00000 00000000]
IEEE(-INF) [FFF00000 00000000] --> double(-INF) [FFF00000 00000000]
IEEE(2.22507e-308) [00100000 00000000] --> double(2.22507e-308) [00100000 00000000]
IEEE(1.11254e-308) [00080000 00000000] --> double(1.11254e-308) [00080000 00000000]
IEEE(4.94066e-324) [00000000 00000001] --> double(4.94066e-324) [00000000 00000001]
IEEE(-4.94066e-324) [80000000 00000001] --> double(-4.94066e-324) [80000000 00000001]
IEEE(1.99556) [3FFFEDCB A9876543] --> double(1.99556) [3FFFEDCB A9876543]
IEEE(NAN(001)) [7FF80020 00000000] --> double(INF) [7FF00000 00000000]
IEEE(NAN(001)) [7FF00020 00000000] --> double(INF) [7FF00000 00000000]
double(0) [00000000 00000000] --> IEEE(0) [00000000 00000000]
double(-0) [80000000 00000000] --> IEEE(0) [00000000 00000000]
double(1) [3FF00000 00000000] --> IEEE(1) [3FF00000 00000000]
double(-1) [BFF00000 00000000] --> IEEE(-1) [BFF00000 00000000]
double(2) [40000000 00000000] --> IEEE(2) [40000000 00000000]
double(-2) [C0000000 00000000] --> IEEE(-2) [C0000000 00000000]
double(3) [40080000 00000000] --> IEEE(3) [40080000 00000000]
double(-3) [C0080000 00000000] --> IEEE(-3) [C0080000 00000000]
double(INF) [7FF00000 00000000] --> IEEE(INF) [7FF00000 00000000]
double(-INF) [FFF00000 00000000] --> IEEE(-INF) [FFF00000 00000000]
double(2.22507e-308) [00100000 00000000] --> IEEE(2.22507e-308) [00100000 00000000]
double(2.22507e-308) [00100000 80000000] --> IEEE(2.22507e-308) [00100000 80000000]
double(1.11254e-308) [00080000 00000000] --> IEEE(1.11254e-308) [00080000 00000000]
double(1.061e-314) [00000000 80000000] --> IEEE(1.061e-314) [00000000 80000000]
double(4.94066e-324) [00000000 00000001] --> IEEE(4.94066e-324) [00000000 00000001]
double(2.47033e-324) [00000000 00000000] --> IEEE(0) [00000000 00000000]
double(1.99556) [3FFFEDCB A9876543] --> IEEE(1.99556) [3FFFEDCB A9876543]
double(NAN(001)) [7FF80020 00000000] --> IEEE(INF) [7FF00000 00000000]
double(NAN(001)) [7FF80020 00000000] --> IEEE(INF) [7FF00000 00000000]
IEEE(0) [0000 00000000 00000000] --> extended(0) [0000 00000000 00000000]
IEEE(-0) [8000 00000000 00000000] --> extended(-0) [8000 00000000 00000000]
IEEE(1) [3FFF 80000000 00000000] --> extended(1) [3FFF 80000000 00000000]
IEEE(-1) [BFFF 80000000 00000000] --> extended(-1) [BFFF 80000000 00000000]
IEEE(2) [4000 80000000 00000000] --> extended(2) [4000 80000000 00000000]
IEEE(-2) [C000 80000000 00000000] --> extended(-2) [C000 80000000 00000000]
IEEE(INF) [7FFF 00000000 00000000] --> extended(INF) [7FFF 00000000 00000000]
IEEE(-INF) [FFFF 00000000 00000000] --> extended(-INF) [FFFF 00000000 00000000]
IEEE(NAN(001)) [7FFF 80010000 00000000] --> extended(INF) [7FFF 00000000 00000000]
IEEE(NAN(001)) [7FFF 00010000 00000000] --> extended(INF) [7FFF 00000000 00000000]
IEEE(1.99111) [3FFF FEDCBA98 76543210] --> extended(1.99111) [3FFF FEDCBA98 76543210]
extended(0) --> IEEE(0) [0000 00000000 00000000]
extended(-0) --> IEEE(0) [0000 00000000 00000000]
extended(1) --> IEEE(1) [3FFF 80000000 00000000]
extended(-1) --> IEEE(-1) [BFFF 80000000 00000000]
extended(2) --> IEEE(2) [4000 80000000 00000000]
extended(-2) --> IEEE(-2) [C000 80000000 00000000]
extended(INF) --> IEEE(INF) [7FFF 00000000 00000000]
extended(-INF) --> IEEE(-INF) [FFFF 00000000 00000000]
extended(NAN(001)) --> IEEE(INF) [7FFF 00000000 00000000]
extended(NAN(001)) --> IEEE(INF) [7FFF 00000000 00000000]
extended(5.94866e+4931) --> IEEE(5.94866e+4931) [7FFE 80000000 00000000]
extended(1e-4927) --> IEEE(1e-4927) [0000 80000000 00000000]
extended(1e-4927) --> IEEE(1e-4927) [0000 00000000 00000001]
extended(1.99111) --> IEEE(1.99111) [3FFF FEDCBA98 76543210]
*/

#endif TEST_FP

