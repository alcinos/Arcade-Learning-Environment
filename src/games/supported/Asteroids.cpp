/* *****************************************************************************
 * The lines 42 - 44, 92, 102 and 110 are based on Xitari's code, from Google Inc.
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
#include "Asteroids.hpp"

#include "../RomUtils.hpp"


AsteroidsSettings::AsteroidsSettings() {
    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
    m_lives    = 4;
}


/* create a new instance of the rom */
RomSettings* AsteroidsSettings::clone() const { 
    
    RomSettings* rval = new AsteroidsSettings();
    *rval = *this;
    return rval;
}


/* process the latest information from ALE */
void AsteroidsSettings::step(const System& system) {

    // update the reward
    reward_t score = getDecimalScore(62, 61, &system);
    score *= 10;
    m_reward = score - m_score;
    m_score = score;

    // update terminal status
    int byte = readRam(&system, 0x3C);
    m_lives = (byte - (byte & 15)) >> 4;
    m_terminal = (m_lives == 0);
}


/* is end of game */
bool AsteroidsSettings::isTerminal() const {

    return m_terminal;
};


/* get the most recently observed reward */
reward_t AsteroidsSettings::getReward() const { 

    return m_reward; 
}


/* is an action part of the minimal set? */
bool AsteroidsSettings::isMinimal(const Action &a) const {
    switch (a) {
        case PLAYER_A_NOOP:
        case PLAYER_A_FIRE:
        case PLAYER_A_UP:
        case PLAYER_A_RIGHT:
        case PLAYER_A_LEFT:
        case PLAYER_A_DOWN:
        case PLAYER_A_UPRIGHT:
        case PLAYER_A_UPLEFT:
        case PLAYER_A_UPFIRE:
        case PLAYER_A_RIGHTFIRE:
        case PLAYER_A_LEFTFIRE:
        case PLAYER_A_DOWNFIRE:
        case PLAYER_A_UPRIGHTFIRE:
        case PLAYER_A_UPLEFTFIRE:
            return true;
        default:
            return false;
    }
}


/* reset the state of the game */
void AsteroidsSettings::reset(System& system, StellaEnvironment& environment) {
    
    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
    m_lives    = 4;
    setMode(m_mode,system,environment);
}


        
/* saves the state of the rom settings */
void AsteroidsSettings::saveState(Serializer & ser) {
  ser.putInt(m_reward);
  ser.putInt(m_score);
  ser.putBool(m_terminal);
  ser.putInt(m_lives);
}

// loads the state of the rom settings
void AsteroidsSettings::loadState(Deserializer & ser) {
  m_reward = ser.getInt();
  m_score = ser.getInt();
  m_terminal = ser.getBool();
  m_lives = ser.getInt();
}

//Returns a list of mode that the game can be played in.
ModeVect AsteroidsSettings::getAvailableModes(){
    ModeVect modes(32);
    for(unsigned i=0;i<32;i++){
        modes[i]=i;
    }
    modes.push_back(128);//this is the "kid" mode
    return modes;
}

//Set the mode of the game. The given mode must be one returned by the previous function. 
void AsteroidsSettings::setMode(mode_t m,System &system, StellaEnvironment& environment){
    if(m>=0 && (m<32||m==128)){
        m_mode = m;
        //Read the mode we are currently in
        unsigned char mode = readRam(&system,0);
        //press select until the correct mode is reached
        while(mode!=m_mode){
            environment.pressSelect(2);
            mode = readRam(&system,0);
        }
        //reset the environment to apply changes.
        environment.soft_reset();
    }else{
        throw std::runtime_error("This mode doesn't currently exist for this game");
    }

}
