/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 05:04:29 by mshariar          #+#    #+#             */
/*   Updated: 2026/08/20 05:07:06 by mshariar         ###   ########.fr       */
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

int main(int argc, char **argv)
{

    
    return 0;
}