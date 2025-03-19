#------------------------------------------------------------------------
#   numerics.tcl --
#
#   All IRC command numerics known.
#
#------------------------------------------------------------------------
#  RCS: @(#) $Id: numerics.tcl,v 1.4 2002/09/14 20:03:01 davygrvy Exp $
#------------------------------------------------------------------------

# place all these variables in the ::IRC namespace
namespace eval ::IRC {

#
# Reserve numerics 000-099 for server-client connections where the client
# is local to the server. If any server is passed a numeric in this range
# from another server then it is remapped to 100-199. -avalon
#
variable RPL_WELCOME	001
variable RPL_YOURHOST	002
variable RPL_CREATED	003
variable RPL_MYINFO	004

####################
variable RPL_MAP              005    ;# u2.10.02
variable RPL_BOUNCE           005    ;# ircd2.9.5
variable RPL_PROTOCTL         005    ;# dal4.6.3
####################

variable RPL_MAPMORE          006    ;# u2.10.02
variable RPL_MAPEND           007    ;# u2.10.02
variable RPL_SNOMASK          008    ;# u2.10.02
variable RPL_STATMEMTOT       009    ;# u2.10.02
variable RPL_STATMEM          010    ;# u2.10.02
variable RPL_YOURCOOKIE       014    ;# TS4

#
# Errors are in the range from 400-599 currently and are grouped by what
# commands they come from.
#
variable ERR_NOSUCHNICK       401
variable ERR_NOSUCHSERVER     402
variable ERR_NOSUCHCHANNEL    403
variable ERR_CANNOTSENDTOCHAN 404
variable ERR_TOOMANYCHANNELS  405
variable ERR_WASNOSUCHNICK    406
variable ERR_TOOMANYTARGETS   407
variable ERR_NOSUCHSERVICE    408
variable ERR_NOORIGIN         409

variable ERR_NORECIPIENT      411
variable ERR_NOTEXTTOSEND     412
variable ERR_NOTOPLEVEL       413
variable ERR_WILDTOPLEVEL     414

###################
variable ERR_QUERYTOOLONG     416    ;# u2.10.02
variable ERR_TOOMANYMATCHES   416    ;# ircd2.9.3
###################

variable ERR_UNKNOWNCOMMAND   421
variable ERR_NOMOTD           422
variable ERR_NOADMININFO      423
# variable ERR_FILEERROR        424     removed from RFC1459

variable ERR_NONICKNAMEGIVEN  431
variable ERR_ERRONEUSNICKNAME 432
variable ERR_NICKNAMEINUSE    433
variable ERR_SERVICENAMEINUSE 434
variable ERR_SERVICECONFUSED  435
variable ERR_NICKCOLLISION    436

####################
variable ERR_BANNICKCHANGE    437    ;# u2.10.02 and dal4.6.3
variable ERR_UNAVAILRESOURCE  437    ;# ircd2.9.5
variable ERR_TOOMANYNICKS     437    ;# CSr30.5
####################

variable ERR_NICKTOOFAST      438    ;# u2.10.02
variable ERR_TARGETTOOFAST    439    ;# u2.10.02

variable ERR_USERNOTINCHANNEL 441
variable ERR_NOTONCHANNEL     442
variable ERR_USERONCHANNEL    443
# variable ERR_NOLOGIN          444    removed from RFC1459
# variable ERR_SUMMONDISABLED   445    removed from RFC1459
# variable ERR_USERSDISABLED    446    removed from RFC1459

variable ERR_NOTREGISTERED    451
variable ERR_IDCOLLISION      452    ;# IRCnet
variable ERR_NICKLOST         453    ;# IRCnet

variable ERR_NEEDMOREPARAMS   461
variable ERR_ALREADYREGISTRED 462
variable ERR_NOPERMFORHOST    463
variable ERR_PASSWDMISMATCH   464
variable ERR_YOUREBANNEDCREEP 465
variable ERR_YOUWILLBEBANNED  466
variable ERR_KEYSET           467    ;# u2.10.02
variable ERR_INVALIDUSERNAME  468    ;# u2.10.02

variable ERR_CHANNELISFULL    471
variable ERR_UNKNOWNMODE      472
variable ERR_INVITEONLYCHAN   473
variable ERR_BANNEDFROMCHAN   474
variable ERR_BADCHANNELKEY    475
variable ERR_BADCHANMASK      476    ;# u2.10.02

#####################
variable ERR_MODELESS         477    ;# u2.10.02
variable ERR_NEEDREGGEDNICK   477    ;# dal4.6.3
#####################

variable ERR_BANLISTFULL      478    ;# u2.10.02

variable ERR_NOPRIVILEGES     481
variable ERR_CHANOPRIVSNEEDED 482
variable ERR_CANTKILLSERVER   483

#####################
variable ERR_ISCHANSERVICE    484    ;# u2.10.02
variable ERR_DESYNC           484    ;# hybrid-5
variable ERR_RESTRICTED       484    ;# ircd2.9.5
#####################

variable ERR_CHANTOORECENT    487    ;# IRCnet
variable ERR_TSLESSCHAN       488    ;# IRCnet

variable ERR_NOOPERHOST       491
variable ERR_NOSERVICEHOST    492

variable ERR_UMODEUNKNOWNFLAG 501
variable ERR_USERSDONTMATCH   502

variable ERR_GHOSTEDCLIENT    503    ;# TS4 efnet
variable ERR_LAST_ERR_MSG     504    ;# TS4 efnet

variable ERR_SILELISTFULL     511    ;# u2.10.02

variable ERR_NOSUCHGLINE      512    ;# u2.10.02
variable ERR_BADPING          513    ;# u2.10.02

#
# Numberic replies from server commands.
# These are currently in the range 200-399.
#
variable RPL_NONE             300
variable RPL_AWAY             301
variable RPL_USERHOST         302
variable RPL_ISON             303
variable RPL_TEXT             304
variable RPL_UNAWAY           305
variable RPL_NOWAWAY          306

################
variable RPL_USERIP           307    ;# u2.10.02
variable RPL_WHOISREGNICK     307    ;# dal4.6.3
################

variable RPL_WHOISUSER        311
variable RPL_WHOISSERVER      312
variable RPL_WHOISOPERATOR    313

variable RPL_WHOWASUSER       314
variable RPL_ENDOFWHO         315    ;# See RPL_WHOREPLY/RPL_WHOSPCRPL

# variable RPL_WHOISCHANOP      316      removed from RFC1459
variable RPL_WHOISIDLE        317

variable RPL_ENDOFWHOIS       318
variable RPL_WHOISCHANNELS    319

variable RPL_LISTSTART        321
variable RPL_LIST             322
variable RPL_LISTEND          323
variable RPL_CHANNELMODEIS    324
variable RPL_CHANNELPASSIS    325    ;# IRCnet
variable RPL_NOCHANPASS       326    ;# IRCnet
variable RPL_CHPASSUNKNOWN    327    ;# IRCnet
variable RPL_CREATIONTIME     329

variable RPL_NOTOPIC          331
variable RPL_TOPIC            332
variable RPL_TOPICWHOTIME     333    ;# u2.10.02
variable RPL_LISTUSAGE        334    ;# u2.10.02
variable RPL_CHANPASSOK       338    ;# IRCnet
variable RPL_BADCHANPASS      339    ;# IRCnet

variable RPL_INVITING         341
# variable RPL_SUMMONING        342   removed from RFC1459
variable RPL_EXCEPTLIST       348    ;# IRCnet
variable RPL_ENDOFEXCEPTLIST  349    ;# IRCnet

variable RPL_VERSION          351

variable RPL_WHOREPLY         352    ;# See also RPL_ENDOFWHO
variable RPL_NAMREPLY         353    ;# See also RPL_ENDOFNAMES
variable RPL_WHOSPCRPL        354    ;# u2.10.02, See also RPL_ENDOFWHO


variable RPL_KILLDONE         361
variable RPL_CLOSING          362
variable RPL_CLOSEEND         363
variable RPL_LINKS            364
variable RPL_ENDOFLINKS       365
variable RPL_ENDOFNAMES       366    ;# See RPL_NAMREPLY
variable RPL_BANLIST          367
variable RPL_ENDOFBANLIST     368
variable RPL_ENDOFWHOWAS      369

variable RPL_INFO             371
variable RPL_MOTD             372
variable RPL_INFOSTART        373
variable RPL_ENDOFINFO        374
variable RPL_MOTDSTART        375
variable RPL_ENDOFMOTD        376

variable RPL_ISASERVICE       377    ;# undocumented  irc.pacbell.net uses this
                                 # in replace of RPL_MOTD.  WTF?

variable RPL_YOUREOPER        381
variable RPL_REHASHING        382
variable RPL_YOURESERVICE     383
variable RPL_MYPORTIS         384
variable RPL_NOTOPERANYMORE   385    ;# Extension to RFC1459

variable RPL_TIME             391
variable RPL_USERSSTART       392
variable RPL_USERS            393
variable RPL_ENDOFUSERS       394
variable RPL_NOUSERS          395

variable RPL_TRACELINK        200
variable RPL_TRACECONNECTING  201
variable RPL_TRACEHANDSHAKE   202
variable RPL_TRACEUNKNOWN     203
variable RPL_TRACEOPERATOR    204
variable RPL_TRACEUSER        205
variable RPL_TRACESERVER      206
variable RPL_TRACESERVICE     207
variable RPL_TRACENEWTYPE     208
variable RPL_TRACECLASS       209

variable RPL_STATSLINKINFO    211
variable RPL_STATSCOMMANDS    212
variable RPL_STATSCLINE       213
variable RPL_STATSNLINE       214
variable RPL_STATSILINE       215
variable RPL_STATSKLINE       216

##################
variable RPL_STATSQLINE       217    ;# u2.10.02
variable RPL_STATSPLINE       217
##################

variable RPL_STATSYLINE       218
variable RPL_ENDOFSTATS       219    ;# See also RPL_STATSDLINE

variable RPL_UMODEIS          221

##################
variable RPL_SQLINE_NICK      222    ;# dal4.6.3
variable RPL_STATSBLINE       222    ;# hybrid-5 and CSr30.5
##################

##################
#variable RPL_STATSDLINE       224    ;# CSr30.5
variable RPL_STATSFLINE       224    ;# hybrid-5
##################

##################
variable RPL_STATSGLINE       225    ;# CSr30.5
#variable RPL_STATSDLINE       225    ;# hybrid-5
##################

variable RPL_SERVICEINFO      231
variable RPL_ENDOFSERVICES    232
variable RPL_SERVICE          233
variable RPL_SERVLIST         234
variable RPL_SERVLISTEND      235

variable RPL_STATSLLINE       241
variable RPL_STATSUPTIME      242
variable RPL_STATSOLINE       243
variable RPL_STATSHLINE       244
# variable RPL_STATSSLINE       245    Reserved and not used

##################
variable RPL_STATSTLINE       246    ;# u2.10.02
variable RPL_STATSPING        246    ;# ircd2.9.5
##################

##################
variable RPL_STATSGLINE       247    ;# u2.10.02
variable RPL_STATSBLINE       247    ;# ircd 2.9.5
variable RPL_STATSXLINE       247    ;# dal4.6.3
##################

##################
variable RPL_STATSULINE       248    ;# u2.10.02 and dal4.6.3
variable RPL_STATSDEFINE      248    ;# ircd2.9.5
##################

variable RPL_STATSDEBUG       249    ;# Extension to RFC1459

##################
variable RPL_STATSCONN        250    ;# u2.10.02
#variable RPL_STATSDLINE       250    ;# ircd2.9.5
##################

variable RPL_LUSERCLIENT      251
variable RPL_LUSEROP          252
variable RPL_LUSERUNKNOWN     253
variable RPL_LUSERCHANNELS    254
variable RPL_LUSERME          255
variable RPL_ADMINME          256
variable RPL_ADMINLOC1        257
variable RPL_ADMINLOC2        258
variable RPL_ADMINEMAIL       259

variable RPL_TRACELOG         261

##################
variable RPL_TRACEPING        262    ;# u2.10.02
variable RPL_ENDOFTRACE       262    ;# ircd2.9.5, CSr30.5, and hybrid-5 
##################

##################
variable RPL_LOAD2HI          263    ;# hybrid-5
variable RPL_TRYAGAIN         263    ;# ircd2.9.5
##################

variable RPL_LOCALUSERS       265    ;# TS4
variable RPL_GLOBALUSERS      266    ;# TS4

variable RPL_SILELIST         271    ;# u2.10.02
variable RPL_ENDOFSILELIST    272    ;# u2.10.02

variable RPL_STATSDELTA       274    ;# IRCnet
variable RPL_STATSDLINE       275    ;# u2.10.02

variable RPL_GLIST            280    ;# u2.10.02
variable RPL_ENDOFGLIST       281    ;# u2.10.02

variable ERR_NUMERIC_ERR	999	;# dalnet
}
