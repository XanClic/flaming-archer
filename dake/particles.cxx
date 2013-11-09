#include <cgv_gl/gl/gl.h>
#include <cstdio>

#include "particles.h"


vec3 force_air_drag(const vec3 &position, const vec3 &speed)
{
    (void)position;
    return speed * .95f;
}

vec3 force_gravity(const vec3 &position, const vec3 &speed)
{
    (void)position;
    return vec3(speed.x(), speed.y() - .05f, speed.z());
}

vec3 force_bounce(const vec3 &position, const vec3 &speed)
{
    if (position.y() <= -5.4f)
        return vec3(speed.x(), -speed.y(), speed.z());
    return speed;
}


particle::particle(int lifetime, const vec3 &initial_position, const vec3 &initial_speed):
    position(initial_position),
    speed(initial_speed),
    lifetime_rem(lifetime),
    lifetime_full(lifetime)
{
}

particle &particle::add_force(force_apply_t force)
{
    forces.push_back(reinterpret_cast<void *>(force));
    return *this;
}

void particle::apply_forces(void)
{
    for (std::list<void *>::iterator i = forces.begin(); i != forces.end(); i++)
        speed = reinterpret_cast<force_apply_t>(*i)(position, speed);

    position += speed;

    if (lifetime_rem > 0)
        lifetime_rem--;
}

void particle::draw(void)
{
    glVertex3fv(position);
    glVertex3fv(position + speed);
}

float particle::lifetime(void)
{
    return (float)lifetime_rem / lifetime_full;
}


particle &particle_generator::new_particle(int lifetime, const vec3 &initial_position, const vec3 &initial_speed)
{
    particles.push_back(particle(lifetime, initial_position, initial_speed));
    return particles.back();
}


void particle_generator::draw(void)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBegin(GL_LINES);

    for (std::list<particle>::iterator i = particles.begin(); i != particles.end(); i++)
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
    glPopAttrib();
}


void particle_generator::tick(void)
{
    for (std::list<particle>::iterator i = particles.begin(); i != particles.end();)
    {
        (*i).apply_forces();

        if ((*i).lifetime() <= 0.f)
            particles.erase(i++);
        else
            i++;
    }
}
