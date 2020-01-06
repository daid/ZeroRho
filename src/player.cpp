#include "player.h"
#include "playerInput.h"

#include <sp2/engine.h>
#include <sp2/tween.h>
#include <sp2/random.h>
#include <sp2/scene/scene.h>
#include <sp2/graphics/mesh/obj.h>
#include <sp2/collision/2d/circle.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/audio/sound.h>
#include <sp2/audio/music.h>


PlayerShip::PlayerShip(PlayerInput& controller)
: sp::Node(sp::Scene::get("MAIN")->getRoot())
, controller(controller)
{
    ship = new sp::Node(this);
    
    ship->render_data.shader = sp::Shader::get("internal:basic_shaded.shader");
    ship->render_data.type = sp::RenderData::Type::Normal;
    ship->render_data.mesh = sp::obj_loader.get("models/spaceCraft3.obj");
    ship->render_data.texture = sp::obj_loader.getTextureFor("models/spaceCraft3.obj");

    ship->render_data.scale = sp::Vector3f(0.1, 0.1, 0.1);
    ship->setRotation(180);

    sp::P<sp::Node> shadow = new sp::Node(this);
    shadow->render_data.shader = sp::Shader::get("internal:color.shader");
    shadow->render_data.type = sp::RenderData::Type::Transparent;
    shadow->render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(1, 1));
    shadow->render_data.color = sp::Color(0,0,0, 0.5);
    shadow->setPosition(sp::Vector3d(0, 0, -1));
    
    sp::collision::Circle2D shape(0.5);
    shape.fixed_rotation = true;
    shape.linear_damping = 30.0;
    //shape.restitution = 0.5;
    setCollisionShape(shape);
    
    smoke_generator = new sp::ParticleEmitter(this, 16, sp::ParticleEmitter::Origin::Global);
    smoke_generator->render_data.texture = sp::texture_manager.get("particles/smoke.png");
    
    fire_generator = new sp::ParticleEmitter(this, 16, sp::ParticleEmitter::Origin::Local);
    fire_generator->render_data.texture = sp::texture_manager.get("particles/fire.png");
    fire_generator->render_data.type = sp::RenderData::Type::Additive;
}

void PlayerShip::onFixedUpdate()
{
    time_delta += sp::Engine::fixed_update_delta;
    
    float movement_request = controller.right.getValue() - controller.left.getValue();
    setLinearVelocity(sp::Vector2d(getLinearVelocity2D().x + movement_request * 8.0, forward_velocity));
    ship->setRotation(sp::Quaterniond::fromAxisAngle(sp::Vector3d(0, 0, 1), 180) * sp::Quaterniond::fromAxisAngle(sp::Vector3d(0, 1, 0), -movement_request * 15));
    if (time_delta < drop_in_time)
        ship->setPosition(sp::Vector3d(0, 0, sp::Tween<float>::easeOutQuad(time_delta, 0, drop_in_time, 10, std::sin(time_delta * sp::pi) * 0.2)));
    else
        ship->setPosition(sp::Vector3d(0, 0, std::sin(time_delta * sp::pi) * 0.2));
    
    if (health < 3)
    {
        sp::ParticleEmitter::Parameters p;
        p.position = sp::Vector3f(ship->getPosition3D()) + sp::Vector3f(sp::random(-0.5, 0.5), sp::random(-0.5, 0.5), 0);
        p.velocity = sp::Vector3f(sp::random(-1, 1), forward_velocity*0.65, 1);
        //p.acceleration = sp::Vector3f(0, -30, 1);
        //p.start_size = 0.1;
        //p.end_size = 1.0;
        //p.start_color = sp::Color(1,1,1);
        //p.end_color = sp::Color(1,1,1,0);
        p.lifetime = 1.0;
        smoke_generator->emit(p);
    }
    
    if (health < 2)
    {
        sp::ParticleEmitter::Parameters p;
        p.position = sp::Vector3f(ship->getPosition3D()) + sp::Vector3f(sp::random(-0.25, 0.25), sp::random(-0.25, 0.25), 0);
        p.velocity = sp::Vector3f(sp::random(-1, 1), sp::random(-5, -3), 2);
        //p.acceleration = sp::Vector3f(0, -10, 1);
        //p.start_size = 0.5;
        //p.end_size = 0.1;
        //p.start_color = sp::Color(1,1,1);
        //p.end_color = sp::Color(1,0,0,0);
        p.lifetime = sp::random(0.3, 0.5);
        fire_generator->emit(p);
    }
    
    if (invulnerability)
        invulnerability--;
}

void PlayerShip::onCollision(sp::CollisionInfo& info)
{
    if (info.other->isSolid())
    {
        sp::audio::Sound::play("sfx/hit.ogg");
        setLinearVelocity(sp::Vector2d(getLinearVelocity2D().x - info.normal.x * 15, forward_velocity));
        
        if (!invulnerability)
        {
            health -= 1;
            invulnerability = 30;
            LOG(Debug, "health:", health);

            if (health < 1)
            {
                sp::audio::Sound::play("sfx/explosion.ogg");
                sp::audio::Music::stop();
                explosion();
                delete this;
            }
        }
    }
}

void PlayerShip::explosion()
{
    sp::ParticleEmitter* pe = new sp::ParticleEmitter(sp::Scene::get("MAIN")->getRoot(), 16, sp::ParticleEmitter::Origin::Local);
    pe->setPosition(getGlobalPosition3D());
    pe->render_data.texture = sp::texture_manager.get("particles/smoke.png");
    pe->auto_destroy = true;

    for(int n=0; n<8; n++)
    {
        sp::ParticleEmitter::Parameters p;
        p.position = sp::Vector3f(ship->getPosition3D()) + sp::Vector3f(sp::random(-0.5, 0.5), sp::random(-0.5, 0.5), 0);
        p.velocity = sp::Vector3f(sp::random(-3, 3), sp::random(5, 10), sp::random(-3, 3));
        //p.acceleration = -p.velocity * 0.3f;
        //p.start_size = 0.5;
        //p.end_size = 10.0;
        //p.start_color = sp::Color(1,1,1);
        //p.end_color = sp::Color(1,1,1,0);
        p.lifetime = 5.0;
        pe->emit(p);
    }

    pe = new sp::ParticleEmitter(sp::Scene::get("MAIN")->getRoot(), 16, sp::ParticleEmitter::Origin::Local);
    pe->setPosition(getGlobalPosition3D());
    pe->render_data.texture = sp::texture_manager.get("particles/fire.png");
    pe->render_data.type = sp::RenderData::Type::Additive;
    pe->auto_destroy = true;
    for(int n=0; n<16; n++)
    {
        sp::ParticleEmitter::Parameters p;
        p.position = sp::Vector3f(ship->getPosition3D()) + sp::Vector3f(sp::random(-0.25, 0.25), sp::random(-0.25, 0.25), 0);
        p.velocity = sp::Vector3f(0, sp::random(0, 5), 0);
        //p.acceleration = sp::Vector3f(0, 0, 0);
        //p.start_size = 1.5;
        //p.end_size = 2.5;
        //p.start_color = sp::Color(1,1,1);
        //p.end_color = sp::Color(1,0,0,0);
        p.lifetime = sp::random(0.5,1.5);
        pe->emit(p);
    }
    
    //A white full screen flash wouldn't hurt as well.
}
