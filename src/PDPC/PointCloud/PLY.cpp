#include <PDPC/PointCloud/PLY.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/PointCloud/internal/plyio.h>

#include <PDPC/Common/Log.h>

#include <fstream>

namespace pdpc {

bool PLY::load(const std::string& filename, PointCloud& g, bool v)
{
    constexpr Scalar coeff = 1./255;

    g = PointCloud();

    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return false;
    }

    plyio::PLYReader ply;
    ply.read_header(ifs);

    const int vertex_count = ply.element_count("vertex");
    const int face_count   = ply.element_count("face");
    info().iff(v) << "Reading " << vertex_count << " points from '" << filename << "'...";

    g.resize_points(vertex_count);
    g.reserve_faces(face_count); // reserve because of potential quads

    int quad_count = 0;
    int not_loaded_count = 0;

    for(auto& e : ply.elements())
    {
        if(e.name == "vertex")
        {
            for(auto& p : e.properties)
            {
                if(p.name == "x")
                {
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float x) {g.point(i).x() = x;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double x){g.point(i).x() = x;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property x";
                    }
                }
                else if(p.name == "y")
                {
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float y) {g.point(i).y() = y;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double y){g.point(i).y() = y;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property y";
                    }
                }
                else if(p.name == "z")
                {
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float z) {g.point(i).z() = z;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double z){g.point(i).z() = z;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property z";
                    }
                }
                else if(p.name == "nx")
                {
                    g.request_normals(Vector3::Zero());
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float nx) {g.normal(i).x() = nx;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double nx){g.normal(i).x() = nx;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property nx";
                    }
                }
                else if(p.name == "ny")
                {
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float ny) {g.normal(i).y() = ny;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double ny){g.normal(i).y() = ny;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property ny";
                    }
                }
                else if(p.name == "nz")
                {
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float nz) {g.normal(i).z() = nz;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double nz){g.normal(i).z() = nz;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property nz";
                    }
                }
                else if(p.name == "red")
                {
                    g.request_colors(Vector4(0,0,0,1));
                    switch(p.type)
                    {
                    case plyio::type_float: p.reader.read_float_f() = [&](int i, float red)        {g.color(i)[0] = red;};                 break;
                    case plyio::type_uchar: p.reader.read_uchar_f() = [&](int i, unsigned char red){g.color(i)[0] = coeff * Scalar(red);}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property red";
                    }
                }
                else if(p.name == "green")
                {
                    switch(p.type)
                    {
                    case plyio::type_float: p.reader.read_float_f() = [&](int i, float green)        {g.color(i)[1] = green;};                 break;
                    case plyio::type_uchar: p.reader.read_uchar_f() = [&](int i, unsigned char green){g.color(i)[1] = coeff * Scalar(green);}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property green";
                    }
                }
                else if(p.name == "blue")
                {
                    switch(p.type)
                    {
                    case plyio::type_float: p.reader.read_float_f() = [&](int i, float blue)        {g.color(i)[2] = blue;};                 break;
                    case plyio::type_uchar: p.reader.read_uchar_f() = [&](int i, unsigned char blue){g.color(i)[2] = coeff * Scalar(blue);}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property blue";
                    }
                }
                else if(p.name == "alpha")
                {
                    switch(p.type)
                    {
                    case plyio::type_float: p.reader.read_float_f() = [&](int i, float alpha)        {g.color(i)[3] = alpha;};                 break;
                    case plyio::type_uchar: p.reader.read_uchar_f() = [&](int i, unsigned char alpha){g.color(i)[3] = coeff * Scalar(alpha);}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property alpha";
                    }
                }
                else if(p.name == "u")
                {
                    g.request_uv(Vector2::Zero());
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float  u) {g.uv(i)[0] = u;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double u) {g.uv(i)[0] = u;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property u";
                    }
                }
                else if(p.name == "v")
                {
                    switch(p.type)
                    {
                    case plyio::type_float:  p.reader.read_float_f()  = [&](int i, float  v) {g.uv(i)[1] = v;}; break;
                    case plyio::type_double: p.reader.read_double_f() = [&](int i, double v) {g.uv(i)[1] = v;}; break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for vertex property v";
                    }
                }
            }
        }
        else if(e.name == "face")
        {
            for(auto& p : e.properties)
            {
                if(p.name == "vertex_indices")
                {
                    switch(p.type)
                    {
                    case plyio::type_int:
                        p.reader.read_int_list_f() = [&](int, const std::vector<int>& idx)
                        {
                            if(idx.size() == 3)
                            {
                                g.faces_data().emplace_back(idx[0],idx[1],idx[2]);
                            }
                            else if(idx.size() == 4)
                            {
                                ++quad_count;
                                g.faces_data().emplace_back(idx[0],idx[1],idx[2]);
                                g.faces_data().emplace_back(idx[0],idx[2],idx[3]);
                            }
                            else
                            {
                                ++not_loaded_count;
                            }
                        };
                        break;
                    case plyio::type_uint:
                        p.reader.read_uint_list_f() = [&](int, const std::vector<uint>& idx)
                        {
                            if(idx.size() == 3)
                            {
                                g.faces_data().emplace_back(idx[0],idx[1],idx[2]);
                            }
                            else if(idx.size() == 4)
                            {
                                ++quad_count;
                                g.faces_data().emplace_back(idx[0],idx[1],idx[2]);
                                g.faces_data().emplace_back(idx[0],idx[2],idx[3]);
                            }
                            else
                            {
                                ++not_loaded_count;
                            }
                        };
                        break;
                    default: warning().iff(v) << "Type " << plyio::to_string(p.type) << " not supported for face property vertex_indices";
                    }
                }
                else
                {
                    warning().iff(v) << "Property " << p.name << " not supported for faces";
                }
            }
        }
        else
        {
            warning().iff(v) << "Unkown element " << e.name;
        }
    }

    ply.read_body(ifs);

    info().iff(v) << "    points  : " << g.size();
    info().iff(v) << "    normals : " << (g.has_normals() ? "yes":"no");
    info().iff(v) << "    colors  : " << (g.has_colors()  ? "yes":"no");
    info().iff(v) << "    uv      : " << (g.has_uv()      ? "yes":"no");
    info().iff(v) << "    faces   : " << g.face_count()
                  << (quad_count>0 ? " (" + std::to_string(quad_count) + " quads)":"")
                  << (not_loaded_count>0 ? " (" + std::to_string(not_loaded_count) + " polygons not loaded)":"");
    return true;
}

bool PLY::save(const std::string& filename, const PointCloud& g, bool v, bool binary)
{
    using uchar = unsigned char;

    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return false;
    }

    info().iff(v) << "Saving " << g.size() << " points to '" << filename << "'...";

    plyio::PLYWriter ply;
    if(binary)
    {
        ply.set_binary();
    }
    else
    {
        ply.set_ascii();
    }

    ply.add_comment("PDPC");
    ply.add_element("vertex", g.point_count());
    ply.add_element("face", g.face_count());

    ply.add_property("vertex", "x", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.point(i).x());};
    ply.add_property("vertex", "y", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.point(i).y());};
    ply.add_property("vertex", "z", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.point(i).z());};

    if(g.has_normals())
    {
        ply.add_property("vertex", "nx", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.normal(i).x());};
        ply.add_property("vertex", "ny", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.normal(i).y());};
        ply.add_property("vertex", "nz", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.normal(i).z());};
    }

    if(g.has_colors())
    {
        ply.add_property("vertex", "red",   plyio::type_uchar).writer.write_uchar_f() = [&](int i){return uchar(255*g.color(i)[0]);};
        ply.add_property("vertex", "green", plyio::type_uchar).writer.write_uchar_f() = [&](int i){return uchar(255*g.color(i)[1]);};
        ply.add_property("vertex", "blue",  plyio::type_uchar).writer.write_uchar_f() = [&](int i){return uchar(255*g.color(i)[2]);};
        ply.add_property("vertex", "alpha", plyio::type_uchar).writer.write_uchar_f() = [&](int i){return uchar(255*g.color(i)[3]);};
    }

    if(g.has_uv())
    {
        ply.add_property("vertex", "u", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.uv(i)[0]);};
        ply.add_property("vertex", "v", plyio::type_float).writer.write_float_f() = [&](int i){return float(g.uv(i)[1]);};
    }

    auto& f = ply.add_property("face", "vertex_indices", plyio::type_int, plyio::type_uchar);
    f.writer.size_uchar_f() = [](int){return uchar(3);};
    f.writer.write_int_list_f() = [&](int i, int k){return int(g.face(i)[k]);};

    ply.write(ofs);

    info().iff(v) << "    points  : " << g.size();
    info().iff(v) << "    normals : " << (g.has_normals() ? "yes":"no");
    info().iff(v) << "    colors  : " << (g.has_colors()  ? "yes":"no");
    info().iff(v) << "    uv      : " << (g.has_uv()      ? "yes":"no");
    info().iff(v) << "    faces   : " << g.face_count();

    return true;
}

} // namespace pdpc
