#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>
#include <cgv/media/image/image_reader.h>
#include <cgv_gl/gl/gl.h>

#include "texture.h"


texture::texture(const std::string &name):
    fname(name)
{
    cgv::data::data_format df;
    cgv::media::image::image_reader ir(df);

    cgv::data::data_view dv;
    if (!ir.read_image(name, dv))
    {
        fprintf(stderr, "Could not load image: %s\n", ir.get_last_error().c_str());
        throw 23;
    }

    glGenTextures(1, &tex_id);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dv.get_format()->get_width(), dv.get_format()->get_width(), 0, GL_RGB, GL_UNSIGNED_BYTE, dv.get_ptr(0));
}


texture::~texture(void)
{
    glDeleteTextures(1, &tex_id);
}


void texture::bind(void) const
{
    glBindTexture(GL_TEXTURE_2D, tex_id);
}


texture_manager::~texture_manager(void)
{
    for (std::list<texture *>::iterator i = textures.begin(); i != textures.end(); i++)
        delete *i;
}


const texture *texture_manager::find_texture(const std::string &name)
{
    for (std::list<texture *>::iterator i = textures.begin(); i != textures.end(); i++)
        if (name == (*i)->get_fname())
            return *i;

    texture *nt = new texture(name);
    textures.push_back(nt);
    return nt;
}
