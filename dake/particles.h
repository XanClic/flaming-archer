#ifndef PARTICLES_H
#define PARTICLES_H

#include <list>

#include "obj_reader.h"


typedef vec3 (*force_apply_t)(const vec3 &position, const vec3 &speed);

extern vec3 force_air_drag(const vec3 &position, const vec3 &speed);
extern vec3 force_gravity(const vec3 &position, const vec3 &speed);
extern vec3 force_bounce(const vec3 &position, const vec3 &speed);


class particle
{
    private:
        vec3 position, speed;
        std::list<void *> forces;
        int lifetime_rem, lifetime_full;

    public:
        particle(int lifetime, const vec3 &initial_position, const vec3 &initial_speed);

        particle &add_force(force_apply_t force);
        void apply_forces(void);
        void draw(void);
        float lifetime(void);
};


class particle_generator
{
    private:
        std::list<particle> particles;

    public:
        particle &new_particle(int lifetime, const vec3 &initial_position, const vec3 &initial_speed);
        void tick(void);
        void draw(void);

        static particle_generator &instance(void)
        {
            static particle_generator *pgen = NULL;
            if (!pgen) pgen = new particle_generator;
            return *pgen;
        }
};

#endif
