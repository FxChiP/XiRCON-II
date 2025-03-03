# Tcl itself doesn't know about Tk for some reason.
set d [expr {[info exist ::tcl_platform(debug)] && $::tcl_platform(debug) == 1 ? "d" : ""}]
package ifneeded Tk $::tcl_version [list load [file join [info library] .. .. bin tk[string map {. {}} $::tcl_version]$d.dll]]

source ../common/tkcon.tcl
