#ifndef PARTICLES_H
#define PARTICLES_H

#include <list>

#include "vector.h"

namespace dake
{

extern vec3 force_air_drag(const vec3 &position, const vec3 &speed, float time_passed);
extern vec3 force_gravity (const vec3 &position, const vec3 &speed, float time_passed);
extern vec3 force_bounce  (const vec3 &position, const vec3 &speed, float time_passed);


class particle
{
    private:
        vec3 position, speed;
        float lifetime_rem, lifetime_full;
        int force_mask;

    public:
        enum force
        {
            AIR_DRAG,
            GRAVITY,
            BOUNCE
        };

        particle(float lifetime, const vec3 &initial_position, const vec3 &initial_speed);

        particle &add_force(force f);
        void apply_forces(float time_passed);
        void draw(void);
        float lifetime(void);
};


class particle_generator
{
    private:
        std::list<particle> particles;

    public:
        particle &new_particle(int lifetime, const vec3 &initial_position, const vec3 &initial_speed);
        void tick(float time_passed);
        void draw(void);

        static particle_generator &instance(void)
        {
            static particle_generator *pgen = NULL;
            if (!pgen) pgen = new particle_generator;
            return *pgen;
        }
};

}

#endif
