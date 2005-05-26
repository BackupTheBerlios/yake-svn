/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
*/

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/native/yakeNative.h>
#include <yake/base/native/win32/yakeConfig.h>
#include <yake/base/native/win32/yakeResource.h>
#include <cassert>
#include <cstdio>
#include <locale>


//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace native
{

// The result of the >>ASSERT() failed<< dialog box.
enum
{
	RESULT_EXIT								= 1,
	RESULT_DEBUG							= 2,
	RESULT_CONTINUE						= 8,
	RESULT_FLAG_IGNORE_ALWAYS	= 4,
};

// Various parameters passed to the dialog procedure.
static const char* g_pszMessage		= 0;
static const char* g_pszCondition = 0;
static const char* g_pszFile			= 0;
static int g_nLine								= 0;

/** Checks if the failure of the current assertion should be always ignored. */
#define IGNORE_ALWAYS	( SendMessage( GetDlgItem( hDlg, IDC_IGNORE_ALWAYS ), BM_GETSTATE, 0L, 0L ) == BST_CHECKED ? RESULT_FLAG_IGNORE_ALWAYS : 0 )

// The dialog procedure of the >>ASSERT() failed<< dialog box.
::BOOL CALLBACK DialogProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM )
{
	if( uMsg == WM_INITDIALOG )
	{
		char szBuffer[ 1024 ];
		sprintf( szBuffer,
			"An ASSERT() failed under the following circumstances:\n\nMessage:\t%s\nCondition:\t%s\nFile:\t\t%s\nLine:\t\t%i",
			g_pszMessage,
			g_pszCondition,
			g_pszFile,
			g_nLine );
		SetWindowTextA( GetDlgItem( hDlg, IDC_ERROR_TEXT ), szBuffer );

		RECT rc;
		GetWindowRect( hDlg, &rc );
		SetWindowPos( hDlg,
			0,
			( GetDeviceCaps( GetDC( 0 ), HORZRES ) - ( rc.right - rc.left ) ) / 2,
			( GetDeviceCaps( GetDC( 0 ), VERTRES ) - ( rc.bottom - rc.top ) ) / 2, 
			0,
			0,
			SWP_NOSIZE | SWP_NOZORDER );
	}
	else if( uMsg == WM_COMMAND )
	{
		UINT uID = LOWORD( wParam );

		if( uID == IDC_EXIT )
			EndDialog( hDlg, RESULT_EXIT );
		else if( uID == IDC_DEBUG )
			EndDialog( hDlg, RESULT_DEBUG | IGNORE_ALWAYS );
		else if( uID == IDC_CONTINUE )
			EndDialog( hDlg, RESULT_CONTINUE | IGNORE_ALWAYS );
	}
	else if( uMsg == WM_SYSCOMMAND )
	{
		if( wParam == SC_CLOSE )
			EndDialog( hDlg, RESULT_EXIT );
	}

	return FALSE;
}

YAKE_BASE_NATIVE_API bool debug_AssertFailed( const char* pszMessage, const char* pszCondition, const char* pszFile, int nLine, bool& rbIgnoreAlways )
{
	g_pszMessage		= pszMessage;
	g_pszCondition	= pszCondition;
	g_pszFile				= pszFile;
	g_nLine					= nLine;

	int nResult = ( int )DialogBoxA( win32::g_hInstance, (LPCSTR)MAKEINTRESOURCE( IDD_ASSERT_FAILED ), 0, DialogProc );
	assert( nResult != -1 );

	if( nResult & RESULT_EXIT )
		ExitProcess( 1 );
	else if( nResult & RESULT_FLAG_IGNORE_ALWAYS )
		rbIgnoreAlways = true;

	return ( nResult & RESULT_CONTINUE ) == RESULT_CONTINUE;
}


static class LogConsole
{
public:
	LogConsole()
	{
	}

	~LogConsole()
	{
	}

	void Print( const char * text, WORD wAttributes )
	{
		AllocConsole();

		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), wAttributes );
		DWORD dwWritten;
    int length = (int)strlen( text );
		::BOOL bSuccess = WriteConsoleA( GetStdHandle( STD_OUTPUT_HANDLE ), text, ( DWORD )length, &dwWritten, 0 );
		assert( bSuccess );
	}

private:
    static int niftyCounter_;
    static bool consoleApp_;
} logConsole_g;

YAKE_BASE_NATIVE_API void debug_Log( const char * what, DebugLog::Severity eSeverity, const char * source )
{
	static char szBuffer[ 1024 ];

  WORD wColor = ( WORD )( eSeverity == DebugLog::INFORMATIONS ? FOREGROUND_GREEN :
    ( eSeverity == DebugLog::WARNINGS ? FOREGROUND_GREEN | FOREGROUND_RED : FOREGROUND_RED ) );
	wColor |= FOREGROUND_INTENSITY;

	if( *source != NULL )
		sprintf( szBuffer, "%s: %s", source, what );
	else
		sprintf( szBuffer, "%s", what );

	logConsole_g.Print( szBuffer, wColor );
	logConsole_g.Print( "\n", FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED );
}

YAKE_BASE_NATIVE_API void debug_Print( const char * string )
{
  logConsole_g.Print( string, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED );
}


} // native
} // base
} // yake
