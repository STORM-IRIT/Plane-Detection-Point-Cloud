#pragma once

#include <PDPC/PointCloud/Point.h>
#include <memory>

namespace pdpc {

class KdTree;
class KnnGraph;

class PointCloud
{
    // PointCloud --------------------------------------------------------------
public:
    PointCloud(int n = 0, int m = 0);
    PointCloud(const PointCloud& other);
    PointCloud(PointCloud&& other);

    PointCloud& operator = (const PointCloud& other);
    PointCloud& operator = (PointCloud&& other);

    PointCloud to_soup() const;

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    // Capacity Accessors ------------------------------------------------------
public:
    bool empty() const;
    int size() const;

    int point_count() const;
    int vertex_count() const;
    int face_count() const;

    bool has_normals() const;
    bool has_colors() const;
    bool has_uv() const;

    // Capacity Modifiers ------------------------------------------------------
public:
    void clear();
    void resize(int n);
    void reserve(int n);

    void clear_points();
    void clear_vertices();

    void resize_points(int n);
    void resize_vertices(int n);

    void reserve_points(int n);
    void reserve_vertices(int n);

    void clear_faces();
    void resize_faces(int m);
    void reserve_faces(int m);

    void shrink_to_fit();

    void request_normals();
    void request_normals(const Vector3& normal);
    void request_normals(const Vector3Array& normals);

    void request_colors();
    void request_colors(const Vector4& color);
    void request_colors(const Vector4Array& colors);

    void request_uv();
    void request_uv(const Vector2& uv);
    void request_uv(const Vector2Array& uv);

    void remove_normals();
    void remove_colors();
    void remove_uv();

    // Accessors ---------------------------------------------------------------
public:
    const Vector3& operator[](int i) const;
          Vector3& operator[](int i);

    const Vector3& point(int i) const;
          Vector3& point(int i);

    const Vector3& vertex(int i) const;
          Vector3& vertex(int i);

    const Vector3& normal(int i) const;
          Vector3& normal(int i);

    const Vector4& color(int i) const;
          Vector4& color(int i);

    const Vector2& uv(int i) const;
          Vector2& uv(int i);

    const Vector3i& face(int j) const;
          Vector3i& face(int j);

    ConstPoint at(int i) const;
         Point at(int i);

    // Modifiers ---------------------------------------------------------------
public:
    void emplace_back(const Vector3& point, const Vector3& normal, const Vector4& color, const Vector2& uv);
    void emplace_back(const Vector3& point, const Vector3& normal, const Vector4& color);
    void emplace_back(const Vector3& point, const Vector3& normal, const Vector2& uv);
    void emplace_back(const Vector3& point, const Vector3& normal);
    void emplace_back(const Vector3& point, const Vector4& color, const Vector2& uv);
    void emplace_back(const Vector3& point, const Vector4& color);
    void emplace_back(const Vector3& point, const Vector2& uv);
    void emplace_back(const Vector3& point);
    void emplace_back(const Vector3i& face);

    void set_random();
    void set_random(int n);

    void fill_point(const Vector3& point = Vector3::Zero());
    void fill_normal(const Vector3& normal = Vector3::Zero());
    void fill_color(const Vector4& color = Vector4::Zero());
    void fill_uv(const Vector2& uv = Vector2::Zero());

    void erase(int begin, int end);

    // Operations --------------------------------------------------------------
public:
    void append(const PointCloud& other);
    void sample(const PointCloud& other, const std::vector<bool>& to_keep);
    void sample(const PointCloud& other, const std::vector<int>& to_keep);

    // Analysis ----------------------------------------------------------------
public:
    Scalar  mesh_area() const;
    Scalar  face_area(int j) const;
    Scalar  face_area_doubled(int j) const;
    Vector3 face_normal(int j) const;
    Vector3 face_center(int j) const;
    Scalar  line_length() const;
    Scalar  mean_edge_length() const;
    Aabb    aabb() const;
    Scalar  aabb_diag() const;
    Scalar  edge_length_min() const;
    Scalar  edge_length_max() const;
    Scalar  edge_length_min_squared() const;
    Scalar  edge_length_max_squared() const;
    std::pair<Scalar,Scalar> edge_length_minmax() const;
    std::pair<Scalar,Scalar> edge_length_minmax_squared() const;

    // Processing --------------------------------------------------------------
public:
    void translate_and_scale(const Vector3& t, Scalar s);
    void normalize_normals();
    void compute_normals_from_faces();

    // Data Accessors ----------------------------------------------------------
public:
    Vector3Array&  points_data();
    Vector3Array&  vertices_data();
    Vector3Array&  normals_data();
    Vector4Array&  colors_data();
    Vector2Array&  uv_data();
    Vector3iArray& faces_data();

    const Vector3Array&  points_data() const;
    const Vector3Array&  vertices_data() const;
    const Vector3Array&  normals_data() const;
    const Vector4Array&  colors_data() const;
    const Vector2Array&  uv_data() const;
    const Vector3iArray& faces_data() const;

    // Data Ptr Accessors ------------------------------------------------------
public:
    std::shared_ptr<Vector3Array>& points_ptr();
    std::shared_ptr<Vector3Array>& vertices_ptr();
    std::shared_ptr<Vector3Array>& normals_ptr();
    std::shared_ptr<Vector4Array>& colors_ptr();
    std::shared_ptr<Vector2Array>& uv_ptr();
    std::shared_ptr<Vector3iArray>& faces_ptr();

    const std::shared_ptr<Vector3Array>& points_ptr() const;
    const std::shared_ptr<Vector3Array>& vertices_ptr() const;
    const std::shared_ptr<Vector3Array>& normals_ptr() const;
    const std::shared_ptr<Vector4Array>& colors_ptr() const;
    const std::shared_ptr<Vector2Array>& uv_ptr() const;
    const std::shared_ptr<Vector3iArray>& faces_ptr() const;

    // Space Partitioning ------------------------------------------------------
public:
    const KdTree& kdtree() const;
          KdTree& kdtree();

    const KnnGraph& knn_graph() const;
          KnnGraph& knn_graph();

    const std::shared_ptr<KdTree>& kdtree_ptr() const;
          std::shared_ptr<KdTree>& kdtree_ptr();

    const std::shared_ptr<KnnGraph>& knngraph_ptr() const;
          std::shared_ptr<KnnGraph>& knngraph_ptr();

    bool has_kdtree() const;
    bool has_knn_graph() const;

    void clear_kdtree();
    void clear_knn_graph();

    void build_kdtree();
    void build_knn_graph(int k, bool keep_kdtree = false);

    // Data --------------------------------------------------------------------
protected:
    std::shared_ptr<Vector3Array>  m_points;
    std::shared_ptr<Vector3Array>  m_normals;
    std::shared_ptr<Vector4Array>  m_colors;
    std::shared_ptr<Vector2Array>  m_uv;
    std::shared_ptr<Vector3iArray> m_faces;

    std::shared_ptr<KdTree>   m_kdtree;
    std::shared_ptr<KnnGraph> m_knngraph;
};

} // namespace pdpc
