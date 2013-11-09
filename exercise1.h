#include <cgv/gui/event_handler.h>
#include <cgv/gui/trigger.h>
#include <cgv/gui/provider.h>
#include <cgv/render/drawable.h>
#include <cgv/render/context.h>
#include <cgv_gl/gl/gl.h>

#include "obj_reader.h"

using namespace cgv::base;
using namespace cgv::signal;
using namespace cgv::gui;
using namespace cgv::math;
using namespace cgv::render;
using namespace cgv::utils;

class exercise1 : public node,
                  public drawable,
                  public provider,
                  public event_handler
{
private:
    // The example meshs
    obj_reader *meshs[11];
    // The animation counter
    int counter;
    // True if the mesh shall be rendered as a point cloud
    bool is_pointcloud;
    // True if a bounding box shall be rendered
    bool show_bbox;
    // True if coordinate systems shall be rendered
    bool show_coordinate_system;
    // True iff in free mode
    bool free_mode;
    // Counter to ensure smooth frames
    double timer_offset;

    bool meshs_loaded;

    // True if the ascending animation is shown
    bool ascending;
    // Time when the ascension is/was started
    int ascension_counter_start;
    // Step animation position when the ascension started
    float ascension_step_ani;
    // Current ascension speed
    vec3 ascension_speed;
    // Current center in ascension
    vec3 ascension_position;
    // Current ascension acceleration
    vec3 ascension_acceleration;
    // Current direction
    float dir_x, dir_y;


    // Render the mesh "model". This method calls render_mesh_pointcloud
    // if the variable "is_pointcloud" is true and "render_mesh_solid"
    // otherwise.
    void render_mesh(obj_reader *model);

    // Render the mesh "model" as a point cloud
    void render_mesh_pointcloud(obj_reader *model);

    // Render the mesh "model" as solid geometry
    void render_mesh_solid(obj_reader *model);

    // Render the bounding box of a mesh. This method is called from
    // render_mesh if show_bbox is set to true
    void render_bounding_box(obj_reader* model);

    // Render a simple coordinate system
    void render_coordinate_system();

    // The timer event. This method is called every 1/60 sec.
    void timer_event(double, double dt);

    // Goddess Madoka gives you hope
    void ascension(void);

    // But I want Madoka D:
    void rematerialize(void);

    // Toggle free mode™
    void toggle_free_mode(void);

    // Method that is called whenever a gui element is clicked
    void on_set(void* member_ptr);

public:
    // The constructor of this class
    exercise1();

    // Create the gui elements
    void create_gui();
    // Draw the scene
    void draw(context& c);

    virtual bool handle(event &e);
    virtual void stream_help(std::ostream &os);
};
