# Date: September 01, 2003
# Description: Partial implementation of the YMSG protocol version 0xA

# Example use from frontend:
  #  package require http
  #  source ymsg.tcl
  #  # We need the new md5 package with support for md5crypt
  #  source md5-ver2.tcl
  #  source md5crypt.tcl
  #  # Register .t as a place where ymsg can print debug (.t must exist)
  #  ymsg debugwin .t
  #  # Connect to socket
  #  ymsg connect
  #  # Register message callback right away (gotmessage is a proc in frontend)
  #  ymsg recvmsgcallback gotmessage
  #  # Login with my id and password, register getbuddies as callback
  #  ymsg login $myyahooid $mypasswd getbuddies
  #  # When logged in send messages with i.e:
  #  ymsg sendmsg \[$lf1.e get\] \[$lf1.e1 get\]
  #  # Logoff
  #  ymsg logoff

############################################################
# Debug procedures

# Provide nice formatting of binary data in a text widget
proc hputs {data} {
    set width 16
    set t $::ymsg(debugwin)

    set dl [string length $data]
    set display ""
    set j 1
    for {set i 0} {$i < $dl} {incr i} {
        if {$j == 1} {
            $t insert end "[format %04x $i]\:  "
        }
        set val [scan [string index $data $i] %c]
        set val2 [format %02x $val]
        $t insert end $val2
        $t insert end " "

        if {$val < "32"} {
            set val 46
        } elseif {$val > "126"} {
            set val 46
        }

        append display [format %c $val]
        if {$j == $width} {
            $t insert end "  $display\n"
            set display ""
            set j 0
        }
        incr j
    }
    if {$j != 1} {
        set k [expr {3*($width-$j+1)}]
        $t insert end [string repeat " " $k]
        $t insert end "  $display\n"
    }
}

# This is only to debug the authentication strings
# Used when offline with results obtained from packet sniffer
proc ymsg_testauth {} {
    package require base64
    set ver [package require md5]
    dputs "md5 version: $ver"
    # Insert same id and password as you use for yahoo messenger here
    set id "<insert id here>"
    set pwd "<insert password here>"
    # Use a packet sniffer to obtain these strings from yahoo messenger
    set chalstring "Z_YW4e.YxbWTYL6uSiBT5w--"
    set yahooauth1 "aQqeQK4tMjenYvUzsnBTpg--"
    set yahooauth2 "v.ova53LuYUWUQZJEV3r3A--"
    dputs "yahooauth1: $yahooauth1"
    dputs "yahooauth2: $yahooauth2"
    # Then we calculate the authentication strings and print them for comparison
    # with the strings that yahoo messenger generated.
    set myauth [ymsg_calcmd5strings $chalstring $id $pwd]
    foreach {a b} $myauth {}
    dputs "myauth1: $a"
    dputs "myauth2: $b"
}

############################################################
# MD5 hashing and encryption procedures

proc md5raw {str} {
    set md5 [::md5::md5 $str]
    return $md5
}

proc md5_crypt {text key} {
    set md5 [::md5crypt::md5crypt_tcl "\$1\$" $text $key]
    return $md5
}

proc to_y64 {str length} {
    set string [string range $str 0 [expr {$length-1}]]
    set string [::base64::encode $string]
    return [string map {+ . / _ = -} $string]
}

proc ymsg_calcmd5strings {challenge username password} {
    # Do some renaming because I want to use same variable names as original PHP code
    set seed $challenge

    set sv [string index $seed 15]
    set svdec [scan $sv %c]
    set sv [expr {int(fmod($svdec,8))}]
    set sv [format %c $sv]

    set result [md5raw $password]
    set password_hash [to_y64 $result 16]

    set ctx [md5_crypt $password $::ymsg(salt)]
    set result [md5raw $ctx]
    set crypt_hash [to_y64 $result 16]

    set svtemp [scan $sv %c]
    set length [expr ([string length $username]+50-1)]
    dputs "Svtemp: $svtemp"
    # Verified working svtemps: 0,1,2,5,6,7
    if {($svtemp == 1) || ($svtemp == 6)} {
        set checksum [string index $seed [expr (int(fmod([scan [string index $seed 9] %c],16)))]]
        set hash_string_p $checksum$username$seed$password_hash
        set hash_string_c $checksum$username$seed$crypt_hash
    } elseif {($svtemp == 2) || ($svtemp == 7)} {
        set checksum [string index $seed [expr (int(fmod([scan [string index $seed 15] %c],16)))]]
        set hash_string_p $checksum$seed$password_hash$username
        set hash_string_c $checksum$seed$crypt_hash$username
    } elseif {($svtemp == 3)} {
        set checksum [string index $seed [expr (int(fmod([scan [string index $seed 1] %c],16)))]]
        set hash_string_p $checksum$username$password_hash$seed
        set hash_string_c $checksum$username$crypt_hash$seed
    } elseif {($svtemp == 4)} {
        set checksum [string index $seed [expr (int(fmod([scan [string index $seed 3] %c],16)))]]
        set hash_string_p $checksum$password_hash$seed$username
        set hash_string_c $checksum$crypt_hash$seed$username
    } elseif {($svtemp == 0) || ($svtemp == 5)} {
        set checksum [string index $seed [expr (int(fmod([scan [string index $seed 7] %c],16)))]]
        set hash_string_p $checksum$password_hash$username$seed
        set hash_string_c $checksum$crypt_hash$username$seed
    } else {
        dputs "ERROR!!!No matching if statement"
    }
    #dputs "hash_string_p: $hash_string_p"
    #dputs "hash_string_c: $hash_string_c"
    set result [md5raw $hash_string_p]
    set result6 [to_y64 $result 16]

    set result [md5raw $hash_string_c]
    set result96 [to_y64 $result 16]

    lappend ret $result6
    lappend ret $result96
    return $ret
}

############################################################
# YMSG Procedures

proc ymsg_connect {callback} {
    set ::ymsg(buffer) ""
    dputs "Connecting to $::ymsg(server) port $::ymsg(port)"
    set ::ymsg(socket) [socket $::ymsg(server) $::ymsg(port)]
    dputs " - OK: Socket: $::ymsg(socket)"

    fconfigure $::ymsg(socket) -blocking 0 -translation binary
    fileevent $::ymsg(socket) readable [list ymsg_readsock $::ymsg(socket)]
}

proc ymsg_readsock {sock {args ""}} {
    set data [read $sock]
    if {[string length $data] != 0} {
        append ::ymsg(buffer) $data
    }

    ymsg_parseresponse
}

proc ymsg_senddata {data} {
    dputs "Packet to be sent:"
    hputs $data
    puts -nonewline $::ymsg(socket) $data
    flush $::ymsg(socket)
}

# Creates the YMSG packet header
# command: type of packet
# payload: payload
proc ymsg_header {command payload} {
    set length [string length $payload]
    set data "YMSG"
    # Protocol version - according to venkydude
    append data \xa
    # Padding? - Field length: 3 bytes
    append data \x0\x0\x0
    # Length of message in bytes (not including header) - Field length: 2 bytes 
    set t [format %04x $length]
    append data [subst \\x[subst [string range $t 0 1]]]
    append data [subst \\x[subst [string range $t 2 3]]]
    # Padding? - Field length: 1 byte
    append data \x0
    # This is followed by a character that says which command is being sent
    # and 4 bytes that I don't know what is yet - they seem static though
    if {$command == "challenge"} {
        append data "W"
        append data \x0\x0\x0\x0
    } elseif {$command == "service"} {
        append data  \x4c
        append data \x0\x0\x0\x0
    } elseif {$command == "login"} {
        append data "T"
        append data \x0\x0\x0\xC
    } elseif {$command == "online"} {
        append data \x4
        append data \x0\x0\x0\xC
    } elseif {$command == "sendmsg"} {
        append data \x6
        append data \x5a\x55\xaa\x56
    }

    # This is the session identifier - initially 0 because there are no session.
    if {$::ymsg(session) == ""} {
        append data \x0\x0\x0\x0
    } else {
        append data $::ymsg(session)
    }

    return $data$payload
}

# Splits a packet around the 0xC080 separator.
proc ymsg_split {data} {
    set ret ""
    while {1} {
        set index [string first "\xC0\x80" $data]
        if {$index < 0} {
            break
        }
        lappend ret [string range $data 0 [expr {$index-1}]]
        set data [string range $data [expr {$index+2}] end]
    }
    return $ret
}

# Concatenates arguments around the 0xC080 separator
proc ymsg_arg {args} {
    set ret ""
    foreach el $args {
        append ret $el
        append ret \xC0\x80
    }
    return $ret
}

# This procedures parses the response from Yahoo
proc ymsg_parseresponse {} {
    set data $::ymsg(buffer)
    # Length of data in buffer
    set buflength [string length $data]
    if {$buflength <= 0} {
        dputs "ZERO length data received"
        return
    }
    # Find length of payload (byte 8 and 9)
    set payl [string range $data 8 9]
    scan $payl %c%c t1 t2
    set payl [expr {$t1*256+$t2}]
    #dputs "Payloadlength: $payl"
    # Do a quick check to see if we have a complete packet in the buffer
    set totallength [expr {$payl+$::ymsg(headerlength)}]
    if {$buflength < $totallength} {
        dputs "Partial data received"
        return
    }
    
    # We had enough data, rip this out of the buffer
    set data [string range $::ymsg(buffer) 0 [expr {$totallength-1}]]
    set ::ymsg(buffer) [string range $::ymsg(buffer) $totallength end]
    # Some debug info
    dputs "Complete packet received: (length: [string length $data])"
    hputs $data
    # A packet usually ends with C0 80, but one of the packets received during
    # logon ends with C0 80 00.
    # TBD: Insert check here some time

    # The packet payload
    set payload [string range $data 20 end]
    set payload [ymsg_split $payload]

    set command [string range $data 11 11]
    if {$command == "W"} {
        # Challenge command received so do login
        set ::ymsg(session) [string range $data 16 19]
        set challenge [lindex $payload 3]
        ymsg_login $challenge
    } elseif {$command == "U"} {
        # This is the buddylist, also contains a cookie which can be
        # used for some (to me) unknown http features
        foreach {item data} $payload {
            if {$item == "87"} {
                set ::ymsg(buddylist) $data
            } elseif {$item == "59"} {
                # There are several cookies in this packet
                # Don't know what to do with them
                lappend ::ymsg(cookies) $data
            }
        }
        eval uplevel #0 $::ymsg(logincallback)
    } elseif {$command == "\x01"} {
        # This is the "friends online" packet
        dputs "Friends online received"
        foreach {item data} $payload {
            if {$item == "7"} {
                dputs "Online friend: $data"
            }
        }
        ymsg_goonline
    } elseif {$command == "\x6"} {
        # Messsage received
        foreach {item data} $payload {
            if {$item == "4"} {
                dputs "Message from: $data"
                set from $data
            } elseif {$item == "14"} {
                set message $data
            }
        }
        eval uplevel #0 [list $::ymsg(recvmsgcallback) $from \"$message\"]
        
    } elseif {$command == "\x4c"} {
        # This is the service verify packet
        dputs "Service verify packet received"
    }

    # If still data in the buffer then go again
    if {$::ymsg(buffer) != ""} {
        ymsg_parseresponse
    }
}

# This procedure parses the data we got from yahoo during the logon procedure
# and returns it to the calling procedure
# Exmple data: venn:lars_e_mobak,infrasonics\xavenner:venn1
proc ymsg_getbuddies {} {
    set ret ""
    set list $::ymsg(buddylist)
    set groups [split $list \xa]
    foreach group $groups {
        if {$group == ""} {
            continue
        }
        set temp ""
        set groupname [lindex [split $group ":"] 0]
        lappend ret $groupname
        set users [split [lindex [split $group ":"] end] ","]
        foreach user $users {
            lappend temp $user
        }
        lappend ret $temp
    }
    return $ret
}

# I don't know what this is good for yet
# Login may fail even if service is verified by yahoo response
proc ymsg_service_verify {} {
    ymsg_senddata [ymsg_header "service" ""]
}

# Initiate login procedure
proc ymsg_startlogin {} {
    set payload [ymsg_arg \x31 $::ymsg(id)]
    ymsg_senddata [ymsg_header "challenge" $payload]
}

# Do actual login
set ::ymsg_login_runonce 0
proc ymsg_login {chalstring} {
    # Run once. For some reason Yahoo keeps sending challenge strings
    # at blistering speeds if authentication fails.
    if {$::ymsg_login_runonce == 1} {
        dputs "Login alread done - stop."
        return
    }
    set ::ymsg_login_runonce 1

    # Calculate authentication strings
    set strings [ymsg_calcmd5strings $chalstring $::ymsg(id) $::ymsg(passwd)]
    set auth1 [lindex $strings 0]
    set auth2 [lindex $strings 1]

    # Create payload
    # NOTE: Venkydude forgets to mention the id in his explanation
    set payload [ymsg_arg \x30 $::ymsg(id)]
    # The first 24 character response string
    append payload [ymsg_arg "6" $auth1]
    # The second 24 character response string
    append payload [ymsg_arg "96" $auth2]
    # Unknown parts
    append payload [ymsg_arg "2"]
    append payload [ymsg_arg "1"]
    # ID once more - don't know why
    append payload [ymsg_arg "1" $::ymsg(id)]

    ymsg_senddata [ymsg_header "login" $payload]
}


proc ymsg_goonline {} {
    ymsg_senddata [ymsg_header "online" ""]
}

proc ymsg_sendmessage {to message} {
    set payload ""
    append payload [ymsg_arg "1" $::ymsg(id)]
    append payload [ymsg_arg "5" $to]
    append payload [ymsg_arg "14" $message]
    append payload [ymsg_arg "63" "\;0"]
    append payload [ymsg_arg "64" "0"]
    append payload [ymsg_arg "1002" "1"]

    ymsg_senddata [ymsg_header "sendmsg" $payload]
}

# Main YMSG procedure. Handles all interaction with the frontend
proc ymsg {command {args ""}} {
    # I am somewhat confused about which server I am
    # supposed to log into. Packet sniffing YM has shown
    # login to all the servers below. But from time to
    # to time they won't respond to the login packet even though
    # they respond to the service packet.
    # Need to look in i.e GAIM source to see what they do.

    #set ::ymsg(server) "cs63.msg.sc5.yahoo.com" ;# 216.136.233.150
    #set ::ymsg(server) "cs99.msg.sc5.yahoo.com" ;# 216.136.128.167
    set ::ymsg(server) "cs101.msg.sc5.yahoo.com" ;# 216.136.173.180
    #set ::ymsg(server) "scs.yahoo.com"
    set ::ymsg(port) "5050"
    set ::ymsg(headerlength) 20
    set ::ymsg(salt) "_2S43d5f"

    # All commands that use the "args" argument except the login command
    # use the "args" argument for the callback
    set callback $args

    if {$command == "test"} {
        ymsg_testauth
    }
    if {$command == "login"} {
        package require base64
        package require md5
        set ::ymsg(id) [lindex $args 0]
        set ::ymsg(passwd) [lindex $args 1]
        set ::ymsg(status) "login"
        set callback [lindex $args 2]
        set ::ymsg(logincallback) $callback
        ymsg_startlogin
    } elseif {$command == "connect"} {
        set ::ymsg(session) ""
        set ::ymsg(status) "login"
        set callback $args
        ymsg_connect $callback
        ymsg_service_verify
    } elseif {$command == "buddies"} {
        return [ymsg_getbuddies]
    } elseif {$command == "sendmsg"} {
        ymsg_sendmessage [lindex $args 0] [lindex $args 1]
    } elseif {$command == "recvmsgcallback"} {
        set ::ymsg(recvmsgcallback) $args
    } elseif {$command == "logoff"} {
        close $::ymsg(socket)
    } elseif {$command == "debugwin"} {
        set ::ymsg(debugwin) $args
    }
}
