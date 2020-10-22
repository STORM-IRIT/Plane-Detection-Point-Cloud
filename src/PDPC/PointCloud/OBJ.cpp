#include <PDPC/PointCloud/OBJ.h>
#include <PDPC/PointCloud/PointCloud.h>

#include <PDPC/Common/Log.h>

#include <fstream>

namespace pdpc {

inline void print_face(std::ostream& os, int v0, int v1, int v2, bool has_uv, bool has_normal)
{
         if( has_uv &&  has_normal) os << "f " << v0 << "/"  << v0 << "/" << v0 << " " << v1 << "/"  << v1 << "/" << v1 << " " << v2 << "/"  << v2 << "/" << v2 << "\n";
    else if( has_uv && !has_normal) os << "f " << v0 << "/"  << v0              << " " << v1 << "/"  << v1              << " " << v2 << "/"  << v2              << "\n";
    else if(!has_uv &&  has_normal) os << "f " << v0 << "//" << v0              << " " << v1 << "//" << v1              << " " << v2 << "//" << v2              << "\n";
    else                            os << "f " << v0                            << " " << v1                            << " " << v2                            << "\n";
}

inline bool start_by(const std::string& line, const std::string& str)
{
    return line.size() >= str.size() && line.substr(0,str.size()) == str;
}

inline int simple_slash_count(const std::string& line)
{
    return std::count(line.begin(), line.end(), '/');
}

inline int double_slash_count(const std::string& line)
{
    int count = 0;
    for(auto it = line.find("//", 0); it != std::string::npos; it = line.find("//", it+1))
    {
        ++count;
    }
    return count;
}

bool OBJ::load(const std::string& filename, PointCloud& g, bool v)
{
    std::string tmp;
    return load(filename, g, tmp, v);
}

bool OBJ::load_soup(const std::string& filename, PointCloud& g, bool v)
{
    std::string tmp;
    return load_soup(filename, g, tmp, v);
}

std::string absolute_path(const std::string& obj_filename, const std::string& mtl_filename)
{
    auto filename = mtl_filename;
    if(!mtl_filename.empty() && mtl_filename.front() != '/')
    {
        const auto slash = obj_filename.find_last_of('/');
        const auto path  = obj_filename.substr(0, slash);
        filename = path + '/' + mtl_filename;
    }
    return filename;
}

std::string extract_texture(const std::string& filename, bool v)
{
    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return "";
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        const auto i = line.find_first_of(' ');
        const auto type = line.substr(0,i);
        if(type == "map_Kd")
        {
            return absolute_path(filename, line.substr(i+1));
        }
    }
    warning().iff(v) << "Texture map_Kd not found in " << filename;
    return "";
}

//!
//! - no object
//! - no group
//! - no vertex color
//! - no w texture coordinate
//! - zero or one mtl link = zero or one diffuse texture
//!
bool OBJ::load(const std::string& filename, PointCloud& g, std::string& texture, bool v)
{
    g = PointCloud();
    texture = "";

    std::string mtlfilename;

    g.request_normals();
    g.request_uv();

    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return false;
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        const auto i = line.find_first_of(' ');
        const auto type = line.substr(0,i);
        line = line.substr(i+1);

        std::istringstream str(line);

        if(type == "mtllib")
        {
            mtlfilename = absolute_path(filename, line);
            texture = extract_texture(mtlfilename, v);
        }
        else if(type == "v")
        {
            Scalar x, y, z;
            if(str >> x && str >> y && str >> z)
            {
                g.points_data().push_back(Vector3(x,y,z));
            }
        }
        else if(type == "vt")
        {
            Scalar u, v;
            if(str >> u && str >> v)
            {
                g.uv_data().push_back(Vector2(u,v));
            }
        }
        else if(type == "vn")
        {
            Scalar nx, ny, nz;
            if(str >> nx && str >> ny && str >> nz)
            {
                g.normals_data().push_back(Vector3(nx,ny,nz));
            }
        }
        else if(type == "f")
        {
            const int double_count = double_slash_count(line);
            const int simple_count = simple_slash_count(line);

            int v1, v2, v3, vt1, vt2, vt3, vn1, vn2, vn3;
            char c;

            if(double_count == 3)
            {
                if(str >> v1  &&
                   str >> c   && c=='/' &&
                   str >> c   && c=='/' &&
                   str >> vn1 &&
                   str >> v2  &&
                   str >> c   && c=='/' &&
                   str >> c   && c=='/' &&
                   str >> vn2 &&
                   str >> v3  &&
                   str >> c   && c=='/' &&
                   str >> c   && c=='/' &&
                   str >> vn3)
                {
                    g.faces_data().push_back(Vector3i(v1-1,v2-1,v3-1));
                }
            }
            else if(simple_count == 0)
            {
                if(str >> v1 &&
                   str >> v2 &&
                   str >> v3)
                {
                    g.faces_data().push_back(Vector3i(v1-1,v2-1,v3-1));
                }
            }
            else if(simple_count == 3)
            {
                if(str >> v1  &&
                   str >> c   && c=='/' &&
                   str >> vt1 &&
                   str >> v2  &&
                   str >> c   && c=='/' &&
                   str >> vt2 &&
                   str >> v3  &&
                   str >> c   && c=='/' &&
                   str >> vt3)
                {
                    g.faces_data().push_back(Vector3i(v1-1,v2-1,v3-1));
                }
            }
            else if(simple_count == 6)
            {
                if(str >> v1  &&
                   str >> c   && c=='/' &&
                   str >> vt1 &&
                   str >> c   && c=='/' &&
                   str >> vn1 &&
                   str >> v2  &&
                   str >> c   && c=='/' &&
                   str >> vt2 &&
                   str >> c   && c=='/' &&
                   str >> vn2 &&
                   str >> v3  &&
                   str >> c   && c=='/' &&
                   str >> vt3 &&
                   str >> c   && c=='/' &&
                   str >> vn3)
                {
                    g.faces_data().push_back(Vector3i(v1-1,v2-1,v3-1));
                }
            }
        }
    }

    if(g.normals_data().empty()) g.remove_normals();
    if(g.uv_data().empty()) g.remove_uv();

    info().iff(v) << g.size() << " points readed from '" << filename << "'...";
    info().iff(v) << "    points  : " << g.size();
    info().iff(v) << "    normals : " << (g.has_normals() ? "yes":"no");
    info().iff(v) << "    colors  : " << (g.has_colors()  ? "yes":"no");
    info().iff(v) << "    uv      : " << (g.has_uv()      ? "yes":"no");
    info().iff(v) << "    faces   : " << g.face_count();
    info().iff(v) << "    texture : " << (texture.empty() ? "no" : texture + " (" + mtlfilename + ")");

    return true;
}

//!
//! same as previous load() function but handle vertex where indices are not the
//! same for vertex/texture/normal
//!
bool OBJ::load_soup(const std::string& filename, PointCloud& g, std::string& texture, bool v)
{
    g = PointCloud();
    texture = "";

    std::string mtlfilename;

    g.request_normals();
    g.request_uv();

    Vector3Array positions;
    Vector2Array tex_coords;
    Vector3Array normals;

    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return false;
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        const auto i = line.find_first_of(' ');
        const auto type = line.substr(0,i);
        line = line.substr(i+1);

        std::istringstream str(line);

        if(type == "mtllib")
        {
            mtlfilename = absolute_path(filename, line);
            texture = extract_texture(mtlfilename, v);
        }
        else if(type == "v")
        {
            Scalar x, y, z;
            if(str >> x && str >> y && str >> z)
            {
                positions.push_back(Vector3(x,y,z));
            }
        }
        else if(type == "vt")
        {
            Scalar u, v;
            if(str >> u && str >> v)
            {
                tex_coords.push_back(Vector2(u,v));
            }
        }
        else if(type == "vn")
        {
            Scalar nx, ny, nz;
            if(str >> nx && str >> ny && str >> nz)
            {
                normals.push_back(Vector3(nx,ny,nz));
            }
        }
        else if(type == "f")
        {
            const int double_count = double_slash_count(line);
            const int simple_count = simple_slash_count(line);

            int v1, v2, v3, vt1, vt2, vt3, vn1, vn2, vn3;
            char c;

            if(double_count == 3)
            {
                if(str >> v1  &&
                   str >> c   && c=='/' &&
                   str >> c   && c=='/' &&
                   str >> vn1 &&
                   str >> v2  &&
                   str >> c   && c=='/' &&
                   str >> c   && c=='/' &&
                   str >> vn2 &&
                   str >> v3  &&
                   str >> c   && c=='/' &&
                   str >> c   && c=='/' &&
                   str >> vn3)
                {
                    const int idx = g.vertex_count();
                    g.vertices_data().push_back(positions[v1-1]);
                    g.vertices_data().push_back(positions[v2-1]);
                    g.vertices_data().push_back(positions[v3-1]);
                    g.normals_data().push_back(normals[vn1-1]);
                    g.normals_data().push_back(normals[vn2-1]);
                    g.normals_data().push_back(normals[vn3-1]);
                    g.faces_data().push_back(Vector3i(idx,idx+1,idx+2));
                }
            }
            else if(simple_count == 0)
            {
                if(str >> v1 &&
                   str >> v2 &&
                   str >> v3)
                {
                    const int idx = g.vertex_count();
                    g.vertices_data().push_back(positions[v1-1]);
                    g.vertices_data().push_back(positions[v2-1]);
                    g.vertices_data().push_back(positions[v3-1]);
                    g.faces_data().push_back(Vector3i(idx,idx+1,idx+2));
                }
            }
            else if(simple_count == 3)
            {
                if(str >> v1  &&
                   str >> c   && c=='/' &&
                   str >> vt1 &&
                   str >> v2  &&
                   str >> c   && c=='/' &&
                   str >> vt2 &&
                   str >> v3  &&
                   str >> c   && c=='/' &&
                   str >> vt3)
                {
                    const int idx = g.vertex_count();
                    g.vertices_data().push_back(positions[v1-1]);
                    g.vertices_data().push_back(positions[v2-1]);
                    g.vertices_data().push_back(positions[v3-1]);
                    g.uv_data().push_back(tex_coords[vt1-1]);
                    g.uv_data().push_back(tex_coords[vt2-1]);
                    g.uv_data().push_back(tex_coords[vt3-1]);
                    g.faces_data().push_back(Vector3i(idx,idx+1,idx+2));
                }
            }
            else if(simple_count == 6)
            {
                if(str >> v1  &&
                   str >> c   && c=='/' &&
                   str >> vt1 &&
                   str >> c   && c=='/' &&
                   str >> vn1 &&
                   str >> v2  &&
                   str >> c   && c=='/' &&
                   str >> vt2 &&
                   str >> c   && c=='/' &&
                   str >> vn2 &&
                   str >> v3  &&
                   str >> c   && c=='/' &&
                   str >> vt3 &&
                   str >> c   && c=='/' &&
                   str >> vn3)
                {
                    const int idx = g.vertex_count();
                    g.vertices_data().push_back(positions[v1-1]);
                    g.vertices_data().push_back(positions[v2-1]);
                    g.vertices_data().push_back(positions[v3-1]);
                    g.normals_data().push_back(normals[vn1-1]);
                    g.normals_data().push_back(normals[vn2-1]);
                    g.normals_data().push_back(normals[vn3-1]);
                    g.uv_data().push_back(tex_coords[vt1-1]);
                    g.uv_data().push_back(tex_coords[vt2-1]);
                    g.uv_data().push_back(tex_coords[vt3-1]);
                    g.faces_data().push_back(Vector3i(idx,idx+1,idx+2));
                }
            }
        }
    }

    if(g.normals_data().empty()) g.remove_normals();
    if(g.uv_data().empty()) g.remove_uv();

    // point cloud
    if(g.face_count() == 0)
    {
        g.points_data() = positions;
        if(!normals.empty()) g.request_normals(normals);
        if(!tex_coords.empty()) g.request_uv(tex_coords);
    }

    info().iff(v) << g.size() << " points readed from '" << filename << "'...";
    info().iff(v) << "    points  : " << g.size();
    info().iff(v) << "    normals : " << (g.has_normals() ? "yes":"no");
    info().iff(v) << "    colors  : " << (g.has_colors()  ? "yes":"no");
    info().iff(v) << "    uv      : " << (g.has_uv()      ? "yes":"no");
    info().iff(v) << "    faces   : " << g.face_count();
    info().iff(v) << "    texture : " << (texture.empty() ? "no" : texture + " (" + mtlfilename + ")");

    return true;
}

bool OBJ::save(const std::string& filename, const PointCloud& g, bool v)
{
    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return false;
    }

    info().iff(v) << "Saving " << g.size() << " points to '" << filename << "'...";

    for(int i=0; i<g.vertex_count(); ++i)
    {
        ofs << "v "
            << g.point(i).x() << " "
            << g.point(i).y() << " "
            << g.point(i).z() << "\n";
    }
    if(g.has_uv())
    {
        for(int i=0; i<g.vertex_count(); ++i)
        {
            ofs << "vt "
                << g.uv(i).x() << " "
                << g.uv(i).y() << "\n";
        }
    }
    if(g.has_normals())
    {
        for(int i=0; i<g.vertex_count(); ++i)
        {
            ofs << "vn "
                << g.normal(i).x() << " "
                << g.normal(i).y() << " "
                << g.normal(i).z() << "\n";
        }
    }
    for(int i=0; i<g.face_count(); ++i)
    {
        const int v0 = g.face(i)[0] + 1;
        const int v1 = g.face(i)[1] + 1;
        const int v2 = g.face(i)[2] + 1;
        print_face(ofs, v0, v1, v2, g.has_uv(), g.has_normals());
    }

    info().iff(v) << "    points  : " << g.size();
    info().iff(v) << "    normals : " << (g.has_normals() ? "yes":"no");
    info().iff(v) << "    colors  : " << (g.has_colors()  ? "yes":"no");
    info().iff(v) << "    uv      : " << (g.has_uv()      ? "yes":"no");
    info().iff(v) << "    faces   : " << g.face_count();

    return true;
}

bool OBJ::save(const std::string& filename, const PointCloud& g, const std::string& texture, bool v)
{
    const auto dot = filename.find_last_of('.');
    if(dot == std::string::npos)
    {
        warning().iff(v) << "Missing .obj extension in " << filename;
        return false;
    }
    const std::string mtlfilename = filename.substr(0, dot+1) + "mtl";

    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        warning().iff(v) << "Failed to open file " << filename;
        return false;
    }

    info().iff(v) << "Saving " << g.size() << " points to '" << filename << "'...";

    ofs << "mtllib " << mtlfilename << "\n";
    ofs << "usemtl default_material\n";

    for(int i=0; i<g.vertex_count(); ++i)
    {
        ofs << "v "
            << g.point(i).x() << " "
            << g.point(i).y() << " "
            << g.point(i).z() << "\n";
    }
    if(g.has_uv())
    {
        for(int i=0; i<g.vertex_count(); ++i)
        {
            ofs << "vt "
                << g.uv(i).x() << " "
                << g.uv(i).y() << "\n";
        }
    }
    if(g.has_normals())
    {
        for(int i=0; i<g.vertex_count(); ++i)
        {
            ofs << "vn "
                << g.normal(i).x() << " "
                << g.normal(i).y() << " "
                << g.normal(i).z() << "\n";
        }
    }
    for(int i=0; i<g.face_count(); ++i)
    {
        const int v0 = g.face(i)[0] + 1;
        const int v1 = g.face(i)[1] + 1;
        const int v2 = g.face(i)[2] + 1;
        print_face(ofs, v0, v1, v2, g.has_uv(), g.has_normals());
    }

    ofs.close();
    ofs.open(mtlfilename);
    if(!ofs.is_open())
    {
        warning().iff(v) << "Failed to open file " << mtlfilename;
        return false;
    }
    ofs << "newmtl default_material" << "\n";
    ofs << "Ka 1.000 1.000 1.000"    << "\n";
    ofs << "Kd 1.000 1.000 1.000"    << "\n";
    ofs << "Ks 0.000 0.000 0.000"    << "\n";
    ofs << "d 1.0"                   << "\n";
    ofs << "illum 2"                 << "\n";
    ofs << "map_Kd " << texture      << "\n";

    info().iff(v) << "    points  : " << g.size();
    info().iff(v) << "    normals : " << (g.has_normals() ? "yes":"no");
    info().iff(v) << "    colors  : " << (g.has_colors()  ? "yes":"no");
    info().iff(v) << "    uv      : " << (g.has_uv()      ? "yes":"no");
    info().iff(v) << "    faces   : " << g.face_count();
    info().iff(v) << "    texture : " << texture << " (" << mtlfilename << ")";

    return true;
}

} // namespace pdpc
