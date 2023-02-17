/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#pragma once

#include "message.hpp"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#define IRC_REP_MAP(_MACRO)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    /* Error Replies */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(401, ERR_NOSUCHNICK, "<nickname> :No such nick/channel", "- Used to indicate the nickname parameter supplied to a command is currently unused.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(402, ERR_NOSUCHSERVER, "<server name> :No such server", "- Used to indicate the server name given currently doesn't exist.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(403, ERR_NOSUCHCHANNEL, "<channel name> :No such channel", "- Used to indicate the given channel name is invalid.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(404, ERR_CANNOTSENDTOCHAN, "<channel name> :Cannot send to channel", "- Sent to a user who is either (a) not on a channel which is mode +n or (b) not a chanop (or mode +v) on a channel which has mode +m set and is trying to send a PRIVMSG static inline message to that channel.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(405, ERR_TOOMANYCHANNELS, "<channel name> :You have joined too many channels", "- Sent to a user when they have joined the maximum number of allowed channels and they try to join another channel.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(406, ERR_WASNOSUCHNICK, "<nickname> :There was no such nickname", "- Returned by WHOWAS to indicate there is no history information for that nickname.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(407, ERR_TOOMANYTARGETS, "<target> :Duplicate recipients. No static inline message delivered", "- Returned to a client which is attempting to send a PRIVMSG/NOTICE using the user@host destination format and for a user@host which has several occurrences.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(409, ERR_NOORIGIN, ":No origin specified", "- PING or PONG static inline message missing the originator parameter which is required since these commands must work without valid prefixes.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(411, ERR_NORECIPIENT, ":No recipient given (<command>)", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(412, ERR_NOTEXTTOSEND, ":No text to send", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    _MACRO(413, ERR_NOTOPLEVEL, "<mask> :No toplevel domain specified", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(414, ERR_WILDTOPLEVEL, "<mask> :Wildcard in toplevel domain", "- 412 - 414 are returned by PRIVMSG to indicate that the static inline message wasn't delivered for some reason. ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that are returned when an invalid use of \" PRIVMSG $<server> \" or \" PRIVMSG #<host> \" is attempted.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(421, ERR_UNKNOWNCOMMAND, "<command> :Unknown command", "- Returned to a registered client to indicate that the command sent is unknown by the server.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(422, ERR_NOMOTD, ":MOTD File is missing", "- Server's MOTD file could not be opened by the server.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(423, ERR_NOADMININFO, "<server> :No administrative info available", "- Returned by a server in response to an ADMIN static inline message when there is an error in finding the appropriate information.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    _MACRO(424, ERR_FILEERROR, ":File error doing <file op> on <file>", "- Generic error static inline message used to report a failed file operation during the processing of a static inline message.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    _MACRO(431, ERR_NONICKNAMEGIVEN, ":No nickname given", "- Returned when a nickname parameter expected for a command and isn't found.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(432, ERR_ERRONEUSNICKNAME, "<nick> :Erroneus nickname", "- Returned after receiving a NICK static inline message which contains characters which do not fall in the defined set.  See section x.x.x for details on valid nicknames.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(433, ERR_NICKNAMEINUSE, "<nick> :Nickname is already in use", "- Returned when a NICK static inline message is processed that results in an attempt to change to a currently existing nickname.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    _MACRO(436, ERR_NICKCOLLISION, "<nick> :Nickname collision KILL", "- Returned by a server to a client when it detects a nickname collision (registered of a NICK that already exists by another server).")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(441, ERR_USERNOTINCHANNEL, "<nick> <channel> :They aren't on that channel", "- Returned by the server to indicate that the target user of the command is not on the given channel.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(442, ERR_NOTONCHANNEL, "<channel> :You're not on that channel", "- Returned by the server whenever a client tries to perform a channel effecting command for which the client isn't a member.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(443, ERR_USERONCHANNEL, "<user> <channel> :is already on channel", "- Returned when a client tries to invite a user to a channel they are already on.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(444, ERR_NOLOGIN, "<user> :User not logged in", "- Returned by the summon after a SUMMON command for a user was unable to be performed since they were not logged in.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(445, ERR_SUMMONDISABLED, ":SUMMON has been disabled", "- Returned as a response to the SUMMON command.  Must be returned by any server which does not implement it.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(446, ERR_USERSDISABLED, ":USERS has been disabled", "- Returned as a response to the USERS command.  Must be returned by any server which does not implement it.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(451, ERR_NOTREGISTERED, ":You have not registered", "- Returned by the server to indicate that the client must be registered before the server will allow it to be parsed in detail.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(461, ERR_NEEDMOREPARAMS, "<command> :Not enough parameters", "- Returned by the server by numerous commands to indicate to the client that it didn't supply enough parameters.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(462, ERR_ALREADYREGISTRED, ":You may not reregister", "- Returned by the server to any link which tries to change part of the registered details (such as password or user details from second USER static inline message).")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
    _MACRO(463, ERR_NOPERMFORHOST, ":Your host isn't among the privileged", "- Returned to a client which attempts to register with a server which does not been setup to allow connections from the host the attempted connection is tried.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(464, ERR_PASSWDMISMATCH, ":Password incorrect", "- Returned to indicate a failed attempt at registering a connection for which a password was required and was either not given or incorrect.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(465, ERR_YOUREBANNEDCREEP, ":You are banned from this server", "- Returned after an attempt to connect and register yourself with a server which has been setup to explicitly deny connections to you.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(467, ERR_KEYSET, "<channel> :Channel key already set", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    _MACRO(471, ERR_CHANNELISFULL, "<channel> :Cannot join channel (+l)", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(472, ERR_UNKNOWNMODE, "<char> :is unknown mode char to me", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(473, ERR_INVITEONLYCHAN, "<channel> :Cannot join channel (+i)", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(474, ERR_BANNEDFROMCHAN, "<channel> :Cannot join channel (+b)", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(475, ERR_BADCHANNELKEY, "<channel> :Cannot join channel (+k)", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(481, ERR_NOPRIVILEGES, ":Permission Denied- You're not an IRC operator", "- Any command requiring operator privileges to operate must return this error to indicate the attempt was unsuccessful.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(482, ERR_CHANOPRIVSNEEDED, "<channel> :You're not channel operator", "- Any command requiring 'chanop' privileges (such as MODE messages) must return this error if the client making the attempt is not a chanop on the specified channel.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
    _MACRO(483, ERR_CANTKILLSERVER, ":You cant kill a server!", "- Any attempts to use the KILL command on a server are to be refused and this error returned directly to the client.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(491, ERR_NOOPERHOST, ":No O-lines for your host", "- If a client sends an OPER static inline message and the server has not been configured to allow connections from the client's host as an operator, this error must be returned.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(501, ERR_UMODEUNKNOWNFLAG, ":Unknown MODE flag", "- Returned by the server to indicate that a MODE static inline message was sent with a nickname parameter and that the a mode flag sent was not recognized.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(502, ERR_USERSDONTMATCH, ":Cant change mode for other users", "- Error sent to any user trying to view or change the user mode for a user other than themselves.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    /* Command responses */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(300, RPL_NONE, "", "Dummy reply number. Not used.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(302, RPL_USERHOST, ":[<reply>{<space><reply>}]", "- Reply format used by USERHOST to list replies to the query list.  The reply string is composed as follows: <reply> ::= <nick>['*'] '=' <'+'|'-'><hostname> The '*' indicates whether the client has registered as an Operator.  The '-' or '+' characters represent whether the client has set an AWAY static inline message or not respectively.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(303, RPL_ISON, ":[<nick> {<space><nick>}]", "- Reply format used by ISON to list replies to the query list.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
    _MACRO(301, RPL_AWAY, "<nick> :<away static inline message>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    _MACRO(305, RPL_UNAWAY, ":You are no longer marked as being away", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(306, RPL_NOWAWAY, ":You have been marked as being away", "- These replies are used with the AWAY command (if allowed).  RPL_AWAY is sent to any client sending a PRIVMSG to a client which is away.  RPL_AWAY is only sent by the server to which the client is connected. Replies RPL_UNAWAY and RPL_NOWAWAY are sent when the client removes and sets an AWAY static inline message.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(311, RPL_WHOISUSER, "<nick> <user> <host> * :<real name>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    _MACRO(312, RPL_WHOISSERVER, "<nick> <server> :<server info>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 \
    _MACRO(313, RPL_WHOISOPERATOR, "<nick> :is an IRC operator", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    _MACRO(317, RPL_WHOISIDLE, "<nick> <integer> :seconds idle", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    _MACRO(318, RPL_ENDOFWHOIS, "<nick> :End of /WHOIS list", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(319, RPL_WHOISCHANNELS, "<nick> :{[@|+]<channel><space>}", "- Replies 311 - 313, 317 - 319 are all replies generated in response to a WHOIS static inline message.  Given that there are enough parameters present, the answering server must either formulate a reply out of the above numerics (if the query nick is found) or return an error reply.  The '*' in RPL_WHOISUSER is there as the literal character and not as a wild card.  For each reply set, only RPL_WHOISCHANNELS may appear more than once (for long lists of channel names). The '@' and '+' characters next to the channel name indicate whether a client is a channel operator or has been granted permission to speak on a moderated channel.  The RPL_ENDOFWHOIS reply is used to mark the end of processing a WHOIS static inline message.")                                                                                                                                                                                                   \
    _MACRO(314, RPL_WHOWASUSER, "<nick> <user> <host> * :<real name>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(369, RPL_ENDOFWHOWAS, "<nick> :End of WHOWAS", "- When replying to a WHOWAS static inline message, a server must use the replies RPL_WHOWASUSER, RPL_WHOISSERVER or ERR_WASNOSUCHNICK for each nickname in the presented list.  At the end of all reply batches, there must be RPL_ENDOFWHOWAS (even if there was only one reply and it was an error).")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(321, RPL_LISTSTART, "Channel :Users  Name", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(322, RPL_LIST, "<channel> <# visible> :<topic>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(323, RPL_LISTEND, ":End of /LIST", "- Replies RPL_LISTSTART, RPL_LIST, RPL_LISTEND mark the start, actual replies with data and end of the server's response to a LIST command.  If there are no channels available to return, only the start and end reply must be sent.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 \
    _MACRO(324, RPL_CHANNELMODEIS, "<channel> <mode> <mode params>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
    _MACRO(331, RPL_NOTOPIC, "<channel> :No topic is set", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(332, RPL_TOPIC, "<channel> :<topic>", "- When sending a TOPIC static inline message to determine the channel topic, one of two replies is sent.  If the topic is set, RPL_TOPIC is sent back else RPL_NOTOPIC.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(341, RPL_INVITING, "<channel> <nick>", "- Returned by the server to indicate that the attempted INVITE static inline message was successful and is being passed onto the end client.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(342, RPL_SUMMONING, "<user> :Summoning user to IRC", "- Returned by a server answering a SUMMON static inline message to indicate that it is summoning that user.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(351, RPL_VERSION, "<version>.<debuglevel> <server> :<comments>", "- Reply by the server showing its version details. The <version> is the version of the software being used (including any patchlevel revisions) and the <debuglevel> is used to indicate if the server is running in \" debug mode \". The \" comments \" field may contain any comments about the version or further version details.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    _MACRO(352, RPL_WHOREPLY, "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(315, RPL_ENDOFWHO, "<name> :End of /WHO list", "- The RPL_WHOREPLY and RPL_ENDOFWHO pair are used to answer a WHO static inline message.  The RPL_WHOREPLY is only sent if there is an appropriate match to the WHO query.  If there is a list of parameters supplied with a WHO static inline message, a RPL_ENDOFWHO must be sent after processing each list item with <name> being the item.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    _MACRO(353, RPL_NAMREPLY, "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(366, RPL_ENDOFNAMES, "<channel> :End of /NAMES list", "- To reply to a NAMES static inline message, a reply pair consisting of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the server back to the client.  If there is no channel found as in the query, then only RPL_ENDOFNAMES is returned.  The exception to this is when a NAMES static inline message is sent with no parameters and all visible channels and contents are sent back in a series of RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark the end.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(364, RPL_LINKS, "<mask> <server> :<hopcount> <server info>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(365, RPL_ENDOFLINKS, "<mask> :End of /LINKS list", "- In replying to the LINKS static inline message, a server must send replies back using the RPL_LINKS numeric and mark the end of the list using an RPL_ENDOFLINKS reply.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(367, RPL_BANLIST, "<channel> <banid>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
    _MACRO(368, RPL_ENDOFBANLIST, "<channel> :End of channel ban list", "- When listing the active 'bans' for a given channel, a server is required to send the list back using the RPL_BANLIST and RPL_ENDOFBANLIST messages.  A separate RPL_BANLIST is sent for each active banid.  After the banids have been listed (or if none present) a RPL_ENDOFBANLIST must be sent.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(371, RPL_INFO, ":<string>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(374, RPL_ENDOFINFO, ":End of /INFO list", "- A server responding to an INFO static inline message is required to send all its 'info' in a series of RPL_INFO messages with a RPL_ENDOFINFO reply to indicate the end of the replies.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(375, RPL_MOTDSTART, ":- <server> Message of the day - ", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(372, RPL_MOTD, ":- <text>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(376, RPL_ENDOFMOTD, ":End of /MOTD command", "- When responding to the MOTD static inline message and the MOTD file is found, the file is displayed line by line, with each line no longer than 80 characters, using RPL_MOTD format replies.  These should be surrounded by a RPL_MOTDSTART (before the RPL_MOTDs) and an RPL_ENDOFMOTD (after).")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(381, RPL_YOUREOPER, ":You are now an IRC operator", "- RPL_YOUREOPER is sent back to a client which has just successfully issued an OPER static inline message and gained operator status.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(382, RPL_REHASHING, "<config file> :Rehashing", "- If the REHASH option is used and an operator sends a REHASH static inline message, an RPL_REHASHING is sent back to the operator.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(391, RPL_TIME, "<server> :<string showing server's local time>", "- When replying to the TIME static inline message, a server must send the reply using the RPL_TIME format above.  The string showing the time need only contain the correct day and time there.  There is no further requirement for the time string.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    _MACRO(392, RPL_USERSSTART, ":UserID   Terminal  Host", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(393, RPL_USERS, ":%-8s %-9s %-8s", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(394, RPL_ENDOFUSERS, ":End of users", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    _MACRO(395, RPL_NOUSERS, ":Nobody logged in", "- If the USERS static inline message is handled by a server, the replies RPL_USERSTART, RPL_USERS, RPL_ENDOFUSERS and RPL_NOUSERS are used.  RPL_USERSSTART must be sent first, following by either a sequence of RPL_USERS or a single RPL_NOUSER.  Following this is RPL_ENDOFUSERS.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(200, RPL_TRACELINK, "Link <version & debug level> <destination> <next server>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(201, RPL_TRACECONNECTING, "Try. <class> <server>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(202, RPL_TRACEHANDSHAKE, "H.S. <class> <server>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(203, RPL_TRACEUNKNOWN, "???? <class> [<client IP address in dot form>]", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(204, RPL_TRACEOPERATOR, "Oper <class> <nick>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(205, RPL_TRACEUSER, "User <class> <nick>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    _MACRO(206, RPL_TRACESERVER, "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    _MACRO(208, RPL_TRACENEWTYPE, "<newtype> 0 <client name>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(261, RPL_TRACELOG, "File <logfile> <debug level>", "- The RPL_TRACE* are all returned by the server in response to the TRACE static inline message.  How many are returned is dependent on the the TRACE static inline message and whether it was sent by an operator or not.  There is no predefined order for which occurs first. Replies RPL_TRACEUNKNOWN, RPL_TRACECONNECTING and RPL_TRACEHANDSHAKE are all used for connections which have not been fully established and are either unknown, still attempting to connect or in the process of completing the 'server handshake'. RPL_TRACELINK is sent by any server which handles a TRACE static inline message and has to pass it on to another server.  The list of RPL_TRACELINKs sent in response to a TRACE command traversing the IRC network should reflect the actual connectivity of the servers themselves along that path. RPL_TRACENEWTYPE is to be used for any connection which does not fit in the other categories but is being displayed anyway.") \
    _MACRO(211, RPL_STATSLINKINFO, "<linkname> <sendq> <sent messages> <sent bytes> <received messages> <received bytes> <time open>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             \
    _MACRO(212, RPL_STATSCOMMANDS, "<command> <count>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(213, RPL_STATSCLINE, "C <host> * <name> <port> <class>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(214, RPL_STATSNLINE, "N <host> * <name> <port> <class>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(215, RPL_STATSILINE, "I <host> * <host> <port> <class>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(216, RPL_STATSKLINE, "K <host> * <username> <port> <class>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(218, RPL_STATSYLINE, "Y <class> <ping frequency> <connect frequency> <max sendq>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(219, RPL_ENDOFSTATS, "<stats letter> :End of /STATS report", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(241, RPL_STATSLLINE, "L <hostmask> * <servername> <maxdepth>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(242, RPL_STATSUPTIME, ":Server Up %d days %d:%02d:%02d", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
    _MACRO(243, RPL_STATSOLINE, "O <hostmask> * <name>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    _MACRO(244, RPL_STATSHLINE, "H <hostmask> * <servername>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(221, RPL_UMODEIS, "<user mode string>", "- To answer a query about a client's own mode, RPL_UMODEIS is sent back.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(251, RPL_LUSERCLIENT, ":There are <integer> users and <integer> invisible on <integer> servers", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(252, RPL_LUSEROP, "<integer> :operator(s) online", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(253, RPL_LUSERUNKNOWN, "<integer> :unknown connection(s)", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    _MACRO(254, RPL_LUSERCHANNELS, "<integer> :channels formed", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    _MACRO(255, RPL_LUSERME, ":I have <integer> clients and <integer>  servers", "- In processing an LUSERS static inline message, the server sends a set of replies from RPL_LUSERCLIENT, RPL_LUSEROP, RPL_USERUNKNOWN, RPL_LUSERCHANNELS and RPL_LUSERME.  When replying, a server must send back RPL_LUSERCLIENT and RPL_LUSERME.  The other replies are only sent back if a non-zero count is found for them.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(256, RPL_ADMINME, "<server> :Administrative info", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
    _MACRO(257, RPL_ADMINLOC1, ":<admin info>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(258, RPL_ADMINLOC2, ":<admin info>", "")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
    _MACRO(259, RPL_ADMINEMAIL, ":<admin info>", "- When replying to an ADMIN static inline message, a server is expected to use replies RLP_ADMINME through to RPL_ADMINEMAIL and provide a text static inline message with each.  For RPL_ADMINLOC1 a description of what city, state and country the server is in is expected, followed by details of the university and department (RPL_ADMINLOC2) and finally the administrative contact for the server (an email address here is required) in RPL_ADMINEMAIL.")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    /* Reserved numerics */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
    _MACRO(209, RPL_TRACECLASS, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(217, RPL_STATSQLINE, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    _MACRO(231, RPL_SERVICEINFO, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(232, RPL_ENDOFSERVICES, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    _MACRO(233, RPL_SERVICE, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    _MACRO(234, RPL_SERVLIST, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(235, RPL_SERVLISTEND, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(316, RPL_WHOISCHANOP, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(361, RPL_KILLDONE, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(362, RPL_CLOSING, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    _MACRO(363, RPL_CLOSEEND, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(373, RPL_INFOSTART, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         \
    _MACRO(384, RPL_MYPORTIS, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
    _MACRO(466, ERR_YOUWILLBEBANNED, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    _MACRO(476, ERR_BADCHANMASK, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       \
    _MACRO(492, ERR_NOSERVICEHOST, "", "RESERVED")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     \
    /* END */

namespace ft
{
    namespace irc
    {
        enum reply_numerics
        {
#define _MAKE_ENUM(numeric, name, syntax, desc) name = numeric,
            IRC_REP_MAP(_MAKE_ENUM)
#undef _MAKE_ENUM
        };

        struct make_reply_base
        {
            typedef const std::string& param_t;

            std::string prefix;
        };

        struct make_error : make_reply_base
        {
            static inline message no_such_nickname(param_t nickname)
            {
                return message(ERR_NOSUCHNICK) << nickname << "No such nick/channel";
            }

            static inline message no_such_server(param_t server_name)
            {
                return message(ERR_NOSUCHSERVER) << server_name << "No such server";
            }

            static inline message no_such_channel(param_t channel_name)
            {
                return message(ERR_NOSUCHCHANNEL) << channel_name << "No such channel";
            }

            static inline message cannot_send_to_channel(param_t channel_name)
            {
                return message(ERR_CANNOTSENDTOCHAN) << channel_name << "Cannot send to channel";
            }

            static inline message too_many_channels(param_t channel_name)
            {
                return message(ERR_TOOMANYCHANNELS) << channel_name << "You have joined too many channels";
            }

            static inline message was_no_such_nickname(param_t nickname)
            {
                return message(ERR_WASNOSUCHNICK) << nickname << "There was no such nickname";
            }

            static inline message too_many_targets(param_t target)
            {
                return message(ERR_TOOMANYTARGETS) << target << "Duplicate recipients. No message delivered";
            }

            static inline message no_origin()
            {
                return message(ERR_NOORIGIN) << "No origin specified";
            }

            static inline message no_recipient(param_t command)
            {
                return message(ERR_NORECIPIENT) << "No recipient given (" + command + ")";
            }

            static inline message no_text_to_send()
            {
                return message(ERR_NOTEXTTOSEND) << "No text to send";
            }

            static inline message no_toplevel(param_t mask)
            {
                return message(ERR_NOTOPLEVEL) << mask << "No toplevel domain specified";
            }

            static inline message wildcard_toplevel(param_t mask)
            {
                return message(ERR_WILDTOPLEVEL) << mask << "Wildcard in toplevel domain";
            }

            static inline message unknown_command(param_t command)
            {
                return message(ERR_UNKNOWNCOMMAND) << command << "Unknown command";
            }

            static inline message no_motd()
            {
                return message(ERR_NOMOTD) << "MOTD File is missing";
            }

            static inline message no_admin_info(param_t server_name)
            {
                return message(ERR_NOADMININFO) << server_name << "No administrative info available";
            }

            static inline message file_error(param_t file_operation, param_t filename)
            {
                return message(ERR_FILEERROR) << "File error doing " + file_operation + " on " + filename;
            }

            static inline message no_nickname_given()
            {
                return message(ERR_NONICKNAMEGIVEN) << "No nickname given";
            }

            static inline message erroneous_nickname(param_t nickname)
            {
                return message(ERR_ERRONEUSNICKNAME) << nickname << "Erroneus nickname";
            }

            static inline message nickname_in_use(param_t nickname)
            {
                return message(ERR_NICKNAMEINUSE) << nickname << "Nickname is already in use";
            }

            static inline message nickname_collision(param_t nickname)
            {
                return message(ERR_NICKCOLLISION) << nickname << "Nickname collision KILL";
            }

            static inline message user_not_in_channel(param_t nickname, param_t channel_name)
            {
                return message(ERR_USERNOTINCHANNEL) << nickname << channel_name << "They aren't on that channel";
            }

            static inline message not_on_channel(param_t channel_name)
            {
                return message(ERR_NOTONCHANNEL) << channel_name << "You're not on that channel";
            }

            static inline message user_on_channel(param_t username, param_t channel_name)
            {
                return message(ERR_USERONCHANNEL) << username << channel_name << "is already on channel";
            }

            static inline message no_login(param_t username)
            {
                return message(ERR_NOLOGIN) << username << "User not logged in";
            }

            static inline message summon_disabled()
            {
                return message(ERR_SUMMONDISABLED) << "SUMMON has been disabled";
            }

            static inline message users_disabled()
            {
                return message(ERR_USERSDISABLED) << "USERS has been disabled";
            }

            static inline message not_registered()
            {
                return message(ERR_NOTREGISTERED) << "You have not registered";
            }

            static inline message need_more_parameters(param_t command)
            {
                return message(ERR_NEEDMOREPARAMS) << command << "Not enough parameters";
            }

            static inline message already_registered()
            {
                return message(ERR_ALREADYREGISTRED) << "You may not reregister";
            }

            static inline message no_permission_for_host()
            {
                return message(ERR_NOPERMFORHOST) << "Your host isn't among the privileged";
            }

            static inline message password_mismatch()
            {
                return message(ERR_PASSWDMISMATCH) << "Password incorrect";
            }

            static inline message banned_from_server()
            {
                return message(ERR_YOUREBANNEDCREEP) << "You are banned from this server";
            }

            static inline message channel_key_already_set(param_t channel_name)
            {
                return message(ERR_KEYSET) << channel_name << "Channel key already set";
            }

            static inline message channel_is_full(param_t channel_name)
            {
                return message(ERR_CHANNELISFULL) << channel_name << "Cannot join channel (+l)";
            }

            static inline message unknown_mode(char mode_char)
            {
                return message(ERR_UNKNOWNMODE) << mode_char << "is unknown mode char to me";
            }

            static inline message invite_only_channel(param_t channel_name)
            {
                return message(ERR_INVITEONLYCHAN) << channel_name << "Cannot join channel (+i)";
            }

            static inline message banned_from_channel(param_t channel_name)
            {
                return message(ERR_BANNEDFROMCHAN) << channel_name << "Cannot join channel (+b)";
            }

            static inline message bad_channel_key(param_t channel_name)
            {
                return message(ERR_BADCHANNELKEY) << channel_name << "Cannot join channel (+k)";
            }

            static inline message no_privileges()
            {
                return message(ERR_NOPRIVILEGES) << "Permission Denied- You're not an IRC operator";
            }

            static inline message channel_operator_privileges_needed(param_t channel_name)
            {
                return message(ERR_CHANOPRIVSNEEDED) << channel_name << "You're not channel operator";
            }

            static inline message cannot_kill_server()
            {
                return message(ERR_CANTKILLSERVER) << "You cant kill a server!";
            }

            static inline message no_operator_host()
            {
                return message(ERR_NOOPERHOST) << "No O-lines for your host";
            }

            static inline message user_mode_unknown_flag()
            {
                return message(ERR_UMODEUNKNOWNFLAG) << "Unknown MODE flag";
            }

            static inline message users_donot_match()
            {
                return message(ERR_USERSDONTMATCH) << "Cant change mode for other users";
            }
        };

        struct person_info
        {
            std::string nickname;
            std::string username;
            std::string host;
            bool is_operator;
            bool is_away;
        };

        struct channel_info
        {
            std::string channel_name;
            bool is_chanop;
            bool is_chanspk;
        };

        struct member_info
        {
            std::string nickname;
            bool is_chanop;
            bool is_chanspk;
        };

        struct make_reply : make_reply_base
        {
            static inline message none()
            {
                return message(RPL_NONE);
            }

            static inline message user_host(std::vector<person_info> found_person_list)
            {
                std::ostringstream oss;
                for (std::vector<person_info>::iterator it = found_person_list.begin(); it != found_person_list.end(); ++it)
                {
                    oss << it->nickname << (it->is_operator ? "*" : "") << '=' << (it->is_away ? '-' : '+') << it->username << '@' << it->host << ' ';
                }
                return message(RPL_USERHOST) << oss.str();
            }

            static inline message ison(const std::vector<std::string>& nickname_list)
            {
                std::ostringstream oss;
                for (std::vector<std::string>::iterator it = nickname_list.begin(); it != nickname_list.end(); ++it)
                {
                    oss << *it << ' ';
                }
                return message(RPL_ISON);
            }

            static inline message away(param_t nickname, param_t msg)
            {
                return message(RPL_AWAY) << nickname << msg;
            }

            static inline message unaway()
            {
                return message(RPL_UNAWAY) << "You are no longer marked as being away";
            }

            static inline message now_away()
            {
                return message(RPL_NOWAWAY) << "You have been marked as being away";
            }

            static inline message whois_user(param_t nickname, param_t username, param_t host, param_t info)
            {
                const char* const pass = "*";
                return message(RPL_WHOISUSER) << nickname << username << host << pass << info;
            }

            static inline message whois_server(param_t nickname, param_t server_name, param_t info)
            {
                return message(RPL_WHOISSERVER) << nickname << server_name << info;
            }

            static inline message whois_operator(param_t nickname)
            {
                return message(RPL_WHOISOPERATOR) << nickname << "is an IRC operator";
            }

            static inline message whois_idle(param_t nickname, long idle_time_second)
            {
                return message(RPL_WHOISIDLE) << nickname << idle_time_second << "seconds idle";
            }

            static inline message end_of_whois(param_t nickname)
            {
                return message(RPL_ENDOFWHOIS) << nickname << "End of /WHOIS list";
            }

            static inline message whois_channels(param_t nickname, std::vector<channel_info> channel_list)
            {
                std::ostringstream oss;
                for (std::vector<channel_info>::iterator it = channel_list.begin(); it != channel_list.end(); ++it)
                {
                    oss << (it->is_chanop ? "@" : (it->is_chanspk ? "+" : "")) << it->channel_name << ' ';
                }
                return message(RPL_WHOISCHANNELS) << nickname << oss.str();
            }

            static inline message whowas_user(param_t nickname, param_t username, param_t host, param_t info)
            {
                const char* const pass = "*";
                return message(RPL_WHOWASUSER) << nickname << username << host << pass << info;
            }

            static inline message end_of_whowas(param_t nickname)
            {
                return message(RPL_ENDOFWHOWAS) << nickname << "End of WHOWAS";
            }

            static inline message list_start()
            {
                return message(RPL_LISTSTART) << "Channel"
                                              << "Users  Name";
            }

            static inline message list(param_t channel_name, int visible_count, param_t channel_topic)
            {
                return message(RPL_LIST) << channel_name << visible_count << channel_topic;
            }

            static inline message list_end()
            {
                return message(RPL_LISTEND) << "End of /LIST";
            }

            static inline message channel_mode_is(param_t channel_name, param_t channel_mode, param_t member_mode_param)
            {
                return message(RPL_CHANNELMODEIS) << channel_name << channel_mode << member_mode_param;
            }

            static inline message no_topic(param_t channel_name)
            {
                return message(RPL_NOTOPIC) << channel_name << "No topic is set";
            }

            static inline message topic(param_t channel_name, param_t channel_topic)
            {
                return message(RPL_TOPIC) << channel_name << channel_topic;
            }

            static inline message inviting(param_t channel_name, param_t nickname)
            {
                return message(RPL_INVITING) << channel_name << nickname;
            }

            static inline message summoning(param_t username)
            {
                return message(RPL_SUMMONING) << username << "Summoning user to IRC";
            }

            static inline message version(param_t version, int debug_level, param_t server_name, param_t comments)
            {
                std::ostringstream oss;
                oss << version << "." << debug_level;
                return message(RPL_VERSION) << oss.str() << server_name << comments;
            }

            static inline message who_reply(param_t channel_name, param_t username, param_t host, param_t server_name, param_t nickname, bool is_away, bool is_operator, bool is_chanop, bool is_chanspk, int hop_count, param_t info)
            {
                std::ostringstream status, oss;
                status << (is_away ? "G" : "H") << (is_operator ? "*" : "") << (is_chanop ? "@" : (is_chanspk ? "+" : ""));
                oss << hop_count << " " << info;
                return message(RPL_WHOREPLY) << channel_name << username << host << server_name << nickname << status.str() << oss.str();
            }

            static inline message end_of_who(param_t name)
            {
                return message(RPL_ENDOFWHO) << name << "End of /WHO list";
            }

            static inline message name_reply(param_t channel_name, std::vector<member_info> user_list)
            {
                std::ostringstream oss;
                for (std::vector<member_info>::iterator it = user_list.begin(); it != user_list.end(); ++it)
                {
                    oss << it->nickname << (it->is_chanop ? "@" : (it->is_chanspk ? "+" : "")) << ' ';
                }
                return message(RPL_NAMREPLY) << channel_name << oss.str();
            }

            static inline message end_of_names(param_t channel_name)
            {
                return message(RPL_ENDOFNAMES) << channel_name << "End of /NAMES list";
            }

            static inline message links(param_t mask, param_t server_name, int hop_count, param_t info)
            {
                std::ostringstream oss;
                oss << hop_count << " " << info;
                return message(RPL_LINKS) << mask << server_name << oss.str();
            }

            static inline message end_of_links(param_t mask)
            {
                return message(RPL_ENDOFLINKS) << mask << "End of /LINKS list";
            }

            static inline message ban_list(param_t channel_name, int ban_id)
            {
                return message(RPL_BANLIST) << channel_name << ban_id;
            }

            static inline message end_of_ban_list(param_t channel_name)
            {
                return message(RPL_ENDOFBANLIST) << channel_name << "End of channel ban list";
            }

            static inline message info(param_t info)
            {
                return message(RPL_INFO) << info;
            }

            static inline message end_of_info()
            {
                return message(RPL_ENDOFINFO) << "End of /INFO list";
            }

            static inline message motd_start(param_t server_name)
            {
                return message(RPL_MOTDSTART) << "- " + server_name + " Message of the day - ";
            }

            static inline message motd(param_t text)
            {
                return message(RPL_MOTD) << "- " + text;
            }

            static inline message end_of_motd()
            {
                return message(RPL_ENDOFMOTD) << "End of /MOTD command";
            }

            static inline message now_operator()
            {
                return message(RPL_YOUREOPER) << "You are now an IRC operator";
            }

            static inline message rehashing(param_t config_filename)
            {
                return message(RPL_REHASHING) << config_filename << "Rehashing";
            }

            static inline message time(param_t server_name, param_t local_time_str)
            {
                return message(RPL_TIME) << server_name << local_time_str;
            }

            static inline message users_start()
            {
                return message(RPL_USERSSTART) << "UserID   Terminal  Host";
            }

            static inline message users(param_t utmp_name, param_t utmp_line, param_t utmp_host)
            {
                // NOTE: utmp.h
                std::ostringstream oss;
                oss << std::left << std::setw(8) << utmp_name << ' ' // "%-8s "
                    << std::left << std::setw(9) << utmp_line << ' ' // "%-9s "
                    << std::left << std::setw(8) << utmp_host;       // "%-8s"
                return message(RPL_USERS) << oss.str();
            }

            static inline message end_of_users()
            {
                return message(RPL_ENDOFUSERS) << "End of users";
            }

            static inline message no_users()
            {
                return message(RPL_NOUSERS) << "Nobody logged in";
            }

            static inline message trace_link(param_t version, int debug_level, param_t destination_server_name, param_t next_server_name)
            {
                std::ostringstream oss;
                oss << version << debug_level;
                return message(RPL_TRACELINK) << "Link" << oss.str() << destination_server_name << next_server_name;
            }

            static inline message trace_connecting(int client_class, param_t client_name)
            {
                return message(RPL_TRACECONNECTING) << "Try." << client_class << client_name;
            }

            static inline message trace_handshake(int client_class, param_t client_name)
            {
                return message(RPL_TRACEHANDSHAKE) << "H.S." << client_class << client_name;
            }

            static inline message trace_unknown(int client_class, param_t client_name)
            {
                return message(RPL_TRACEUNKNOWN) << "????" << client_class << client_name;
            }

            static inline message trace_operator(int client_class, param_t client_name)
            {
                return message(RPL_TRACEOPERATOR) << "Oper" << client_class << client_name;
            }

            static inline message trace_user(int client_class, param_t client_name)
            {
                return message(RPL_TRACEUSER) << "User" << client_class << client_name;
            }

            static inline message trace_server(int client_class, int server_count, int client_count, param_t client_name, param_t by, param_t uesrname, param_t host)
            {
                std::ostringstream oss_s, oss_c, oss;
                oss_s << server_count << 'S';
                oss_c << client_count << 'C';
                oss << by << '!' << username << '@' << host; // ME: <by or '*'>!*@<my name>
                return message(RPL_TRACESERVER) << "Serv" << client_class << oss_s.str() << oss_c.str() << client_name << oss.str();
            }

            static inline message trace_new_type(param_t new_type, param_t client_name)
            {
                return message(RPL_TRACENEWTYPE) << new_type << "0" << client_name;
            }

            static inline message trace_log(param_t log_filename, int debug_level)
            {
                return message(RPL_TRACELOG) << "File" << log_filename << debug_level;
            }

            static inline message stats_link_info(param_t linkname, int sendq, param_t sent_messages, int sent_bytes, param_t received_messages, int received_bytes, int time_open)
            {
                return message(RPL_STATSLINKINFO) << linkname << sendq << sent_messages << sent_bytes << received_messages << received_bytes << time_open;
            }

            static inline message stats_commands(param_t command, unsigned count)
            {
                return message(RPL_STATSCOMMANDS) << command << count;
            }

            static inline message stats_c_line(param_t host, param_t pass, param_t name, int port, int conf_class)
            {
                return message(RPL_STATSCLINE) << 'C' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
            }

            static inline message stats_n_line(param_t host, param_t pass, param_t name, int port, int conf_class)
            {
                return message(RPL_STATSNLINE) << 'N' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
            }

            static inline message stats_i_line(param_t host, param_t pass, param_t name, int port, int conf_class)
            {
                return message(RPL_STATSILINE) << 'I' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
            }

            static inline message stats_k_line(param_t host, param_t pass, param_t name, int port, int conf_class)
            {
                return message(RPL_STATSKLINE) << 'K' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
            }

            static inline message stats_y_line(int class_class, int ping_freq, int conn_freq, int max_sendq)
            {
                return message(RPL_STATSYLINE) << 'Y' << class_class << ping_freq << conn_freq << max_sendq;
            }

            static inline message end_of_stats(char stats_letter)
            {
                return message(RPL_ENDOFSTATS) << stats_letter << "End of /STATS report";
            }

            static inline message stats_l_line(param_t host, param_t pass, param_t name, int max_depth)
            {
                return message(RPL_STATSLLINE) << 'L' << host << (!pass.empty() ? "*" : "<NULL>") << name << max_depth;
            }

            static inline message stats_uptime(int days, int hours, int minutes, int seconds)
            {
                std::ostringstream oss;
                oss << "Server Up " << days << " days "
                    << hours << ":"
                    << std::setfill('0') << std::setw(2) << std::dec << minutes << ":" // "%02d:"
                    << std::setfill('0') << std::setw(2) << std::dec << seconds;       // "%02d"
                return message(RPL_STATSUPTIME) << oss.str();
            }

            static inline message stats_o_line(param_t host, param_t pass, param_t name)
            {
                return message(RPL_STATSOLINE) << 'O' << host << (!pass.empty() ? "*" : "<NULL>") << name;
            }

            static inline message stats_h_line(param_t host, param_t pass, param_t name)
            {
                return message(RPL_STATSHLINE) << 'H' << host << (!pass.empty() ? "*" : "<NULL>") << name;
            }

            static inline message user_mode_is(param_t user_mode)
            {
                return message(RPL_UMODEIS) << user_mode;
            }

            static inline message luser_client(int user_count, int invisible_count, int server_count)
            {
                std::ostringstream oss;
                oss << "There are " << user_count << " users and " << invisible_count << " invisible on " << server_count << " servers";
                return message(RPL_LUSERCLIENT) << oss.str();
            }

            static inline message luser_operator(int op_count)
            {
                return message(RPL_LUSEROP) << op_count << "operator(s) online";
            }

            static inline message luser_unknown(int unknown_count)
            {
                return message(RPL_LUSERUNKNOWN) << unknown_count << "unknown connection(s)";
            }

            static inline message luser_channels(int channel_count)
            {
                return message(RPL_LUSERCHANNELS) << channel_count << "channels formed";
            }

            static inline message luser_me(int client_count, int server_count)
            {
                std::ostringstream oss;
                oss << "I have " << client_count << " clients and " << server_count << " servers";
                return message(RPL_LUSERME) << oss.str();
            }

            static inline message admin_me(param_t server_name)
            {
                return message(RPL_ADMINME) << server_name << ":Administrative info";
            }

            static inline message admin_loc1(param_t location)
            {
                return message(RPL_ADMINLOC1) << location;
            }

            static inline message admin_loc2(param_t location_detail)
            {
                return message(RPL_ADMINLOC2) << location_detail;
            }

            static inline message admin_email(param_t email_address)
            {
                return message(RPL_ADMINEMAIL) << email_address;
            }
        };
    }
}
