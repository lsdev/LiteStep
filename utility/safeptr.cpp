#include "safeptr.h"

BOOL IsValidStringPtr(LPCSTR lpsz)
{
	return ((lpsz != NULL) && (IsBadStringPtr(lpsz, (UINT) - 1) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtr(LPCSTR lpsz, UINT_PTR ucchMax)
#else
BOOL IsValidStringPtrCch(LPCSTR lpsz, UINT_PTR ucchMax)
#endif
{
	return ((lpsz != NULL) && (IsBadStringPtr(lpsz, ucchMax) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtr(LPSTR lpsz)
#else
BOOL IsValidStringWritePtr(LPSTR lpsz)
#endif
{
	return ((lpsz != NULL) && (IsBadWritePtr(lpsz, (strlen(lpsz) * sizeof(char))) == 0));
}
#ifdef __cplusplus
BOOL IsValidStringPtr(LPSTR lpsz, UINT_PTR ucchMax)
#else
BOOL IsValidStringWritePtrCch(LPSTR lpsz, UINT_PTR ucchMax)
#endif
{
	return ((ucchMax > 0) && (lpsz != NULL) && (IsBadWritePtr(lpsz, (ucchMax * sizeof(char))) == 0));
}
BOOL IsValidReadPtr(CONST VOID *lp)
{
	return ((lp != NULL) && (IsBadReadPtr(lp, sizeof(lp)) == 0));
}
#ifdef __cplusplus
BOOL IsValidReadPtr(CONST VOID *lp, UINT_PTR ucb)
#else
BOOL IsValidReadPtrUcb(CONST VOID *lp, UINT_PTR ucb)
#endif
{
	return ((lp != NULL) && (IsBadReadPtr(lp, ucb) == 0));
}
BOOL IsValidWritePtr(LPVOID lp)
{
	return ((lp != NULL) && (IsBadWritePtr(lp, sizeof(lp)) == 0));
}
#ifdef __cplusplus
BOOL IsValidWritePtr(LPVOID lp, UINT_PTR ucb)
#else
BOOL IsValidWritePtrUcb(LPVOID lp, UINT_PTR ucb)
#endif
{
	return ((lp != NULL) && (IsBadWritePtr(lp, ucb) == 0));
}
BOOL IsValidCodePtr(FARPROC lfpn)
{
	return ((lfpn != NULL) && (IsBadCodePtr(lfpn) == 0));
}
