#include "obj_reader.h"
#include "texture.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <libgen.h>

using namespace std;


obj_reader::obj_reader(const std::string &filename)
{
    // Create a new file stream and open the file
    ifstream file(filename.c_str());

    // Show an error message if the file could not be loaded
    if (!file.is_open()) {
        std::cerr<<"Error: Could not find file "<<filename<<"."<<std::endl;
        return;
    }

    char copy[filename.length() + 1];
    strcpy(copy, filename.c_str());
    obj_dirname = std::string(dirname(copy));


    static material *default_mat = NULL;
    if (!default_mat)
    {
        default_mat = new material;
        default_mat->name = std::string("__DEFAULT__");
        default_mat->ambient = vec4(0.f, 0.f, 0.f, 1.f);
        default_mat->diffuse = vec4(1.f, 1.f, 1.f, 1.f);
        default_mat->specular = vec4(1.f, 1.f, 1.f, 1.f);
        default_mat->spec_co = 100.f;
        default_mat->illum = 2;
        default_mat->tex = NULL;
    }

    current_mat = default_mat;


    // This string represents one line of the file
    string str_line;

    // Repeat the following instructions as long as there are
    // lines left to read. The lines shall end with a return (\n) character
    while(getline(file, str_line, '\n')) {
        // Convert the line from a string to a string stream.
        // The stringstream has methods to extract numbers and to
        // split the string further
        stringstream line(str_line);

        // Read characters until the first whitespace and store the
        // result in the variable "definition_type" that contains
        // the id of the definition in the rest of the line
        string definition_type;
        line>>definition_type;

        // If the definition type is "v" then a vertex is defined
        if (definition_type == "v")
            process_vertex(line);
        else
        // If the definition type is "vn" then a normal is defined
        if (definition_type == "vn")
            process_normal(line);
        else
        // If the definition type is "vt" then a texture coordinate is defined
        if (definition_type == "vt")
            process_tex_coord(line);
        else
        // If the definition type is "f" then a face is defined
        if (definition_type == "f")
            process_face(line);
        else
        // If the definition type is "mtllib" then a material lib shall be loaded
        if (definition_type == "mtllib")
            process_mtllib(line);
        else
        // If the definition type is "usemtl" then a material shall be used
        if (definition_type == "usemtl")
            process_usemtl(line);
    }

    // All done. Close this file
    file.close();

    // Calculate the bounding box
    calculate_bounding_box();
}




// Calculate the bounding box. This method is called after the mesh
// was loaded. The results are stored in bbox_min and bbox_max.
// They are used in get_center and get_max_extent.
void obj_reader::calculate_bounding_box()
{
    // Set the minimum point to a very large value and the maximum
    // point to a very small value.
    bbox_max = -std::numeric_limits<float>::max();
    bbox_min = -bbox_max;

    // *** Begin of task 1.2.2 (1) ***
    // Iterate over all points and calculate the minimum and maximum point
    // of the bounding box. The minimum point pmin is defined by the component
    // wise minimum of all points while the maximum point pmax is defined by the
    // component wise maximum.
    // Hint: When you work with variables from type vec3, standard
    // C++-operators can be used.
    // For example:
    //  - "pt1 - pt2" performs a component wise subtraction
    //  - "pt1 * pt2" performs a component wise multiplication
    // To access a component of a vec3, you can use the []-operator.
    // for example pt1[0] accesses the first component of "pt1" etc.

    // for (auto vertex: vertices)
    for (vector<vec3>::iterator vi = vertices.begin(); vi != vertices.end(); vi++)
    {
        const vec3 &vertex = *vi;

        for (int i = 0; i < 3; i++)
        {
            if (vertex[i] < bbox_min[i])
                bbox_min[i] = vertex[i];
            if (vertex[i] > bbox_max[i])
                bbox_max[i] = vertex[i];
        }
    }


    // *** End of task 1.2.2 (1) ***
}




// Get the minimum point of the bounding box
const vec3& obj_reader::get_bbox_min()
{
    return bbox_min;
}




// Get the maximum point of the bounding box
const vec3& obj_reader::get_bbox_max()
{
    return bbox_max;
}




// This method is called for every line in the obj file that contains
// a vertex definition.
// The parameter "line" contains a string stream which contains the
// complete line and has the reading position at the first entry of
// the actual definition.
void obj_reader::process_vertex(std::stringstream &line)
{
    // The method must parse the string stream and read 3 floating point
    // values which are stored into a variable of the type "vec3". This
    // variable must be added to the list of vertices.
    vec3 new_vertex;
    // Extract 3 floating point numbers from the line
    // and store them in x(), y() or z().
    line>>new_vertex.x()>>new_vertex.y()>>new_vertex.z();

    // Add this vertex to the list of vertices
    vertices.push_back(new_vertex);
}




// This method is called for every line in the obj file that contains
// a normal definition and works similar to process_vertex.
// The parameter "line" contains a string stream which contains the
// complete line and has the reading position at the first entry of
// the actual definition.
void obj_reader::process_normal(std::stringstream &line)
{
    // The method must parse the string stream and read 3 floating point
    // values which are stored into a variable of the type "vec3". This
    // variable must be added to the list of normales.

    // *** Begin of task 1.2.3 ***
    // The parameter "line" is a string stream that contains the line
    // of the .obj-file where a normal is defined. The reading pointer
    // if this stream lies at the first normal coordinate.
    // Take a look at process_vertex for hints on this task.
    vec3 new_normal;
    line >> new_normal.x() >> new_normal.y() >> new_normal.z();
    normals.push_back(new_normal);


    // *** End of task 1.2.3 ***
}



void obj_reader::process_tex_coord(std::stringstream &line)
{
    vec2 new_tex_coord;
    line >> new_tex_coord.x() >> new_tex_coord.y();
    tex_coords.push_back(new_tex_coord);
}




// This method is called for every line in the obj file that contains
// a face definition.
// The parameter "line" contains a string stream which contains the
// complete line and has the reading position at the first entry of
// the actual definition.
void obj_reader::process_face(std::stringstream &line)
{
    // The method must parse the line and read the definition. More information
    // on how this line is defined can be found in the body of this method.
    face new_face;

    new_face.mat = current_mat;

    // *** Begin of task 1.2.4 ****
    // The parameter "line" is a string stream that contains the line
    // of the .obj-file where a normal is defined. The reading pointer
    // if this stream lies at the first face point.
    // The result of this method is the filled variable "new_face"
    // that has the type "face". A face has one property "corners" which
    // is a list of "face_point"s.

    // Split the line into face points that are separated by whitespaces
    string entry;
    while(getline(line, entry, ' ')) {

        // Do nothing if the variable "entry" does not contain any data
        if (entry.empty())
            continue;

        // Make a stream from this face point definition
        stringstream entry_stream(entry);

        face_corner new_corner;
        // Your task is to fill the new face corner "new_corner". First,
        // split the stringstream "entry_stream" on every occurence of
        // a slash. You can use "getline", just as used above, here.
        // Set new_corner.index_vertex to the read vertex index,
        // new_corner.index_texcoord to the read texture coordinate index
        // or -1 if no index exists and new_corner.index_normal to the
        // read normal index or -1.
        // For the conversion of a string to an integer you can use
        // the helper method cgv::utils::from_string. The method returns
        // true if the conversion was successful and false otherwise
        // For example:
        //   string number = "12.3";
        //   float result;
        //   if (!cgv::utils::from_string(result, number))
        //        result = -1;
        // The code above tries to convert the string "number" and stores
        // the result in "result". If the conversion was not successful
        // then "result" is set to -1.

        string indexstr;
        for (int i = 0; getline(entry_stream, indexstr, '/'); i++)
        {
            int index;
            char *end;

            if (indexstr.empty())
                index = -1;
            else
            {
                index = strtol(indexstr.c_str(), &end, 10);
                if (*end)
                    throw 42;
            }

            switch (i)
            {
                case 0: new_corner.index_vertex = index; break;
                case 1: new_corner.index_texcoord = index; break;
                case 2: new_corner.index_normal = index; break;
                default: throw 42;
            }
        }

        // Finally, add the new corner to the list "corners" of new_face.
        new_face.corners.push_back(new_corner);

        // ...
    }

    // Remember to add the newly created face to the end of the
    // list "faces" here.
    faces.push_back(new_face);

    // *** End of task 1.2.4 ***
}



void obj_reader::process_mtllib(std::stringstream &line)
{
    std::string remaining;
    line >> remaining;
    if (remaining[0] != '/')
        remaining = obj_dirname + "/" + remaining;

    std::ifstream file(remaining.c_str());
    if (!file.is_open())
    {
        fprintf(stderr, "Could not open material lib %s\n", remaining.c_str());
        throw 42;
    }

    material *mat = NULL;

    std::string mtl_str_line;
    while (std::getline(file, mtl_str_line, '\n'))
    {
        std::stringstream mtl_line(mtl_str_line);

        std::string deftype;
        mtl_line >> deftype;

        if (deftype == "newmtl")
        {
            if (mat)
            {
                materials.push_back(*mat);
                delete mat;
            }

            mat = new material;
            mtl_line >> mat->name;
            mat->ambient.w() = mat->diffuse.w() = mat->specular.w() = 1.f;
        }
        else if (deftype == "Ka")
            mtl_line >> mat->ambient.x() >> mat->ambient.y() >> mat->ambient.z();
        else if (deftype == "Kd")
            mtl_line >> mat->diffuse.x() >> mat->diffuse.y() >> mat->diffuse.z();
        else if (deftype == "Ks")
            mtl_line >> mat->specular.x() >> mat->specular.y() >> mat->specular.z();
        else if (deftype == "Ni")
            mtl_line >> mat->spec_co;
        else if ((deftype == "d") || (deftype == "Tr"))
        {
            float alpha;
            mtl_line >> alpha;
            mat->ambient.w() = mat->diffuse.w() = mat->specular.w() = alpha;
        }
        else if (deftype == "illum")
            mtl_line >> mat->illum;
        else if (deftype == "map_Kd")
        {
            std::string fname;
            mtl_line >> fname;
            if (fname != ".")
            {
                if (fname[0] != '/')
                    fname = obj_dirname + "/" + fname;
                mat->tex = dake::texture_manager::instance().find_texture(fname);
            }
        }
    }

    if (mat)
    {
        materials.push_back(*mat);
        delete mat;
    }
}



void obj_reader::process_usemtl(std::stringstream &line)
{
    std::string name;
    line >> name;

    current_mat = &get_material(name);
}



// Get the list of vertices
const vector<vec3> &obj_reader::get_vertices() {
    return vertices;
}




// Get the list of normals
const vector<vec3> &obj_reader::get_normals() {
    return normals;
}



// Get the list of texture coordinates
const vector<vec2> &obj_reader::get_tex_coords(void) {
    return tex_coords;
}




// Get the list of faces
const vector<face> &obj_reader::get_faces() {
    return faces;
}



const material &obj_reader::get_material(const std::string &name)
{
    for (std::vector<material>::iterator i = materials.begin(); i != materials.end(); i++)
        if ((*i).name == name)
            return *i;

    fprintf(stderr, "Could not find material %s\n", name.c_str());
    throw 84;
}
