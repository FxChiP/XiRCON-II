#------------------------------------------------------------------------
#   default.tcl --
#
#	Master default client behavior.
#
#------------------------------------------------------------------------
#   RCS: @(#) $Id: default.tcl,v 1.27 2006/09/24 07:02:24 davygrvy Exp $
#------------------------------------------------------------------------

package require XiRCON 2.0

proc bgerror { msg } {
    global errorInfo
    IRC::echo "[IRC::color error]$msg" status
    IRC::echo "[IRC::color error]TCL ERR: $errorInfo" status
}

source [file join [file dirname [info script]] numerics.tcl]

proc icomp {a b} {
    return [string compare [string tolower $a] [string tolower $b]]
}

### load a fairly complete list of IRC events to define the default
### behavior of the client.

namespace eval ::IRC {

    package require msgcat 1.6
    namespace import ::msgcat::mc

    ### Load our message catalog for multi-lingual messages.
    ::msgcat::mcload [file join [file dirname [info script]] msgs]

    on @socket {
	echo "[color socket]*** [join [args]]" status
	complete
    }

    on PRIVMSG {
	set dest [lindex [args] 0]
	set msg [lindex [args] 1]

	### ignore an empty one.
	if {![string length $msg]} {complete; return}

	if {![icomp $dest [my_nick]]} {
	    ### private message
	    echo "[color highlight]*[color nick][nick][color highlight]*[color private] $msg" [nick]

	} elseif {[string index $dest 0] == "\$"} {
	    ### server (global) message
	    echo "[color highlight]<[color nick][nick][color highlight]>[color default] $msg"

	} else {
	    ### channel message
	    echo "[color highlight]<[color nick][nick][color highlight]>[color default] $msg" $dest

	}
	complete
    }

    alias PRIVMSG {
	set dest [lindex [args] 0]
	set msg [lindex [args] 1]
	if {[ischannel $dest]} {
	    echo "[color highlight]<[color nick][nick][color highlight]>[color default] $msg" $dest
	} else {
	    echo "[color highlight]*[color nick][nick][color highlight]*[color default] $msg" $dest
	}
	say $dest $msg
	complete
    }

    on NOTICE  {
	set dest [lindex [args] 0]
	set msg [lindex [args] 1]

	### ignore an empty one.
	if {![string length $msg]} {complete; return}

	if {![string length [nick]]} {
	    echo "[color notice]$msg" status

	} elseif {![icomp [nick] [host]]} {
	    ### notice from the server
	    echo "[color highlight]-[color nick][host][color highlight]-[color notice] $msg" status

	} elseif {![icomp $dest [my_nick]]} {
	    ### private notice
	    echo "[color highlight]-[color nick][nick][color highlight]-[color notice] $msg" query [nick]

	} else {
	    echo "[color highlight]-[color nick][nick][color highlight]-[color notice] $msg" channel $dest

	}
	complete
    }

    alias NOTICE {
	set dest [lindex [args] 0]
	set msg [lindex [args] 1]
	echo "[color highlight]-[color nick][nick][color highlight]-[color notice] $msg" $dest
	notice $dest $msg
	complete
    }

    on PING {
	set data [raw_args]
	echo "[mc {ping?pong!}] $data" status
	raw "pong $data"
	complete
    }

    on WALLOPS {
	echo "[color highlight]![color nick][nick][color highlight]![color default] [lindex [args] 1]" status
	complete
    }

    on WALLCHOPS {
	set dest [lindex [args] 0]
	echo "[color highlight]-[color nick][nick]@$dest[color highlight]-[color default] [lindex [args] 1]" channel $dest
	complete
    }

    on JOIN    {
	if {![icomp [nick] [my_nick]]} {
	    set chan [lindex [args] 0]
	    # create the new window, get ban list and channel modes and all that
#	    window create channel [lindex [args] 0]
	    echo "[color change]*** [mc {Now talking in}] [color channel]$chan" channel $chan
	    raw "mode $chan"
	    raw "mode $chan b"
	} else {
#	    window -addnick [lindex [args] 0] [nick]
	    echo "[color join]*** Join: [color nick][nick][color join] ([user]@[host])" channel [lindex [args] 0]
	}
	complete
    }

    on PART    {
	if {![icomp [my_nick] [nick]]} {
#	    window -destroy channel [lindex [args] 0]
	    echo "[color change]*** [mc {No longer talking in}] [color channel][lindex [args] 0]" status
	} else {
#	    window -delnick [lindex [args] 0] [nick]
	    echo "[color part]*** [mc {Part}]: [color nick][nick][color part] ([user]@[host]) [lindex [args] 1]" channel [lindex [args] 0]
	}
	complete
    }

    on KICK    {
	set channel [lindex [args] 0]
	set kicked [lindex [args] 1]
	set reason [lindex [args] 2]
	if {![icomp [my_nick] $kicked]} {
	    echo "*** [nick] [mc {kicked you from}] $channel: $reason" status
#	    window -destroy channel $channel
#	    echo "Rejoining..." channel $channel
#	    raw "join $channel"
	} else {
#	    nicklist delnick $channel $kicked
	    echo "[color kick]*** [color nick]$kicked[color kick] [mc {has been kicked by}] [color nick][nick][color kick] ($reason\006N\006[color kick])" channel $channel
	}
	complete
    }

    on NICK    {
	if {![icomp [my_nick] [nick]]} {
#	    nicklist yournick [lindex [args] 0]
#	    nicklist exchange [nick] [lindex [args] 0]
	    echo "[color change]*** [mc {You are now known as}] [color nick][lindex [args] 0]" status
	} else {
#	    nicklist exchange [nick] [lindex [args] 0]
	    echo "[color change]*** [color nick][nick][color change] [mc {is now known as}] [color nick][lindex [args] 0]" ;#channel $i
	}
	complete
    }

    on QUIT    {
#	window -delnick all [nick]
	echo "[color quit]*** [mc {Quit}]: [color nick][nick][color quit] ([join [args]]\006N\006[color quit])"
	complete
    }

    on MODE    {
	set target [lindex [args] 0]

	if {![icomp $target [my_nick]]} {
	    echo "[color change]*** [mc {Your modes change is}] [color mode]\"[join [lrange [args] 1 end]]\"" status
	} else {
	    echo "[color change]*** [mc {Mode change}] [color mode]\"[join [lrange [args] 1 end]]\"[color change] for [color channel]$target[color change] by [color nick][nick]" channel $target
	}

	# this is missing loads and loads of logic here!
	complete
    }

    on TOPIC   {
#	window -settopic channel [lindex [args] 0] [lindex [args] 1]
	echo "[color change]*** [mc {Topic changed to}] [color mode]\"[lindex [args] 1]\"[color change] by [color nick][nick]" channel [lindex [args] 0]
	complete
    }

    on INVITE  {
	complete
    }

    on ERROR {
	echo "[color error]*** [mc {ERROR}]: [raw_args]"
	complete
    }

    # on PONG {}
    # on KILL {}
    # on RPING {}
    # on RPONG {}

    on $RPL_WELCOME {
	set msg [split [lindex [args] 1]]
	echo "*** [join [lrange $msg 0 end-1]] [color nick][join [lrange $msg end end]]" status
	complete
    }

    on $RPL_MOTDSTART {
	echo "*** [lindex [args] 1]" status
	complete
    }

    on $RPL_MOTD {
	echo "*** [lindex [args] 1]" status
	complete
    }

    on $RPL_ENDOFMOTD { complete }

    on $RPL_PROTOCTL {
	###  Is this really an RPL_PROTOCTL reply?  This is a NON-STANDARD reply!
	###  RFC2812 calls it RPL_BOUNCE and undernet calls it RPL_MAP.
	###  All these must be managed for.
	###
	###  For now, just ignore the others and handle RPL_MAP by
	###  verifying it.

	### TODO: do something logical here
    }

    on $RPL_CHANNELMODEIS    { 
	echo "*** [join [lrange [args] 1 end]]"
#	window -mode [lindex [args] 1] [join [lrange [args] 2 end]]
#	set chanModes([string tolower [lindex [args] 1]]) [join [lrange [args] 2 end]]
	complete
    }

    on $RPL_CREATIONTIME     {
	echo "[color change]*** [color channel][lindex [args] 1][color change] [mc {was created on}] [clock format [lindex [args] 2] -format "%x %r"]"
	complete
    }

    on $RPL_NOTOPIC          {
	echo "*** [join [lrange [args] 1 end]]"
	complete
    }

    on $RPL_TOPIC            {
#	window -settopic [lindex [args] 1] [lindex [args] 2]
	echo "[color change]*** [mc {Topic for}] [color channel][lindex [args] 1][color change] [mc {is}] [color mode]\"[lindex [args] 2][color mode]\""
	complete
    }

    on $RPL_TOPICWHOTIME     {
	echo "[color change]*** [mc {Topic for}] [color channel][lindex [args] 1][color change] [mc {was set by}] [color nick][lindex [args] 2][color change] [mc {on}] [clock format [lindex [args] 3]]"
	complete
    }

    on $RPL_NAMREPLY         {
#	window -addnick [lindex [args] 2] [lindex [args] 3]
	echo "[color change]*** [mc {Names for}] [color channel][lindex [args] 2][color change] [mc {are}] [color nick][lindex [args] 3]" status
	complete
    }

    on $RPL_ENDOFNAMES	    {
	complete
    }

    on $RPL_UMODEIS	    {
	echo "[color change]*** [mc {Your modes are}] [color mode]\"[join [lrange [args] 1 end]]\"" status
	complete
    }

    on CTCP {
	set orig [nick]
	set dest [lindex [args] 0] 
	set cmd [string tolower [lindex [args] 1]]
	set text [join [lrange [args] 2 end]]
	switch -glob $cmd {
	    "ping" { 
		if {![icomp [my_nick] $dest]} {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig" status
		} else {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig[color ctcp] [mc {to}] [color channel]$dest" status
		}
		### CTCP/2 spec section 4.2 says:
		### An argument longer than 32 octets may be truncated by the recipient (to
		### a minimum of 32 octets in length) before it is echoed back.
		raw "notice $orig :\001[lindex [args] 1] [string range $text 0 31]\001" 
	    }
	    "version" {
		if {![icomp [my_nick] $dest]} {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig" status
		} else {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig[color ctcp] [mc {to}] [color channel]$dest" status
		}
		raw "notice $orig :\001[lindex [args] 1] irc_engine.dll,\
		    an IRC extension library for \037T\037ool\
		    \037C\037ommand \037L\037anguage,\
		    development version : http://sf.net/projects/tomasoft\001"
	    }
	    "time" {
		if {![icomp [my_nick] $dest]} {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig" status
		} else {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig[color ctcp] [mc {to}] [color channel]$dest" status
		}
		set clock [clock seconds]
		set diff [expr {($clock - [clock scan \
			[clock format $clock -format "%d %b %Y %H:%M:%S" -gmt true]])/60}]
		set zone [format {%+03d%02d} [expr {$diff/60}] [expr {$diff%60}]]
		raw "notice $orig :\001[lindex [args] 1]\
			[clock format $clock -format "%a, %e %b %Y %H:%M:%S $zone (%Z)"]\001"
	    }
	    "action" {
		if {![icomp [my_nick] $dest]} {
		    echo "[color action]** [color nick]$orig[color action] $text"
		} else {
		    echo "[color action]* [color nick]$orig[color action] $text" channel $dest
		}
	    }
	    "clientinfo" {
		if {![icomp [my_nick] $dest]} {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig" status
		} else {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [mc {from}] [color nick]$orig[color ctcp] [mc {to}] [color channel]$dest" status
		}
		raw "notice $orig :\001[lindex [args] 1] PING VERSION TIME ACTION AWAY SOUND PLAY STREAM DCC\001"
	    }
	    "away" -
	    "dcc" -
	    "sound" -
	    "play" -
	    "stream" -
	    default {
		if {![icomp [my_nick] $dest]} {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [expr {$text!=""?"$text ":""}][mc {from}] [color nick]$orig"
		} else {
		    echo "[color ctcp]*** CTCP [lindex [args] 1] [expr {$text!=""?"$text ":""}][mc {from}] [color nick]$orig[color ctcp] [mc {to}] [color channel]$dest" status
		}
	    }
	}
	complete
    }

    on CTCP_REPLY {
	set orig [nick]
	set dest [lindex [args] 0] 
	set cmd [string tolower [lindex [args] 1]]
	set text [join [lrange [args] 2 end]]
	switch -- $cmd {
	    "ping" {
		# validate the string as a number, first.
		if {[string is double $text]} {
        	    echo "[color ctcp]*** CTCP PING REPLY [mc {from}] $orig: [expr {([clock clicks -milliseconds] - double($text))/1000}] seconds" status
        	} else {
		    echo "[color ctcp]*** CTCP PING REPLY [mc {from}] $orig: $text \[ERROR: INVALID NUMERICAL OPERAND!\]" status
		}
	    }
	    "action" {
		# umm, like no!
	    }
	    default {
		echo "[color ctcp]*** CTCP [lindex [args] 1] REPLY [mc {from}] $orig: $text" status
	    }
	}
	complete
    }
}
# ^-- end of namespace eval ...

