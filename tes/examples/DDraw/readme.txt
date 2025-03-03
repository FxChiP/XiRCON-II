A silly DirectDraw drawing program.  Through the Tcl interface you can call
low level GDI+ routines for drawing commands.  When the Gdipus::Graphics instance
is destroyed, that's when the page flips.  See gdiplus.itcl for the class
interface, and DDraw.itcl for the procedures the numeric keys run.  ESC closes
the app.  You'll need Itcl 3.1 or greater along with Tcl 8.1 or greater (8.4a2+
prefered).

The big prereq is gdiplus.dll found in either the system32 directory in Whistler
beta2 or it was installed by the .NET framework SDK beta2.  Comes stock with WinXP

*NEWSFLASH*
gdiplus.dll redistributable now available @
http://www.microsoft.com/downloads/release.asp?releaseid=31468

You definately will need the header files for GDI+ from the newest Platform SDK.

You might need the DirectX SDK:
http://msdn.microsoft.com/library/default.asp?url=/downloads/list/directx.asp

Please edit DDraw.itcl and send me some cool pages.
Better yet, finish off that asteroids game and send me the script back :)

commands:
<esc>	-- exit
<1>	-- Draws a line, proc Do1 in 
<2>
<0>	-- start the asteroids game
<space> -- fire
<X>	-- thrust
<Z>	-- hyperspace
<left arrow> -- turn ship counterclockwise

--
David Gravereaux <davygrvy@pobox.com>