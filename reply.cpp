/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "reply.hpp"

#include "message.hpp"
#include "reply.hpp"

#include <iomanip>
#include <sstream>
#include <string>

ft::irc::message ft::irc::make_error::no_such_nickname(param_t nickname)
{
    return message(ERR_NOSUCHNICK, this->prefix) << nickname << "No such nick/channel";
}

ft::irc::message ft::irc::make_error::no_such_server(param_t servername)
{
    return message(ERR_NOSUCHSERVER, this->prefix) << servername << "No such server";
}

ft::irc::message ft::irc::make_error::no_such_channel(param_t channelname)
{
    return message(ERR_NOSUCHCHANNEL, this->prefix) << channelname << "No such channel";
}

ft::irc::message ft::irc::make_error::cannot_send_to_channel(param_t channelname)
{
    return message(ERR_CANNOTSENDTOCHAN, this->prefix) << channelname << "Cannot send to channel";
}

ft::irc::message ft::irc::make_error::too_many_channels(param_t channelname)
{
    return message(ERR_TOOMANYCHANNELS, this->prefix) << channelname << "You have joined too many channels";
}

ft::irc::message ft::irc::make_error::was_no_such_nickname(param_t nickname)
{
    return message(ERR_WASNOSUCHNICK, this->prefix) << nickname << "There was no such nickname";
}

ft::irc::message ft::irc::make_error::too_many_targets(param_t target)
{
    return message(ERR_TOOMANYTARGETS, this->prefix) << target << "Duplicate recipients. No message delivered";
}

ft::irc::message ft::irc::make_error::no_origin()
{
    return message(ERR_NOORIGIN, this->prefix) << "No origin specified";
}

ft::irc::message ft::irc::make_error::no_recipient(param_t command)
{
    return message(ERR_NORECIPIENT, this->prefix) << "No recipient given (" + command + ")";
}

ft::irc::message ft::irc::make_error::no_text_to_send()
{
    return message(ERR_NOTEXTTOSEND, this->prefix) << "No text to send";
}

ft::irc::message ft::irc::make_error::no_toplevel(param_t mask)
{
    return message(ERR_NOTOPLEVEL, this->prefix) << mask << "No toplevel domain specified";
}

ft::irc::message ft::irc::make_error::wildcard_toplevel(param_t mask)
{
    return message(ERR_WILDTOPLEVEL, this->prefix) << mask << "Wildcard in toplevel domain";
}

ft::irc::message ft::irc::make_error::unknown_command(param_t command)
{
    return message(ERR_UNKNOWNCOMMAND, this->prefix) << command << "Unknown command";
}

ft::irc::message ft::irc::make_error::no_motd()
{
    return message(ERR_NOMOTD, this->prefix) << "MOTD File is missing";
}

ft::irc::message ft::irc::make_error::no_admin_info(param_t servername)
{
    return message(ERR_NOADMININFO, this->prefix) << servername << "No administrative info available";
}

ft::irc::message ft::irc::make_error::file_error(param_t file_operation, param_t filename)
{
    return message(ERR_FILEERROR, this->prefix) << "File error doing " + file_operation + " on " + filename;
}

ft::irc::message ft::irc::make_error::no_nickname_given()
{
    return message(ERR_NONICKNAMEGIVEN, this->prefix) << "No nickname given";
}

ft::irc::message ft::irc::make_error::erroneous_nickname(param_t nickname)
{
    return message(ERR_ERRONEUSNICKNAME, this->prefix) << nickname << "Erroneus nickname";
}

ft::irc::message ft::irc::make_error::nickname_in_use(param_t nickname)
{
    return message(ERR_NICKNAMEINUSE, this->prefix) << nickname << "Nickname is already in use";
}

ft::irc::message ft::irc::make_error::nickname_collision(param_t nickname)
{
    return message(ERR_NICKCOLLISION, this->prefix) << nickname << "Nickname collision KILL";
}

ft::irc::message ft::irc::make_error::user_not_in_channel(param_t nickname, param_t channelname)
{
    return message(ERR_USERNOTINCHANNEL, this->prefix) << nickname << channelname << "They aren't on that channel";
}

ft::irc::message ft::irc::make_error::not_on_channel(param_t channelname)
{
    return message(ERR_NOTONCHANNEL, this->prefix) << channelname << "You're not on that channel";
}

ft::irc::message ft::irc::make_error::user_on_channel(param_t username, param_t channelname)
{
    return message(ERR_USERONCHANNEL, this->prefix) << username << channelname << "is already on channel";
}

ft::irc::message ft::irc::make_error::no_login(param_t username)
{
    return message(ERR_NOLOGIN, this->prefix) << username << "User not logged in";
}

ft::irc::message ft::irc::make_error::summon_disabled()
{
    return message(ERR_SUMMONDISABLED, this->prefix) << "SUMMON has been disabled";
}

ft::irc::message ft::irc::make_error::users_disabled()
{
    return message(ERR_USERSDISABLED, this->prefix) << "USERS has been disabled";
}

ft::irc::message ft::irc::make_error::not_registered()
{
    return message(ERR_NOTREGISTERED, this->prefix) << "You have not registered";
}

ft::irc::message ft::irc::make_error::need_more_parameters(param_t command)
{
    return message(ERR_NEEDMOREPARAMS, this->prefix) << command << "Not enough parameters";
}

ft::irc::message ft::irc::make_error::already_registered()
{
    return message(ERR_ALREADYREGISTRED, this->prefix) << "You may not reregister";
}

ft::irc::message ft::irc::make_error::no_permission_for_host()
{
    return message(ERR_NOPERMFORHOST, this->prefix) << "Your host isn't among the privileged";
}

ft::irc::message ft::irc::make_error::password_mismatch()
{
    return message(ERR_PASSWDMISMATCH, this->prefix) << "Password incorrect";
}

ft::irc::message ft::irc::make_error::banned_from_server()
{
    return message(ERR_YOUREBANNEDCREEP, this->prefix) << "You are banned from this server";
}

ft::irc::message ft::irc::make_error::channel_key_already_set(param_t channelname)
{
    return message(ERR_KEYSET, this->prefix) << channelname << "Channel key already set";
}

ft::irc::message ft::irc::make_error::channel_is_full(param_t channelname)
{
    return message(ERR_CHANNELISFULL, this->prefix) << channelname << "Cannot join channel (+l)";
}

ft::irc::message ft::irc::make_error::unknown_mode(char mode_char)
{
    return message(ERR_UNKNOWNMODE, this->prefix) << mode_char << "is unknown mode char to me";
}

ft::irc::message ft::irc::make_error::invite_only_channel(param_t channelname)
{
    return message(ERR_INVITEONLYCHAN, this->prefix) << channelname << "Cannot join channel (+i)";
}

ft::irc::message ft::irc::make_error::banned_from_channel(param_t channelname)
{
    return message(ERR_BANNEDFROMCHAN, this->prefix) << channelname << "Cannot join channel (+b)";
}

ft::irc::message ft::irc::make_error::bad_channel_key(param_t channelname)
{
    return message(ERR_BADCHANNELKEY, this->prefix) << channelname << "Cannot join channel (+k)";
}

ft::irc::message ft::irc::make_error::no_privileges()
{
    return message(ERR_NOPRIVILEGES, this->prefix) << "Permission Denied- You're not an IRC operator";
}

ft::irc::message ft::irc::make_error::channel_operator_privileges_needed(param_t channelname)
{
    return message(ERR_CHANOPRIVSNEEDED, this->prefix) << channelname << "You're not channel operator";
}

ft::irc::message ft::irc::make_error::cannot_kill_server()
{
    return message(ERR_CANTKILLSERVER, this->prefix) << "You cant kill a server!";
}

ft::irc::message ft::irc::make_error::no_operator_host()
{
    return message(ERR_NOOPERHOST, this->prefix) << "No O-lines for your host";
}

ft::irc::message ft::irc::make_error::user_mode_unknown_flag()
{
    return message(ERR_UMODEUNKNOWNFLAG, this->prefix) << "Unknown MODE flag";
}

ft::irc::message ft::irc::make_error::users_donot_match()
{
    return message(ERR_USERSDONTMATCH, this->prefix) << "Cant change mode for other users";
}

ft::irc::message ft::irc::make_reply::none()
{
    return message(RPL_NONE, this->prefix);
}

ft::irc::message ft::irc::make_reply::user_host(std::vector<std::tuple<std::string, std::string, bool, bool> > infos)
{
    // FIXME: tuple???
    std::ostringstream oss;
    for (auto&& [nickname, hostname, is_operator, is_away] : infos)
    {
        oss << nickname << (is_operator ? "*" : "") << "=" << (is_away ? "-" : "+") << hostname << ' ';
    }
    return message(RPL_USERHOST, this->prefix) << oss.str();
}

ft::irc::message ft::irc::make_reply::ison(std::vector<std::string> nicks)
{
    // FIXME: foreach???
    std::ostringstream oss;
    for (auto&& nickname : nicks)
    {
        oss << nickname << ' ';
    }
    return message(RPL_ISON, this->prefix);
}

ft::irc::message ft::irc::make_reply::away(param_t nickname, param_t msg)
{
    return message(RPL_AWAY, this->prefix) << nickname << msg;
}

ft::irc::message ft::irc::make_reply::unaway()
{
    return message(RPL_UNAWAY, this->prefix) << "You are no longer marked as being away";
}

ft::irc::message ft::irc::make_reply::now_away()
{
    return message(RPL_NOWAWAY, this->prefix) << "You have been marked as being away";
}

ft::irc::message ft::irc::make_reply::whois_user(param_t nickname, param_t username, param_t hostname, param_t info)
{
    return message(RPL_WHOISUSER, this->prefix) << nickname << username << hostname << "*" << info;
}

ft::irc::message ft::irc::make_reply::whois_server(param_t nickname, param_t servername, param_t info)
{
    return message(RPL_WHOISSERVER, this->prefix) << nickname << servername << info;
}

ft::irc::message ft::irc::make_reply::whois_operator(param_t nickname)
{
    return message(RPL_WHOISOPERATOR, this->prefix) << nickname << "is an IRC operator";
}

ft::irc::message ft::irc::make_reply::whois_idle(param_t nickname, long idle_time_second)
{
    return message(RPL_WHOISIDLE, this->prefix) << nickname << idle_time_second << "seconds idle";
}

ft::irc::message ft::irc::make_reply::end_of_whois(param_t nickname)
{
    return message(RPL_ENDOFWHOIS, this->prefix) << nickname << "End of /WHOIS list";
}

ft::irc::message ft::irc::make_reply::whois_channels(param_t nickname, std::vector<std::tuple<std::string, std::string, bool, bool> > infos)
{
    // FIXME: tuple???
    std::ostringstream oss;
    for (auto&& [channelname, is_chanop, is_chanspk] : infos)
    {
        oss << (is_chanop ? "@" : (is_chanspk ? "+" : "")) << channelname << ' ';
    }
    return message(RPL_WHOISCHANNELS, this->prefix) << nickname << oss.str();
}

ft::irc::message ft::irc::make_reply::whowas_user(param_t nickname, param_t username, param_t hostname, param_t info)
{
    return message(RPL_WHOWASUSER, this->prefix) << nickname << username << hostname << "*" << info;
}

ft::irc::message ft::irc::make_reply::end_of_whowas(param_t nickname)
{
    return message(RPL_ENDOFWHOWAS, this->prefix) << nickname << "End of WHOWAS";
}

ft::irc::message ft::irc::make_reply::list_start()
{
    return message(RPL_LISTSTART, this->prefix) << "Channel"
                                                << "Users  Name";
}

ft::irc::message ft::irc::make_reply::list(param_t channelname, int number_of_visible_user, param_t channel_topic)
{
    return message(RPL_LIST, this->prefix) << channelname << number_of_visible_user << channel_topic;
}

ft::irc::message ft::irc::make_reply::list_end()
{
    return message(RPL_LISTEND, this->prefix) << "End of /LIST";
}

ft::irc::message ft::irc::make_reply::channel_mode_is(param_t channelname, param_t channel_mode, param_t member_mode_param)
{
    return message(RPL_CHANNELMODEIS, this->prefix) << channelname << channel_mode << member_mode_param;
}

ft::irc::message ft::irc::make_reply::no_topic(param_t channelname)
{
    return message(RPL_NOTOPIC, this->prefix) << channelname << "No topic is set";
}

ft::irc::message ft::irc::make_reply::topic(param_t channelname, param_t channel_topic)
{
    return message(RPL_TOPIC, this->prefix) << channelname << channel_topic;
}

ft::irc::message ft::irc::make_reply::inviting(param_t channelname, param_t nickname)
{
    return message(RPL_INVITING, this->prefix) << channelname << nickname;
}

ft::irc::message ft::irc::make_reply::summoning(param_t username)
{
    return message(RPL_SUMMONING, this->prefix) << username << "Summoning user to IRC";
}

ft::irc::message ft::irc::make_reply::version(param_t version, int debug_level, param_t servername, param_t comments)
{
    std::ostringstream oss;
    oss << version << "." << debug_level;
    return message(RPL_VERSION, this->prefix) << oss.str() << servername << comments;
}

ft::irc::message ft::irc::make_reply::who_reply(param_t channelname, param_t username, param_t hostname, param_t servername, param_t nickname, bool is_away, bool is_operator, bool is_chanop, bool is_chanspk, int hop_count, param_t info)
{
    std::ostringstream status, oss;
    status << (is_away ? "G" : "H") << (is_operator ? "*" : "") << (is_chanop ? "@" : (is_chanspk ? "+" : ""));
    oss << hop_count << " " << info;
    return message(RPL_WHOREPLY, this->prefix) << channelname << username << hostname << servername << nickname << status.str() << oss.str();
}

ft::irc::message ft::irc::make_reply::end_of_who(param_t name)
{
    return message(RPL_ENDOFWHO, this->prefix) << name << "End of /WHO list";
}

ft::irc::message ft::irc::make_reply::name_reply(param_t channelname, std::vector<std::tuple<std::string, bool, bool> > infos)
{
    // FIXME: tuple???
    std::ostringstream oss;
    for (auto&& [nickname, is_chanop, is_chanspk] : infos)
    {
        oss << nickname << (is_chanop ? "@" : (is_chanspk ? "+" : "")) << ' ';
    }
    return message(RPL_NAMREPLY, this->prefix) << channelname << oss.str();
}

ft::irc::message ft::irc::make_reply::end_of_names(param_t channelname)
{
    return message(RPL_ENDOFNAMES, this->prefix) << channelname << "End of /NAMES list";
}

ft::irc::message ft::irc::make_reply::links(param_t mask, param_t server_name, int hop_count, param_t info)
{
    std::ostringstream oss;
    oss << hop_count << " " << info;
    return message(RPL_LINKS, this->prefix) << mask << server_name << oss.str();
}

ft::irc::message ft::irc::make_reply::end_of_links(param_t mask)
{
    return message(RPL_ENDOFLINKS, this->prefix) << mask << "End of /LINKS list";
}

ft::irc::message ft::irc::make_reply::ban_list(param_t channelname, int ban_id)
{
    return message(RPL_BANLIST, this->prefix) << channelname << ban_id;
}

ft::irc::message ft::irc::make_reply::end_of_ban_list(param_t channelname)
{
    return message(RPL_ENDOFBANLIST, this->prefix) << channelname << "End of channel ban list";
}

ft::irc::message ft::irc::make_reply::info(param_t info)
{
    return message(RPL_INFO, this->prefix) << info;
}

ft::irc::message ft::irc::make_reply::end_of_info()
{
    return message(RPL_ENDOFINFO, this->prefix) << "End of /INFO list";
}

ft::irc::message ft::irc::make_reply::motd_start(param_t servername)
{
    return message(RPL_MOTDSTART, this->prefix) << "- " + servername + " Message of the day - ";
}

ft::irc::message ft::irc::make_reply::motd(param_t text)
{
    return message(RPL_MOTD, this->prefix) << "- " + text;
}

ft::irc::message ft::irc::make_reply::end_of_motd()
{
    return message(RPL_ENDOFMOTD, this->prefix) << "End of /MOTD command";
}

ft::irc::message ft::irc::make_reply::now_operator()
{
    return message(RPL_YOUREOPER, this->prefix) << "You are now an IRC operator";
}

ft::irc::message ft::irc::make_reply::rehashing(param_t config_filename)
{
    return message(RPL_REHASHING, this->prefix) << config_filename << "Rehashing";
}

ft::irc::message ft::irc::make_reply::time(param_t servername, param_t local_time_str)
{
    return message(RPL_TIME, this->prefix) << servername << local_time_str;
}

ft::irc::message ft::irc::make_reply::users_start()
{
    return message(RPL_USERSSTART, this->prefix) << "UserID   Terminal  Host";
}

ft::irc::message ft::irc::make_reply::users(param_t utmp_name, param_t utmp_line, param_t utmp_host)
{
    // NOTE: utmp.h
    std::ostringstream oss;
    oss << std::left << std::setw(8) << utmp_name << ' ' // "%-8s "
        << std::left << std::setw(9) << utmp_line << ' ' // "%-9s "
        << std::left << std::setw(8) << utmp_host;       // "%-8s"
    return message(RPL_USERS, this->prefix) << oss.str();
}

ft::irc::message ft::irc::make_reply::end_of_users()
{
    return message(RPL_ENDOFUSERS, this->prefix) << "End of users";
}

ft::irc::message ft::irc::make_reply::no_users()
{
    return message(RPL_NOUSERS, this->prefix) << "Nobody logged in";
}

ft::irc::message ft::irc::make_reply::trace_link(param_t version, int debug_level, param_t destination_servername, param_t next_servername)
{
    std::ostringstream oss;
    oss << version << debug_level;
    return message(RPL_TRACELINK, this->prefix) << "Link" << oss.str() << destination_servername << next_servername;
}

ft::irc::message ft::irc::make_reply::trace_connecting(int client_class, param_t client_name)
{
    return message(RPL_TRACECONNECTING, this->prefix) << "Try." << client_class << client_name;
}

ft::irc::message ft::irc::make_reply::trace_handshake(int client_class, param_t client_name)
{
    return message(RPL_TRACEHANDSHAKE, this->prefix) << "H.S." << client_class << client_name;
}

ft::irc::message ft::irc::make_reply::trace_unknown(int client_class, param_t client_name)
{
    return message(RPL_TRACEUNKNOWN, this->prefix) << "????" << client_class << client_name;
}

ft::irc::message ft::irc::make_reply::trace_operator(int client_class, param_t client_name)
{
    return message(RPL_TRACEOPERATOR, this->prefix) << "Oper" << client_class << client_name;
}

ft::irc::message ft::irc::make_reply::trace_user(int client_class, param_t client_name)
{
    return message(RPL_TRACEUSER, this->prefix) << "User" << client_class << client_name;
}

ft::irc::message ft::irc::make_reply::trace_server(int client_class, int number_of_server, int number_of_client, param_t servername, param_t uri)
{
    // uri: %s!%s@%s
    std::ostringstream oss_s, oss_c;
    oss_s << number_of_server << 'S';
    oss_c << number_of_client << 'C';
    return message(RPL_TRACESERVER, this->prefix) << "Serv" << client_class << oss_s.str() << oss_c.str() << servername << uri;
}

ft::irc::message ft::irc::make_reply::trace_new_type(param_t new_type, param_t client_name)
{
    return message(RPL_TRACENEWTYPE, this->prefix) << new_type << "0" << client_name;
}

ft::irc::message ft::irc::make_reply::trace_log(param_t log_filename, int debug_level)
{
    return message(RPL_TRACELOG, this->prefix) << "File" << log_filename << debug_level;
}

ft::irc::message ft::irc::make_reply::stats_link_info(param_t linkname, int sendq, param_t sent_messages, int sent_bytes, param_t received_messages, int received_bytes, int time_open)
{
    return message(RPL_STATSLINKINFO, this->prefix) << linkname << sendq << sent_messages << sent_bytes << received_messages << received_bytes << time_open;
}

ft::irc::message ft::irc::make_reply::stats_commands(param_t command, unsigned count)
{
    return message(RPL_STATSCOMMANDS, this->prefix) << command << count;
}

ft::irc::message ft::irc::make_reply::stats_c_line(param_t host, param_t pass, param_t name, int port, int conf_class)
{
    return message(RPL_STATSCLINE, this->prefix) << 'C' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
}

ft::irc::message ft::irc::make_reply::stats_n_line(param_t host, param_t pass, param_t name, int port, int conf_class)
{
    return message(RPL_STATSNLINE, this->prefix) << 'N' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
}

ft::irc::message ft::irc::make_reply::stats_i_line(param_t host, param_t pass, param_t name, int port, int conf_class)
{
    return message(RPL_STATSILINE, this->prefix) << 'I' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
}

ft::irc::message ft::irc::make_reply::stats_k_line(param_t host, param_t pass, param_t name, int port, int conf_class)
{
    return message(RPL_STATSKLINE, this->prefix) << 'K' << host << (!pass.empty() ? "*" : "<NULL>") << name << port << conf_class;
}

ft::irc::message ft::irc::make_reply::stats_y_line(int class_class, int ping_freq, int conn_freq, int max_sendq)
{
    return message(RPL_STATSYLINE, this->prefix) << 'Y' << class_class << ping_freq << conn_freq << max_sendq;
}

ft::irc::message ft::irc::make_reply::end_of_stats(char stats_letter)
{
    return message(RPL_ENDOFSTATS, this->prefix) << stats_letter << "End of /STATS report";
}

ft::irc::message ft::irc::make_reply::stats_l_line(param_t host, param_t pass, param_t name, int max_depth)
{
    return message(RPL_STATSLLINE, this->prefix) << 'L' << host << (!pass.empty() ? "*" : "<NULL>") << name << max_depth;
}

ft::irc::message ft::irc::make_reply::stats_uptime(int days, int hours, int minutes, int seconds)
{
    std::ostringstream oss;
    oss << "Server Up " << days << " days "
        << hours << ":"
        << std::setfill('0') << std::setw(2) << std::dec << minutes << ":" // "%02d:"
        << std::setfill('0') << std::setw(2) << std::dec << seconds;       // "%02d"
    return message(RPL_STATSUPTIME, this->prefix) << oss.str();
}

ft::irc::message ft::irc::make_reply::stats_o_line(param_t host, param_t pass, param_t name)
{
    return message(RPL_STATSOLINE, this->prefix) << 'O' << host << (!pass.empty() ? "*" : "<NULL>") << name;
}

ft::irc::message ft::irc::make_reply::stats_h_line(param_t host, param_t pass, param_t name)
{
    return message(RPL_STATSHLINE, this->prefix) << 'H' << host << (!pass.empty() ? "*" : "<NULL>") << name;
}

ft::irc::message ft::irc::make_reply::user_mode_is(param_t user_mode)
{
    return message(RPL_UMODEIS, this->prefix) << user_mode;
}

ft::irc::message ft::irc::make_reply::luser_client(int user_count, int invisible_count, int server_count)
{
    std::ostringstream oss;
    oss << "There are " << user_count << " users and " << invisible_count << " invisible on " << server_count << " servers";
    return message(RPL_LUSERCLIENT, this->prefix) << oss.str();
}

ft::irc::message ft::irc::make_reply::luser_operator(int op_count)
{
    return message(RPL_LUSEROP, this->prefix) << op_count << "operator(s) online";
}

ft::irc::message ft::irc::make_reply::luser_unknown(int unknown_count)
{
    return message(RPL_LUSERUNKNOWN, this->prefix) << unknown_count << "unknown connection(s)";
}

ft::irc::message ft::irc::make_reply::luser_channels(int channel_count)
{
    return message(RPL_LUSERCHANNELS, this->prefix) << channel_count << "channels formed";
}

ft::irc::message ft::irc::make_reply::luser_me(int client_count, int server_count)
{
    std::ostringstream oss;
    oss << "I have " << client_count << " clients and " << server_count << " servers";
    return message(RPL_LUSERME, this->prefix) << oss.str();
}

ft::irc::message ft::irc::make_reply::admin_me(param_t servername)
{
    return message(RPL_ADMINME, this->prefix) << servername << ":Administrative info";
}

ft::irc::message ft::irc::make_reply::admin_loc1(param_t location)
{
    return message(RPL_ADMINLOC1, this->prefix) << location;
}

ft::irc::message ft::irc::make_reply::admin_loc2(param_t location_detail)
{
    return message(RPL_ADMINLOC2, this->prefix) << location_detail;
}

ft::irc::message ft::irc::make_reply::admin_email(param_t email_address)
{
    return message(RPL_ADMINEMAIL, this->prefix) << email_address;
}
