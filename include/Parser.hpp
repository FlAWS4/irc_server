/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchowdhu <hchowdhu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 00:11:18 by hchowdhu          #+#    #+#             */
/*   Updated: 2026/07/13 00:11:19 by hchowdhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

struct IrcMsg
{
    std::string     prefix;
    std::string     command;
    std::vector<std::string> params;
};

class Parser
{
    private:
        Parser();
        parser(const parser &other);
        parser &operator=(const parser &other);
        
        static  std::string toUpper(const std::string &value);

        public:
            static  IrcMsg  parse(const std::string &line);
};

