#include "camera.h"
#include "main.h"
#include "player.h"

#include <sp2/scene/scene.h>

Camera::Camera()
: sp::Camera(sp::Scene::get("MAIN")->getRoot())
{
    setPerspective();
    getScene()->setDefaultCamera(this);
}

void Camera::onUpdate(float delta)
{
    if (player && (player->getPosition2D().y < finish_length || finish_length == 0.0))
    {
        setRotation(sp::Quaterniond::fromAxisAngle(sp::Vector3d(1, 0, 0), 80) * sp::Quaterniond::fromAxisAngle(sp::Vector3d(0, 0, 1), player->getLinearVelocity2D().x * 0.1));
        setPosition(sp::Vector3d(player->getPosition2D().x * 0.3, player->getPosition2D().y-8.0, 5.0));
    }
}

