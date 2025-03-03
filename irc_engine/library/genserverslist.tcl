#------------------------------------------------------------------------
#   Generate a list of servers starting from http://irc.netsplit.de
#
# sudo apt-get install html-xml-utils curl
# generate css selectors using Chrome with http://selectorgadget.com/
#------------------------------------------------------------------------
# RCS: @(#) $Id: genserverslist.tcl,v 1.2 2015/07/16 07:34:29 davygrvy Exp $
#------------------------------------------------------------------------

package require inifile
#package require irc_engine 0.2    ;# connect to each and search for ports!


proc getHTML {addr} {
    # curl works better than hxnormalize doing the fetching.
    # This might be overly defensive, now.
    if {[catch {exec -ignorestderr -- curl --silent $addr \
	2> /dev/null} result]
    } {
	set end end-1
    } else {
	set end end
    }
    # remove the last line if exec added any errorinfo
    set rawHtml [join [lrange [split $result \n] 0 $end] \n]
    if {[catch {exec -ignorestderr -- hxnormalize -x << $rawHtml \
	2> /dev/null} result]
    } {
	set end end-1
    } else {
	set end end
    }
    # remove the last line if exec added any errorinfo
    return [join [lrange [split $result \n] 0 $end] \n]
}

proc cssSelect {selector html {list no}} {
    if {[catch {exec -ignorestderr -- hxselect -ics \001 $selector \
	<< $html 2> /dev/null} result]
    } {
	# remove the second to last line also if exec added errorinfo
	set end end-2
    } else {
	# apparently the last entry is always bogus
	set end end-1
    }
    if $list {
	return [lrange [split $result \001] 0 $end]
    } else {
	return [join [lrange [split $result \001] 0 $end] \n]
    }
}

proc NetSplit_GetList {} {
    global ini

    set html [getHTML http://irc.netsplit.de/networks/]
    if {$html eq ""} {error "bad web crawl"}

    # get the table only
    set table [cssSelect table.netlist $html]

    # get all the links in the table
    set links [split [exec hxwls << $table] \n]

    # for each link, get their servers and other data
    set networks [list]
    foreach link $links {
	set networkName [lindex [file split $link] 2]

	# get server info page
	set html [getHTML http://irc.netsplit.de/servers/?net=$networkName]
	if {$html eq ""} {error "bad web crawl"}

	# main server
	set mainServer [cssSelect \
		{.center-column td tr:nth-child(2) tr:nth-child(2) td:nth-child(1)} \
		$html]

	# remove the markup garbage
	regsub -all {&#\d{4};} $mainServer {} mainServer
	set mainServer [string trim $mainServer]

	# port of main server
	set mainServerPort [cssSelect \
		{.center-column td tr:nth-child(2) tr:nth-child(2) td:nth-child(2)} \
		$html]

	if {$mainServer ne ""} {
	    lappend networks [list $networkName $mainServer $mainServerPort]
	}
    }

    # save all the networks with main server and port
    foreach network $networks {
	::ini::set $ini [lindex $network 0] Main [join [lrange $network 1 2] :]
    }

    # get top 100
    set html [getHTML http://irc.netsplit.de/networks/top100.php]
    set ranked100 [cssSelect {.center-column .tn-network a} $html yes]

    # save their ranking if they made the top 100
    if {[llength $ranked100] == 100} {
	for {set i 1} {$i <= 100} {incr i} {
	    ::ini::set $ini [lindex $ranked100 $i-1] Rank "$i:[clock seconds]"
	}
    }

    ::ini::commit $ini
}

#------------------------------------------------------------------------
# IRCnet
proc IRCnet_GetServersWeb {} {
    global ini
    set net IRCnet

    ::ini::set $ini $net Description "IRCnet"
    ::ini::set $ini $net HomePage "http://ircnet.org/"
    ::ini::set $ini $net ServersPage "http://irc.tu-ilmenau.de/all_servers/"

    # crawl the IRCnet server page
    set html [getHTML [::ini::value $ini $net ServersPage]]
    if {$html eq ""} {error "bad web crawl"}
    set servers [cssSelect {tr+ tr td:nth-child(1) a} $html yes]
    set portranges [cssSelect {tr+ tr td+ td:nth-child(2)} $html yes]
    set descriptions [cssSelect {tr+ tr td~ td+ td:nth-child(3)} $html yes]
    set count 0
    foreach server $servers portrange $portranges description $descriptions {
	# ignore the hubs
	if {[regexp {^(hub\.)} $server] || $server eq ""} {continue}
	if {$portrange eq "not found"} {set portrange 6667}
	# use server hostname when both host and name exist
	regexp {\[(.*)\]} $server - server
	# remove all whitespace in portrange
	regsub -all {\s} $portrange {} portrange
	# remove superflious whitespace in description
	# (hxnormalize probably did this)
	regsub {\n(\s)+} $description { } description
	::ini::set $ini $net "s[incr count]" "${server}:${portrange}:${description}"
    }

    ::ini::commit $ini
}

#------------------------------------------------------------------------
# EFnet
proc EFnet_GetServersWeb {} {
    global ini
    set net EFnet

    ::ini::set $ini $net Description "Eris-Free network"
    ::ini::set $ini $net HomePage "http://www.efnet.org"
    ::ini::set $ini $net ServersPage "http://stats.efnet.org/servers/"

    # crawl the EFnet server page
    set html [getHTML [::ini::value $ini $net ServersPage]]
    if {$html eq ""} {error "bad web crawl"}
    set servers [cssSelect {.section td:nth-child(2) a} $html yes]
    set count 0
    set portrange {}
    set description {}
    foreach server $servers {
	if {$server eq ""} {continue}
	::ini::set $ini $net "s[incr count]" "${server}:${portrange}:${description}"
    }

    ::ini::commit $ini
}


#------------------------------------------------------------------------
# QuakeNet
proc QuakeNet_GetServersWeb {} {
    global ini
    set net QuakeNet

    ::ini::set $ini $net Description "QuakeNet"
    ::ini::set $ini $net HomePage "https://www.quakenet.org/"
    ::ini::set $ini $net ServersPage "https://www.quakenet.org/servers"

    # crawl the QuakeNet server page
    set html [getHTML [::ini::value $ini $net ServersPage]]
    if {$html eq ""} {error "bad web crawl"}
    set servers [cssSelect {td:nth-child(1)} $html yes]
    set count 0
    set portranges {}
    set descriptions {}
    foreach server $servers portrange $portranges description $descriptions {
	if {$server eq ""} {continue}
	::ini::set $ini $net "s[incr count]" "${server}:${portrange}:${description}"
    }
    ::ini::commit $ini
}


#------------------------------------------------------------------------
# Rizon
proc Rizon_GetServersWeb {} {
    global ini
    set net Rizon

    ::ini::set $ini $net Description "Rizon chat network"
    ::ini::set $ini $net HomePage "http://www.rizon.net/"
    ::ini::set $ini $net ServersPage "https://wiki.rizon.net/index.php?title=Servers"

    # crawl the Rizon server page
    set html [getHTML [::ini::value $ini $net ServersPage]]
    if {$html eq ""} {error "bad web crawl"}
    set servers [cssSelect {td:nth-child(1) .text} $html yes]
    set portranges [list]
    set descriptions [cssSelect {table:nth-child(11) td:nth-child(2)} $html yes]
    set descriptions [lreplace $descriptions end end \
	{*}[cssSelect {table:nth-child(13) td:nth-child(2)} $html yes]]
    set descriptions [lreplace $descriptions end end \
	{*}[cssSelect {table:nth-child(15) td:nth-child(2)} $html yes]]
    set count 0
    foreach server $servers portrange $portranges description $descriptions {
	if {$server eq ""} {continue}
	set description [string trim $description]
	::ini::set $ini $net "s[incr count]" "${server}:${portrange}:${description}"
    }
    ::ini::commit $ini
}

#------------------------------------------------------------------------
# Undernet
proc Undernet_GetServersWeb {} {
    global ini
    set net Undernet

    ::ini::set $ini $net Description "Undernet chat network"
    ::ini::set $ini $net HomePage "http://www.undernet.org/"
    ::ini::set $ini $net ServersPage "http://www.undernet.org/servers/"

    # crawl the Rizon server page
    set html [getHTML [::ini::value $ini $net ServersPage]]
    if {$html eq ""} {error "bad web crawl"}
    set servers [cssSelect {center td td:nth-child(1) a:first-child} $html yes]
    set portranges [cssSelect {tr+ tr td:nth-child(2) font[size="1"]} $html yes]
    set descriptions [cssSelect {tr+ tr td:nth-child(3) font[size="1"] a} $html yes]
    set count 0
    foreach server $servers portrange $portranges description $descriptions {
	# remove all whitespace in portrange
	regsub -all {\s} $portrange {} portrange
	# remove superflious whitespace in description
	# (hxnormalize probably did this)
	regsub {\s(\s)+} $description { } description
	::ini::set $ini $net "s[incr count]" "${server}:${portrange}:${description}"
    }
    ::ini::commit $ini
}

#------------------------------------------------------------------------

set ini [::ini::open newservers.ini w]

#------------------------------------------------------------------------
# Step #1 - Get the master list from NetSplit
#------------------------------------------------------------------------

NetSplit_GetList

#------------------------------------------------------------------------
# Step #2 - Fill in more detail from known networks
#------------------------------------------------------------------------

# TCL hackery ahead...
foreach net {
    EFnet QuakeNet Rizon Undernet
} {
    ${net}_GetServersWeb
}

::ini::close $ini


