/* *****************************************************************************
 * The lines 65, 118, 134 and 142 are based on Xitari's code, from Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and 
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details. 
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 */
#include "Alien.hpp"

#include "../RomUtils.hpp"


AlienSettings::AlienSettings() {
    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
    m_lives    = 3;
}


/* create a new instance of the rom */
RomSettings* AlienSettings::clone() const { 
    
    RomSettings* rval = new AlienSettings();
    *rval = *this;
    return rval;
}


/* process the latest information from ALE */
void AlienSettings::step(const System& system) {

    // update the reward
    int b1 = getDigit(system, 0x8B);
    int b2 = getDigit(system, 0x89); 
    int b3 = getDigit(system, 0x87); 
    int b4 = getDigit(system, 0x85); 
    int b5 = getDigit(system, 0x83); 
    reward_t score = b1 + b2 * 10 + b3 * 100 + b4 * 1000 + b5 * 10000;
    score *= 10;
    m_reward = score - m_score;
    m_score = score;

    // update terminal status
    int byte = readRam(&system, 0xC0);
    byte = byte & 15;
    m_terminal = byte <= 0;
    m_lives = byte;
}


/* is end of game */
bool AlienSettings::isTerminal() const {

    return m_terminal;
};


/* get the most recently observed reward */
reward_t AlienSettings::getReward() const { 

    return m_reward; 
}


/* is an action part of the minimal set? */
bool AlienSettings::isMinimal(const Action &a) const {

    switch (a) {
        case PLAYER_A_NOOP:
        case PLAYER_A_FIRE:
        case PLAYER_A_UP:
        case PLAYER_A_RIGHT:
        case PLAYER_A_LEFT:
        case PLAYER_A_DOWN:
        case PLAYER_A_UPRIGHT:
        case PLAYER_A_UPLEFT:
        case PLAYER_A_DOWNRIGHT:
        case PLAYER_A_DOWNLEFT:
        case PLAYER_A_UPFIRE:
        case PLAYER_A_RIGHTFIRE:
        case PLAYER_A_LEFTFIRE:
        case PLAYER_A_DOWNFIRE:
        case PLAYER_A_UPRIGHTFIRE:
        case PLAYER_A_UPLEFTFIRE:
        case PLAYER_A_DOWNRIGHTFIRE:
        case PLAYER_A_DOWNLEFTFIRE:
            return true;
        default:
            return false;
    }   
}


/* reset the state of the game */
void AlienSettings::reset(System& system, StellaEnvironment& environment) {
    
    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
    m_lives = 4;
    writeRam(&system,0x81,m_mode);
}


/* special code to read digits for Alien */
int AlienSettings::getDigit(const System& system, int address) const {
  
    int byte = readRam(&system, address);
    return byte == 0x80 ? 0 : byte >> 3;
}
        
/* saves the state of the rom settings */
void AlienSettings::saveState(Serializer & ser) {
  ser.putInt(m_reward);
  ser.putInt(m_score);
  ser.putBool(m_terminal);
  ser.putInt(m_lives);
}

// loads the state of the rom settings
void AlienSettings::loadState(Deserializer & ser) {
  m_reward = ser.getInt();
  m_score = ser.getInt();
  m_terminal = ser.getBool();
  m_lives = ser.getInt();
}


//Returns a list of mode that the game can be played in.
ModeVect AlienSettings::getAvailableModes(){
    ModeVect modes(4);
    for(unsigned i=0;i<4;i++){
        modes[i]=i;
    }
    return modes;
}

//Set the mode of the game. The given mode must be one returned by the previous function. 
void AlienSettings::setMode(mode_t m,System &system, StellaEnvironment& environment){
    if(m>=0 && m<4){
        m_mode = m;
        //write the new mode in ram
        writeRam(&system,0x81,m);
        //reset the environment to apply changes.
        environment.soft_reset();
    }else{
        throw std::runtime_error("This mode doesn't currently exist for this game");
    }

}
