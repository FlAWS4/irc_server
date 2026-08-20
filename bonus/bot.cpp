/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 05:04:29 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/20 05:28:22 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>


static std::string intTostr(int n)
{
    std::stringstream ss;

    ss << n;
    return (ss.str());
}

void    sendLine(int fd, const std::string &line)
{
    std::string msg = line + "\r\n"
    send(fd, msg.c_str(), msg.size(), 0);
}

int connectBot(const std::string &host, int port)
{
    int fd;
    struct sockaddr_in addr;
    
    
}

std::string getNIck(const std::string &line)
{
    int end;
    if (line.empty(), || line[0] != ' ')
        return ("Unknown");
    end = line.find('!');
    if(end == std::string::npos)
        end = line.find(' ');
    if (end == std::string::npos)
        return("Unknown");
    return(line.substr(1, end - 1));
    
}

void    handleLine(int, fd, const std::string &botNick, const std::string &line)
{
    int msgPos;
    int targetStart;
    int textStart;
    std::string sender;
    std::string text;
    std::string target;

    std::cout << line << std::endl;

    if (line.substr(0, 4) == "PING" )
    {
        sendLine(fd, "PONG" + line.substr(5));
        return ;
    }
}

int main(int argc, char **argv)
{
	int fd;
	int port;
	std::string botNick;

	if (argc != 5)
	{
		std::cerr << "Usage: ./ircbot <host> <port> <password> <channel>" << std::endl;
		return (1);
	}

	signal(SIGPIPE, SIG_IGN);
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	port = std::atoi(argv[2]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Bot: invalid port" << std::endl;
		return (1);
	}

	botNick = "ftbot";
	fd = connectBot(argv[1], port);
	if (fd == -1)
	{
		std::cerr << "Bot: connection failed" << std::endl;
		return (1);
	}

	sendLine(fd, "PASS " + std::string(argv[3]));
	sendLine(fd, "NICK " + botNick);
	sendLine(fd, "USER ftbot 0 * :ftbot");
	sendLine(fd, "JOIN " + std::string(argv[4]));
	sendLine(fd, "PRIVMSG " + std::string(argv[4]) + " :ftbot online. Try !help");

	readServer(fd, botNick);
	close(fd);
	return (0);
}