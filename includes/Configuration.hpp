/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: waboutzo <waboutzo@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 15:38:53 by waboutzo          #+#    #+#             */
/*   Updated: 2023/05/23 22:05:28 by waboutzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "server.hpp"

class Configuration
{
    // delait the duplicated servers that do same HOST:PORT
    private:
        std::vector<Route> _routes;
    public:
        Configuration(std::string tokens);
        std::vector<Route> getRoutes() const;
        friend std::ostream& operator<<(std::ostream& o, Configuration obj);
};