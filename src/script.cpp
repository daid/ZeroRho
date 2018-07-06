#include "script.h"
#include "main.h"
#include "trench.h"
#include "staticObject.h"

#include <sp2/audio/music.h>
#include <sp2/random.h>

#include <random>


static TrenchPoint final_trench_point(0, 0);
static std::mt19937_64 random_engine;

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

static void luaf_addFinish()
{
    finish_length = trench_length;
}

static void luaf_addStaticObject(sp::string name, float collision_size, float scale, sp::Vector2d position)
{
    new StaticObject("models/static/" + name + ".obj", collision_size, scale, position + sp::Vector2d(0.0, trench_length));
}

static double luaf_random(double min, double max)
{
    return std::uniform_real_distribution<>(min, max)(random_engine);
}

static void luaf_music(sp::string name)
{
    sp::audio::Music::play("music/" + name + ".ogg");
}

ScriptEnvironment::ScriptEnvironment()
{
    final_trench_point = TrenchPoint(0, 0);
    finish_length = 0.0;
    trench_length = 0.0;
    random_engine.seed(0);
    
    this->setGlobal("include", luaf_include);
    this->setGlobal("random", luaf_random);

    this->setGlobal("addTrench", luaf_addTrench);
    this->setGlobal("addFinish", luaf_addFinish);
    this->setGlobal("addStaticObject", luaf_addStaticObject);
    this->setGlobal("music", luaf_music);
}
