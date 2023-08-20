/*
 * European Train Control System
 * Copyright (C) 2019-2023  César Benito <cesarbema2009@hotmail.com>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "terminal.h"
#include "platform_runtime.h"
mobile_terminal *mobile_terminals[2] = {new mobile_terminal(), new mobile_terminal()};

std::map<int, std::string> RadioNetworkNames;
optional<std::vector<int>> AllowedRadioNetworks;
int RadioNetworkId = 0;

std::string radio_network_name(int id)
{
    auto it = RadioNetworkNames.find(id);
    if (it != RadioNetworkNames.end())
        return it->second;
    return std::to_string(id/100)+"-"+std::to_string(id%100);
}

mobile_terminal::mobile_terminal()
{
    network_id = -1;
    registered = mobile_data_available = csd_available = false;
}

void mobile_terminal::update()
{
    if (cs_connection && cs_connection->expired())
        cs_connection = {};
    for (auto it=ps_connections.begin(); it != ps_connections.end();) {
        if (it->expired())
            it = ps_connections.erase(it);
        else
            ++it;
    }

    if (!registered && last_register_order && get_milliseconds() - *last_register_order > 4000)
        registered = true;
    if (registered && last_register_order)
        last_register_order = {};

    csd_available = mobile_data_available = registered;
        
    if (RadioNetworkId != network_id && !cs_connection && ps_connections.empty()) {
        network_register(RadioNetworkId);
    }
}

void mobile_terminal::network_register(int id)
{
    network_id = id;
    registered = false;
    if (!last_register_order)
        last_register_order = get_milliseconds();
}

void retrieve_radio_networks()
{
    RadioNetworkNames[0] = "GSMR";
    AllowedRadioNetworks = {0,10001};
}
