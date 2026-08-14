/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:11 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/07/13 00:11:13 by hchowdhu         ###   ########.fr       */
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
        static  bool handlePing(Server &server, Client &Client, const IrcMsg &msg);
        static  bool handleQuit(Server &server, Client &Client, const IrcMsg &msg);
        static  bool sendUknownCommand(Server &server, Client &Client, const IrcMsg &msg);
    public:
        static  bool execute(Server &server, Client &Client, const IrcMsg &msg);



};