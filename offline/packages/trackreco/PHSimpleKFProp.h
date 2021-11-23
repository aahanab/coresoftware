/*!
 *  \file		  PHTrackPropagating.h
 *  \brief		Base class for track seeding
 *  \author		Haiwang Yu <yuhw@nmsu.edu>
 */

#ifndef TRACKRECO_PHSIMPLEKFPROP_H
#define TRACKRECO_PHSIMPLEKFPROP_H

// PHENIX includes
#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsTrackingGeometry.h>
#include <trackbase/ActsSurfaceMaps.h>
#include <trackbase_historic/SvtxTrack_v2.h>
#include <phfield/PHField.h>
#include "nanoflann.hpp"
#include "ALICEKF.h"

// STL includes
#include <string>
#include <vector>
#include <memory>

#include <Eigen/Core>

// forward declarations
class PHCompositeNode;
class TrkrHitSetContainer;
class TrkrClusterContainer;
class TrkrClusterIterationMapv1;
class SvtxVertexMap;
class SvtxTrackMap;
class AssocInfoContainer;

typedef std::map<TrkrDefs::cluskey, Acts::Vector3F> PositionMap;

class PHSimpleKFProp : public SubsysReco
{
 public:
  PHSimpleKFProp(const std::string &name = "PHSimpleKFProp");
  virtual ~PHSimpleKFProp() {}

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;
  //void set_track_map_name(const std::string &map_name) { _track_map_name = map_name; }
  //void SetUseTruthClusters(bool setit){_use_truth_clusters = setit;}
  void set_field_dir(const double rescale)
  {
    std::cout << "rescale: " << rescale << std::endl;
    _fieldDir = 1;
    if(rescale > 0)
      _fieldDir = -1;
  }
  void set_max_window(double s){_max_dist = s;}
  void useConstBField(bool opt){_use_const_field = opt;}
  void useFixedClusterError(bool opt){_use_fixed_clus_err = opt;}
  void setFixedClusterError(int i, double val){_fixed_clus_err.at(i) = val;}
  void use_truth_clusters(bool truth)
  { _use_truth_clusters = truth; }
  void set_track_map_name(const std::string &map_name) { _track_map_name = map_name; }
  void SetIteration(int iter){_n_iteration = iter;}

 private:
  bool _use_truth_clusters = false;
  
  /// fetch node pointers
  int get_nodes(PHCompositeNode *topNode);
  std::vector<double> radii;
  std::vector<double> _vertex_x;
  std::vector<double> _vertex_y;
  std::vector<double> _vertex_z;
  std::vector<double> _vertex_xerr;
  std::vector<double> _vertex_yerr;
  std::vector<double> _vertex_zerr;
  std::vector<double> _vertex_ids;
  double _Bzconst = 10*0.000299792458f;
  //double _Bz = 1.4*_Bzconst;
  double _max_dist = .05;
  size_t _min_clusters_per_track = 20;
  double _fieldDir = -1;
  double _max_sin_phi = 1.;
  double _rz_outlier_threshold = .1;
  double _xy_outlier_threshold = .1;

  TrkrClusterContainer *_cluster_map = nullptr;
  SvtxTrackMap *_track_map = nullptr;
  TrkrHitSetContainer *_hitsets = nullptr;
  PHField* _field_map = nullptr;
  ActsTrackingGeometry *_tgeometry = nullptr;
  ActsSurfaceMaps *_surfmaps = nullptr;
  void MoveToFirstTPCCluster();
  PositionMap PrepareKDTrees();
  std::vector<TrkrDefs::cluskey> PropagateTrack(SvtxTrack* track, PositionMap& globalPositions);
  std::vector<std::vector<TrkrDefs::cluskey>> RemoveBadClusters(std::vector<std::vector<TrkrDefs::cluskey>> seeds, PositionMap& globalPositions);
  template <typename T>
  struct KDPointCloud
  {
    KDPointCloud<T>(){}
    std::vector<std::vector<T>> pts;
    inline size_t kdtree_get_point_count() const
    {
      return pts.size();
    }
    inline T kdtree_distance(const T* p1, const size_t idx_p2, size_t /*size*/) const
    {
      const T d0 = p1[0] - pts[idx_p2][0];
      const T d1 = p1[1] - pts[idx_p2][1];
      const T d2 = p1[2] - pts[idx_p2][2];
      return d0 * d0 + d1 * d1 + d2 * d2;
    }
    inline T kdtree_get_pt(const size_t idx, int dim) const
    {
      if (dim == 0)
        return pts[idx][0];
      else if (dim == 1)
        return pts[idx][1];
      else
        return pts[idx][2];
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /*bb*/) const
    {
      return false;
    }
  };
  std::vector<std::shared_ptr<KDPointCloud<double>>> _ptclouds;
  std::vector<std::shared_ptr<nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<double, KDPointCloud<double>>, KDPointCloud<double>,3>>> _kdtrees;
  std::shared_ptr<ALICEKF> fitter;
  double get_Bz(double x, double y, double z);
  void publishSeeds(std::vector<SvtxTrack_v2>);
  void publishSeeds(std::vector<SvtxTrack>);
  void MoveToVertex();

  void line_fit(std::vector<std::pair<double,double>> points, double &A, double &B);
  void line_fit_clusters(std::vector<TrkrCluster*> clusters, double &A, double &B);
  void CircleFitByTaubin(std::vector<Acts::Vector3F> points, double &R, double &X0, double &Y0);
  void findRoot(const double R, const double X0, const double Y0,
		double& x, double& y);
  bool _use_const_field = false;
  bool _use_fixed_clus_err = false;
  std::array<double,3> _fixed_clus_err = {.1,.1,.1};
  TrkrClusterIterationMapv1* _iteration_map = nullptr;
  int _n_iteration = 0;
  std::string _track_map_name = "SvtxTrackMap";

};

#endif
