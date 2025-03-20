package require Itcl
puts [package require IRC_UserInterface]
load IRC_Engine.dll


itcl::class IRC::engine {
    constructor {args} {
	eval _initEngine $args
    }
    destructor {
	_destroyEngine
    }

    common irc_engine_library [file dirname [info script]]

    public {
	method destroy {} {itcl::delete object $this}
	method connect {} @irc-engine-connect
	method disconnect {} @irc-engine-disconnect
	method connected {} @irc-engine-connected
	method raw {} @irc-engine-raw
	method server {} @irc-engine-server
	method my_nick {} @irc-engine-my_nick
	method my_ip {} @irc-engine-my_ip
	method my_host {} @irc-engine-my_host
	method my_user {} @irc-engine-my_user
	method my_port {} @irc-engine-my_port
	method channels {} @irc-engine-channels
	method nicks {} @irc-engine-nicks
	method scripts {} @irc-engine-scripts
	method mode {} @irc-engine-mode
	method whois {} @irc-engine-whois
	method lookup {} @irc-engine-lookup
	method color {} @irc-engine-color
	method topic {} @irc-engine-topic

	method rehash {} @irc-engine-rehash
    }
    private {
	method _initEngine {} @irc-engine-construct
	method _destroyEngine {} @irc-engine-destruct
    }
}


itcl::class IRC::ui {
    constructor {args} {
	_initUI
    }
    destructor {
	_destroyUI
    }
    public {
	method destroy {} {itcl::delete object $this}
	method echo {} @irc-ui-echo
#	method window {} @irc-ui-window
	method menu {} @irc-ui-menu
	method hotkey {} @irc-ui-hotkey
	method channel {} @irc-ui-channel
	method query {} @irc-ui-query
	method chat {} @irc-ui-chat
	method queries {} @irc-ui-queries
	method chats {} @irc-ui-chats
	method say {} @irc-ui-say
	method input {} @irc-ui-input
    }
    private {
	method _initUI {} @irc-ui-construct
	method _destroyUI {} @irc-ui-destruct
    }
}




itcl::class IRC::connection {
 
    # ===============================================
    # Bring the 2 halves together using inheritence.
    # ===============================================
    inherit engine ui
 
    constructor {args} {
        eval engine::constructor $args
    } {}
    public method destroy {} {itcl::delete object $this}
}

