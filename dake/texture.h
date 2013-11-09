#ifndef TEXTURE_H
#define TEXTURE_H

#include <list>
#include <string>
#include <cgv_gl/gl/gl.h>


namespace dake
{

class texture
{
    private:
        GLuint tex_id;
        std::string fname;

    public:
        texture(const std::string &name);
        ~texture(void);

        void bind(void) const;

        const std::string &get_fname(void) const { return fname; }
};


class texture_manager
{
    private:
        std::list<texture *> textures;

    public:
        ~texture_manager(void);

        const texture *find_texture(const std::string &name);

        static texture_manager &instance(void)
        {
            static texture_manager *texman = NULL;
            if (!texman) texman = new texture_manager;
            return *texman;
        }
};

}

#endif
