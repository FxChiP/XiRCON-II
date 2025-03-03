# Tcl itself doesn't know about Tk for some reason.
#set d [expr {[info exist ::tcl_platform(debug)] && $::tcl_platform(debug) == 1 ? "d" : ""}]
#package ifneeded Tk $::tcl_version [list load [file join [info library] .. .. bin tk[string map {. {}} $::tcl_version]$d.dll]]

namespace eval tkcon {
    set PRIV(histfile) ./tkcon.hst
    set ::tkcon::OPT(history) 10
}

if {1} {
    set argv [list ../common/gdiplus.itcl]
    set argc [llength $argv]
    source ../common/tkcon.tcl
} else {
    source ../common/gdiplus.itcl
    source ../common/toids.itcl

    proc Fire {} {
	ship fire
    }
    proc MoveCw {} {
	ship moveCw
    }
    proc MoveCcw {} {
	ship moveCcw
    }
    proc StopMove {} {
	ship stopMove
    }
    proc DoThrust {} {
	ship doThrust
    }
    proc StopThrust {} {
	ship stopThrust
    }
    proc HyperSpace {} {
	ship hyperSpace
    }

    Gdiplus::Pen p1 Gray 0.5
    Gdiplus::Pen p2 White 1
    Gdiplus::Pen p3 Yellow 1.5
    Gdiplus::Pen p4 Red 1.5

    proc doit {} {
	global clockTimer drawables
	set clockTimer [after 20 doit]

	itcl::local Gdiplus::Graphics surface

	foreach drawable $drawables {
	    $drawable bump
	}
	surface Clear
	foreach drawable $drawables {
	    $drawable draw surface p1 p2 p3 p4
	}
    }

    proc Pause {} {
	global clockTimer
	if {[info exist clockTimer]} {
	    after cancel $clockTimer
	    unset clockTimer
	    itcl::local Gdiplus::Graphics surface
	    itcl::local Gdiplus::SolidBrush b Blue
	    itcl::local Gdiplus::Font f {Arial} 36
	    surface DrawString {Paused!} f 240 190 b
	} else {
	    Start
	}
    }

    proc Start {} {
	global drawables
	if {![info exist drawables]} {
	    for {set i 0} {$i < 10} {incr i} {
		BigAsteroid #auto
	    }
	    Ship ship
	}
	doit
    }

    # shazam!
    Start

    set argv [list ../common/gdiplus.itcl ../common/toids.itcl]
    set argc [llength $argv]
    source ../common/tkcon.tcl
}
