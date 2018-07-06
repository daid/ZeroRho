#ifndef PLAYER_H
#define PLAYER_H

#include <sp2/scene/node.h>
#include <sp2/scene/particleEmitter.h>

class PlayerInput;
class PlayerShip : public sp::Node
{
public:
    PlayerShip(PlayerInput& controller);
    
    virtual void onFixedUpdate() override;

    virtual void onCollision(sp::CollisionInfo& info) override;
    
    static constexpr float forward_velocity = 100;
    static constexpr float drop_in_time = 1.5;
private:
    void explosion();

    PlayerInput& controller;
    sp::P<sp::Node> ship;
    sp::P<sp::ParticleEmitter> smoke_generator, fire_generator;
    double time_delta = 0.0;
    
    int health = 3;
    int invulnerability = 0;
};

#endif//PLAYER_H
