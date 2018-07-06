#include "staticObject.h"

#include <sp2/scene/scene.h>
#include <sp2/graphics/mesh/obj.h>
#include <sp2/collision/2d/circle.h>
#include <sp2/random.h>


StaticObject::StaticObject(sp::string model, float collision_radius, float scale, sp::Vector2d position)
: sp::Node(sp::Scene::get("MAIN")->getRoot())
{
    render_data.shader = sp::Shader::get("internal:basic_shaded.shader");
    render_data.type = sp::RenderData::Type::Normal;
    render_data.mesh = sp::obj_loader.get(model);
    render_data.texture = sp::obj_loader.getTextureFor(model);
    render_data.scale = sp::Vector3f(scale, scale, scale);
    
    if (collision_radius > 0)
    {
        sp::collision::Circle2D shape(collision_radius);
        shape.type = sp::collision::Shape::Type::Static;
        setCollisionShape(shape);
    }
    
    setPosition(sp::Vector3d(position.x, position.y, -2));
    setRotation(sp::random(0, 360));
}
