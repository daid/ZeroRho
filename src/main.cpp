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

sp::P<sp::Window> window;

sp::io::Keybinding escape_key{"exit", "Escape"};

sp::P<PlayerShip> player;

float finish_length = 0.0;
float trench_length = 0.0;

class GameplayScene : public sp::Scene
{
public:
    GameplayScene()
    : sp::Scene("MAIN")
    {
        script_environment = new ScriptEnvironment();
        script_environment->load(sp::io::ResourceProvider::get("stages/stage_1.lua"));
        coroutine = script_environment->callCoroutine("run");
        
        player = new PlayerShip(PlayerInput::left_controller);
        new Camera();
        
        onFixedUpdate();
    }

    virtual void onFixedUpdate() override
    {
        double player_y = player->getPosition2D().y;
        while(coroutine && player_y > trench_length - 500)
        {
            if (!coroutine->resume())
                coroutine = nullptr;
        }
    }

private:
    sp::P<ScriptEnvironment> script_environment;
    sp::script::CoroutinePtr coroutine;
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

    new GameplayScene();

    engine->run();

    return 0;
}
