/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#define XSTR(_s) STR(_s)
#define STR(_s) #_s

#define FT_IRC_HARDWARE_CONCURRENCY 0
#define FT_IRC_MESSAGE_LINE_LIMIT 512
#define FT_IRC_MESSAGE_PARAM_LIMIT 15
#define FT_IRC_CHANNEL_LIMIT_PER_USER 10

#define FT_IRC_UMODES "oia"
#define FT_IRC_CMODES "ospmtinbvlk"

#define FT_IRC_SERVER_NAME "irc.v42.dev"
#define FT_IRC_VERSION "ft_irc-0.42"
#define FT_IRC_OPERATOR_NAME "admin"
#define FT_IRC_OPERATOR_PASS "password"
#define FT_IRC_ADD_ALL_MOTD(_ADD)                        \
    _ADD("Welcome to ft_irc server");                    \
    _ADD("This server was created for libserv example"); \
    _ADD("ENJOY!!");

#define FT_IRC_AWAYLEN 200
#define FT_IRC_CASEMAPPING "rfc1459"
#define FT_IRC_CHANLIMIT "#:" XSTR(FT_IRC_CHANNEL_LIMIT_PER_USER)
#define FT_IRC_CHANMODES "b,k,l,imnpst"
#define FT_IRC_CHANNELLEN 32
#define FT_IRC_CHANTYPES "#"
#define FT_IRC_HOSTLEN 64
#define FT_IRC_KICKLEN 255
#define FT_IRC_MAXLIST "b:25"
#define FT_IRC_MAXTARGETS 4
#define FT_IRC_MODES 4
#define FT_IRC_NETWORK "42"
#define FT_IRC_NICKLEN 9
#define FT_IRC_PREFIX "(ov)@+"
#define FT_IRC_STATUSMSG "@+"
#define FT_IRC_TOPICLEN 307
#define FT_IRC_USERLEN 10

#define FT_IRC_ADD_ALL_SUPPORT_PARAM(_ADD)       \
    _ADD("AWAYLEN=" XSTR(FT_IRC_AWAYLEN));       \
    _ADD("CASEMAPPING=" FT_IRC_CASEMAPPING);     \
    _ADD("CHANLIMIT=" FT_IRC_CHANLIMIT);         \
    _ADD("CHANMODES=" FT_IRC_CHANMODES);         \
    _ADD("CHANNELLEN=" XSTR(FT_IRC_CHANNELLEN)); \
    _ADD("CHANTYPES=" FT_IRC_CHANTYPES);         \
    _ADD("HOSTLEN=" XSTR(FT_IRC_HOSTLEN));       \
    _ADD("KICKLEN=" XSTR(FT_IRC_KICKLEN));       \
    _ADD("MAXLIST=" FT_IRC_MAXLIST);             \
    _ADD("MAXTARGETS=" XSTR(FT_IRC_MAXTARGETS)); \
    _ADD("MODES=" XSTR(FT_IRC_MODES));           \
    _ADD("NETWORK=" FT_IRC_NETWORK);             \
    _ADD("NICKLEN=" XSTR(FT_IRC_NICKLEN));       \
    _ADD("PREFIX=" FT_IRC_PREFIX);               \
    _ADD("STATUSMSG=" FT_IRC_STATUSMSG);         \
    _ADD("TOPICLEN=" XSTR(FT_IRC_TOPICLEN));     \
    _ADD("USERLEN=" XSTR(FT_IRC_USERLEN));\

