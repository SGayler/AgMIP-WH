/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by ZeusSoft, Ing. Buero Bauer
 *                              www.zeussoft.de
 *                              Tel: 0700 zeussoft
 *
 * Author:  Christian Bauer
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *  Main Module of the ModLib Dll containing the main function "DllMain".
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: modlib.c $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 19:59
 * Updated in $/Projekte/ExpertN/ModLib/control
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 13.12.01   Time: 22:51
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Implemented methods InitDll and ReleaseDll.
 * 
 * *****************  Version 1  *****************
 * User: Christian Bauer Date: 8.12.01    Time: 13:06
 * Created in $/Projekte/ExpertN/ModLib/ModLib/control
 * 
*******************************************************************************/

#include <windows.h>
#include <crtdbg.h>
#include "expert.h"
#include "trace.h"


/*==============================================================================
Static Variables
==============================================================================*/

static HINSTANCE s_hInst = INVALID_HANDLE_VALUE;
static BOOL      s_bDllInitialized = FALSE;

#ifdef _DEBUG
  static char    g_szCRTWARNFILE[]   = { "ModLibW.err" };
  static char    g_szCRTERRFILE[]    = { "ModLibE.err" };
  static char    g_szCRTASSERTFILE[] = { "ModLibA.err" };
  static char    g_szCrtWarnPath[200];
  static char    g_szCrtErrPath[200];
  static char    g_szCrtAssertPath[200];
  static HANDLE  g_hFileCrtWarn   = INVALID_HANDLE_VALUE;
  static HANDLE  g_hFileCrtErr    = INVALID_HANDLE_VALUE;
  static HANDLE  g_hFileCrtAssert = INVALID_HANDLE_VALUE;
#endif


/*==============================================================================
Static Method Interface Description
==============================================================================*/

static BOOL InitDll   ( HINSTANCE i_hInstExe, DWORD i_dwReason );
static BOOL ReleaseDll( HINSTANCE i_hInstExe, DWORD i_dwReason );


/*==============================================================================
Static Method Implementation
==============================================================================*/

//------------------------------------------------------------------------------
int WINAPI DllMain( HINSTANCE i_hInst, DWORD i_dwReason, PVOID i_pvReserved )
//------------------------------------------------------------------------------
{
  switch( i_dwReason )
  {
    case DLL_PROCESS_ATTACH:
    {
      REGISTERDLL  registerdll =
      {
        /* hInst         */  i_hInst
        /* initdllfct    */ ,InitDll
        /* releasedllfct */ ,ReleaseDll
      };

      s_hInst = i_hInst;

      RegisterDll( registerdll );

      break;
    
    } // DLL_PROCESS_ATTACH

    case DLL_THREAD_ATTACH:
    {
      break;
    } // case DLL_THREAD_ATTACH

    case DLL_PROCESS_DETACH:
    {
      DeregisterDll( s_hInst );

      s_hInst = INVALID_HANDLE_VALUE;

      break;
    
    } // case DLL_PROCESS_DETACH

    case DLL_THREAD_DETACH:
    {
      break;
    } // case DLL_THREAD_DETACH
  
  } // switch( Reason )

  return TRUE;

} // DllMain


//------------------------------------------------------------------------------
static BOOL InitDll( HINSTANCE i_hInstExe, DWORD i_dwReason )
//------------------------------------------------------------------------------
{
  #ifdef _DEBUG
  
  if( !s_bDllInitialized )
  {
    strcpy( g_szCrtWarnPath, GetExpertNExeDevice() );
    strcat( g_szCrtWarnPath, "\\" );
    strcat( g_szCrtWarnPath, GetExpertNExeDir() );
    strcat( g_szCrtWarnPath, "\\" );
    strcat( g_szCrtWarnPath, g_szCRTWARNFILE );
    g_hFileCrtWarn = CreateFile(
      /* lpFileName             */  g_szCrtWarnPath
      /* dwDesiredAccess        */ ,GENERIC_WRITE
      /* dwShareMode            */ ,0
      /* lpSecurityAttributes   */ ,NULL
      /* dwCreationDistribution */ ,CREATE_ALWAYS
      /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
      /* hTemplateFile          */ ,NULL );
  
    strcpy( g_szCrtErrPath, GetExpertNExeDevice() );
    strcat( g_szCrtErrPath, "\\" );
    strcat( g_szCrtErrPath, GetExpertNExeDir() );
    strcat( g_szCrtErrPath, "\\" );
    strcat( g_szCrtErrPath, g_szCRTERRFILE );
    g_hFileCrtErr = CreateFile(
      /* lpFileName             */  g_szCrtErrPath
      /* dwDesiredAccess        */ ,GENERIC_WRITE
      /* dwShareMode            */ ,0
      /* lpSecurityAttributes   */ ,NULL
      /* dwCreationDistribution */ ,CREATE_ALWAYS
      /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
      /* hTemplateFile          */ ,NULL );
  
    strcpy( g_szCrtAssertPath, GetExpertNExeDevice() );
    strcat( g_szCrtAssertPath, "\\" );
    strcat( g_szCrtAssertPath, GetExpertNExeDir() );
    strcat( g_szCrtAssertPath, "\\" );
    strcat( g_szCrtAssertPath, g_szCRTASSERTFILE );
    g_hFileCrtAssert = CreateFile(
      /* lpFileName             */  g_szCrtWarnPath
      /* dwDesiredAccess        */ ,GENERIC_WRITE
      /* dwShareMode            */ ,0
      /* lpSecurityAttributes   */ ,NULL
      /* dwCreationDistribution */ ,CREATE_ALWAYS
      /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
      /* hTemplateFile          */ ,NULL );
  
    _CrtSetReportMode( _CRT_WARN,   _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN,   g_hFileCrtWarn );
    _CrtSetReportMode( _CRT_ERROR,  _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR,  g_hFileCrtErr );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW );
    _CrtSetReportFile( _CRT_ASSERT, g_hFileCrtAssert );
  
  } // if( DllInitialized )

  #endif // #ifdef _DEBUG

  s_bDllInitialized = TRUE;

  return TRUE;

} // InitDll

//------------------------------------------------------------------------------
static BOOL ReleaseDll( HINSTANCE i_hInstExe, DWORD i_dwReason )
//------------------------------------------------------------------------------
{
  #ifdef _DEBUG

    if( s_bDllInitialized )
    {
      M_TRACE_MESSAGEBOX_IF(
        /* Condition */  _CrtDumpMemoryLeaks()
        /* Method    */ ,"ReleaseDll()"
        /* AddInfo1  */ ,"Memory leaks detected"
        /* AddInfo2  */ ,"For further details see object dump in file"
        /* AddInfo3  */ ,g_szCRTWARNFILE );
      CloseHandle( g_hFileCrtWarn );
      CloseHandle( g_hFileCrtErr );
      CloseHandle( g_hFileCrtAssert );

    } // if( !DllInitialized )

  #endif // #ifdef _DEBUG

  s_bDllInitialized = FALSE;

  return TRUE;

} // ReleaseDll

/*******************************************************************************
** EOF */
