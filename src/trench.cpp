#include "trench.h"

#include <sp2/scene/scene.h>
#include <sp2/tween.h>
#include <sp2/random.h>
#include <sp2/graphics/meshbuilder.h>
#include <sp2/collision/2d/chains.h>


Trench::Trench(const TrenchPoint& entry, const TrenchPoint& exit, int segments)
: sp::Node(sp::Scene::get("MAIN")->getRoot())
{
    std::vector<TrenchPoint> points;
    for(int n=0; n<segments+1; n++)
    {
        points.emplace_back(sp::Tween<double>::easeInOutQuad(n, 0, segments, entry.width, exit.width), sp::Tween<double>::easeInOutQuad(n, 0, segments, entry.center, exit.center));
        if (n != 0 && n != segments)
        {
            points[n].center += sp::random(-1, 1);
            points[n].width += sp::random(-1, 1);
        }
    }

    const float side_top_z = 3;
    const float side_bottom_z = -1;
    const float center_z = -2;
    const float top_x_offset = 1;

    sp::MeshBuilder builder;
    for(unsigned int idx=0; idx<points.size()-1; idx++)
    {
        float x0_center = points[idx].center;
        float x1_center = points[idx + 1].center;
        float x0_left = points[idx].center - points[idx].width / 2;
        float x1_left = points[idx+1].center - points[idx+1].width / 2;
        float x0_right = points[idx].center + points[idx].width / 2;
        float x1_right = points[idx+1].center + points[idx+1].width / 2;
        float y0 = idx*segment_lenght;
        float y1 = idx*segment_lenght + segment_lenght;
        
        builder.addQuad(sp::Vector3f(x0_left, y0, side_bottom_z), sp::Vector3f(x0_center, y0, center_z), sp::Vector3f(x1_left, y1, side_bottom_z), sp::Vector3f(x1_center, y1, center_z));
        builder.addQuad(sp::Vector3f(x0_center, y0, center_z), sp::Vector3f(x0_right, y0, side_bottom_z), sp::Vector3f(x1_center, y1, center_z), sp::Vector3f(x1_right, y1, side_bottom_z));
        builder.addQuad(sp::Vector3f(x0_left - top_x_offset, y0, side_top_z), sp::Vector3f(x0_left, y0, side_bottom_z), sp::Vector3f(x1_left - top_x_offset, y1, side_top_z), sp::Vector3f(x1_left, y1, side_bottom_z));
        builder.addQuad(sp::Vector3f(x0_right, y0, side_bottom_z), sp::Vector3f(x0_right + top_x_offset, y0, side_top_z), sp::Vector3f(x1_right, y1, side_bottom_z), sp::Vector3f(x1_right + top_x_offset, y1, side_top_z));

        if (idx < points.size()-2)
        {
            builder.addQuad(sp::Vector3f(x0_left-50, y0, side_top_z), sp::Vector3f(x0_left - top_x_offset, y0, side_top_z), sp::Vector3f(x1_left-50, y1, side_top_z), sp::Vector3f(x1_left - top_x_offset, y1, side_top_z));
            builder.addQuad(sp::Vector3f(x0_right + top_x_offset, y0, side_top_z), sp::Vector3f(x0_right+50, y0, side_top_z), sp::Vector3f(x1_right + top_x_offset, y1, side_top_z), sp::Vector3f(x1_right+50, y1, side_top_z));
        }
        else
        {
            builder.addQuad(sp::Vector3f(x0_left-150, y0, side_top_z), sp::Vector3f(x0_left - top_x_offset, y0, side_top_z), sp::Vector3f(x1_left-150, y1, side_top_z), sp::Vector3f(x1_left - top_x_offset, y1, side_top_z));
            builder.addQuad(sp::Vector3f(x0_right + top_x_offset, y0, side_top_z), sp::Vector3f(x0_right+150, y0, side_top_z), sp::Vector3f(x1_right + top_x_offset, y1, side_top_z), sp::Vector3f(x1_right+150, y1, side_top_z));
        }
    }
    
    render_data.shader = sp::Shader::get("internal:color_shaded.shader");
    render_data.type = sp::RenderData::Type::Normal;
    render_data.mesh = builder.create();
    render_data.color = sp::Color(0.7372549, 0.4823529, 0.3215686);

    sp::collision::Chains2D::Path collision_left, collision_right;
    for(unsigned int idx=0; idx<points.size(); idx++)
    {
        collision_left.emplace_back(points[idx].center - points[idx].width / 2, idx*segment_lenght);
        collision_right.emplace_back(points[idx].center + points[idx].width / 2, idx*segment_lenght);
    }
    sp::collision::Chains2D chains;
    chains.chains.push_back(collision_left);
    chains.chains.push_back(collision_right);
    setCollisionShape(chains);
}
