#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "dake/texture.h"
#include "dake/vector.h"


// A face point contains indices for a vertex, a normal
// and a texture coordinate
struct face_corner {
    int index_vertex;
    int index_normal;
    int index_texcoord;
};

struct material
{
    material(void) { tex = NULL; }

    std::string name;
    dake::vec4 ambient, diffuse, specular;
    float spec_co;
    int illum;
    const dake::texture *tex;
};

// A face is a list of face points
struct face {
    std::vector<face_corner> corners;
    const material *mat;
};


class obj_reader {

private:
    // List of points. Access this list with the method "get_vertices".
    std::vector<dake::vec3> vertices;
    // List of normals. Access this list with the method "get_normals".
    std::vector<dake::vec3> normals;
    // List of texture coordinates. Access this list with the method "get_tex_coords".
    std::vector<dake::vec2> tex_coords;
    // List of faces. Access this list with the method "get_faces".
    std::vector<face> faces;
    // List of materials.
    std::vector<material> materials;

    // The minimum and maximum point of the bounding box
    dake::vec3 bbox_min, bbox_max;

    // For internal use during loading only
    const material *current_mat;

    // This method is called for every line in the obj file that contains
    // a vertex definition.
    // The parameter "line" contains a string stream which contains the
    // complete line and has the reading position at the first entry of
    // the actual definition.
    void process_vertex(std::stringstream &line);

    // This method is called for every line in the obj file that contains
    // a normal definition and works similar to process_vertex.
    // The parameter "line" contains a string stream which contains the
    // complete line and has the reading position at the first entry of
    // the actual definition.
    void process_normal(std::stringstream &line);

    // This method is called for every line in the obj file that contains
    // a texture coordinate definition and works similar to process_vertex.
    void process_tex_coord(std::stringstream &line);

    // This method is called for every line in the obj file that contains
    // a face definition.
    // The parameter "line" contains a string stream which contains the
    // complete line and has the reading position at the first entry of
    // the actual definition.
    void process_face(std::stringstream &line);

    // This method is called for every line in the obj file that contains
    // a mtllib reference.
    void process_mtllib(std::stringstream &line);

    // nc
    void process_usemtl(std::stringstream &line);

    // Calculate the bounding box. This method is called after the mesh
    // was loaded. The results are stored in bbox_min and bbox_max.
    // They are used in get_center and get_max_extent.
    void calculate_bounding_box();

public:
    // Read the obj file whose name is stored in the variable "filename".
    // After calling this constructor, 3 lists are filled:
    //  * vertices: A list of vec3 elements that contain the vertex positions
    //  * normals: A list of normals that contain the normal positions
    //  * faces: A list of faces that contain the face positions
    // All three lists can be used from an object of this class by calling
    // the getters below.
    obj_reader(const std::string &filename);

    // Get the list of vertices
    const std::vector<dake::vec3> &get_vertices();

    // Get the list of normals
    const std::vector<dake::vec3> &get_normals();

    // Get the list of texture coordinates
    const std::vector<dake::vec2> &get_tex_coords(void);

    // Get the list of faces
    const std::vector<face> &get_faces();

    // Get a specific material
    const material &get_material(const std::string &name);

    // Get the minimum point of the bounding box
    const dake::vec3 &get_bbox_min();

    // Get the maximum point of the bounding box
    const dake::vec3 &get_bbox_max();

    std::string obj_dirname;
};
