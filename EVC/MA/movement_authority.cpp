/*
 * European Train Control System
 * Copyright (C) 2019  César Benito <cesarbema2009@hotmail.com>
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

#include "movement_authority.h"
#include "../Supervision/supervision.h"
double getScale(int val, int q_scale)
{
    return val * (q_scale ? (q_scale == 2 ? 100 : 10) : 1);
}
/*movement_authority::movement_authority(parsed_packet p)
{
    p.read("NID_PACKET");
    p.read("Q_DIR");
    p.read("L_PACKET");
    double scale = p.read("Q_SCALE");
    v_main = p.read("V_MAIN");
    v_ema = p.read("V_EMA");
    int iter = p.read("N_ITER");
    for(int i=0; i<iter; i++)
    {
        ma_section s;
        s.length = getScale(p.read("L_SECTION"), scale);
        if(p.read("Q_SECTIONTIMER"))
        {
            s.stimer = new section_timer(p.read("T_SECTIONTIMER"), getScale(p.read("D_SECTIONTIMERSTOPLOC"),scale));
        }
        sections.push_back(s);
    }
    endsection.length = getScale(p.read("L_ENDSECTION"), scale);
    if(p.read("Q_SECTIONTIMER"))
    {
        endsection.stimer = new section_timer(p.read("T_SECTIONTIMER"), getScale(p.read("D_SECTIONTIMERSTOPLOC"),scale));
    }
    if(p.read("Q_DANGERPOINT"))
    {
        dp = new danger_point({getScale(p.read("D_DP"),scale), p.read("V_RELEASEDP")});
    }
    if(p.read("Q_OVERLAP"))
    {
        ol = new overlap({getScale(p.read("D_STARTOL"),scale), p.read("T_OL"), getScale(p.read("D_OL"),scale), p.read("V_RELEASEOL")});
    }
}*/

movement_authority::~movement_authority()
{

}
movement_authority *MA;
speed_restriction *LoA;
speed_restriction *MA_speed;
void set_MA(movement_authority *ma)
{
    if (MA != nullptr) {
        mrsp_candidates.remove_restriction(MA_speed);
        delete MA_speed;
    }
    MA = ma;
    if (MA->v_ema == 0) {
        EoA = new distance(MA->end);
        if (MA->ol != nullptr) {
            SvL = new distance(MA->end+MA->ol->distance);
            V_releaseSvL = MA->ol->vrelease;
        } else if (MA->dp != nullptr) {
            SvL = new distance(MA->end+MA->dp->distance);
            V_releaseSvL = MA->dp->vrelease;
        } else {
            SvL = new distance(MA->end);
            V_releaseSvL = 0;
        }
    } else {
        EoA = SvL = nullptr;
        //LoA = new speed_profile(MA->v_ema, MA->end, TODO);
    }
    MA_speed = new speed_restriction(MA->v_main, MA->start, MA->end, false);
    mrsp_candidates.insert_restriction(MA_speed);
    if (MA->v_ema == 0)
        V_release = calculate_V_release();
}
