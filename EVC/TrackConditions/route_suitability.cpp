/*
 * European Train Control System
 * Copyright (C) 2023  César Benito <cesarbema2009@hotmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "route_suitability.h"
#include "../optional.h"
#include "../MA/movement_authority.h"
#include "../Supervision/train_data.h"
#include "../DMI/text_message.h"
#include "../language/language.h"
optional<distance> restore_route_suitability;
std::map<int, distance> route_suitability;
void load_route_suitability(RouteSuitabilityData &data, distance ref)
{
    if (data.Q_TRACKINIT == Q_TRACKINIT_t::InitialState) {
        restore_route_suitability = ref + data.D_TRACKINIT.get_value(data.Q_SCALE);
        return;
    }
    restore_route_suitability = {};
    route_suitability.clear();
    std::vector<RouteSuitability_element> elements;
    elements.push_back(data.element);
    elements.insert(elements.end(), data.elements.begin(), data.elements.end());
    for (auto &el : elements) {
        ref += el.D_SUITABILITY.get_value(data.Q_SCALE);
        switch (el.Q_SUITABILITY.rawdata) {
            case Q_SUITABILITY_t::LoadingGauge:
                break;
            case Q_SUITABILITY_t::MaxAxleLoad:
                break;
            case Q_SUITABILITY_t::TractionSystem:
                break;
        }
    }
    for (auto &rs : route_suitability) {
        int type = rs.first;
        std::string text;
        switch (type) {
            case 0:
                text = get_text("Route suitability - loading gauge");
                break;
            case 1:
                text = get_text("Route suitability - axle load category");
                break;
            case 2:
                text = get_text("Route suitability - traction system");
                break;
        }
        add_message(text_message(text, true, false, 0, [type](text_message &m) {
            return route_suitability.find(type) == route_suitability.end();
        }));
    }
    calculate_SvL();
}
void update_route_suitability()
{
    if (restore_route_suitability && *restore_route_suitability<d_minsafefront(*restore_route_suitability)) {
        restore_route_suitability = {};
        route_suitability.clear();
        calculate_SvL();
    }
}