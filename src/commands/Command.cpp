/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 01:40:00 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/15 02:38:26 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "Client.hpp"

Command::Command() {}

Command::Command(const Command &other)
{
	(void)other;
}

Command &Command::operator=(const Command &other)
{
	(void)other;
	return (*this);
}

bool    Command::execute(Server &server, Client &client, const IrcMsg &msg)
{
    if(msg.command.empty())
        return true;
    if (msg.command == "PING")
        return (Command::handlePing(server, client, msg));
    if (msg.command == "QUIT")
        return (Command::handleQuit(server, client, msg));
    Command::sendUknownCommand(server, client, msg);
    return true;
}

bool Command::handlePing(Server &server, Client &client, const IrcMsg &msg)
{
    if (msg.params.empty())
        server.queueMessage(client.getFd(), ":ircserv 409 * :No origin specified");
    else
        server.queueMessage(client.getFd(), "PONG :" + msg.params[0]);
    return true;
}

bool Command::handleQuit(Server &server, Client &client,
		const IrcMessage &msg)
{
	(void)server;
	(void)client;
	(void)msg;
	return (false);
}

 void Command::sendUknownCommand(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    server.queueMessage(client.getFd(), ":ircserv 421 " + user + " " + msg.command + " :unknown command");
    
}