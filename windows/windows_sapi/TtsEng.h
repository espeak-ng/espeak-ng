/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Dec 16 21:19:27 2006
 */
/* Compiler settings for C:\Program Files\Microsoft Speech SDK 5.1\Samples\CPP\Engines\TTS\TtsEng.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __TtsEng_h__
#define __TtsEng_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __SampleTTSEngine_FWD_DEFINED__
#define __SampleTTSEngine_FWD_DEFINED__

#ifdef __cplusplus
typedef class SampleTTSEngine SampleTTSEngine;
#else
typedef struct SampleTTSEngine SampleTTSEngine;
#endif /* __cplusplus */

#endif 	/* __SampleTTSEngine_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "sapiddk.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_TtsEng_0000 */
/* [local] */ 

typedef struct  VOICEITEM
    {
    LPCWSTR pText;
    ULONG ulTextLen;
    ULONG ulNumAudioBytes;
    BYTE __RPC_FAR *pAudio;
    }	VOICEITEM;



extern RPC_IF_HANDLE __MIDL_itf_TtsEng_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TtsEng_0000_v0_0_s_ifspec;


#ifndef __SAMPLETTSENGLib_LIBRARY_DEFINED__
#define __SAMPLETTSENGLib_LIBRARY_DEFINED__

/* library SAMPLETTSENGLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SAMPLETTSENGLib;

EXTERN_C const CLSID CLSID_SampleTTSEngine;

#ifdef __cplusplus

class DECLSPEC_UUID("BE985C8D-BE32-4A22-AA93-55C16A6D1D91")
SampleTTSEngine;
#endif
#endif /* __SAMPLETTSENGLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
