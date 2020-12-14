# Plane Detection in 3D Point Clouds

## Persistence Analysis of Multi-scale Planar Structure Graph in Point Clouds [(PDF)](https://hal.archives-ouvertes.fr/hal-02490721/document)  

*Thibault Lejemble*  
*Claudio Mura*  
*Loïc Barthe*  
*Nicolas Mellado*  

Computer Graphics Forum 2020  
Eurographics 2020  

**Abstract**  
Abstract Modern acquisition techniques generate detailed point clouds that sample complex geometries. For instance, we are able to produce millimeter-scale acquisition of whole buildings. Processing and exploring geometrical information within such point clouds requires scalability, robustness to acquisition defects and the ability to model shapes at different scales. In this work, we propose a new representation that enriches point clouds with a multi-scale planar structure graph. We define the graph nodes as regions computed with planar segmentations at increasing scales and the graph edges connect regions that are similar across scales. Connected components of the graph define the planar structures present in the point cloud within a scale interval. For instance, with this information, any point is associated to one or several planar structures existing at different scales. We then use topological data analysis to filter the graph and provide the most prominent planar structures. Our representation naturally encodes a large range of information. We show how to efficiently extract geometrical details (e.g. tiles of a roof), arrangements of simple shapes (e.g. steps and mean ramp of a staircase), and large-scale planar proxies (e.g. walls of a building) and present several interactive tools to visualize, select and reconstruct planar primitives directly from raw point clouds. The effectiveness of our approach is demonstrated by an extensive evaluation on a variety of input data, as well as by comparing against state-of-the-art techniques and by showing applications to polygonal mesh reconstruction.

## TODO
- Important
    - Persistence analysis
    - scripts to run the pipeline
    - test data
- Other
    - program to compute normals
    - replace 'simple' proj by 'quasi-orthogonal' proj
    - use cuda for computing multi-scale features
    - polygonal reconstruction
    - project point on planes or colorize them
    - provides timings
    - handle txt/xyz files