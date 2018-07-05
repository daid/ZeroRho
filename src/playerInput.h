#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include <sp2/io/keybinding.h>

class PlayerInput
{
public:
    PlayerInput(int index);
    
    sp::io::Keybinding left;
    sp::io::Keybinding right;
    sp::io::Keybinding up;
    sp::io::Keybinding down;
    sp::io::Keybinding jump;
    sp::io::Keybinding attack;
    sp::io::Keybinding extra1;
    sp::io::Keybinding extra2;
    sp::io::Keybinding extra3;
    sp::io::Keybinding extra4;
    sp::io::Keybinding start;
    
    static PlayerInput left_controller;
    static PlayerInput right_controller;
};

#endif//PLAYER_INPUT_H
