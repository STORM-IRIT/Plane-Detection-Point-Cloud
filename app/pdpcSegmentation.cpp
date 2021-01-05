#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Algorithms/has_duplicate.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/PointCloud/orthonormal_basis.h>
#include <PDPC/PointCloud/triangle_area.h>
#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>
#include <PDPC/ScaleSpace/ScaleSampling.h>
#include <PDPC/Segmentation/SeededKNNGraphRegionGrowing.h>
#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/Segmentation/MSSegmentationGraph.h>
#include <PDPC/Segmentation/RegionSet.h>
#include <PDPC/Graph/HierarchicalGraph.h>
#include <PDPC/Persistence/ComponentSet.h>
#include <PDPC/Persistence/ComponentDataSet.h>

#include <set>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/algorithm.h>
#include <CGAL/assertions.h>

#include <PDPC/Common/Colors.h>
#include <PDPC/Common/String.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef K::FT                                                FT;
typedef K::Point_2                                           CGALPoint;
typedef K::Segment_2                                         Segment;
typedef CGAL::Alpha_shape_vertex_base_2<K>                   Vb;
typedef CGAL::Alpha_shape_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>          Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                Triangulation_2;
typedef CGAL::Alpha_shape_2<Triangulation_2>                 Alpha_shape_2;
typedef Alpha_shape_2::Alpha_shape_edges_iterator            Alpha_shape_edges_iterator;

using namespace pdpc;

Scalar compute_area(const PointCloud& points,
                    const std::vector<int>& region,
                    int i,
                    Scalar alpha);

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input    = opt.get_string("input",    "i").set_brief("Input point cloud (.ply/.obj)").set_required();
    const std::string in_scales   = opt.get_string("scales",   "s").set_brief("Input scales (.txt)"          ).set_required();
    const std::string in_features = opt.get_string("features", "f").set_brief("Input features (.txt/.bin)"   ).set_required();
    const std::string in_output   = opt.get_string("output",   "o").set_brief("Output name"                  ).set_default("output");

    const int    in_k     = opt.get_int(  "knn",  "k").set_default(10).set_brief("Region growing nearest neighbors count");
    const Scalar in_theta = opt.get_float("theta"    ).set_default(5.).set_brief("Region growing angular threshold (degrees)");
    const Scalar in_phi   = opt.get_float("phi"      ).set_default(1.).set_brief("Region growing curvature threshold");

    const Scalar in_j_min = opt.get_float("jaccard_min", "jmin").set_default(0.5).set_brief("Jaccard index min threshold");
    const int    in_p_min = opt.get_int(  "pers_min",    "pmin").set_default(2).set_brief("Persistence min threshold");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

    const bool in_debug = opt.get_bool("debug").set_default(false).set_brief("Save before/after segmentations as colored ply");

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    ScaleSampling scales;
    ok = scales.load(in_scales, in_v);
    const int scale_count = scales.size();

    MultiScaleFeatures features;
    ok = features.load(in_features, in_v);
    if(!ok) return 1;

    if(features.m_point_count != point_count)
    {
        error().iff(in_v) << "Point counts do not match: " << features.m_point_count << " != " << point_count;
        return 1;
    }
    if(features.m_scale_count != scale_count)
    {
        error().iff(in_v) << "Scale counts do not match: " << features.m_scale_count << " != " << scale_count;
        return 1;
    }

    if(in_debug) points.request_colors();

    // 1. Segmentations --------------------------------------------------------
    info().iff(in_v) << "Performing " << scale_count << " planar region growing";
    MSSegmentation ms_seg(scale_count, Segmentation(point_count));
    {
        const Scalar threshold_angle = std::cos(in_theta / 180. * M_PI);
        const Scalar threshold_curva = in_phi;

        points.build_knn_graph(in_k);

//        #pragma omp parallel for
        for(int j=0; j<scale_count; ++j)
        {
            #pragma omp critical (seg_info)
            {
                info().iff(in_v) << j+1 << "/" << scale_count;
            }

            // 1.0 Mean planarity dev ------------------------------------------
            std::vector<Scalar> mean_planarity_dev(point_count, 0.);
            for(int i=0; i<point_count; ++i)
            {
                mean_planarity_dev[i] = features.plane_dev(i,j);
                for(int k : points.knn_graph().k_nearest_neighbors(i))
                {
                    mean_planarity_dev[i] += features.plane_dev(k,j);
                }
                mean_planarity_dev[i] /= (in_k + 1);
            }

            // 1.1 Region growing ----------------------------------------------
            std::vector<int> seeds;
            Segmentation& seg = ms_seg[j];

            SeededKNNGraphRegionGrowing::compute(points, seg,
            // Comparison function for growing
            [&features,&seeds,j,threshold_angle,threshold_curva](int rg_l, int rg_i, int rg_j) -> bool
            {
                const int idx_seed = seeds[rg_l];
                PDPC_UNUSED(rg_i);
                return features.normal(idx_seed,j).dot(features.normal(rg_j,j)) > threshold_angle &&
                       features.plane_dev(rg_j,j) < threshold_curva;
            },
            // Priority function for seeds
            [&mean_planarity_dev](int rg_i, int rg_j) -> bool
            {
                return mean_planarity_dev[rg_i] > mean_planarity_dev[rg_j];
            },
            // Called for region initialization
            [&seeds](int rg_l, int rg_i)
            {
                PDPC_UNUSED(rg_l);
                seeds.push_back(rg_i);
            });

            PDPC_ASSERT(seg.region_count() == int(seeds.size()));

            // 1.2 Filtering ---------------------------------------------------
#if 1
            const Scalar scale = scales[j];
            const Scalar alpha = 2 * scale;
            std::vector<std::vector<int>> regions;
            seg.fill(regions);

            std::vector<bool> to_invalidate(seg.region_count(), false);

            for(int label=0; label<seg.region_count(); ++label)
            {
                const int point_count = seg.region_size(label);

                if(point_count <= 10)
                {
                    to_invalidate[label] = true;
                }
                else
                {
                    const Scalar area = compute_area(points, regions[label], seeds[label], alpha);
                    const Scalar dist = std::sqrt(area);

                    if(dist < 2 * scale)
                    {
                        to_invalidate[label] = true;
                    }
                }
            }

            if(in_debug)
            {
                #pragma omp critical (debug_before)
                {
                    const auto colormap = Colormap::Tab20();
                    seg.set_colors(points.colors_data(), Colors::Black(), colormap);
                    Loader::Save("debug_before_" + str::to_string(j,3)+".ply", points, false);
                }
            }

            seg.invalidate_regions(to_invalidate);
            seg.make_full();

            if(in_debug)
            {
                #pragma omp critical (debug_after)
                {
                    const auto colormap = Colormap::Tab20();
                    seg.set_colors(points.colors_data(), Colors::Black(), colormap);
                    Loader::Save("debug_after_" + str::to_string(j,3)+".ply", points, false);
                }
            }

#else
            std::vector<bool> to_invalidate(seg.region_count(), false);
            std::vector<std::vector<int>> regions;
            seg.fill(regions);

            for(int l=0; l<seg.region_count(); ++l)
            {
                const Vector3 plane_p = points.point(seeds[l]);
                const Vector3 plane_n = points.normal(seeds[l]);
                const Matrix3 plane_T = orthonormal_basis(plane_n).transpose();

                Aabb2 aabb;
                for(int i : regions[l])
                {
                    aabb.extend( (plane_T * (points[i]-plane_p)).head<2>() );
                }
                to_invalidate[l] = aabb.diagonal().norm() < scales[j];
            }

            seg.invalidate_regions(to_invalidate);
            seg.make_full();
#endif
        }
    }

    // 2. Graph ----------------------------------------------------------------
    info().iff(in_v) << "Building graph";
    HierarchicalGraph g;
    MSSegmentationGraph::create(ms_seg, g);

    // 3. Component extraction -------------------------------------------------
    info().iff(in_v) << "Extracting component";
    ComponentSet comp_set(g.node_count(0));
    {
        PDPC_ASSERT(g.node_properties(0).has("size"));
        PDPC_ASSERT(g.edge_properties(0).has("weight"));
        const int prop_size   = g.node_properties(0).index("size");
        const int prop_weight = g.edge_properties(0).index("weight");

        int level = 0;
        std::vector<int> region_to_comp(g.node_count(level));
        std::vector<int> region_to_comp2;

        for(int i=0; i<comp_set.size(); ++i)
        {
            comp_set[i].initialize(level, i);
            region_to_comp[i] = i;
        }

        //       vvv                                      vvvv
        for(level=1; level<g.level_count(); ++level, std::swap(region_to_comp,region_to_comp2))
        {
            PDPC_DEBUG_ASSERT(!has_duplicate(region_to_comp));
            PDPC_DEBUG_ASSERT(std::all_of(region_to_comp.begin(), region_to_comp.end(), [&](int idx_comp)->bool{return 0 <= idx_comp && idx_comp < comp_set.size() && comp_set[idx_comp].death_level() == level-1;}));

            std::vector<int> indices(g.edge_count(level-1));
            std::iota(indices.begin(), indices.end(), int(0));
            std::sort(indices.begin(), indices.end(), [&](int i, int j)->bool
            {
                const Scalar coeff_i = MSSegmentationGraph::jaccard(g, level-1, i, prop_size, prop_weight);
                const Scalar coeff_j = MSSegmentationGraph::jaccard(g, level-1, j, prop_size, prop_weight);

                return coeff_i > coeff_j;
            });

            region_to_comp2.resize(g.node_count(level)); //           vvv
            std::fill(region_to_comp2.begin(), region_to_comp2.end(), -1);

            for(int idx_edge : indices)
            {
                const int idx_source = g.source(level-1, idx_edge);
                const int idx_target = g.target(level-1, idx_edge);
                const int idx_comp   = region_to_comp[idx_target];

                PDPC_DEBUG_ASSERT(g.contains_node(level,   idx_source));
                PDPC_DEBUG_ASSERT(g.contains_node(level-1, idx_target));
                PDPC_DEBUG_ASSERT(0 <= idx_comp && idx_comp < comp_set.size());

                // no component has the source node  &&  the candidate component has no node at level
                if(region_to_comp2[idx_source] == -1 && comp_set[idx_comp].death_level() == level-1)
                {
                    Scalar coeff = MSSegmentationGraph::jaccard(g, level-1, idx_edge, prop_size, prop_weight);
                    if(in_j_min <= coeff)
                    {
                        // expand current component
                        comp_set[idx_comp].push_back(idx_source);
                        region_to_comp2[idx_source] = idx_comp;
                    }
                    else
                    {
                        // stop current component = create a new one
                        int idx_comp_new = comp_set.size();
                        comp_set.push_back();
                        comp_set.back().initialize(level, idx_source);
                        region_to_comp2[idx_source] = idx_comp_new;
                    }
                }
            }

            // create new components if needed
            for(int idx_node=0; idx_node<g.node_count(level); ++idx_node)
            {
                if(region_to_comp2[idx_node] == -1)
                {
                    int idx_comp = comp_set.size();
                    comp_set.push_back();
                    comp_set[idx_comp].initialize(level, idx_node);
                    region_to_comp2[idx_node] = idx_comp;
                }
            }
        }

        // sort
        PDPC_ASSERT(comp_set.properties().count() == 0);
        std::sort(comp_set.data().begin(), comp_set.data().end(), [](const auto& x, const auto& y)
        {
            return x.persistence() > y.persistence();
        });

        // filter
        auto it = std::find_if(comp_set.data().begin(), comp_set.data().end(), [in_p_min](const auto& x)
        {
            return x.persistence() < in_p_min;
        });
        comp_set.data().erase(it, comp_set.data().end());
        comp_set.properties().resize(comp_set.data().size());

        info().iff(in_v) << comp_set.size() << " components extracted";
    }

    // 4. Final regions --------------------------------------------------------
    RegionSet reg_set(comp_set.size());
    {
        #pragma omp parallel for
        for(int i=0; i<reg_set.size(); ++i)
        {
            std::set<int> indices;

            for(int level=comp_set[i].birth_level(); level<=comp_set[i].death_level(); ++level)
            {
                for(int idx : ms_seg[level].indices(comp_set[i][level]))
                {
                    indices.insert(idx);
                }
            }

            reg_set[i].assign(indices.begin(), indices.end());
        }
    }

    // 5. Final segmentation ---------------------------------------------------
    MSSegmentation comp_seg(scale_count, Segmentation(point_count));
    {
        comp_seg.for_each([&](Segmentation& seg)
        {
            seg.resize_region(comp_set.size() + 1); // +1 for the empty region
        });

        for(int idx_comp=0; idx_comp<comp_set.size(); ++idx_comp)
        {
            const Component& comp = comp_set[idx_comp];

            for(int level=comp.birth_level(); level<=comp.death_level(); ++level)
            {
                const int label = comp.index(level);

                for(int idx_point : ms_seg[level].indices(label))
                {
                    PDPC_DEBUG_ASSERT(comp_seg[level][idx_point] == -1);
                    comp_seg[level].set_label(idx_point, idx_comp);
                }
            }
        }
    }

    ComponentDataSet comp_data(comp_set, std::move(reg_set));
    comp_data.save(in_output + "_comp.txt");

    comp_seg.save(in_output + "_seg.txt");

    return 0;
}

class PointIterator
{
public:
    using value_type        = CGALPoint;
    using difference_type   = int;
    using pointer           = CGALPoint*;
    using reference         = CGALPoint&;
    using iterator_category = std::random_access_iterator_tag;

public:
    PointIterator(int idx, const PointCloud* points, const std::vector<int>* region, const Matrix3& T, const Vector3& p) :
        m_idx(idx),
        m_points(points),
        m_region(region),
        m_T(T),
        m_p(p)
    {
    }

public:
    PointIterator& operator ++ (){++m_idx; return *this;}
    PointIterator& operator -- (){--m_idx; return *this;}
    PointIterator  operator + (int /*i*/) const{PDPC_ERROR_MSG("Not used"); return *this;}
    PointIterator  operator - (int /*i*/) const{PDPC_ERROR_MSG("Not used"); return *this;}
    bool           operator == (const PointIterator& other) const{return m_idx == other.m_idx;}
    bool           operator != (const PointIterator& other) const{return m_idx != other.m_idx;}
    CGALPoint      operator * () const
    {
        const Vector3 q = m_T * (m_points->point(m_region->operator[](m_idx)) - m_p);
        return CGALPoint(q.x(), q.y());
    }

    friend int operator - (const PointIterator& it1, const PointIterator& it2){return it1.m_idx - it2.m_idx;}

protected:
//    void advance();

protected:
    int m_idx;
    const PointCloud* m_points;
    const std::vector<int>* m_region;
    Matrix3 m_T;
    Vector3 m_p;
};


Scalar compute_area(const PointCloud& points,
                    const std::vector<int>& region,
                    int i,
                    Scalar alpha)
{
    const Matrix3 T = orthonormal_basis(points.normal(i)).transpose();

    PointIterator first(0,            &points, &region, T, points[i]);
    PointIterator last(region.size(), &points, &region, T, points[i]);
    Alpha_shape_2 alpha_shape(first, last, alpha, Alpha_shape_2::GENERAL);

    Scalar area = 0;

    for(auto f_it = alpha_shape.faces_begin(); f_it != alpha_shape.faces_end(); ++f_it)
    {
        const auto classif = alpha_shape.classify(f_it);
        if(classif != Alpha_shape_2::EXTERIOR)
        {
            const auto& p1 = alpha_shape.point(f_it, 0);
            const auto& p2 = alpha_shape.point(f_it, 1);
            const auto& p3 = alpha_shape.point(f_it, 2);

            area += triangle_area(p1.x(), p1.y(),
                                  p2.x(), p2.y(),
                                  p3.x(), p3.y());
        }
    }
    return area;
}
