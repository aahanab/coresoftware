// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef G4MAIN_PHG4STEPPINGACTION_H
#define G4MAIN_PHG4STEPPINGACTION_H

#include <limits>
#include <set>
#include <string>

class G4Step;
class PHCompositeNode;
class PHG4Hit;

class PHG4SteppingAction
{
 public:
  PHG4SteppingAction(const std::string& name, const int i = 0);
  virtual ~PHG4SteppingAction() = default;

  //! stepping action. This defines processing of a single step in a given volume
  /*!
  returns true if hits associated to this step was processed by this detector
  \param step pointer to the geant 4 step class
  \param was_used: true if the hit was already used by a previously registered subsystem
  */
  virtual bool UserSteppingAction(const G4Step* step, bool was_used) = 0;

  virtual void Verbosity(const int i) { m_Verbosity = i; }
  virtual int Verbosity() const { return m_Verbosity; }
  virtual int Init() { return 0; }
  virtual int InitWithNode(PHCompositeNode*) { return 0; };
  //! get scintillation photon count. It require a custom set SCINTILLATIONYIELD property to work
  virtual double GetScintLightYield(const G4Step* step);

  //! get amount of energy that can make scintillation light, in Unit of GeV.
  virtual double GetVisibleEnergyDeposition(const G4Step* step);

  //! Extract local coordinate of the hit and save to PHG4Hit
  virtual void StoreLocalCoordinate(PHG4Hit* hit, const G4Step* step, const bool do_prepoint, const bool do_postpoint);

  virtual void SetInterfacePointers(PHCompositeNode*) { return; }
  virtual void Print(const std::string& /*what*/) const { return; }
  const std::string &GetName() const { return m_Name; }
  void SetName(const std::string& name) { m_Name = name; }
  virtual void SetLightCorrection(const double inner_radius, const double inner_corr, const double outer_radius, const double outer_corr);
  virtual double GetLightCorrection(const double r) const;
  virtual double GetLightCorrection(const double xpos, const double ypos) const;
  virtual bool ValidCorrection() const;

  //! Set the G4HIT node names from Subsystem rather than constructing your own
  virtual void SetHitNodeName(const std::string&, const std::string&) { return; }

 private:
  int m_Verbosity{0};
  double m_LightBalanceInnerRadius{std::numeric_limits<double>::quiet_NaN()};
  double m_LightBalanceInnerCorr{std::numeric_limits<double>::quiet_NaN()};
  double m_LightBalanceOuterRadius{std::numeric_limits<double>::quiet_NaN()};
  double m_LightBalanceOuterCorr{std::numeric_limits<double>::quiet_NaN()};
  std::string m_Name;
  std::set<std::string> m_ScintLightYieldMissingMaterialSet;
};

#endif  // G4MAIN_PHG4STEPPINGACTION_H
