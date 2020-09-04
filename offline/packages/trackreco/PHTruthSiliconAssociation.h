// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PHTRUTHSILICONASSOCIATION_H
#define PHTRUTHSILICONASSOCIATION_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;
class SvtxTrackMap;
class SvtxTrack;
class TrkrClusterContainer;
class SvtxEvaluator;
class SvtxEvalStack;
class SvtxClusterEval;
class PHG4TruthInfoContainer;
class AssocInfoContainer;

class PHTruthSiliconAssociation : public SubsysReco
{
 public:

  PHTruthSiliconAssociation(const std::string &name = "PHTruthSiliconAssociation");

  virtual ~PHTruthSiliconAssociation();

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;
  
 private:

  int GetNodes(PHCompositeNode* topNode);

  SvtxEvaluator *_svtxEvaluator{nullptr};
  PHG4TruthInfoContainer *_truthInfo{nullptr};
  
  TrkrClusterContainer *_cluster_map;
  SvtxEvalStack *_svtxEvalStack;
  SvtxTrackMap *_track_map;
  AssocInfoContainer *_assoc_container;
  SvtxTrack *_tracklet;

  int _ntrack = 0;
  const int  _min_clusters_per_track = 20;


};

#endif // PHTRUTHSILICONASSOCIATION_H
