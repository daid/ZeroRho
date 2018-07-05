#ifndef CAMERA_H
#define CAMERA_H

#include <sp2/scene/camera.h>

class Camera : public sp::Camera
{
public:
    Camera();
    
    virtual void onUpdate(float delta) override;
};

#endif//CAMERA_H
