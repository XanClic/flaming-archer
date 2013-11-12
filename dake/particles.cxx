#include <cgv_gl/gl/gl.h>
#include <cmath>
#include <cstdio>

#include "particles.h"


dake::vec3 dake::force_air_drag(const dake::vec3 &position, const dake::vec3 &speed, float time_passed)
{
    (void)position;
    return speed * powf(.2f, time_passed);
}

dake::vec3 dake::force_gravity(const dake::vec3 &position, const dake::vec3 &speed, float time_passed)
{
    (void)position;
    // 1.5 should be 9.81; but the units used here aren't meters anyway
    return dake::vec3(speed.x(), speed.y() - 1.5f * time_passed, speed.z());
}

dake::vec3 dake::force_bounce(const dake::vec3 &position, const dake::vec3 &speed, float time_passed)
{
    (void)time_passed;
    if (position.y() <= -5.4f)
        return dake::vec3(speed.x(), -speed.y(), speed.z());
    return speed;
}


static dake::vec3 (*const force_array[])(const dake::vec3 &position, const dake::vec3 &speed, float time_passed) = {
#ifdef __GNUC__
    [dake::particle::AIR_DRAG] = dake::force_air_drag,
    [dake::particle::GRAVITY]  = dake::force_gravity,
    [dake::particle::BOUNCE]   = dake::force_bounce
#else
    dake::force_air_drag, dake::force_gravity, dake::force_bounce
#endif
};


dake::particle::particle(float lifetime, const dake::vec3 &initial_position, const dake::vec3 &initial_speed):
    position(initial_position),
    speed(initial_speed),
    lifetime_rem(lifetime),
    lifetime_full(lifetime),
    force_mask(0)
{
}

dake::particle &dake::particle::add_force(dake::particle::force f)
{
    force_mask |= (1 << f);
    return *this;
}

void dake::particle::apply_forces(float time_passed)
{
    for (int i = 0; i < 16; i++)
        if (force_mask & (1 << i))
            speed = force_array[i](position, speed, time_passed);

    position += speed;

    if (lifetime_rem >= time_passed)
        lifetime_rem -= time_passed;
    else
        lifetime_rem = 0.f;
}

void dake::particle::draw(void)
{
    glVertex3fv(position);
    glVertex3fv(position + speed);
}

float dake::particle::lifetime(void)
{
    return lifetime_rem / lifetime_full;
}


dake::particle &dake::particle_generator::new_particle(float lifetime, const dake::vec3 &initial_position, const dake::vec3 &initial_speed)
{
    particles.push_back(particle(lifetime, initial_position, initial_speed));
    return particles.back();
}


void dake::particle_generator::draw(void)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBegin(GL_LINES);

    for (std::list<dake::particle>::iterator i = particles.begin(); i != particles.end(); i++)
    {
        float lifetime = (*i).lifetime();

        if (lifetime > .75f)
            glColor3f(lifetime, lifetime, lifetime * (lifetime - .75f) * 4.f);
        else if (lifetime > .25f)
            glColor3f(lifetime, lifetime * (lifetime - .25f) * 2.f, 0.f);
        else
            glColor3f(lifetime, 0.f, 0.f);

        (*i).draw();
    }

    glEnd();

    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(GL_TRUE);
    glPopAttrib();
}


void dake::particle_generator::tick(float time_passed)
{
    for (std::list<dake::particle>::iterator i = particles.begin(); i != particles.end();)
    {
        (*i).apply_forces(time_passed);

        if ((*i).lifetime() <= 0.f)
            particles.erase(i++);
        else
            i++;
    }
}
