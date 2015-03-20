/************************************************************************/
/*                                                                      */
/*	dpcutil.h  --	Interface Declarations for DPCUTIL.DLL				*/
/*                                                                      */
/************************************************************************/
/*	Author: Gene Apperson												*/
/*	Copyright 2003, 2004, Digilent Inc.									*/
/************************************************************************/
/*  File Description:                                                   */
/*                                                                      */
/*	This header file contains the interface declarations for the		*/
/*	applications programming interface to the Digilent DPCUTIL.DLL		*/
/*																		*/
/*	This DLL provides API services to provide an interface for a		*/
/*	configuration channel and a data channel into a Digilent System 	*/
/*	board using one of the Digilent Communications Interface Modules.	*/
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*	02/26/2003(GeneA): Created											*/
/*  04/11/2005(Seth):  Added SPI interface                              */
/*                                                                      */
/************************************************************************/

#if !defined(DPCUTIL_INCLUDED)
#define	DPCUTIL_INCLUDED

#if !defined(DPCDEFS_INCLUDED)
#include	<dpcdefs.h>
#endif

/* ------------------------------------------------------------ */
/*				Miscellaneous Declarations						*/
/* ------------------------------------------------------------ */

#if !defined(DPCAPI)
#if defined(__cplusplus)
#define	DPCAPI extern "C" __declspec(dllimport)
#else
#define	DPCAPI __declspec(dllimport)
#endif
#endif

/* ------------------------------------------------------------ */
/*				Type Declarations								*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Interface Procedure Declarations				*/
/* ------------------------------------------------------------ */

DPCAPI	BOOL	DpcInit(ERC * perc);
DPCAPI	void	DpcTerm(void);

DPCAPI	BOOL	DpcGetDpcVersion(char * szVersion, ERC *perc);
DPCAPI	BOOL	DpcStartNotify(HWND hwndTemp, WORD idNotifyTemp, ERC *perc);
DPCAPI	BOOL	DpcEndNotify(HWND hwndTemp, ERC *perc);

DPCAPI	BOOL	DpcGetVersion(HANDLE hif, BYTE * rgbVersion, int cbVersion,
							  ERC * perc, TRID * ptrid);

DPCAPI  BOOL    DpcWaitForTransaction(HANDLE hif, TRID trid, ERC * perc);
DPCAPI  BOOL    DpcPendingTransactions(HANDLE hif, int * pctran, ERC * perc);
DPCAPI  BOOL    DpcQueryConfigStatus(HANDLE hif, TRID trid, TRS * ptrs, ERC * perc);
DPCAPI  BOOL    DpcAbortConfigTransaction(HANDLE hif, TRID trid, ERC * perc);
DPCAPI  BOOL    DpcClearConfigStatus(HANDLE hif, TRID trid, ERC * perc);
DPCAPI  ERC     DpcGetFirstError(HANDLE hif);

DPCAPI  BOOL    DpcOpenJtag(HANDLE * phif, char * szdvc, ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcCloseJtag(HANDLE hif, ERC * perc);
DPCAPI  BOOL    DpcEnableJtag(HANDLE hif, ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcDisableJtag(HANDLE hif, ERC * perc, TRID * ptrid);

DPCAPI  BOOL    DpcSetTmsTdiTck(HANDLE hif, BOOL fTms, BOOL fTdi, BOOL fTck, 
							  ERC * perc, TRID * ptrid);

DPCAPI  BOOL    DpcPutTdiBits(HANDLE hif, int cbit, BYTE * rgbSnd, BOOL bitTms, 
							BOOL fReturnTdo, BYTE * rgbRcv,
							ERC * perc, TRID * ptrid);

DPCAPI  BOOL    DpcPutTmsTdiBits(HANDLE hif, int cbit, BYTE * rgbSnd, 
							   BOOL fReturnTdo, BYTE * rgbRcv,
							   ERC * perc, TRID * ptrid);

DPCAPI  BOOL    DpcGetTdoBits(HANDLE hif, int cbits, BOOL bitTdi, BOOL bitTms,
							BYTE *rgbRcv, ERC *perc, TRID *ptrid);

DPCAPI  BOOL    DpcOpenData(HANDLE * phif, char * szdvc, ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcCloseData(HANDLE hif, ERC * perc);
DPCAPI  BOOL    DpcPutReg(HANDLE hif, BYTE bAddr, BYTE bData,
							  ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcGetReg(HANDLE hif, BYTE bAddr, BYTE * pbData,
							 ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcPutRegSet(HANDLE hif, BYTE * rgbAddr, BYTE * rgbData,
							   int cbData, ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcGetRegSet(HANDLE hif, BYTE * rgbAddr, BYTE * rgbData,
							  int cbData, ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcPutRegRepeat(HANDLE hif, BYTE bAddr, BYTE * rgbData,
								int cbData,	ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcGetRegRepeat(HANDLE hif, BYTE bAddr, BYTE * rgbData,
							   int cbData, ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcStreamWrite(HANDLE hif, BYTE * rgbData, int cbData,	
                               ERC * perc, TRID * ptrid);
DPCAPI  BOOL    DpcStreamRead(HANDLE hif, BYTE * rgbData, int cbData, 
                              ERC * perc, TRID * ptrid);


/* Device Manager Functions.
** The device manager is used to maintain the table of Digilent
** interface modules installed in the system.
*/
DPCAPI void		DvmgStartConfigureDevices(HWND hWnd, ERC * perc);

DPCAPI int		DvmgGetDevCount(ERC * perc);
DPCAPI BOOL		DvmgGetDevName(int idvc, char * szdvcTemp, ERC * perc);
DPCAPI BOOL		DvmgGetDevType(int idvc, int * dvtp, ERC * perc);
DPCAPI int		DvmgGetDefaultDev(ERC * perc);


/* ------------------------------------------------------------ */

#endif					// DPCUTIL_INCLUDED

/************************************************************************/
