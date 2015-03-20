/************************************************************************/
/*                                                                      */
/*  DjtgTwoWireDemo.cpp  --  DjtgTwoWireDemo main program               */
/*                                                                      */
/************************************************************************/
/*  Author: MTA                                                         */
/*  Copyright 2012 Digilent, Inc.                                       */
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*  The DjtgTwoWireDemo demonstrates how to read the IDCODEs of one or  */
/*  more IEEE 1149.1-2011 targets that are connected in series to an    */
/*  IEEE 1149.7-2009 ADTAPC that supports the following scan formats:   */
/*  JScan0 - JScan3, MSCan, OScan0, and OScan1. The application         */
/*  functions as follows:                                               */
/*      1. Reset the tap controller.                                    */
/*      2. Read IDCODE's in four wire mode using the default            */
/*         configuration of the DJTG port (JScan0, compatible with      */
/*         JScan0-JScan3 targets). The received IDCODES are output to   */
/*         stdout.                                                      */
/*      3. Configure the target and DJTG port with the RDY bit count.   */
/*         This bit count can be specified using the "-rdyc" option. If */
/*         no ready bit count is specified then a default bit count of  */
/*         1 is used.                                                   */
/*      4. Configure the target and DJTG port with the DLY bit count.   */
/*         This bit count can be specified using the "-dlyc" option. If */
/*         no delay bit count is specified then a default bit count of  */
/*         0 is used.                                                   */
/*      5. Configure the target and DJTG port with the specified scan   */
/*         format. The scan format can be specified using the "-sfmt"   */
/*         option. If no scan format is specified then a default scan   */
/*         format of OScan1 is used.                                    */
/*      6. Read IDCODE's in two-wire mode and output any received       */
/*         IDCODE's to stdout.                                          */
/*                                                                      */
/*                                                                      */
/*  Required Hardware:                                                  */
/*                                                                      */
/*  This demonstration requires the following pieces of hardware:       */
/*      1. Digilent device that supports the MScan, OScan0, and OScan1  */
/*         formats. At the time of writing, the JTAG-HS2 and JTAG-SMT2  */
/*         were the only Digilent device's supporting all 3 of these    */
/*         formats.                                                     */
/*      2. A device that exposes an IEEE 1149.7-2009 ADTAPC that        */
/*         supports the MScan, OScan0, and OScan1 scan formats and is   */
/*         connected to one or more IEEE 1149.1-2001 targets. This      */
/*         application was tested against a Synopsys DesignWare         */
/*         ARCompact core with JTAG 1149.7 support running on a         */
/*         XUPV5-LX110T (ML509) board.                                  */
/*                                                                      */
/*                                                                      */
/*  Additional Notes:                                                   */
/*                                                                      */
/*  This application assumes that the DJTG port is connected to a scan  */
/*  chain consisting of a single IEEE 1149.7-2009 compatible target. It */
/*  does NOT perform scan chain discovery and will not work correctly   */
/*  in Star 4 or Star 2 scan chains that contain more than one device.  */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  07/17/2012(MTA): Created                                            */
/*                                                                      */
/************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

/* ------------------------------------------------------------ */
/*					Include File Definitions					*/
/* ------------------------------------------------------------ */

#if defined(WIN32)
	
	/* Include Windows specific headers here.
	*/
	#include <windows.h>
	
#else

	/* Include Unix specific headers here.
	*/
	

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dpcdecl.h" 
#include "djtg.h"
#include "dmgr.h"

/* ------------------------------------------------------------ */
/*				Local Type Definitions          				*/
/* ------------------------------------------------------------ */

const   DWORD   cchDescriptionMax = 64;
const   DWORD   cchOptionMax = 64;
const   DWORD   cchJtgsfMax = 16;

typedef struct {
    char    szOption[cchOptionMax + 1];
    char    szDescription[cchDescriptionMax + 1];
} OPTN ;

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

/* Define TMS bit sequences for navigation between common tap states.
*/
BYTE    rgbTLR[]        = {0x1F}; // shift 5 times
BYTE    rgbTLRRTI[]     = {0x1F}; // shift 6 times
BYTE    rgbTLRSDR[]     = {0x5F, 0x00}; // shift 9 times
BYTE    rgbSDRRTI[]     = {0x03}; // shift 3 times
BYTE    rgbRTISDR[]     = {0x01}; // shift 3 times
BYTE    rgbE1DRSDR[]    = {0x02}; // shift 3 times
BYTE    rgbE1DRRTI[]    = {0x06}; // shift 4 times

BYTE    rgbpTLR[]       = {0xAA, 0x02}; // shift 5 times
BYTE    rgbpTLRRTI[]    = {0xAA, 0x02}; // shift 6 times
BYTE    rgbpTLRSDR[]    = {0xAA, 0x22, 0x00}; // shift 9 times
BYTE    rgbpSDRRTI[]    = {0x0A}; // shift 3 times
BYTE    rgbpRTISDR[]    = {0x02}; // shift 3 times
BYTE    rgbpE1DRSDR[]   = {0x08}; // shift 3 times
BYTE    rgbpE1DRRTI[]   = {0x28}; // shift 4 times

const DWORD   cbitTLR     = 5;
const DWORD   cbitTLRRTI  = 6;
const DWORD   cbitTLRSDR  = 9;
const DWORD   cbitSDRRTI  = 3;
const DWORD   cbitRTISDR  = 3;
const DWORD   cbitE1DRSDR = 3;
const DWORD   cbitE1DRRTI = 4;

/* Declare variables used to keep track of the JTAG configuration state.
*/
BOOL    fAdvancedProto;

/* Define an array of supported command line options and descriptions.
*/
OPTN   rgoptn[] = {
    {"-d           ", "device user name or alias"},
    {"-dlyc        ", "delay count for advanced scan formats"},
    {"-f           ", "device TCK frequency in Hz"},
    {"-rdyc        ", "count of expected RDY bits (1-4)"},
    {"-sfmt        ", "JTAG Scan Format to use for the test"},
    {"-?, -help    ", "print usage, supported arguments, and options"},
    {"", ""}
};

/* Delcare variables used to keep track of command line arguments.
*/
BOOL    fDevName;
BOOL    fFreq;
BOOL    fJtgsf;
BOOL    fShowHelp;

char*   pszCmd;
char    szDevName[cchDvcNameMax + 1];
char    szJtgsf[cchJtgsfMax + 1];
DWORD   freqReq;
DWORD   cbitDlyReq;
BYTE    cbitRdyReq;

/* ------------------------------------------------------------ */
/*				Local Variables									*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

BOOL    FSendZBS( HIF hif, BYTE czbs );
BOOL    FLockZBS( HIF hif );
BOOL    FSendTwoPartCmd( HIF hif, BYTE cmd, BYTE op );

BOOL    FParseArguments( int cszArg, char* rgszArg[] );
BOOL    FHelp();

/* ------------------------------------------------------------ */
/*                  Procedure Definitions                       */
/* ------------------------------------------------------------ */
/***    main
**
**  Parameters:
**      cszArg  - number of command line arguments
**      rgszArg - array of command line argument strings
**
**  Return Values:
**      0 for success, 1 otherwise
**
**  Errors:
**
**  Description:
**      Application entry point.
*/
int 
main( int cszArg, char* rgszArg[] ) {
    
    HIF     hif;
    DWORD   freqSet;
    DPRP    dprpJtag;
    BYTE    jtgsfReq;
    DWORD   jid;
    DWORD   cjid;
    BYTE    rgbTdo[4];
    BYTE    b;
    DWORD   cretOutSet;
    DWORD   cbitDlySet;
    
    fAdvancedProto = fFalse;
    
    hif = hifInvalid;
    
    /* Parse the command and command options from the command line
    ** arguments.
    */
    if ( ! FParseArguments(cszArg, rgszArg) ) {
    
        /* If we failed to parse the command line arguments then an error
        ** message has already been output to the user.
        */
        goto lErrorExit;
    }
    
    /* If the help option was specified then we should display the
    ** available options and exit.
    */
    if ( fShowHelp ) {
        
        FHelp();
        goto lExit;
    }
    
    /* Check to see if the user specified a device name/connection string.
    */
    if ( ! fDevName ) {
        
        printf("ERROR: you must specify a device using the \"-d\" option\n");
        goto lErrorExit;
    }
    
    /* Check to see if the user specified a RDY bit count. If a RDY bit
    ** count was specified we should verify that it's within the correct
    ** range (1-4).
    */
    if (( 0 < cbitRdyReq ) && ( 4 < cbitRdyReq )) {
        
        printf("ERROR: invalid RDY bit count specified: %d\n", cbitRdyReq);
        printf("Valid RDY bit counts are between 1 and 4 bits.\n");
        goto lErrorExit;
    }
    
    /* Check to see if the user specified a scan format. If no scan format
    ** was specified then default to OScan1.
    */
    if ( fJtgsf ) {
        
        /* Map the user specified scan format to a scan format that we can
        ** pass to DjtgSetScanFormat.
        */
        if ( 0 == strcmp(szJtgsf, "MScan") ) {
            
            jtgsfReq = jtgsfMScan;
        }
        else if ( 0 == strcmp(szJtgsf, "OScan0") ) {
            
            jtgsfReq = jtgsfOScan0;
        }
        else if ( 0 == strcmp(szJtgsf, "OScan1") ) {
            
            jtgsfReq = jtgsfOScan1;
        }
        else {
            
            printf("ERROR: unsupported scan format specified: \"%s\"\n", szJtgsf);
            printf("The following scan formats are supported: ");
            printf("\"MScan\", \"OScan0\", and \"OScan1\"\n");
            goto lErrorExit;
        }
    }
    else {
        
        jtgsfReq = jtgsfOScan1;
        strcpy(szJtgsf, "OScan1");
    }
    
    /* Attempt to open the device.
    */
    if ( ! DmgrOpen(&hif, szDevName) ) {
        
        printf("ERROR: unable to open device \"%s\"\n", szDevName);
        goto lErrorExit;
    }
    
    /* Get the the properties associated with JTAG port 0. These will be
    ** used later to determine whether or not the device supports certain
    ** features required for communication with IEEE 1149.7-2009 targets.
    */
    if ( ! DjtgGetPortProperties(hif, 0, &dprpJtag) ) {
        
        printf("ERROR: failed to get JTAG port 0 properties, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
    /* Attempt to enable JTAG port 0.
    */
    if ( ! DjtgEnable(hif) ) {
        
        printf("ERROR: failed to enable JTAG, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
    /* Set the TCK frequency if one was specified by the user.
    */
    if ( fFreq ) {
        
        if ( ! DjtgSetSpeed(hif, freqReq, &freqSet) ) {
            
            printf("ERROR: DjtgSetSpeed failed, erc = %d\n", DmgrGetLastError());
            goto lErrorExit;
        }
        
        printf("JTAG TCK Frequency set to: %d Hz\n", freqSet);
    }
    
    /* At this point we don't know what state the TAP.7 controller is in.
    ** The DjtgEscape function can be used to perform a reset escape, which
    ** will reset the configuration registers of any TAP.7 controlleres
    ** that are connected to the DJTG port. Ensure that the DJTG port
    ** supports the DjtgEscape function and then perform a reset escape.
    */
    if ( dprpJtgEscape & dprpJtag ) {
        
        if ( ! DjtgEscape(hif, cedgeJtgReset, fFalse) ) {
            
            printf("ERROR: DjtgEscape failed to perform a reset escape, erc = %d\n", DmgrGetLastError());
            goto lErrorExit;
        }
    }
    else {
        
        printf("ERROR: device doesn't support the DjtgEscape function\n");
        goto lErrorExit;
    }
    
    /* Place the tap controller in the Shift-DR (SDR) state after passing
    ** through Test-Logic-Reset. This will ensure that the current control
    ** level is exited, that any 1149.1-2001 TAPC's behind the TAP.7 are
    ** connected in bypass mode, and that the IDCODE instruction has been
    ** placed in the instruction register.
    */
    if ( ! DjtgPutTmsBits(hif, fFalse, rgbTLRSDR, NULL, cbitTLRSDR, fFalse) ) {
        
        printf("ERROR: DjtgPutTmsBits to shift pattern for TLR->SDR, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
    printf("Listing device ID codes acquired in four-wire mode...\n");
    
    cjid = 0;
    do {
        
        /* Attempt to shift out the ID code of a device in the scan chain.
        ** By shifting in 0's for TDI we can tell when we've reached the
        ** end of the scan chain because we will read back an ID code of
        ** all zeros. ID codes consisting entirely of 1's should also be
        ** considered invalid and terminate the loop.
        */
        if ( ! DjtgGetTdoBits(hif, fFalse, fFalse, rgbTdo, 32, fFalse) ) {
            
            printf("ERROR: failed to get TDO bits from device, erc = %d\n", DmgrGetLastError());
            goto lErrorExit;
        }
        
        jid = (rgbTdo[3]<<24) + (rgbTdo[2]<<16) + (rgbTdo[1]<<8) + rgbTdo[0];
        
        if (( 0 != jid ) && ( 0xFFFFFFFF != jid )) {
            
            printf("    Found Device ID: %08X\n", jid);
            cjid++;
        }
    } while (( 0 != jid ) && ( 0xFFFFFFFF != jid ));
    
    if ( 0 == cjid ) {
        
        printf("ERROR: 0 devices detected in scan chain\n");
        goto lErrorExit;
    }
    
    /* Navigate from the SDR state to the RTI state without passing through
    ** TLR.
    */
    if ( ! DjtgPutTmsBits(hif, fFalse, rgbSDRRTI, NULL, cbitSDRRTI, fFalse) ) {
        
        printf("ERROR: DjtgPutTmsBits to shift pattern for SDR->RTI, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
    /* The TAP.7's confinguration registers can only be written while
    ** operating in control level 2. Attempt to enter control level 2 by
    ** setting the Zero-Bit-Scan count to 2 and locking the count.
    */
    if ( ! FSendZBS(hif, 2) ) {
        
        printf("ERROR: failed to set a ZBS count of 2\n");
        goto lErrorExit;
    }

    /* Lock the ZBS count at 2. This sets control level 2.
    */
    if ( ! FLockZBS(hif) ) {
        
        printf("ERROR: failed to lock ZBS count at 2\n");
        goto lErrorExit;
    }
    
    /* The TAP.7 controller won't respond to some commands unless its
    ** Conditional Group Membership (CGM) bit is set. This bit can be set
    ** by sending the Store Miscellaneous Control command (opcode 0) with
    ** operand 7.
    */
    if ( ! FSendTwoPartCmd(hif, 0, 7) ) {
        
        printf("ERROR: FSendTwoPartCmd(STMC, CGM=1) failed\n");
        goto lErrorExit;
    }
    
    /* Configure the RDYC register and DJTG port as necessary. By default, 
    ** the RDYC register has a value of 0 which indicates an expected RDY
    ** bit count of 1. The maximum count is 4.
    */
    if ( 0 < cbitRdyReq ) {
        
        /* Verify that the DJTG port supports setting the RDY bit count.
        */
        if ( dprpJtgReadyCnt != (dprpJtgReadyCnt & dprpJtag) ) {
            
            printf("ERROR: device doesn't support the DjtgSetReadyCnt function\n");
            goto lErrorExit;
        }
        
        /* The RDYC register can be set by sending the Store Miscellaneous
        ** Control command (opcode 0). The 5-bit operand for the command is
        ** specified as bbbxy where bbb = 2 (RdyCtl) and xy = 0-3 (expected
        ** RDY='1' bit count - 1).
        */
        b = 0x08;
        b |= (cbitRdyReq -1);
        if ( ! FSendTwoPartCmd(hif, 0, b) ) {
            
            printf("ERROR: FSendTwoPartCmd(STMC, RdyCtl) failed\n");
            goto lErrorExit;
        }
        
        /* Configure the DJTG port to expect the correct number of RDY
        ** bits. Please note that the call to DjtgSetReadyCnt doesn't have
        ** any effect until DjtgSetScanFormat is used to configure the port
        ** for a scan format that supports RDY bits.
        */
        if ( ! DjtgSetReadyCnt(hif, cbitRdyReq, NULL, &cretOutSet) ) {
            
            printf("ERROR: DjtgSetReadyCnt failed to set RDY bit count to %d, erc = %d\n", cbitRdyReq, DmgrGetLastError());
            goto lErrorExit;
        }
        
        printf("Expected RDY Bit Count Set to: %d\n", cbitRdyReq);
        printf("Output bitframe retry count: %d\n", cretOutSet);
    }
    
    /* Configure the DLYC register and DJTG port DLY bit count as
    ** necessary. By default the DLYC register has a value of 0 which
    ** indicates no DTS driven delay between scan packets.
    */
    if ( 0 < cbitDlyReq ) {
        
        /* Verify that the DJTG port supports setting the DLY bit count.
        */
        if ( dprpJtgDelayCnt != (dprpJtag & dprpJtgDelayCnt) ) {
            
            printf("ERROR: device doesn't support the DjtgSetDelayCnt function\n");
            goto lErrorExit;
        }
        
        /* The DLYC register can be set by sending the Store Miscellaneous
        ** Control command (opcode 0). The 5-bit operand for the command is
        ** specified as bbbxy where bbb = 3 (DlyCtl) and xy = 0-3 (no
        ** delay, 1 bit of delay, 2 bits of delay, or variable length
        ** delay).
        */
        b = 0x0C;
        if ( 1 == cbitDlyReq ) {
            b |= 0x01;
        }
        else if ( 2 == cbitDlyReq ) {
            b |= 0x02;
        }
        else {
            b |= 0x03;
        }
        
        if ( ! FSendTwoPartCmd(hif, 0, b) ) {
            
            printf("ERROR: FSendTwoPartCmd(STMC, DlyCtl) failed\n");
            goto lErrorExit;
        }
        
        /* Configure the DJTG port for the correct number of delay bits.
        ** Please note that the call to DjtgSetDelayCnt doesn't have any
        ** effect until DjtgSetScanFormat is used to confingure the port
        ** for a scan format that's part of the advanced protocol.
        */
        if ( ! DjtgSetDelayCnt(hif, cbitDlyReq, &cbitDlySet, fFalse) ) {
            
            printf("ERROR: DjtgSetReadyCnt failed to set DLY bit count to %d, erc = %d\n", cbitDlyReq, DmgrGetLastError());
            goto lErrorExit;
        }
        
        printf("Delay count requested: %d\n", cbitDlyReq);
        printf("Delay count set: %d\n", cbitDlySet);
    }
    
    /* Determine the OP code required to configure the target for the
    ** specified scan format.
    */
    switch ( jtgsfReq ) {
        
        case jtgsfMScan:
            b = 16;
            break;
            
        case jtgsfOScan0:
            b = 8;
            break;
            
        case jtgsfOScan1:
            b = 9;
            break;
            
        default:
            printf("ERROR: unsupported scan format specified %d\n", jtgsfReq);
            goto lErrorExit;
    }
    
    /* Attempt to set the scan format to the specified format. The scan\
    ** format is set by issuing the Store Scan Format command (STFMT,
    ** opcode = 3). The operand depends on the desired scan format. A list
    ** of operands can be found in Table 23-2 of the IEEE 1149.7-2009
    ** specification.
    */
    if ( ! FSendTwoPartCmd(hif, 3, b) ) {
        
        printf("ERROR: FSendTwoPartCmd(STFMT, %d) failed\n", b);
        goto lErrorExit;
    }
    
    /* Now that the target (TAP.7) has been configured for the new scan
    ** format the DJTG port must be configured to use the same format.
    */
    if ( ! DjtgSetScanFormat(hif, jtgsfReq, fFalse) ) {
        
        printf("ERROR: DjtgSetScanFormat(%d, fFalse) failed, erc = %d\n", jtgsfReq, DmgrGetLastError());
        goto lErrorExit;
    }
    
    printf("Scan Format Set To: %s\n", szJtgsf);
    
    /* Set the fAdvancedProto to fTrue so that the FSendTwoPartCmd function
    ** will insert a dummy scan packet prior to sending the check packet.
    */
    fAdvancedProto = fTrue;
    
    /* In order to communicate with any 1149.1 TAP's that are sitting
    ** behind the TAP.7 we must exit control level 2 by setting the ECL bit
    ** to a 1. The ECL bit is set to a '1' by issuing the Store
    ** Miscellaneous Control (STMC, opcode = 0) command with an operand of
    ** 1.
    */
    if ( ! FSendTwoPartCmd(hif, 0, 1) ) {
        
        printf("ERROR: failed to send ECL command\n");
        goto lErrorExit;
    }
    
    /* Navigate from RTI to SDR. Since we pass through Capture-DR the
    ** IDCODE corresponding to each device should be reloaded into the data
    ** register when we enter SDR.
    */
    if ( ! DjtgPutTmsBits(hif, fFalse, rgbRTISDR, NULL, cbitRTISDR, fFalse) ) {
        
        printf("ERROR: DjtgPutTmsBits to shift pattern for RTI->SDR, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
    printf("Listing device ID codes acquired in two-wire mode...\n");
    
    cjid = 0;
    do {
        
        /* Attempt to shift out the ID code of a device in the scan chain.
        ** By shifting in 0's for TDI we can tell when we've reached the
        ** end of the scan chain because we will read back an ID code of
        ** all zeros. ID codes consisting entirely of 1's should also be
        ** considered invalid and terminate the loop.
        */
        if ( ! DjtgGetTdoBits(hif, fFalse, fFalse, rgbTdo, 32, fFalse) ) {
            
            printf("ERROR: failed to get TDO bits from device, erc = %d\n", DmgrGetLastError());
            goto lErrorExit;
        }
        
        jid = (rgbTdo[3]<<24) + (rgbTdo[2]<<16) + (rgbTdo[1]<<8) + rgbTdo[0];
        
        if (( 0 != jid ) && ( 0xFFFFFFFF != jid )) {
            
            printf("    Found Device ID: %08X\n", jid);
            cjid++;
        }
    } while (( 0 != jid ) && ( 0xFFFFFFFF != jid ));
    
    if ( 0 == cjid ) {
        
        printf("ERROR: 0 devices detected in scan chain\n");
        goto lErrorExit;
    }
    
    /* Disable the DJTG port.
    */
    if ( ! DjtgDisable(hif) ) {
        
        printf("ERROR: failed to disable JTAG port, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
    /* Close the device handle.
    */
    if ( ! DmgrClose(hif) ) {
        
        printf("ERROR: failed to close device handle, erc = %d\n", DmgrGetLastError());
        goto lErrorExit;
    }
    
lExit:
    
    return 0;
    
lErrorExit:
    
    if ( hifInvalid != hif ) {
        
        DjtgDisable(hif);
        DmgrClose(hif);
    }
    
    return 1;
}

/* ------------------------------------------------------------ */
/***    FSendZBS
**
**  Parameters:
**      hif     - open handle for the device
**      czbs    - number of zero bit scans to perform
**
**  Return Values:
**      fTrue for success, fFalse otherwise
**
**  Errors:
**
**  Description:
**      This function uses DjtgPutTmsBits to perform one or more zero bit
**      scans. Please see the IEEE 1149.7-2009 specification for more
**      information about zero bit scans.
**
**  Notes:
**      This function assumes that the TAP controller is currently in the
**      RTI state.
*/
BOOL    
FSendZBS( HIF hif, BYTE czbs ) {
    
    BYTE    b;
    
    if ( 0 == czbs ) {
        
        return fFalse;
    }
    
    czbs--;
    
    /* By assigning 0x0D and shifting out 4 bits we can make one of the 
    ** following state transitions:  RTI->SELDR->CDR->E1DR->UDR or
    ** UDR->SELDR->CDR->E1DR->UDR. By shifting out 5 TMS bits we can make
    ** the following transition: UDR->SELDR->CDR->E1DR->UDR->RTI.
    */
    b = 0x0D;
    
    while ( 0 < czbs ) {
        
        if ( ! DjtgPutTmsBits(hif, fFalse, &b, NULL, 4, fFalse) ) {
            
            printf("ERROR: FSendZBS->DjtgPutTmsBits failed to shift 4 bits, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        czbs--;
    }
    
    /* Perform the last ZBS and enter RTI.
    */
    if ( ! DjtgPutTmsBits(hif, fFalse, &b, NULL, 5, fFalse) ) {
        
        printf("ERROR: FSendZBS->DjtgPutTmsBits failed to shift 5 bits, erc = %d\n", DmgrGetLastError());
        return fFalse;
    }
    
    return fTrue;
}

/* ------------------------------------------------------------ */
/***    FLockZBS
**
**  Parameters:
**      hif     - open handle for the device
**
**  Return Values:
**      fTrue for success, fFalse otherwise
**
**  Errors:
**
**  Description:
**      This function uses DjtgPutTmsBits to shift out the TMS bit pattern
**      required to lock the ZBS count. Please see the IEEE 1149.7-2009
**      specification for more informaiton on zero bit scans (ZBS).
**
**  Notes:
**      This function assumes that the TAP controller is currently in the
**      RTI state.
*/
BOOL    
FLockZBS( HIF hif ) {
    
    BYTE    b;
    
    /* By assigning 0x19 and shifting 6 TMS bits we make the following
    ** state transitions, which should lock the ZBS count and leave us in
    ** RTI: RTI->SELDR->CDR->SDR->E1DR->UDR->RTI.
    */
    b = 0x19;
    
    if ( ! DjtgPutTmsBits(hif, fFalse, &b, NULL, 6, fFalse) ) {
        
        printf("error: FLockZBS->DjtgPutTmsBits failed to shift 6 bits, erc = %d\n", DmgrGetLastError());
        return fFalse;
    }
    
    return fTrue;
}

/* ------------------------------------------------------------ */
/***    FSendTwoPartCmd
**
**  Parameters:
**      hif     - open handle for the device
**      cmd     - command part 1 (operator)
**      op      - command part 2 (operand)
**
**  Return Values:
**      fTrue for success, fFalse otherwise
**
**  Errors:
**
**  Description:
**      This function uses DjtgPutTmsBits to sends a two part command to an
**      1149.7 target that has been configured for control level 2.
**
**  Notes:
**      This function assumes that the TAP controller is currently in the
**      RTI state.
**      
**      The fAdvancedProto global variable should be set to fTrue
**      immediately after the target and DJTG port are configured for a
**      scan format that's part of the advanced protocol (MScan,
**      OScan0-OSca7, SScan0-SScan3). This tells the function that a dummy
**      scan packet must preced the check packet. The fAdvancedProto
**      variable should be set to fFalse immediately after any operation
**      that causes the target to transition to the standard protocol
**      (JScan0-JScan3).
*/
BOOL    
FSendTwoPartCmd( HIF hif, BYTE cmd, BYTE op ) {
    
    BYTE    rgbTms[4];
    DWORD   cbitDlySet;
    DWORD   cbitDlySet2;
    
    /* Verify that the command and operand are within the range that's
    ** supported by the 1149.7 specification.
    */
    if ( 0xE0 & cmd ) {
        
        printf("ERROR: invalid command opcode specified: 0x%02X\n", cmd);
        return fFalse;
    }
    
    if ( 0xE0 & op ) {
        
        printf("ERROR: invalid command operand specified: 0x%02X\n", op);
        return fFalse;
    }
    
    /* Send the first part of the command (op code). The bit sequence to
    ** send the command depends on both the current tap state (assumed to
    ** be RTI) and the number of times (0-31) that we must pass through the
    ** SDR state.
    */
    if ( 0 < cmd ) {
        
        /* Assume we are in the RTI state, pass through SELDR, CDR, and
        ** park in SDR.
        */
        
        rgbTms[0] = 0x01; // RTI->SELDR->CDR->SDR (3-bits)
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 3, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP1 bit 0 failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        /* Re-enter Shift-DR (SDR) cmd - 1 times.
        */
        memset(rgbTms, 0, 4*sizeof(BYTE));
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, cmd - 1, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP1 failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        /* Now exit Shift-DR and park in the RTI state.
        */
        rgbTms[0] = 0x03; // SDR->E1DR->UDR->RTI (3-bits)
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 3, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP1 UDR failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
    }
    else {
        
        /* Assume we are in the RTI state, pass through SELDR, CDR, E1DR,
        ** UDR, and park in RTI.
        */
        
        rgbTms[0] = 0x0D; // RTI->SELDR->CDR->E1DR->UDR->RTI (5-bits)
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 5, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP1 failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
    }
    
    /* Send the second part of the command. The bit sequence required to
    ** send the operand depends on both the current tap state (RTI) and 
    ** the number of times (0-31) that we must pass through the SDR state.
    */
    if ( 0 < op ) {
        
        /* Since we are in the RTI state and the operand is greater than 0
        ** we need to enter the SDR state.
        */
        rgbTms[0] = 0x01; // RTI->SELDR->CDR->SDR (3-bits)
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 3, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP2 bit 0 failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        /* Re-enter Shift-DR (SDR) op - 1 times.
        */
        memset(rgbTms, 0, 4*sizeof(BYTE));
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, op - 1, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP2 failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        /* Now exit Shift-DR and park in the RTI state.
        */
        rgbTms[0] = 0x03; // SDR->E1DR->UDR->RTI (3-bits)
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 3, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP2 RTI failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
    }
    else {
        
        /* Since we are in the RTI state and the operand is 0 we need to
        ** make the following state progression:
        ** RTI->SELDR->CDR->E1DR->UDR->RTI.
        */
        rgbTms[0] = 0x0D; // RTI->SELDR->CDR->E1DR->UDR->RTI (5-bits)
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 5, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits for CP2 failed, erc = %d\n", DmgrGetLastError());
            return fFalse;
        }
    }
    
    /* When the target has been or is being configured for the advanced
    ** protocol a check packet specifying the CP_END directive must be
    ** sent following command part 2. The CP_END directive consists of four
    ** 0's being clocked onto TMSC. Since we are already in RTI it
    ** shouldn't hurt to send the CP_END directive even when we don't need
    ** to.
    */
    
    if ( fAdvancedProto ) {
        
        /* Figure 21-9 of Section 21.7 of the IEEE 1149.7-2009
        ** specification indicates that a dummy scan packet consisting of
        ** 1 bit of data must be sent after command part 2 and prior to the
        ** check packet when the advanced protocol is being used.
        */
        
        /* According to section 24.3.2 of the IEEE 1149.7-2009
        ** specification the dummy scan packet that preceeds the check
        ** packet does NOT contain a delay element regardless of the
        ** present DLYC setting. This means that if we previously set a
        ** non-zero delay we need to set the delay back to 0 prior to
        ** sending the dummy scan packet and then set the delay back to the
        ** correct value before sending the check packet, which doesn't
        ** include delay elements.
        */
        if ( ! DjtgGetDelayCnt(hif, &cbitDlySet, NULL) ) {
            
            printf("ERROR: DjtgGetDelayCnt failed, erc= %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        if ( 0 < cbitDlySet ) {
            
            /* DTS delay is currently enabled. Disable it.
            */
            if (( ! DjtgSetDelayCnt(hif, 0, &cbitDlySet2, fFalse) ) || 
                ( 0 != cbitDlySet2 )) {
                
                printf("ERROR: DjtgSetDelayCnt failed to set DLY cnt to zero, erc= %d\n", DmgrGetLastError());
                return fFalse;
            }
        }
        
        /* Send the dummy scan packet.
        */
        rgbTms[0] = 0x00;
        if ( ! DjtgPutTmsBits(hif, fFalse, rgbTms, NULL, 1, fFalse) ) {
            
            printf("ERROR: DjtgPutTmsBits failed to put dummy bit, erc= %d\n", DmgrGetLastError());
            return fFalse;
        }
        
        if ( 0 < cbitDlySet ) {
            
            /* DTS delay was requested. Set the delay back to its previous
            ** value.
            */
            if ( ! DjtgSetDelayCnt(hif, cbitDlySet, &cbitDlySet2, fFalse) ) {
                
                printf("ERROR: DjtgSetDelayCnt failed to set DLY cnt to %d, erc= %d\n", cbitDlySet, DmgrGetLastError());
                return fFalse;
            }
        }
    }
    
    /* Send the CP_END directive with 0 CP_NOP bits. This will result in
    ** four zeros being output on TMSC that correspond to four rising edges
    ** on the TCKC pin. The tap controller will remain in the RTI state
    ** regardless of whether or not the target is expecting a check packet.
    */
    if ( ! DjtgCheckPacket(hif, 0, fFalse, fFalse) ) {
        
        printf("ERROR: DjtgCheckPacket failed to send check packet, erc = %d\n", DmgrGetLastError());
        return fFalse;
    }
    
    return fTrue;
}

/* ------------------------------------------------------------ */
/***    FParseArguments
**
**  Parameters:
**      cszArg  - number of command line arguments
**      rgszArg - array of command line argument strings
**
**  Return Values:
**      fTrue for success, fFalse otherwise
**
**  Errors:
**
**  Description:
**      Parse the command line arguments into commands and options.
*/
BOOL    
FParseArguments( int cszArg, char* rgszArg[] ) {
    
    int     iszArg;
    DWORD   ich;
    
    /* Set all of the flags to their default values of fFalse. Flags will
    ** only be set to fTrue when the corresponding command or option has
    ** been found and parsed from the command line arguments.
    */
    fDevName = fFalse;
    fFreq = fFalse;
    fJtgsf = fFalse;
    fShowHelp = fFalse;
    
    /* Set all of the string parameters to their default values: empty
    ** strings.
    */
    szDevName[0] = '\0';
    szJtgsf[0] = '\0';
    freqReq = 0;
    cbitDlyReq = 0;
    cbitRdyReq = 0;
    
    /* Get a pointer to the command string used to launch the application.
    ** This is used when printing the usage as part of the help command.
    */
    pszCmd = rgszArg[0];
    
    /* Parse the command line arguments into commands and options.
    */
    iszArg = 1;
    while ( iszArg < cszArg ) {
        
        /* Check for the -d option. This specifies the device name
        ** used for the showinfo command.
        */
        if ( 0 == strcmp(rgszArg[iszArg], "-d") ) {
            
            iszArg++;
            
            if ( iszArg >= cszArg ) {
                
                printf("ERROR: no device name string specified\n");
                return fFalse;
            }
            
            if (( NULL == rgszArg[iszArg] ) || 
                ( '-' == rgszArg[iszArg][0] ) || 
                ( cchDvcNameMax < strlen(rgszArg[iszArg]) )) {
                
                printf("ERROR: invalid device name string specified\n");
                return fFalse;
            }
            
            strcpy(szDevName, rgszArg[iszArg]);
            fDevName = fTrue;
        }
        
        /* Check for the -dlyc option. This specifies the count of delay
        ** bits that are to be inserted between scan packets when an
        ** advanced scan format is being used.
        */
        else if ( 0 == strcmp(rgszArg[iszArg], "-dlyc") ) {
            
            iszArg++;
            
            if (( iszArg >= cszArg ) || ( NULL == rgszArg[iszArg] )) {
                
                printf("ERROR: no delay count specified\n");
                return fFalse;
            }
            
            /* Make sure that the string consists entirely of digits 0-9.
            */
            ich = 0;
            while ( '\0' != rgszArg[iszArg][ich] ) {
                
                if ( 0 == isdigit(rgszArg[iszArg][ich]) ) {
                    
                    printf("ERROR: invalid character detected in delay count string: %c\n", rgszArg[iszArg][ich]);
                    return fFalse;
                }
                
                ich++;
            }
            
            cbitDlyReq = strtol(rgszArg[iszArg], NULL, 10);
        }
        
        /* Check for the -f option. This specifies the TCK/SCK frequency
        ** used by the device when shifting JTAG or SPI data.
        */
        else if ( 0 == strcmp(rgszArg[iszArg], "-f") ) {
            
            iszArg++;
            
            if (( iszArg >= cszArg ) || ( NULL == rgszArg[iszArg] )) {
                
                printf("ERROR: no frequency specified\n");
                return fFalse;
            }
            
            /* Make sure that the string consists entirely of digits 0-9.
            */
            ich = 0;
            while ( '\0' != rgszArg[iszArg][ich] ) {
                
                if ( 0 == isdigit(rgszArg[iszArg][ich]) ) {
                    
                    printf("ERROR: invalid character detected in frequency string: %c\n", rgszArg[iszArg][ich]);
                    return fFalse;
                }
                
                ich++;
            }
            
            freqReq = strtol(rgszArg[iszArg], NULL, 10);
            fFreq = fTrue;
        }
        
        /* Check for the -rdyc option. This specifies the count of expected
        ** RDY='1' bits that preceeed a TDO bits for scan formats that
        ** include RDY bits.
        */
        else if ( 0 == strcmp(rgszArg[iszArg], "-rdyc") ) {
            
            iszArg++;
            
            if (( iszArg >= cszArg ) || ( NULL == rgszArg[iszArg] )) {
                
                printf("ERROR: no rdy count specified\n");
                return fFalse;
            }
            
            /* Make sure that the string consists entirely of digits 0-9.
            */
            ich = 0;
            while ( '\0' != rgszArg[iszArg][ich] ) {
                
                if ( 0 == isdigit(rgszArg[iszArg][ich]) ) {
                    
                    printf("ERROR: invalid character detected in rdy count string: %c\n", rgszArg[iszArg][ich]);
                    return fFalse;
                }
                
                ich++;
            }
            
            cbitRdyReq = (strtol(rgszArg[iszArg], NULL, 10)  & 0xFF);
        }
        
        /* Check for the -sfmt option. This specifies the JTAG Scan format
        ** that should be used during testing.
        */
        else if ( 0 == strcmp(rgszArg[iszArg], "-sfmt") ) {
            
            iszArg++;
            
            if ( iszArg >= cszArg ) {
                
                printf("ERROR: no scan format string specified\n");
                return fFalse;
            }
            
            if (( NULL == rgszArg[iszArg] ) || 
                ( '-' == rgszArg[iszArg][0] ) || 
                ( cchJtgsfMax < strlen(rgszArg[iszArg]) )) {
                
                printf("ERROR: invalid scan format string specified\n");
                return fFalse;
            }
            
            strcpy(szJtgsf, rgszArg[iszArg]);
            fJtgsf = fTrue;
        }
        
        /* Check for the -? and --help options. These specify whether
        ** or not the user wants the help command to be executed.
        */
        else if (( 0 == strcmp(rgszArg[iszArg], "-?") ) || 
                 ( 0 == strcmp(rgszArg[iszArg], "-help") )) {
            
            fShowHelp = fTrue;
            goto lExit;
        }
        
        /* Unknown command line option specified.
        */
        else {
            
            printf("ERROR: invalid option specified: ");
            
            if ( NULL != rgszArg[iszArg] ) {
                
                printf("%s", rgszArg[iszArg]);
            }
            
            printf("\n");
            
            return fFalse;
        }
        
        iszArg++;
    }
    
lExit:
    
    return fTrue;
}

/* ------------------------------------------------------------ */
/***    FHelp
**
**  Parameters:
**      none
**
**  Return Values:
**      fTrue for success, fFalse otherwise
**
**  Errors:
**
**  Description:
**      Display usage and other information that the user may find
**      helpful.
*/
BOOL    
FHelp() {
    
    DWORD   ioptn;
    
    printf("Usage: %s [-help] [options]\n", pszCmd);
    
    printf("\n");
    
    printf("  Options:\n");
    
    ioptn = 0;
    while ( 0 < strlen(rgoptn[ioptn].szOption) ) {
        
        printf("    %-20s    %s\n", rgoptn[ioptn].szOption, rgoptn[ioptn].szDescription);
        ioptn++;
    }
    
    return fTrue;
}

/* ------------------------------------------------------------ */

/************************************************************************/
