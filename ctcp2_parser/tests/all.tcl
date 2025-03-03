# all.tcl --
#
# This file contains a top-level script to run all of the GUI_irc
# tests.  Execute it by invoking "source ../../tests/all.tcl" when 
# running tclsh83.exe in the Debug or the Release directory.  We
# need at least tclsh83.exe for its tcltest package.
#
# RCS: @(#) $Id: all.tcl,v 1.2 2002/12/02 22:52:10 davygrvy Exp $

set tcltestVersion [package require tcltest]
namespace import -force tcltest::*

if {$tcl_platform(platform) == "macintosh"} {
	tcltest::singleProcess 1
}

tcltest::testsDirectory [file dir [info script]]
tcltest::runAllTests

return
