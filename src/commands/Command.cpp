/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 01:40:00 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/15 03:50:51 by mshariar         ###   ########.fr       */
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
    if (msg.command == "PASS")
        return (Command::handlePass(server, client, msg));
    if (msg.command == "NICK")
        return (Command::handleNick(server, client, msg));
    if (msg.command == "USER")
        return (Command::handleUser(server, client, msg));
    Command::sendUnknownCommand(server, client, msg);
    return true;
}

bool Command::handlePass(Server &server, Client &client, const IrcMsg &msg)
{
    if (client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 462 " + client.getNickname() + " :already registered" );
        return (true);
    }
    if (msg.params.empty())
    {
        server.queueMessage(client.getFd(), ":ircserv 461 * PASS :Not enough parameters");
        return true;
    }
    if (msg.params[0] != server.getPassword())
    {
        server.queueMessage(client.getFd(), ":ircserv 464 * :Password incorrect");
        return true;
    }
    client.setPasswordAccepted(true);
    Command::tryRegister(server, client);
    return (true); 
    
}

bool Command::handleNick(Server &server, Client &client, const IrcMsg &msg)
{
	if (msg.params.empty())
	{
		server.queueMessage(client.getFd(),
			":ircserv 431 * :No nickname given");
		return (true);
	}
	if (Command::isNicknameUsed(server, client, msg.params[0]))
	{
		server.queueMessage(client.getFd(),
			":ircserv 433 * " + msg.params[0] + " :Nickname is already in use");
		return (true);
	}
	client.setNickname(msg.params[0]);
	Command::tryRegister(server, client);
	return (true);
}

bool Command::handleUser(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;
    
    user = client.getNickname();
    if (user.empty())
        user = "*";
    if (client.isRegistered())
    {
        server.queueMessage(client.getFd(), ":ircserv 462 " + client.getNickname() + " :already registered" );
        return (true);
    }
    if (msg.params.size() < 4)
    {
        server.queueMessage(client.getFd(), ":ircserv 461 " + user + "USER :Not enough parameters");
        return true;
    }
    client.setUsername(msg.params[0]);
    client.setRealname(msg.params[3]);
    Command::tryRegister(server, client);
    return(true);
}

void Command::tryRegister(Server &server, Client &client)
{
	if (client.isRegistered())
		return ;
	if (!client.isPasswordAccepted())
		return ;
	if (client.getNickname().empty())
		return ;
	if (client.getUsername().empty())
		return ;
	client.setRegistered(true);
	server.queueMessage(client.getFd(), ":ircserv 001 " + client.getNickname() + " :Welcome to ft_irc, " + client.getNickname());
}

bool Command::isNicknameUsed(Server &server, Client &client, const std::string &nickname)
{
    std::map<int, Client> &clients = server.getClients();

    for(std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.getFd() != client.getFd() && it->second.getNickname() == nickname)
        return true;
    }
    return(false);
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
		const IrcMsg &msg)
{
	(void)server;
	(void)client;
	(void)msg;
	return (false);
}

 void Command::sendUnknownCommand(Server &server, Client &client, const IrcMsg &msg)
{
    std::string user;

    user = client.getNickname();
    if (user.empty())
        user = "*";
    server.queueMessage(client.getFd(), ":ircserv 421 " + user + " " + msg.command + " :Unknown command");
    
}