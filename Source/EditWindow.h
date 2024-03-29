/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is Copyright 1993 Jim Bumgardner.
 * 
 * The Initial Developer of the Original Code is Jim Bumgardner
 * Portions created by Lane Roathe (LR) are
 * Copyright (C) Copyright � 1996-2008.
 * All Rights Reserved.
 *
 * Contributor(s):
 *		Lane Roathe
 *		Nick Shanks (NS)
 *		Scott E. Lasley (SEL) 
 */

#include "ObjectWindow.h"

#ifndef _HexEdit_EditWindow_
#define _HexEdit_EditWindow_

/*** Window Types ***/
typedef enum
{
	kWindowNormal = 0,

	kWindowCompareTop = 1,
	kWindowCompareBtm = 2,

	kWindowTypeCount

}tWindowType;

/*** EDIT CHUNK ***/
typedef struct EditChunk
{
	struct EditChunk	**prev,**next;
	Boolean				loaded;			// Flag if chunk is currently loaded
	short				type;			// 0=Orig File, 1=Work File, 2=Unwritten
	Handle				data;			// Handle to Chunk Data
	long				size;			// Size of Chunk
	long				allocSize;		// Size of allocated Pointer
	long				addr;			// Start Addr in updated File
	long				filePos;		// Start Addr in actual File
	long				lastCtr;		// Use Counter
}	EditChunk;

/*** EDIT WINDOW ***/
typedef struct
{
	ObjectWindowRecord	oWin;			// Window Record
	ControlHandle		vScrollBar;		// Vertical Scroll Bar

	EditChunk			**firstChunk;	// File's First Chunk
	EditChunk			**curChunk;		// File's Current Chunk
	long				useCtr;			// Chunk access Counter
										// Chunks are unloaded from memory based on usage
	long				totLoaded;		// Amount of bytes in Memory

	FSSpec				fsSpec,workSpec;// File Specs for Original, Work File
	FSSpec				destSpec;		// File Spec for Save, Save As

	/* very latest type of info */
#if !defined(__MC68K__) && !defined(__SC__)
	FSCatalogInfo 		catinfo;		// File permissions (including unix bytes, etc.)
	long				OKToSetCatInfo;	// true if no error getting permissions
#endif

	/* old type of info */
	long				fileSize;		// Total File Size
	long				fileType;		// File Type
	long				creator;		// File Creator
	unsigned long		creationDate;	// Creation Date

	short				refNum;			// File's Reference Number
	short				workRefNum;		// Work File's Reference Number

//	HR/LR 050328 - PPC disassembly support
	short				asciiStart;		// first char of ascii dump
	short				hexStart;		// First Character Position of Hex Dump
	short				bytesPerLine;	// Number of bytes per line
	short				drawMode;		// 0=Dump, 1=Disassembly
	
	long				editOffset;		// Display Offset
	long				startSel;		// First Character of Selection
	long				endSel;			// First Character AFTER Selection
	long				lastTypePos;	// Last Typing Insertion Point
	long				workBytesWritten;	// Size of Work File
	long				linesPerPage;	// Lines that fit in the theWin

//	HR/LR 050328 - Fix error in comments
	EditMode			editMode;		// 0=Hex, 1=Decimal, 2=Ascii
	short				fork;				// 0=Undefined, 1=Data, 2=Resource

	short				lastNybble;		// Last Hex Edit Nibble

	Boolean				loByteFlag;		// Editing Low Byte for Hex Editor
	Boolean				dirtyFlag;		// File has been modified
	Boolean				readOnlyFlag;	// File is read-only!

//LR 180	GWorldPtr			offscreen;

	short				csResID;		// LR: color table res ID for drawing theWin
	short				csMenuID;		//LR 181 -- menu ID of window's color (for nice window updates)

}	EditWindowRecord, *EditWindowPtr;

void InitializeEditor( void );
void CleanupEditor( void );
GWorldPtr NewCOffScreen( short width, short height );
void NewEditWindow( void );
OSStatus OpenEditWindow( FSSpec *fsSpec, tWindowType type, Boolean showerr );
void DisposeEditWindow( WindowRef theWin );
Boolean	CloseEditWindow( WindowRef theWin );
Boolean CloseAllEditWindows( void );
void SizeEditWindow( WindowRef theWin, tWindowType type );
EditWindowPtr LocateEditWindow( FSSpec *fs, short fork );
EditWindowPtr FindFirstEditWindow( void );
EditWindowPtr FindNextEditWindow( EditWindowPtr curr );
void MyDraw( WindowRef theWin );
void UpdateOnscreen( WindowRef theWin );
void MyIdle( WindowRef theWin, EventRecord *er );
void MyHandleClick( WindowRef theWin, Point where, EventRecord *er );
void PrintWindow( EditWindowPtr dWin );
void MyProcessKey( WindowRef theWin, EventRecord *er );
void CursorOff( WindowRef theWin );
void CursorOn( WindowRef theWin );
void SaveContents( WindowRef theWin );
void SaveAsContents( WindowRef theWin );
void RevertContents( WindowRef theWin );
void MyActivate( WindowRef theWin, Boolean active );
void UpdateEditWindows( void );

// In order to support Nav and SF we have to go through some pain

short AskEditWindowNav( tWindowType type );
#if !TARGET_API_MAC_CARBON
short AskEditWindowSF( tWindowType type );
#endif

// BB: now a wrapper for Nav or SF based functions
#if __MWERKS__ && !__POWERPC__		//LR 1.73 -- not available for 68K (won't even link!)
	#define AskEditWindow AskEditWindowSF
#elif !TARGET_API_MAC_CARBON
inline short AskEditWindow( tWindowType type )
{if (g.useNavServices) {return AskEditWindowNav( type );} else {return AskEditWindowSF( type );}}
#else
    #define AskEditWindow AskEditWindowNav
#endif

#endif