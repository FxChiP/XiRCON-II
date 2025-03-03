/*
 ------------------------------------------------------------------------------
 * ViewBoxImps.cpp --
 *
 *   Platform specific implimentation class of the component.
 *
 * Copyright (c) 1999-2000 Tomahawk Software Group
 *
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: ViewBoxImps.cpp,v 1.1 2003/11/14 10:36:21 davygrvy Exp $
 ------------------------------------------------------------------------------
 */
#include "ViewBoxImps.hpp"
#include "CTCP2Parse.hpp"


// CTCP/2 color attribute parser with component specific actions (this XObject control)
//
class XterminalCTCP2Action : protected CTCP2Parse
{
public:
    XterminalCTCP2Action(std::wstring *line)
	: CTCP2Parse()
    {
	// Crank it through
	CTCP2Parse::ParseIt(line);
    }

private:
    virtual void PushStart ()
    {
	// A segment's attributes (if any) are about to be pushed.
	// Do what we need to, to prepare our component.

	changed = false;
    }

    virtual void PushBold (bool doit)
    {
	//changed = true;
	// TODO: we can do this!
    }

    virtual void PushReverse (bool doit)
    {
	//changed = true;
	// TODO: we can do this!
    }

    virtual void PushUnderline (bool doit)
    {
	//changed = true;
	// TODO: we can do this!
    }

    virtual void PushOverstrike (bool doit)
    {
	// Ignore! not supported using text mode display.
    }

    virtual void PushItalic (bool doit)
    {
	// Ignore! not supported using text mode display.
    }

    virtual void PushBlink (bool doit)
    {
	//changed = true;
	// TODO: we can do this!
    }

    virtual void PushURL (bool doit)
    {
	//changed = true;
	// TODO: do something that makes sense.
	// maybe underline and reverse?
    }

    virtual void PushSpacing (CharSpacingType sp)
    {
	// Ignore! not supported using text mode display.
    }

    virtual void PushFontSize (int size)
    {
	// Ignore! not supported using text mode display.
    }

    virtual void PushForegroundColor (unsigned char R, unsigned char G, unsigned char B)
    {
	changed = true;
    }

    virtual void PushBackgroundColor (unsigned char R, unsigned char G, unsigned char B)
    {
	changed = true;
    }

    virtual void PushTextSegment (std::wstring *segment)
    {
	if (changed) {
	    // do what's needed to draw this segment to the display
	}
    }

    bool changed;
};

XterminalViewBoxImp::XterminalViewBoxImp ()
    : XObject()
{
}

XterminalViewBoxImp::~XterminalViewBoxImp()
{
}

void
XterminalViewBoxImp::PushLine(std::wstring *line)
{
    XterminalCTCP2Action(line);
    delete line;
}
