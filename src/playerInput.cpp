#include "playerInput.h"

PlayerInput PlayerInput::left_controller{0};
PlayerInput PlayerInput::right_controller{1};


PlayerInput::PlayerInput(int index)
: left(sp::string(index) + "_left", "Left")
, right(sp::string(index) + "_right", "Right")
, up(sp::string(index) + "_up", "Up")
, down(sp::string(index) + "_down", "Down")
, jump(sp::string(index) + "_jump", "Space")
, attack(sp::string(index) + "_attack", "Z")
, extra1(sp::string(index) + "_extra1", "X")
, extra2(sp::string(index) + "_extra2", "C")
, extra3(sp::string(index) + "_extra3", "V")
, extra4(sp::string(index) + "_extra4", "B")
, start(sp::string(index) + "_start", "Num1")
{
    if (index == 1)
    {
        left.setKey("A");
        right.setKey("D");
        up.setKey("W");
        down.setKey("S");
        jump.setKey("Q");
        attack.setKey("E");
        extra1.setKey("R");
        extra2.setKey("F");
        extra3.setKey("T");
        extra4.setKey("G");
        start.setKey("Num2");
    }
}
