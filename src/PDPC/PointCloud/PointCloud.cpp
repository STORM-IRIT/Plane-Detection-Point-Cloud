#include <PDPC/PointCloud/PointCloud.h>

#include <PDPC/SpacePartitioning/KdTree.h>
#include <PDPC/SpacePartitioning/KnnGraph.h>

#include <PDPC/Common/std_vector_algo.h>

namespace pdpc {

// PointCloud ------------------------------------------------------------------

PointCloud::PointCloud(int n, int m) :
    m_points(std::make_shared<Vector3Array>(n)),
    m_normals(nullptr),
    m_colors(nullptr),
    m_uv(nullptr),
    m_faces(std::make_shared<Vector3iArray>(m)),
    m_kdtree(nullptr),
    m_knngraph(nullptr)
{
}

PointCloud::PointCloud(const PointCloud& other) :
    m_points(std::make_shared<Vector3Array>(other.points_data())),
    m_normals(nullptr),
    m_colors(nullptr),
    m_uv(nullptr),
    m_faces(std::make_shared<Vector3iArray>(other.faces_data())),
    m_kdtree(nullptr),
    m_knngraph(nullptr)
{
    if(other.has_normals()) request_normals(other.normals_data());
    if(other.has_colors()) request_colors(other.colors_data());
    if(other.has_uv()) request_uv(other.uv_data());
}

PointCloud::PointCloud(PointCloud&& other) :
    m_points(std::move(other.m_points)),
    m_normals(std::move(other.m_normals)),
    m_colors(std::move(other.m_colors)),
    m_uv(std::move(other.m_uv)),
    m_faces(std::move(other.m_faces)),
    m_kdtree(std::move(other.m_kdtree)),
    m_knngraph(std::move(other.m_knngraph))
{
}

PointCloud& PointCloud::operator = (const PointCloud& other)
{
    points_data() = other.points_data();
    faces_data() = other.faces_data();

    if(other.has_normals()) request_normals(other.normals_data()); else remove_normals();
    if(other.has_colors()) request_colors(other.colors_data());    else remove_colors();
    if(other.has_uv()) request_uv(other.uv_data());                else remove_uv();

    clear_kdtree();
    clear_knn_graph();

    return *this;
}

PointCloud& PointCloud::operator = (PointCloud&& other)
{
    m_points   = std::move(other.m_points);
    m_normals  = std::move(other.m_normals);
    m_colors   = std::move(other.m_colors);
    m_uv       = std::move(other.m_uv);
    m_faces    = std::move(other.m_faces);
    m_kdtree   = std::move(other.m_kdtree);
    m_knngraph = std::move(other.m_knngraph);

    return *this;
}

PointCloud PointCloud::to_soup() const
{
    PointCloud soup(3 * face_count(), face_count());
    if(has_normals()) soup.request_normals();
    if(has_colors())  soup.request_colors();
    if(has_uv())      soup.request_uv();

    int v = 0;
    for(int f=0; f<face_count(); ++f)
    {
        const Vector3i& face = this->face(f);
        for(int k=0; k<3; ++k)
        {
            soup.vertex(v+k) = vertex(face[k]);
            if(has_normals()) soup.normal(v+k) = this->normal(face[k]);
            if(has_colors())  soup.color(v+k)  = this->color(face[k]);
            if(has_uv())      soup.uv(v+k)     = this->uv(face[k]);
        }
        soup.face(f) = Vector3i(v+0, v+1, v+2);
        v += 3;
    }
    return soup;
}

// IO --------------------------------------------------------------------------

std::ostream& PointCloud::write(std::ostream& os) const
{
    const int  point_count = this->point_count();
    const bool has_normals = this->has_normals();
    const bool has_colors  = this->has_colors();
    const bool has_uv      = this->has_uv();
    const int  face_count  = this->face_count();
    os.write(reinterpret_cast<const char*>(&point_count), sizeof(int));
    os.write(reinterpret_cast<const char*>(&has_normals), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&has_colors),  sizeof(bool));
    os.write(reinterpret_cast<const char*>(&has_uv),      sizeof(bool));
    os.write(reinterpret_cast<const char*>(&face_count),  sizeof(int));

    os.write(reinterpret_cast<const char*>(m_points->data()), point_count*3*sizeof(Scalar));
    if(has_normals) os.write(reinterpret_cast<const char*>(m_normals->data()), point_count*3*sizeof(Scalar));
    if(has_colors) os.write(reinterpret_cast<const char*>(m_colors->data()), point_count*4*sizeof(Scalar));
    if(has_uv) os.write(reinterpret_cast<const char*>(m_uv->data()), point_count*2*sizeof(Scalar));

    os.write(reinterpret_cast<const char*>(m_faces->data()), face_count*3*sizeof(int));

    return os;
}

std::istream& PointCloud::read(std::istream& is)
{
    clear_kdtree();
    clear_knn_graph();

    int  point_count = -1;
    bool has_normals = false;
    bool has_colors  = false;
    bool has_uv      = false;
    int  face_count  = -1;

    is.read(reinterpret_cast<char*>(&point_count), sizeof(int));
    is.read(reinterpret_cast<char*>(&has_normals), sizeof(bool));
    is.read(reinterpret_cast<char*>(&has_colors),  sizeof(bool));
    is.read(reinterpret_cast<char*>(&has_uv),      sizeof(bool));
    is.read(reinterpret_cast<char*>(&face_count),  sizeof(int));

    m_points->resize(point_count);
    if(has_normals)
    {
        request_normals();
    }
    else
    {
        remove_normals();
    }
    if(has_colors)
    {
        request_colors();
    }
    else
    {
        remove_colors();
    }
    if(has_uv)
    {
        request_uv();
    }
    else
    {
        remove_uv();
    }

    m_faces->resize(face_count);

    is.read(reinterpret_cast<char*>(m_points->data()), point_count*3*sizeof(Scalar));
    if(has_normals) is.read(reinterpret_cast<char*>(m_normals->data()), point_count*3*sizeof(Scalar));
    if(has_colors) is.read(reinterpret_cast<char*>(m_colors->data()), point_count*4*sizeof(Scalar));
    if(has_uv) is.read(reinterpret_cast<char*>(m_uv->data()), point_count*2*sizeof(Scalar));

    is.read(reinterpret_cast<char*>(m_faces->data()), face_count*3*sizeof(int));

    return is;
}

// Capacity Accessors ----------------------------------------------------------

bool PointCloud::empty() const
{
    return size() == 0;
}

int PointCloud::size() const
{
    return point_count();
}

int PointCloud::point_count() const
{
    return points_data().size();
}

int PointCloud::vertex_count() const
{
    return point_count();
}

int PointCloud::face_count() const
{
    return faces_data().size();
}

bool PointCloud::has_normals() const
{
    return m_normals != nullptr;
}

bool PointCloud::has_colors() const
{
    return m_colors != nullptr;
}

bool PointCloud::has_uv() const
{
    return m_uv != nullptr;
}

// Capacity Modifiers ----------------------------------------------------------

void PointCloud::clear()
{
    clear_points();
    clear_faces();
}

void PointCloud::resize(int n)
{
    resize_points(n);
}

void PointCloud::reserve(int n)
{
    reserve_points(n);
}

void PointCloud::clear_points()
{
    points_data().clear();

    if(has_normals()) normals_data().clear();
    if(has_colors()) colors_data().clear();
    if(has_uv()) uv_data().clear();
}

void PointCloud::clear_vertices()
{
    clear_points();
}

void PointCloud::resize_points(int n)
{
    points_data().resize(n);

    if(has_normals()) normals_data().resize(n);
    if(has_colors()) colors_data().resize(n);
    if(has_uv()) uv_data().resize(n);
}

void PointCloud::resize_vertices(int n)
{
    resize_points(n);
}

void PointCloud::reserve_points(int n)
{
    points_data().reserve(n);

    if(has_normals()) normals_data().reserve(n);
    if(has_colors()) colors_data().reserve(n);
    if(has_uv()) uv_data().reserve(n);
}

void PointCloud::reserve_vertices(int n)
{
    reserve_points(n);
}

void PointCloud::clear_faces()
{
    faces_data().clear();
}

void PointCloud::resize_faces(int m)
{
    faces_data().resize(m);
}

void PointCloud::reserve_faces(int m)
{
    faces_data().reserve(m);
}

void PointCloud::shrink_to_fit()
{
    points_data().shrink_to_fit();
    faces_data().shrink_to_fit();

    if(has_normals()) normals_data().shrink_to_fit();
    if(has_colors()) colors_data().shrink_to_fit();
    if(has_uv()) uv_data().shrink_to_fit();
}

void PointCloud::request_normals()
{
    if(!has_normals())
    {
        m_normals = std::make_shared<Vector3Array>(size());
    }
}

void PointCloud::request_normals(const Vector3& normal)
{
    if(!has_normals())
    {
        m_normals = std::make_shared<Vector3Array>(size(), normal);
    }
    else
    {
        std::fill(m_normals->begin(), m_normals->end(), normal);
    }
}

void PointCloud::request_normals(const Vector3Array& normals)
{
    if(!has_normals())
    {
        m_normals = std::make_shared<Vector3Array>(normals);
    }
    else
    {
        normals_data() = normals;
    }
}

void PointCloud::request_colors()
{
    if(!has_colors())
    {
        m_colors = std::make_shared<Vector4Array>(size());
    }
}

void PointCloud::request_colors(const Vector4& color)
{
    if(!has_colors())
    {
        m_colors = std::make_shared<Vector4Array>(size(), color);
    }
    else
    {
        std::fill(m_colors->begin(), m_colors->end(), color);
    }
}

void PointCloud::request_colors(const Vector4Array& colors)
{
    if(!has_colors())
    {
        m_colors = std::make_shared<Vector4Array>(colors);
    }
    else
    {
        colors_data() = colors;
    }
}

void PointCloud::request_uv()
{
    if(!has_uv())
    {
        m_uv = std::make_shared<Vector2Array>(size());
    }
}

void PointCloud::request_uv(const Vector2& uv)
{
    if(!has_uv())
    {
        m_uv = std::make_shared<Vector2Array>(size(), uv);
    }
    else
    {
        std::fill(m_uv->begin(), m_uv->end(), uv);
    }
}

void PointCloud::request_uv(const Vector2Array& uv)
{
    if(!has_uv())
    {
        m_uv = std::make_shared<Vector2Array>(uv);
    }
    else
    {
        uv_data() = uv;
    }
}

void PointCloud::remove_normals()
{
    m_normals = nullptr;
}

void PointCloud::remove_colors()
{
    m_colors = nullptr;
}

void PointCloud::remove_uv()
{
    m_uv = nullptr;
}

// Accessors -------------------------------------------------------------------

const Vector3& PointCloud::operator[](int i) const
{
    return point(i);
}

Vector3& PointCloud::operator[](int i)
{
    return point(i);
}

const Vector3& PointCloud::point(int i) const
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return points_data()[i];
}

Vector3& PointCloud::point(int i)
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return points_data()[i];
}

const Vector3& PointCloud::vertex(int i) const
{
    return point(i);
}

Vector3& PointCloud::vertex(int i)
{
    return point(i);
}

const Vector3& PointCloud::normal(int i) const
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return normals_data()[i];
}

Vector3& PointCloud::normal(int i)
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return normals_data()[i];
}

const Vector4& PointCloud::color(int i) const
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return colors_data()[i];
}

Vector4& PointCloud::color(int i)
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return colors_data()[i];
}

const Vector2& PointCloud::uv(int i) const
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return uv_data()[i];
}

Vector2& PointCloud::uv(int i)
{
    PDPC_DEBUG_ASSERT(0 <= i && i < size());
    return uv_data()[i];
}

const Vector3i& PointCloud::face(int j) const
{
    PDPC_DEBUG_ASSERT(0 <= j && j < face_count());
    return faces_data()[j];
}

Vector3i& PointCloud::face(int j)
{
    PDPC_DEBUG_ASSERT(0 <= j && j < face_count());
    return faces_data()[j];
}

ConstPoint PointCloud::at(int i) const
{
    return ConstPoint(this, i);
}

Point PointCloud::at(int i)
{
    return Point(this, i);
}

// Modifiers -------------------------------------------------------------------

void PointCloud::emplace_back(const Vector3& point, const Vector3& normal, const Vector4& color, const Vector2& uv)
{
    points_data().emplace_back(point);
    normals_data().emplace_back(normal);
    colors_data().emplace_back(color);
    uv_data().emplace_back(uv);
}

void PointCloud::emplace_back(const Vector3& point, const Vector3& normal, const Vector4& color)
{
    points_data().emplace_back(point);
    normals_data().emplace_back(normal);
    colors_data().emplace_back(color);
}

void PointCloud::emplace_back(const Vector3& point, const Vector3& normal, const Vector2& uv)
{
    points_data().emplace_back(point);
    normals_data().emplace_back(normal);
    uv_data().emplace_back(uv);
}

void PointCloud::emplace_back(const Vector3& point, const Vector3& normal)
{
    points_data().emplace_back(point);
    normals_data().emplace_back(normal);
}

void PointCloud::emplace_back(const Vector3& point, const Vector4& color, const Vector2& uv)
{
    points_data().emplace_back(point);
    colors_data().emplace_back(color);
    uv_data().emplace_back(uv);
}

void PointCloud::emplace_back(const Vector3& point, const Vector4& color)
{
    points_data().emplace_back(point);
    colors_data().emplace_back(color);
}

void PointCloud::emplace_back(const Vector3& point, const Vector2& uv)
{
    points_data().emplace_back(point);
    uv_data().emplace_back(uv);
}

void PointCloud::emplace_back(const Vector3& point)
{
    points_data().emplace_back(point);
}

void PointCloud::emplace_back(const Vector3i& face)
{
    faces_data().emplace_back(face);
}

void PointCloud::set_random()
{
    std::generate(m_points->begin(), m_points->end(), [](){return Vector3::Random();});
    if(has_normals()) std::generate(m_normals->begin(), m_normals->end(), [](){return Vector3::Random().normalized();});
    if(has_colors()) std::generate(m_colors->begin(), m_colors->end(), [](){return 0.5*(Vector4::Ones() + Vector4::Random());});
    if(has_uv()) std::generate(m_uv->begin(), m_uv->end(), [](){return 0.5*(Vector2::Ones() + Vector2::Random());});
}

void PointCloud::set_random(int n)
{
    resize(n);
    set_random();
}

void PointCloud::fill_point(const Vector3& point)
{
    std::fill(m_points->begin(), m_points->end(), point);
}

void PointCloud::fill_normal(const Vector3& normal)
{
    request_normals(normal);
}

void PointCloud::fill_color(const Vector4& color)
{
    request_colors(color);
}

void PointCloud::fill_uv(const Vector2& uv)
{
    request_uv(uv);
}

void PointCloud::erase(int begin, int end)
{
    m_points->erase(m_points->begin()+begin, m_points->begin()+end);
    if(has_normals()) m_normals->erase(m_normals->begin()+begin, m_normals->begin()+end);
    if(has_colors())  m_colors->erase(m_colors->begin()+begin, m_colors->begin()+end);
    if(has_uv())      m_uv->erase(m_uv->begin()+begin, m_uv->begin()+end);
}

// Operations ------------------------------------------------------------------

void PointCloud::append(const PointCloud& other)
{
    const int n0 = point_count();
    const int m0 = face_count();

    m_points->insert(m_points->end(), other.m_points->begin(), other.m_points->end());
    if(this->has_normals() && other.has_normals())
    {
        m_normals->insert(m_normals->end(), other.m_normals->begin(), other.m_normals->end());
    }
    if(this->has_colors() && other.has_colors())
    {
        m_colors->insert(m_colors->end(), other.m_colors->begin(), other.m_colors->end());
    }
    if(this->has_uv() && other.has_uv())
    {
        m_uv->insert(m_uv->end(), other.m_uv->begin(), other.m_uv->end());
    }
    m_faces->insert(m_faces->end(), other.m_faces->begin(), other.m_faces->end());

    // shift all appended faces indices by the initial vertex count
    Vector3i incr = n0 * Vector3i::Ones();
    for(int i=m0; i<face_count(); ++i)
    {
        face(i) += incr;
    }
}

void PointCloud::sample(const PointCloud& other, const std::vector<bool>& to_keep)
{
    clear_faces();

    keep(other.points_data(), points_data(), to_keep);

    if(other.has_normals())
    {
        this->request_normals();
        keep(other.normals_data(), normals_data(), to_keep);
    }
    if(other.has_colors())
    {
        this->request_colors();
        keep(other.colors_data(), colors_data(), to_keep);
    }
    if(other.has_uv())
    {
        this->request_uv();
        keep(other.uv_data(), uv_data(), to_keep);
    }
}

void PointCloud::sample(const PointCloud& other, const std::vector<int>& to_keep)
{
    clear_faces();

    keep(other.points_data(), points_data(), to_keep);

    if(other.has_normals())
    {
        this->request_normals();
        keep(other.normals_data(), normals_data(), to_keep);
    }
    if(other.has_colors())
    {
        this->request_colors();
        keep(other.colors_data(), colors_data(), to_keep);
    }
    if(other.has_uv())
    {
        this->request_uv();
        keep(other.uv_data(), uv_data(), to_keep);
    }
}

// Analysis --------------------------------------------------------------------

Scalar PointCloud::mesh_area() const
{
    Scalar a = 0;
    for(int j=0; j<face_count(); ++j)
    {
        a += face_area(j);
    }
    return a;
}

Scalar PointCloud::face_area(int j) const
{
    return 0.5 * (point(face(j)[1]) - point(face(j)[0])).cross(
                  point(face(j)[2]) - point(face(j)[0])).norm();
}

Scalar PointCloud::face_area_doubled(int j) const
{
    return (point(face(j)[1]) - point(face(j)[0])).cross(
            point(face(j)[2]) - point(face(j)[0])).norm();
}

Vector3 PointCloud::face_normal(int j) const
{
    const Vector3i& f = face(j);
    return (point(f[1]) - point(f[0])).cross(
            point(f[2]) - point(f[0])).normalized();
}

Vector3 PointCloud::face_center(int j) const
{
    const Vector3i& f = face(j);
    return (point(f[0]) + point(f[1]) + point(f[2]))/3;
}

Scalar PointCloud::line_length() const
{
    Scalar l = 0;
    for(int i=1; i<size(); ++i)
    {
        l += (point(i) - point(i-1)).norm();
    }
    return l;
}

Scalar PointCloud::mean_edge_length() const
{
    Scalar mean = 0;
    for(int j=0; j<face_count(); ++j)
    {
        mean += (point(face(j)[0]) - point(face(j)[1])).norm();
        mean += (point(face(j)[0]) - point(face(j)[2])).norm();
        mean += (point(face(j)[1]) - point(face(j)[2])).norm();
    }
    mean /= (3 * face_count());
    return mean;
}

Aabb PointCloud::aabb() const
{
    Aabb aabb;
    for(int i=0; i<size(); ++i)
    {
        if(point(i).array().allFinite()) aabb.extend(point(i));
    }
    return aabb;
}

Scalar PointCloud::aabb_diag() const
{
    return aabb().diagonal().norm();
}

Scalar PointCloud::edge_length_min() const
{
    return edge_length_minmax().first;
}

Scalar  PointCloud::edge_length_max() const
{
    return edge_length_minmax().second;
}

Scalar PointCloud::edge_length_min_squared() const
{
    return edge_length_minmax_squared().first;
}

Scalar  PointCloud::edge_length_max_squared() const
{
    return edge_length_minmax_squared().second;
}

std::pair<Scalar,Scalar> PointCloud::edge_length_minmax() const
{
    auto squared = edge_length_minmax_squared();
    return std::make_pair(std::sqrt(squared.first),std::sqrt(squared.second));
}

std::pair<Scalar,Scalar> PointCloud::edge_length_minmax_squared() const
{
    if(face_count() == 0) return std::make_pair(Scalar(0),Scalar(0));

    Scalar l_min2 = std::numeric_limits<Scalar>::max();
    Scalar l_max2 = 0;

    constexpr int edge[3][2] = {{0,1},{0,2},{1,2}};

    for(const auto& face : faces_data())
    {
        for(int e=0; e<3; ++e)
        {
            const Scalar l2 = (point(face[edge[e][0]]) - point(face[edge[e][1]])).squaredNorm();
            l_min2 = std::min(l_min2, l2);
            l_max2 = std::max(l_max2, l2);
        }
    }
    return std::make_pair(l_min2, l_min2);
}

// Processing ------------------------------------------------------------------

void PointCloud::translate_and_scale(const Vector3& t, Scalar s)
{
    for(auto& p : points_data())
    {
        p += t;
        p *= s;
    }
}

void PointCloud::normalize_normals()
{
    for(auto& n : normals_data())
    {
        n.normalize();
    }
}

void PointCloud::compute_normals_from_faces()
{
    request_normals(Vector3::Zero());

    if(face_count() > 0)
    {
        // sum normal of incident faces (no weights)
        for(int j=0; j<face_count(); ++j)
        {
            const Vector3 n = face_normal(j);
            for(int k=0; k<3; ++k)
            {
                normal(face(j)[k]) += n;
            }
        }
        normalize_normals();
    }
}

// Data Accessors --------------------------------------------------------------

Vector3Array& PointCloud::points_data()
{
    return *m_points;
}

Vector3Array& PointCloud::vertices_data()
{
    return points_data();
}

Vector3Array& PointCloud::normals_data()
{
    PDPC_DEBUG_ASSERT(has_normals());
    return *m_normals;
}

Vector4Array& PointCloud::colors_data()
{
    PDPC_DEBUG_ASSERT(has_colors());
    return *m_colors;
}

Vector2Array& PointCloud::uv_data()
{
    PDPC_DEBUG_ASSERT(has_uv());
    return *m_uv;
}

Vector3iArray& PointCloud::faces_data()
{
    return *m_faces;
}

const Vector3Array& PointCloud::points_data() const
{
    return *m_points;
}

const Vector3Array& PointCloud::vertices_data() const
{
    return points_data();
}

const Vector3Array& PointCloud::normals_data() const
{
    PDPC_DEBUG_ASSERT(has_normals());
    return *m_normals;
}

const Vector4Array& PointCloud::colors_data() const
{
    PDPC_DEBUG_ASSERT(has_colors());
    return *m_colors;
}

const Vector2Array& PointCloud::uv_data() const
{
    PDPC_DEBUG_ASSERT(has_uv());
    return *m_uv;
}

const Vector3iArray& PointCloud::faces_data() const
{
    return *m_faces;
}

// Data Ptr Accessors ----------------------------------------------------------

std::shared_ptr<Vector3Array>& PointCloud::points_ptr()
{
    return m_points;
}

std::shared_ptr<Vector3Array>& PointCloud::vertices_ptr()
{
    return points_ptr();
}

std::shared_ptr<Vector3Array>& PointCloud::normals_ptr()
{
    return m_normals;
}

std::shared_ptr<Vector4Array>& PointCloud::colors_ptr()
{
    return m_colors;
}

std::shared_ptr<Vector2Array>& PointCloud::uv_ptr()
{
    return m_uv;
}

std::shared_ptr<Vector3iArray>& PointCloud::faces_ptr()
{
    return m_faces;
}

const std::shared_ptr<Vector3Array>& PointCloud::points_ptr() const
{
    return m_points;
}

const std::shared_ptr<Vector3Array>& PointCloud::vertices_ptr() const
{
    return points_ptr();
}

const std::shared_ptr<Vector3Array>& PointCloud::normals_ptr() const
{
    return m_normals;
}

const std::shared_ptr<Vector4Array>& PointCloud::colors_ptr() const
{
    return m_colors;
}

const std::shared_ptr<Vector2Array>& PointCloud::uv_ptr() const
{
    return m_uv;
}

const std::shared_ptr<Vector3iArray>& PointCloud::faces_ptr() const
{
    return m_faces;
}

// Space Partitioning ----------------------------------------------------------

const KdTree& PointCloud::kdtree() const
{
    PDPC_DEBUG_ASSERT(has_kdtree());
    return *m_kdtree.get();
}

KdTree& PointCloud::kdtree()
{
    PDPC_DEBUG_ASSERT(has_kdtree());
    return *m_kdtree.get();
}

const KnnGraph& PointCloud::knn_graph() const
{
    PDPC_DEBUG_ASSERT(has_knn_graph());
    return *m_knngraph.get();
}

KnnGraph& PointCloud::knn_graph()
{
    PDPC_DEBUG_ASSERT(has_knn_graph());
    return *m_knngraph.get();
}

const std::shared_ptr<KdTree>& PointCloud::kdtree_ptr() const
{
    return m_kdtree;
}

std::shared_ptr<KdTree>& PointCloud::kdtree_ptr()
{
    return m_kdtree;
}

const std::shared_ptr<KnnGraph>& PointCloud::knngraph_ptr() const
{
    return m_knngraph;
}

std::shared_ptr<KnnGraph>& PointCloud::knngraph_ptr()
{
    return m_knngraph;
}

bool PointCloud::has_kdtree() const
{
    return m_kdtree != nullptr;
}

bool PointCloud::has_knn_graph() const
{
    return m_knngraph != nullptr;
}

void PointCloud::clear_kdtree()
{
    m_kdtree = nullptr;
}

void PointCloud::clear_knn_graph()
{
    m_knngraph = nullptr;
}

void PointCloud::build_kdtree()
{
    m_kdtree = std::make_shared<KdTree>(m_points);
}

void PointCloud::build_knn_graph(int k, bool keep_kdtree)
{
    if(!has_kdtree()) build_kdtree();

    m_knngraph = std::make_shared<KnnGraph>(k);
    m_knngraph->build(*m_kdtree);

    if(!keep_kdtree) clear_kdtree();
}

} // namespace pdpc
