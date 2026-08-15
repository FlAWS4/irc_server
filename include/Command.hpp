/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:11 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/15 01:59:26 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Parser.hpp"
#include "Client.hpp"

class   Server;
class   Client;

class  Command
{
    private:
        command();
        Command(const Command &other);
	    Command &operator=(const Command &other);
        static  bool handlePing(Server &server, Client &client, const IrcMsg &msg);
        static  bool handleQuit(Server &server, Client &client, const IrcMsg &msg);
        static  void sendUknownCommand(Server &server, Client &client, const IrcMsg &msg);
    public:
        static  bool execute(Server &server, Client &client, const IrcMsg &msg);



};