# Plane Detection in 3D Point Clouds

This c++ project is the author's implementation of the following article   

## Persistence Analysis of Multi-scale Planar Structure Graph in Point Clouds [(PDF)](https://hal.archives-ouvertes.fr/hal-02490721/document)  

*Thibault Lejemble*  
*Claudio Mura*  
*Loïc Barthe*  
*Nicolas Mellado*  

Computer Graphics Forum 2020  
Eurographics 2020  

**Abstract**  
Abstract Modern acquisition techniques generate detailed point clouds that sample complex geometries. For instance, we are able to produce millimeter-scale acquisition of whole buildings. Processing and exploring geometrical information within such point clouds requires scalability, robustness to acquisition defects and the ability to model shapes at different scales. In this work, we propose a new representation that enriches point clouds with a multi-scale planar structure graph. We define the graph nodes as regions computed with planar segmentations at increasing scales and the graph edges connect regions that are similar across scales. Connected components of the graph define the planar structures present in the point cloud within a scale interval. For instance, with this information, any point is associated to one or several planar structures existing at different scales. We then use topological data analysis to filter the graph and provide the most prominent planar structures. Our representation naturally encodes a large range of information. We show how to efficiently extract geometrical details (e.g. tiles of a roof), arrangements of simple shapes (e.g. steps and mean ramp of a staircase), and large-scale planar proxies (e.g. walls of a building) and present several interactive tools to visualize, select and reconstruct planar primitives directly from raw point clouds. The effectiveness of our approach is demonstrated by an extensive evaluation on a variety of input data, as well as by comparing against state-of-the-art techniques and by showing applications to polygonal mesh reconstruction.

## Getting Started

### Depencencies

The only required dependency is the C++ header-only library [CGAL](https://github.com/CGAL/cgal).  
To obtain CGAL on Linux use
```
sudo apt-get install libcgal-dev
```   
Other depencencies like [Eigen](https://gitlab.com/libeigen/eigen) and [Ponca](https://github.com/poncateam/ponca) are directly included in the sources of this project. 

### Compilation

To compile the code and install the programs in `/usr/local/bin` run the following commands  
```
git clone git@github.com:ThibaultLejemble/Plane-Detection-Point-Cloud.git
cd Plane-Detection-Point-Cloud
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j 
sudo make install
```

Tested using
- Debian GNU/Linux 10 (buster)
- cmake 3.13.4
- g++ 8.3.0
- CGAL 5.1

### Usage

#### Replicate results 

Several scripts available in the [figures](https://github.com/ThibaultLejemble/Plane-Detection-Point-Cloud/tree/main/figures) directory generate some part of the figures presented in the article **Persistence Analysis of Multi-scale Planar Structure Graph in Point Clouds [(PDF)](https://hal.archives-ouvertes.fr/hal-02490721/document)**.

Once the project is compiled and installed (see above), run the following command from the [figures](https://github.com/ThibaultLejemble/Plane-Detection-Point-Cloud/tree/main/figures) directory
```
./replicate.sh
```
The resulting colored point clouds are contained in several PLY files `fig_*.ply`.

#### Process other 3D point clouds

TODO

## TODO
- add documentations on the different programs
- avoid recomputing intermediate data for figures replication
- improve documentation of the code
- output txt files for results

