/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:11 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/15 03:48:46 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Client.hpp"
#include <string>
#include <map>

class   Server;
class   Client;

class  Command
{
    private:
        Command();
        Command(const Command &other);
	    Command &operator=(const Command &other);
        static  bool handlePing(Server &server, Client &client, const IrcMsg &msg);
        static  bool handleQuit(Server &server, Client &client, const IrcMsg &msg);
        static  void sendUnknownCommand(Server &server, Client &client, const IrcMsg &msg);
        static  bool handlePass(Server &server, Client &client, const IrcMsg &msg);
        static  bool handleNick(Server &server, Client &client, const IrcMsg &msg);
        static  bool handleUser(Server &server, Client &client, const IrcMsg &msg);
        static  void tryRegister(Server &server, Client &client);
        static  bool isNicknameUsed(Server &server, Client &client,const std::string &nickname);
    public:
        static  bool execute(Server &server, Client &client, const IrcMsg &msg);



};