/*
 * European Train Control System
 * Copyright (C) 2019-2023  César Benito <cesarbema2009@hotmail.com>
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "volume.h"
#include "../tcp/server.h"
#include <iostream>
#include <cstdlib>
static int current_volume = 50;
void set_volume(int percent)
{
    platform->set_volume(percent);
    current_volume = percent;
}
int get_volume()
{
    return current_volume;
}
volume_window::volume_window() : input_window("Volume", 1, false)
{
    int vol_orig = get_volume();
    inputs[0] = new input_data("", false);
    inputs[0]->data = std::to_string(vol_orig);
    inputs[0]->setAccepted(true);
    std::vector<Button*> keys;
    for (int i=0; i<12; i++) {
        keys.push_back(nullptr);
    }
    keys[0] = new TextButton(get_text("-"), 102, 50);
    keys[2] = new TextButton(get_text("+"), 102, 50);
    keys[0]->setPressedAction([this]
    {
        int vol = stoi(inputs[0]->data);
        if (vol >= 30) {
            vol -= 10;
            set_volume(vol);
            inputs[0]->setData(std::to_string(vol));
        }
    });
    keys[2]->setPressedAction([this]
    {
        int vol = stoi(inputs[0]->data);
        if (vol <= 90) {
            vol += 10;
            set_volume(vol);
            inputs[0]->setData(std::to_string(vol));
        }
    });
    keys[0]->upType = false;
    keys[2]->upType = false;
    inputs[0]->keys = keys;
    exit_button.setPressedAction([this, vol_orig]
    {
        set_volume(vol_orig);
        write_command("json",R"({"DriverSelection":"CloseWindow"})");
    });
    create();
}
void volume_window::setLayout()
{
    input_window::setLayout();
}
void volume_window::sendInformation()
{
    set_volume(stoi(inputs[0]->data_accepted));
    input_window::sendInformation();
}