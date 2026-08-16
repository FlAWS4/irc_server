/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshariar <mshariar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:01 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/08/16 02:36:27 by mshariar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

class Client;

class Channel
{
    private:
        std::string     _name;
        std::vector<Client *>   _clients;
        std::vector<Client *>   _operators;
        Channel();
        
    public:
        Channel(const std::string &name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		~Channel();

        const   std::string &getName() const;
        size_t  getClientCount() const;
        bool    hasClient(Client  *client) const;
        bool    isOperator(Client  *client) const;
        void    addClient(Client  *client);
        void    addOperator(Client  *client);
        void broadcast(Client *sender, const std::string &msg);
};