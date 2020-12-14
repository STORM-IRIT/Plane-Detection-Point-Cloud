#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Algorithms/has_duplicate.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/PointCloud/orthonormal_basis.h>
#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>
#include <PDPC/ScaleSpace/ScaleSampling.h>
#include <PDPC/Segmentation/SeededKNNGraphRegionGrowing.h>
#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/Segmentation/MSSegmentationGraph.h>
#include <PDPC/Segmentation/RegionSet.h>
#include <PDPC/Graph/HierarchicalGraph.h>
#include <PDPC/Persistence/ComponentSet.h>

#include <set>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input    = opt.get_string("input",    "i").set_brief("Input point cloud (.ply/.obj)").set_required();
    const std::string in_scales   = opt.get_string("scales",   "s").set_brief("Input scales (.txt)"          ).set_default("scales.txt");
    const std::string in_features = opt.get_string("features", "f").set_brief("Input features (.txt/.bin)"   ).set_default("features.bin");

    const int    in_k     = opt.get_int(  "knn",  "k").set_default(10).set_brief("Region growing nearest neighbors count");
    const Scalar in_theta = opt.get_float("theta"    ).set_default(5.).set_brief("Region growing angular threshold (degrees)");
    const Scalar in_phi   = opt.get_float("phi"      ).set_default(1.).set_brief("Region growing curvature threshold");

    const Scalar in_j_min = opt.get_float("jaccard_min", "jmin").set_default(0.5).set_brief("Jaccard index min threshold");
    const int    in_p_min = opt.get_int(  "pers_min",    "pmin").set_default(2).set_brief("Persistence min threshold");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

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

    // 1. Segmentations --------------------------------------------------------
    info().iff(in_v) << "Performing " << scale_count << " planar region growing";
    MSSegmentation ms_seg(scale_count, Segmentation(point_count));
    {
        const Scalar threshold_angle = std::cos(in_theta / 180. * M_PI);
        const Scalar threshold_curva = in_phi;

        points.build_knn_graph(in_k);

        #pragma omp parallel for
        for(int j=0; j<scale_count; ++j)
        {
            #pragma omp critical (seg_info)
            {
                info().iff(in_v) << "Processing scale " << j << "/" << scale_count-1 << " (" << int(Scalar(j)/(scale_count-1)*100) << "%)...";
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
            [&features,j](int rg_i, int rg_j) -> bool
            {
                return features.plane_dev(rg_i, j) > features.plane_dev(rg_j, j);
            },
            // Called for region initialization
            [&seeds](int rg_l, int rg_i)
            {
                PDPC_UNUSED(rg_l);
                seeds.push_back(rg_i);
            });

            PDPC_ASSERT(seg.region_count() == int(seeds.size()));

            // 1.2 Filtering ---------------------------------------------------
            std::vector<bool> to_invalidate(seg.region_count(), false);
            std::vector<std::vector<int>> regions;
            seg.fill(regions);

            for(int l=0; l<seg.region_count(); ++l)
            {
                const Vector3 plane_p = points.point(seeds[l]);
                const Vector3 plane_n = points.normal(seeds[l]);
                const Matrix3 plane_T = orthonormal_basis(plane_n);

                Aabb2 aabb;
                for(int i : regions[l])
                {
                    aabb.extend( (plane_T * (points[i]-plane_p)).head<2>() );
                }
                to_invalidate[l] = aabb.diagonal().norm() < scales[j];
            }
            seg.invalidate(to_invalidate);
            seg.make_full();
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
    MSSegmentation comp_seg(scale_count);
    {
        comp_seg.for_each([&](Segmentation& seg)
        {
            seg.resize(point_count);
            seg.resize_region(comp_set.size());
        });

        #pragma omp parallel for
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




    return 0;
}
