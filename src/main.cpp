#include "main.h"
#include "player.h"
#include "playerInput.h"
#include "camera.h"
#include "trench.h"
#include "script.h"

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
#include <sp2/graphics/meshbuilder.h>
#include <sp2/graphics/mesh/obj.h>
#include <sp2/graphics/spriteAnimation.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/audio/sound.h>
#include <sp2/audio/music.h>
#include <sp2/scene/scene.h>
#include <sp2/collision/2d/box.h>
#include <sp2/collision/2d/circle.h>
#include <sp2/io/keybinding.h>
#include <sp2/tween.h>
#include <sp2/math/plane.h>

sp::P<sp::Window> window;

sp::io::Keybinding escape_key{"exit", "Escape"};
#ifdef DEBUG
sp::io::Keybinding preview_switch{"preview_switch", "F3"};
bool switch_delay;
#endif

sp::P<PlayerShip> player;

float finish_length = 0.0;
float trench_length = 0.0;

class GameplayScene : public sp::Scene
{
public:
    GameplayScene(sp::string stage_name);
    ~GameplayScene();
    virtual void onUpdate(float delta) override;

private:
    void restart();

    sp::P<ScriptEnvironment> script_environment;
    sp::script::CoroutinePtr coroutine;
    sp::string stage_name;
    
    float restart_delay;
};

class PreviewScene : public sp::Scene
{
public:
    PreviewScene(sp::string stage_name)
    : sp::Scene("MAIN"), stage_name(stage_name)
    {
        camera = new sp::Camera(getRoot());
        camera->setPerspective();
        camera->setPosition(sp::Vector3d(0, 0, 100));
        camera->setRotation(sp::Quaterniond::fromAxisAngle(sp::Vector3d(1, 0, 0), 45));
        setDefaultCamera(camera);
        
        reload();
    }
    
    void reload()
    {
        for(auto child : getRoot()->getChildren())
        {
            if (sp::P<sp::Camera>(child))
                continue;
            child.destroy();
        }
    
        sp::P<ScriptEnvironment> script_environment = new ScriptEnvironment();
        script_environment->load(sp::io::ResourceProvider::get("stages/" + stage_name + ".lua"));
        sp::script::CoroutinePtr coroutine = script_environment->callCoroutine("run");
        while(coroutine && coroutine->resume())
        {
        }
        script_environment.destroy();
        LOG(Debug, finish_length);
        
        stage_mod_time = sp::io::ResourceProvider::getModifyTime("stages/" + stage_name + ".lua");
    }

    void onUpdate(float delta) override
    {
        if (stage_mod_time != sp::io::ResourceProvider::getModifyTime("stages/" + stage_name + ".lua"))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            reload();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

#ifdef DEBUG
        if (!switch_delay && preview_switch.getDown())
        {
            sp::string name = stage_name;
            switch_delay = true;
            delete this;
            new GameplayScene(stage_name);
        }
        else
        {
            switch_delay = false;
        }
#endif
    }
    
    bool onPointerDown(sp::io::Pointer::Button button, sp::Ray3d ray, int id) override
    {
        pointer_down = sp::Plane3d(sp::Vector3d(0, 0, 1), 0).intersect(ray);
        return true;
    }
    
    void onPointerDrag(sp::Ray3d ray, int id) override
    {
        sp::Vector3d v = sp::Plane3d(sp::Vector3d(0, 0, 1), 0).intersect(ray);
        camera->setPosition(camera->getPosition3D() - sp::Vector3d(0, v.y - pointer_down.y, 0));
        LOG(Debug, camera->getPosition3D().y);
    }
private:
    sp::string stage_name;
    sp::P<sp::Camera> camera;
    sp::Vector3d pointer_down;
    
    std::chrono::system_clock::time_point stage_mod_time;
};

GameplayScene::GameplayScene(sp::string stage_name)
: sp::Scene("MAIN"), stage_name(stage_name)
{
    restart();
}

GameplayScene::~GameplayScene()
{
    script_environment.destroy();
}

void GameplayScene::restart()
{
    for(auto child : getRoot()->getChildren())
        child.destroy();
    script_environment.destroy();
    script_environment = new ScriptEnvironment();
    script_environment->load(sp::io::ResourceProvider::get("stages/" + stage_name + ".lua"));
    coroutine = script_environment->callCoroutine("run");
    
    player = new PlayerShip(PlayerInput::left_controller);
    new Camera();
    
    restart_delay = 5.0;
    
    onUpdate(0);
}

void GameplayScene::onUpdate(float delta)
{
    if (player)
    {
        double player_y = player->getPosition2D().y;
        while(coroutine && player_y > trench_length - 1000)
        {
            if (!coroutine->resume())
                coroutine = nullptr;
        }
        if (player_y > finish_length && finish_length != 0.0)
        {
            restart_delay -= delta;
            if (restart_delay < 0)
                restart();
        }
    }
    else
    {
        restart_delay -= delta;
        if (restart_delay < 0)
            restart();
    }
    
#ifdef DEBUG
    if (!switch_delay && preview_switch.getDown())
    {
        sp::string name = stage_name;
        switch_delay = true;
        delete this;
        new PreviewScene(name);
    }
    else
    {
        switch_delay = false;
    }
#endif
}

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

    new GameplayScene("stage_1");

    engine->run();

    return 0;
}
