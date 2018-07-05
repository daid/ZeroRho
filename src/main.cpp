#include "main.h"
#include "player.h"
#include "playerInput.h"
#include "camera.h"
#include "trench.h"

#include <sp2/engine.h>
#include <sp2/window.h>
#include <sp2/logging.h>
#include <sp2/random.h>
#include <sp2/tweak.h>
#include <sp2/io/directoryResourceProvider.h>
#include <sp2/graphics/gui/widget/button.h>
#include <sp2/graphics/gui/scene.h>
#include <sp2/graphics/gui/theme.h>
#include <sp2/graphics/gui/loader.h>
#include <sp2/graphics/scene/graphicslayer.h>
#include <sp2/graphics/scene/basicnoderenderpass.h>
#include <sp2/graphics/scene/collisionrenderpass.h>
#include <sp2/graphics/mesh/obj.h>
#include <sp2/graphics/meshbuilder.h>
#include <sp2/graphics/spriteAnimation.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/audio/sound.h>
#include <sp2/audio/music.h>
#include <sp2/scene/scene.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/particleEmitter.h>
#include <sp2/collision/2d/box.h>
#include <sp2/collision/2d/circle.h>
#include <sp2/script/environment.h>
#include <sp2/io/keybinding.h>
#include <sp2/tween.h>

sp::P<sp::Window> window;

sp::io::Keybinding escape_key{"exit", "Escape"};

sp::P<PlayerShip> player;

TrenchPoint final_trench_point(0, 0);
float trench_length = 0.0;
float finish_length = 0.0;

class StaticObject : public sp::Node
{
public:
    StaticObject(sp::string model, float collision_radius, float scale, sp::Vector2d position)
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
};

static int luaf_include(lua_State* L)
{
    sp::string filename = luaL_checkstring(L, 1);

    auto resource = sp::io::ResourceProvider::get("stages/" + filename);
    if (!resource)
        return luaL_error(L, "Failed to find %s", filename.c_str());

    sp::string filecontents = resource->readAll();
    if (luaL_loadbuffer(L, filecontents.c_str(), filecontents.length(), filename.c_str()))
        return lua_error(L);
    //set the environment table it as 1st upvalue for the loaded chunk, else it works in a different environment then where this function was called from.
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setupvalue(L, -2, 1);

    lua_call(L, 0, 0);
    return 0;
}

static int luaf_addTrench(lua_State* L)
{
    float length = luaL_checknumber(L, 1);
    float center = luaL_checknumber(L, 2);
    float width = luaL_checknumber(L, 3);
    int segment_count = std::ceil(length / Trench::segment_lenght);
    TrenchPoint new_point(width, center);
    if (final_trench_point.width == 0.0)
        final_trench_point = new_point;

    Trench* trench = new Trench(final_trench_point, new_point, segment_count);
    trench->setPosition(sp::Vector2d(0, trench_length));
    
    trench_length += segment_count * Trench::segment_lenght;
    final_trench_point = new_point;
    
    return lua_yield(L, 0);
}

void luaf_addFinish()
{
    finish_length = trench_length;
}

void luaf_addStaticObject(sp::string name, float collision_size, float scale, sp::Vector2d position)
{
    new StaticObject("models/static/" + name + ".obj", collision_size, scale, position + sp::Vector2d(0.0, trench_length));
}

class ScriptEnvironment : public sp::script::Environment
{
public:
    ScriptEnvironment()
    {
        this->setGlobal("include", luaf_include);
        this->setGlobal("random", sp::random);

        this->setGlobal("addTrench", luaf_addTrench);
        this->setGlobal("addFinish", luaf_addFinish);
        this->setGlobal("addStaticObject", luaf_addStaticObject);
    }
};

int main(int argc, char** argv)
{
    //Create resource providers, so we can load things.
    new sp::io::DirectoryResourceProvider("resources");

#ifndef DEBUG
    sp::texture_manager.setFallbackColors(sp::Color(0,0,0,0), sp::Color(0,0,0,0));
#endif
    sp::texture_manager.setDefaultSmoothFiltering(false);
    sp::obj_loader.setMode(sp::ObjLoader::Mode::DiffuseMaterialColorToTexture);

    //Create a window to render on, and our engine.
    window = new sp::Window(4.0/3.0);
#ifndef DEBUG
    window->setFullScreen(true);
    window->hideCursor();
#endif
    sp::P<sp::Engine> engine = new sp::Engine();
    
    sp::gui::Theme::loadTheme("default", "gui/theme/basic.theme.txt");
    new sp::gui::Scene(sp::Vector2d(256, 192), sp::gui::Scene::Direction::Horizontal);

    sp::P<sp::SceneGraphicsLayer> scene_layer = new sp::SceneGraphicsLayer(1);
    scene_layer->addRenderPass(new sp::BasicNodeRenderPass());
#ifdef DEBUG
    scene_layer->addRenderPass(new sp::CollisionRenderPass());
#endif
    window->addLayer(scene_layer);
    window->setClearColor(sp::Color(0,0,0));

    sp::P<sp::Scene> scene = new sp::Scene("MAIN");
    sp::P<sp::Camera> camera = new Camera();

    player = new PlayerShip(PlayerInput::left_controller);
/*
    std::vector<sp::string> meshes = {
        "models/static/tree_small.obj",
        "models/static/tree_blocks.obj",
        "models/static/stone_column.obj",
        "models/static/tree_thin_fall.obj",
    };
    TrenchPoint p0(30, 0);
    for(int n=0; n<100; n++)
    {
        //OK:
        //TrenchPoint p1(sp::random(9, 15), sp::random(-5, 5));
        //HARD:
        TrenchPoint p1(sp::random(9, 11), sp::random(-7, 7));
        if (n < 3) p1.width = 30;
        (new Trench(p0, p1, 10))->setPosition(sp::Vector2d(0, 50 * n));
        p0 = p1;
        
        if (n > 3)
        {
            new StaticObject(meshes[sp::irandom(0, meshes.size() - 1)], 1.0, 0.4, sp::Vector2d(p0.center + sp::random(-p0.width*0.4, p0.width*0.4), 50+50*n));
        }
    }
*/
    ScriptEnvironment* se = new ScriptEnvironment();
    se->load(sp::io::ResourceProvider::get("stages/stage_1.lua"));
    auto co = se->callCoroutine("run");
    while(co->resume())
    {
    }

    sp::audio::Music::play("music/Juhani Junkala [Retro Game Music Pack] Level 1.ogg");
    engine->run();

    return 0;
}
