/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Dec 16 21:19:27 2006
 */
/* Compiler settings for C:\Program Files\Microsoft Speech SDK 5.1\Samples\CPP\Engines\TTS\TtsEng.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_SAMPLETTSENGLib = {0x7192AA2F,0xF759,0x43e9,{0x91,0xE7,0x22,0x63,0x71,0xEF,0x6B,0x2F}};


const CLSID CLSID_SampleTTSEngine = {0xBE985C8D,0xBE32,0x4A22,{0xAA,0x93,0x55,0xC1,0x6A,0x6D,0x1D,0x91}};


#ifdef __cplusplus
}
#endif

