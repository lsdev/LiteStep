
#ifndef __SAFEPTR_H
#define __SAFEPTR_H

#include "common.h"

BOOL IsValidStringPtr(LPCSTR lpsz);
#ifdef __cplusplus
BOOL IsValidStringPtr(LPCSTR lpsz, UINT_PTR ucchMax);
#else
BOOL IsValidStringPtrCch(LPCSTR lpsz, UINT_PTR ucchMax);
#endif
#ifdef __cplusplus
BOOL IsValidStringPtr(LPSTR lpsz);
BOOL IsValidStringPtr(LPSTR lpsz, UINT_PTR ucchMax);
#else
BOOL IsValidStringReadPtr(LPSTR lpsz);
BOOL IsValidStringReadPtrCch(LPSTR lpsz, UINT_PTR ucchMax);
#endif
BOOL IsValidReadPtr(CONST VOID *lp);
#ifdef __cplusplus
BOOL IsValidReadPtr(CONST VOID *lp, UINT_PTR ucb);
#else
BOOL IsValidReadPtrUcb(CONST VOID *lp, UINT_PTR ucb);
#endif
BOOL IsValidWritePtr(LPVOID lp);
#ifdef __cplusplus
BOOL IsValidWritePtr(LPVOID lp, UINT_PTR ucb);
#else
BOOL IsValidWritePtrUcb(LPVOID lp, UINT_PTR ucb);
#endif
BOOL IsValidCodePtr(FARPROC lfpn);

#endif // _SAFEPTR_H
