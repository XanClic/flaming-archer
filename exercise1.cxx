#include "exercise1.h"
#include "matrix.h"
#include "obj_reader.h"
#include "particles.h"
#include "texture.h"
#include "vector.h"

#include <cgv/gui/key_event.h>
#include <cgv/gui/mouse_event.h>
#include <cgv/utils/ostream_printf.h>


// #define MADOKA_MODE


using namespace cgv::utils;

// The constructor of this class
exercise1::exercise1():node("Exercise 1"),
    counter(0),
    is_pointcloud(false),
    show_bbox(false),
    show_coordinate_system(false),
    free_mode(false),
    timer_offset(0.0),
    meshs_loaded(false),
    dir_x(0.f),
    dir_y(0.f)
{
    // Connect the timer_event method to the (cgv-library) animation
    // trigger to be called every 1/60 sec.
    connect(get_animation_trigger().shoot, this, &exercise1::timer_event);
}



static float frand(void)
{
    return (rand() % 1024) / 1023.f;
}



// The timer event. This method is called every 1/60 sec.
void exercise1::timer_event(double , double dt)
{
    // Define the frame rate
    const double fps = 1.0/30.0;

    timer_offset += dt;

    // Check whether a new frame shall be rendered
    if (timer_offset >= fps) {
        timer_offset -= fps;
        counter++;
        post_redraw();

        dake::particle_generator::instance().tick(fps);
        if (((counter + 1) % 100 <= 3) && (!ascending || (counter < ascension_counter_start)) && !free_mode)
        {
            for (int i = 0; i < 20; i++)
            {
                float a = frand() * 2 * M_PI;
                dake::particle_generator::instance()
                    .new_particle(frand() * 2.f + .7f, dake::vec3(0.f, -.5f, 0.f), dake::vec3(.6f * cosf(a), frand() * .2f + .4f, .6f * sinf(a)))
                    .add_force(dake::particle::AIR_DRAG)
                    .add_force(dake::particle::GRAVITY)
                    .add_force(dake::particle::BOUNCE);
            }
        }
    }
}



// Method that is called whenever a gui element is clicked
void exercise1::on_set(void* member_ptr)
{
    // Redraw the scene every time a gui value was changed
    post_redraw();
}



// Create the gui elements
void exercise1::create_gui() {
    // Create a toggle button that controls the variable "is_pointcloud".
    // Every time this button is pressed, the method on_set is called
    // that calls post_redraw to redraw the scene.
    add_member_control(this, "Render as Point Cloud", is_pointcloud, "toggle");

    // Create a toggle button that controls the variable "show_bbox".
    // Every time this button is pressed, the method on_set is called
    // that calls post_redraw to redraw the scene.
    add_member_control(this, "Show Bounding Cube", show_bbox, "toggle");

    // Create a toggle button that controls the variable "show_coordinate_system".
    // Every time this button is pressed, the method on_set is called
    // that calls post_redraw to redraw the scene.
    add_member_control(this, "Show Coordinate System", show_coordinate_system, "toggle");


    // To add further control elements you can copy the lines above. Other control
    // elements such as buttons exist. To create a button which calls an arbitrary method
    // uncomment the following two lines
    // connect_copy(add_button("Press me")->click,
    //              rebind(static_cast<exercise1*>(this), &exercise1::button_pressed));
    // Whenever a click was performed, the method "button_pressed" is called.

    connect_copy(add_button("Apotheosis")->click, rebind(this, &exercise1::ascension));
    connect_copy(add_button("Re-Materialize")->click, rebind(this, &exercise1::rematerialize));

    connect_copy(add_control("Free Mode", free_mode, "toggle")->value_change, rebind(this, &exercise1::toggle_free_mode));
}



void exercise1::ascension(void)
{
    ascending = true;
    ascension_counter_start = ((counter + 24) / 100) * 100 + 75;
    ascension_acceleration = dake::vec3(0.f, 0.f, 0.f);
    ascension_speed = dake::vec3(0.f, 0.f, 0.f);
    ascension_position = dake::vec3(0.f, 0.f, 0.f);
    dir_x = dir_y = 0.f;

    ascension_step_ani = sinf(ascension_counter_start / 8.f);
}



void exercise1::rematerialize(void)
{
    if (ascending && (counter - ascension_counter_start > 300))
    {
        ascending = false;
        dir_x = dir_y = 0.f;
    }
}



void exercise1::toggle_free_mode(void)
{
    if (free_mode)
        ascension();
    else
        ascending = false;
}



bool exercise1::handle(cgv::gui::event &e)
{
    if (!free_mode)
        return false;


    if (e.get_kind() == cgv::gui::EID_KEY)
    {
        static bool shift, space;

        cgv::gui::key_event &ke = *dynamic_cast<cgv::gui::key_event *>(&e);
        if (ke.get_key() == cgv::gui::KEY_Left_Shift)
            shift = ke.get_action() == cgv::gui::KA_PRESS;
        else if (ke.get_char() == ' ')
            space = ke.get_action() == cgv::gui::KA_PRESS;

        if (!space)
            ascension_acceleration = dake::vec3(0.f, 0.f, 0.f);
        else
        {
            dake::vec3 direction(sinf(dir_x), cosf(dir_x) * cosf(dir_y), sinf(dir_y));
            ascension_acceleration = direction.normalized() * (shift ? .16f : .08f);
        }

        return true;
    }
    else if (e.get_kind() == cgv::gui::EID_MOUSE)
    {
        static int last_x, last_y; // get_dx() is crap

        cgv::gui::mouse_event &me = *dynamic_cast<cgv::gui::mouse_event *>(&e);
        if (me.get_action() == cgv::gui::MA_MOVE)
        {
            if (ascending)
            {
                // clamp mouse
                last_x = me.get_x();
                last_y = me.get_y();
            }

            int dx = me.get_x() - last_x, dy = me.get_y() - last_y;
            last_x += dx;
            last_y += dy;
            dir_x += dx / 100.f;
            dir_y += dy / 100.f;

            return true;
        }
    }

    return false;
}



void exercise1::stream_help(std::ostream &os)
{
}



void exercise1::draw(context& c) {
    if (!meshs_loaded)
    {
#ifdef MADOKA_MODE
        meshs[0] = new obj_reader("data/madoka/torso_upper.obj");
#else
        meshs[0] = new obj_reader("data/robot/torso_upper.obj");
#endif
        meshs[1] = new obj_reader("data/robot/torso_lower.obj");
        meshs[2] = new obj_reader("data/robot/leg_left.obj");
        meshs[3] = new obj_reader("data/robot/leg_right.obj");
        meshs[4] = new obj_reader("data/robot/arm_left_lower.obj");
        meshs[5] = new obj_reader("data/robot/arm_left_upper.obj");
        meshs[6] = new obj_reader("data/robot/arm_right_lower.obj");
        meshs[7] = new obj_reader("data/robot/arm_right_upper.obj");
        meshs[8] = new obj_reader("data/bear/stem.obj");
        meshs[9] = new obj_reader("data/bear/blossom.obj");
#ifdef MADOKA_MODE
        meshs[10] = new obj_reader("data/madoka/wings.obj");
#else
        meshs[10] = NULL;
#endif

        meshs_loaded = true;
    }

    dake::vec4 robot_col(.6f, .6f, .6f, 1.f);


    // Disable face culling
    glDisable(GL_CULL_FACE);
    // Declare all polygons as two sided. Exporters tend to
    // flip the normals of .obj-files. Using two-sided
    // materials they are still correctly lighted.
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_TEXTURE_2D);

    // *** Begin of task 1.2.6 ***
    // Create an animated transformation hierarchy. To render a mesh
    // use the method "render_mesh" which takes the mesh to be rendered
    // as the only parameter.
    // Transformations can be done using glRotatef, glTranslatef or
    // glScalef. To save the current transformation to a stack use
    // glPushMatrix, and to restore and reactivate it, use glPopMatrix.
    // For animation you can use the integer variable "counter".
    // This variable just counts up, so to have a periodic animation
    // value you can use the sine of this value.
    // As an example, try the line
    // glRotatef(sin(counter*0.01)*20, 0, 0, 1)
    // and see what it does.
    // You can replace the following code.

    enum
    {
        ANI_WALKING,
        ANI_ASC_STOP1,
        ANI_ASC_COMPACT,
        ANI_ASC_STOP2,
        ANI_ASC_ASC1,
        ANI_ASC_ASC2,

        ANI_FREE
    } animation_state;

    if (!ascending && free_mode)
        animation_state = ANI_FREE;
    else if (!ascending || (counter < ascension_counter_start))
        animation_state = ANI_WALKING;
    else if (counter - ascension_counter_start < 20)
        animation_state = ANI_ASC_STOP1;
    else if (counter - ascension_counter_start < 60)
        animation_state = ANI_ASC_COMPACT;
    else if (counter - ascension_counter_start < 80)
        animation_state = ANI_ASC_STOP2;
    else if (counter - ascension_counter_start < 300)
        animation_state = ANI_ASC_ASC1;
    else
        animation_state = ANI_ASC_ASC2;

    if ((animation_state > ANI_ASC_STOP2) && free_mode)
    {
        ascending = false;
        animation_state = ANI_FREE;
    }

    float compact_progress = 0.f;
    if (animation_state == ANI_ASC_COMPACT)
        compact_progress = (counter - ascension_counter_start - 20) / 40.f;
    else if (animation_state > ANI_ASC_COMPACT)
        compact_progress = 1.f;

    float step_ani;
    if (animation_state == ANI_WALKING)
        step_ani = sinf(counter / 8.f);
    else if (animation_state == ANI_ASC_STOP1)
        step_ani = ascension_step_ani;
    else if (animation_state == ANI_ASC_COMPACT)
        step_ani = ascension_step_ani * (1.f - compact_progress);
    else
        step_ani = 0.f;

    if (animation_state == ANI_ASC_ASC1)
    {
        if (ascension_acceleration.y() < .06)
            ascension_acceleration += dake::vec3(0.f, .001f, 0.f);
        else
            ascension_acceleration += dake::vec3(0.f, .0001f, 0.f);
    }
    else if (animation_state == ANI_ASC_ASC2)
        ascension_acceleration = dake::vec3(0.f, .06f, 0.f);
    else if (animation_state != ANI_FREE)
        ascension_acceleration = dake::vec3(0.f, 0.f, 0.f);

    glPushMatrix();

    if (animation_state >= ANI_ASC_ASC1)
    {
        ascension_speed += ascension_acceleration;
        // Fix me: Get the actual time passed
        ascension_speed = dake::force_air_drag(ascension_position, ascension_speed, 1.f / 30.f);
        ascension_speed = dake::force_gravity(ascension_position, ascension_speed, 1.f / 30.f);

        ascension_position += ascension_speed;

        if (ascension_position.y() < 0.f)
        {
            ascension_position.y() = 0.f;

            if (ascension_speed.y() < 0.f)
                ascension_speed.y() = 0.f;
        }

        dake::mat4 mat;
        mat.translate(ascension_position);

        dake::vec3 direction(sinf(dir_x), cosf(dir_x) * cosf(dir_y), sinf(dir_y));
        mat.rotate(acosf(direction.y()), dake::vec3(0.f, 1.f, 0.f) ^ direction.normalized());

        glMultMatrixf(mat);

        int particles = (int)(ascension_acceleration.length() * 4000.f);
        float randomness = ascension_acceleration.length() * 10.f;

        for (int i = 0; i < particles; i++)
        {
            dake::vec3 pos = randomness * dake::vec3(frand() - .5f, 0.f, frand() - .5f)
                           + dake::vec3(1.5f, -6.f - ascension_speed.y(), 0.f);
            dake::vec3 vel = randomness * 2.f * dake::vec3(frand() - .3f, frand() - .5f, frand() - .5f) - 6.f * ascension_acceleration;

            dake::particle_generator::instance().new_particle(frand() * 1.5f + 1.f,
                    dake::vec3(mat * dake::vec4(pos.x(), pos.y(), pos.z(), 1.f)), dake::vec3(mat * dake::vec4(vel.x(), vel.y(), vel.z(), 0.f)))
                .add_force(dake::particle::AIR_DRAG)
                .add_force(dake::particle::GRAVITY)
                .add_force(dake::particle::BOUNCE);
        }

        for (int i = 0; i < particles; i++)
        {
            dake::vec3 pos = randomness * dake::vec3(frand() - .5f, 0.f, frand() - .5f)
                           + dake::vec3(-1.5f, -6.f - ascension_speed.y(), 0.f);
            dake::vec3 vel = randomness * 2.f * dake::vec3(frand() - .7f, frand() - .5f, frand() - .5f) - 6.f * ascension_acceleration;

            dake::particle_generator::instance().new_particle(frand() * 1.5f + 1.f,
                    dake::vec3(mat * dake::vec4(pos.x(), pos.y(), pos.z(), 1.f)), dake::vec3(mat * dake::vec4(vel.x(), vel.y(), vel.z(), 0.f)))
                .add_force(dake::particle::AIR_DRAG)
                .add_force(dake::particle::GRAVITY)
                .add_force(dake::particle::BOUNCE);
        }
    }

    // Lower torso (leg and upper torso attachment)
    glColor4fv(robot_col);
    render_mesh(meshs[1]);
    // Left leg
    glPushMatrix();
        glTranslatef(0.f, -1.f, 0.f);
        glRotatef(step_ani * 10.f, 1.f, 0.f, 0.f);
        glRotatef(compact_progress * 10.f, 0.f, 0.f, -1.f);
        glColor4fv(robot_col);
        render_mesh(meshs[2]);
    glPopMatrix();

    // Right leg
    glPushMatrix();
        glTranslatef(0.f, -1.f, 0.f);
        glRotatef(step_ani * 10.f, -1.f, 0.f, 0.f);
        glRotatef(compact_progress * 10.f, 0.f, 0.f, 1.f);
        glColor4fv(robot_col);
        render_mesh(meshs[3]);
    glPopMatrix();

    // Upper torso (arm and wing attachment)
    int top_rot;
    if (animation_state == ANI_WALKING)
    {
        top_rot = counter % 200;
        if (top_rot < 50)
            top_rot = -25;
        else if (top_rot < 100)
            top_rot = top_rot - 75;
        else if (top_rot < 150)
            top_rot = 25;
        else if (top_rot >= 150)
            top_rot = 175 - top_rot;
    }
    else
        top_rot = 0;

    glRotatef(top_rot, 0.f, 1.f, 0.f);
#ifdef MADOKA_MODE
    glColor4f(1.f, 1.f, 1.f, 1.f);
#else
    glColor4fv(robot_col);
#endif
    render_mesh(meshs[0]);

    // Wings
    if ((animation_state >= ANI_ASC_STOP1) && meshs[10])
    {
        glPushMatrix();
            float scale = 1.f;
            if (animation_state == ANI_ASC_STOP1)
                scale = (counter - ascension_counter_start) / 20.f;
            glScalef(scale, scale, scale);
            glColor4f(1.f, 1.f, 1.f, 1.f);
            render_mesh(meshs[10]);
        glPopMatrix();
    }

    // Left upper arm (lower arm attachment)
    glPushMatrix();
        glTranslatef(1.f, 2.5f, 0.f);
        glRotatef(step_ani * 10.f, -1.f, 0.f, 0.f);
        glColor4fv(robot_col);
        render_mesh(meshs[5]);

        // Left lower arm
        glTranslatef(1.7f, -1.f, -.5f);
        glColor4fv(robot_col);
        render_mesh(meshs[4]);
    glPopMatrix();

    float right_arm_lift = 60.f;
    if (animation_state == ANI_ASC_COMPACT)
        right_arm_lift = (1.f - compact_progress) * 60.f;
    else if (animation_state > ANI_ASC_COMPACT)
        right_arm_lift = 0.f;

    // Right upper arm (lower arm attachment)
    glPushMatrix();
        glTranslatef(-1.f, 2.5f, 0.f);
        glRotatef(step_ani * 5.f, 1.f, 0.f, 0.f);
        glColor4fv(robot_col);
        render_mesh(meshs[7]);

        // Right lower arm
        glTranslatef(-1.7f, -1.f, -.5f);
        glRotatef(right_arm_lift + step_ani * 5.f, -1.f, 0.f, 0.f);
        glColor4fv(robot_col);
        render_mesh(meshs[6]);

        // Flower stem (blossom attachment)
        glTranslatef(.1f, -4.f, 1.f);
        glRotatef(90.f, 1.f, 0.f, 0.f);
        glRotatef(20.f, 0.f, 0.f, -1.f);
        glColor4f(.1f, .5f, 0.f, 1.f);
        render_mesh(meshs[8]);

        // Flower blossom
        glRotatef(counter, 0.f, 1.f, 0.f);
        glColor4f(1.f, 1.f, 1.f, 1.f);
        render_mesh(meshs[9]);
    glPopMatrix();

    glPopMatrix();

    // *** End of task 1.2.6 ***

    dake::particle_generator::instance().draw();

    // Re-enable backface culling
    glEnable(GL_CULL_FACE);
}




// Render the bounding box of a mesh. This method is called from
// render_mesh if show_bbox is set to true
void exercise1::render_bounding_box(obj_reader* model)
{
    // Remember the color
    GLfloat color[4] ={0, 0, 0,0 };
    glGetFloatv(GL_CURRENT_COLOR, color);
    // Remember the OpenGL state on polygon mode and enabled features
    glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
    // Disable lighting
    glDisable(GL_LIGHTING);
    // Disable face culling
    glDisable(GL_CULL_FACE);
    glLineWidth(1.0);


    // *** Begin of task 1.2.2 (2) ***
    // Render the bounding box by either rendering all 12 edges
    // using the vertex mode GL_LINES or by rendering the 6
    // faces of the cube and set the polygon mode to line mode
    // using the command glPolygonMode(GL_FRONT_AND_BACK, GL_LINE).
    glColor4f(1.f, 1.f, 1.f, 1.f);
    dake::vec3 bbox[2] = { model->get_bbox_min(), model->get_bbox_max() }, cur = bbox[0];
    for (int z = 0; z < 2; z++)
    {
        glBegin(GL_LINE_LOOP);
        for (int i = 0, gray = 0; i < 4; i++, gray = i ^ (i >> 1))
            glVertex3fv(dake::vec3(bbox[gray & 1][0], bbox[gray >> 1][1], bbox[z][2]));
        glEnd();
    }
    glBegin(GL_LINES);
    for (int i = 0, gray = 0; i < 4; i++, gray = i ^ (i >> 1))
        for (int z = 0; z < 2; z++)
            glVertex3fv(dake::vec3(bbox[gray & 1][0], bbox[gray >> 1][1], bbox[z][2]));
    glEnd();

    // *** End of task 1.2.2 (2) ***

    // Restore the OpenGL state bits
    glPopAttrib();
    // Restore the color
    glColor4fv(color);

}




void exercise1::render_coordinate_system()
{
    GLfloat color[4] ={0, 0, 0,0 };
    glGetFloatv(GL_CURRENT_COLOR, color);

    // Remember the OpenGL enable bits
    glPushAttrib(GL_ENABLE_BIT);
    // Disable lighting
    glDisable(GL_LIGHTING);

    // Start transferring line information
    glBegin(GL_LINES);

    // Show a red arrow towards the X axis
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);

    // Show a green arrow towards the Y axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);

    // Show a blue arrow towards the Z axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.5f);

    glEnd();

    // Restore enable bits
    glPopAttrib();

    glColor4fv(color);
}





// Render the mesh "model". This method calls render_mesh_pointcloud
// if the variable "is_pointcloud" is true and "render_mesh_solid"
// otherwise.
void exercise1::render_mesh(obj_reader *model)
{
    // Shall a bounding cube be rendered?
    if (show_bbox)
        render_bounding_box(model);

    // Shall the coordinate system be rendered?
    if (show_coordinate_system)
        render_coordinate_system();

    // If the point cloud mode is activated then
    // call "render_mesh_pointcloud" and "render_mesh_solid" otherwise
    if (is_pointcloud)
        render_mesh_pointcloud(model);
    else
        render_mesh_solid(model);
}




// Render the mesh "model" as a point cloud
void exercise1::render_mesh_pointcloud(obj_reader *model)
{
    // Set the point color to white
    glColor3d(1,1,1);
    // Disable lighting
    glDisable(GL_LIGHTING);
    // Set the point size to 5 px
    glPointSize(5);
    // Render smooth points
    glEnable(GL_POINT_SMOOTH);

    // *** Begin of task 1.2.1 ***
    // Iterate over all vertices of "model". For every vertex
    // call glVertex3f to send this point to the graphics card.
    // A vertex has the type "vec3". To access a component you
    // can use the []-operator. For example, to get the first
    // component from the vec3 variable "pt1", use pt1[0]."
    // You can also pass the vertices as an array in one
    // parameter using glVertex3fv.
    // For example:
    //    vec3 pt1;
    //    glVertex3fv(pt1);
    glBegin(GL_POINTS);
    // for (auto vertex: model->get_vertices())
    //    glVertex3fv(vertex);
    for (std::vector<dake::vec3>::const_iterator i = model->get_vertices().begin(); i != model->get_vertices().end(); i++)
        glVertex3fv(*i);
    glEnd();

    // *** End of task 1.2.1 ***

    // Re-enable lighting
    glEnable(GL_LIGHTING);
}




// Render the mesh "model" as solid geometry
void exercise1::render_mesh_solid(obj_reader *model)
{
    // *** Begin of task 1.2.5 ***
    // Iterate over all faces of "model". For every face,
    // iterate over all points. Each of these points contain the
    // indices of the vertices list or normals list, or -1 if
    // the information is not present.

    const std::vector<dake::vec3> &normals = model->get_normals();
    const std::vector<dake::vec2> &tex_coords = model->get_tex_coords();
    const std::vector<dake::vec3> &vertices = model->get_vertices();

    int current_render_mode = -1;

    const material *current_mat = NULL;

    // for (auto f: model->get_faces())
    for (std::vector<face>::const_iterator i = model->get_faces().begin(); i != model->get_faces().end(); i++)
    {
        const face &f = *i;
        int desired_render_mode;

        if (f.mat != current_mat)
        {
            if (current_render_mode >= 0)
                glEnd();
            current_render_mode = -1;

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, f.mat->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, f.mat->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, f.mat->specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, f.mat->spec_co);

            if (f.mat->tex)
                f.mat->tex->bind();
            else
                glBindTexture(GL_TEXTURE_2D, 0);

            current_mat = f.mat;
        }

        switch (f.corners.size())
        {
            case 1: desired_render_mode = GL_POINTS; break;
            case 2: desired_render_mode = GL_LINES; break;
            case 3: desired_render_mode = GL_TRIANGLES; break;
            case 4: desired_render_mode = GL_QUADS; break;
            default: desired_render_mode = GL_TRIANGLE_FAN; break;
        }

        if ((desired_render_mode != current_render_mode) || (current_render_mode == GL_TRIANGLE_FAN))
        {
            if (current_render_mode >= 0)
                glEnd();
            glBegin(current_render_mode = desired_render_mode);
        }

        //for (auto c: f.corners)
        for (std::vector<face_corner>::const_iterator ci = f.corners.begin(); ci != f.corners.end(); ci++)
        {
            const face_corner &c = *ci;

            if (c.index_normal > 0)
                glNormal3fv(normals[c.index_normal - 1]);
            if (c.index_texcoord > 0)
                glTexCoord2fv(tex_coords[c.index_texcoord - 1]);
            glVertex3fv(vertices[c.index_vertex - 1]);
        }
    }

    if (current_render_mode >= 0)
        glEnd();


    // *** End of task 1.2.5 ***
}



