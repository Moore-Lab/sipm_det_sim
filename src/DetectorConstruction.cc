//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  fTargetLength      = 1.5*um; 
  fTargetRadius      = 1*mm;
  fDetectorLength    = 2*cm; 
  fDetectorThickness = 0.5*cm;
  fDetetectorTargetDistance = 0.5*cm;
  
  fWorldLength = 2*cm;
  fWorldThickness = 2*cm;
      
  DefineMaterials();
    
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ delete fDetectorMessenger;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // build materials
  //
  //fDetectorMater = 
  //new G4Material("Germanium", 32, 72.61*g/mole, 5.323*g/cm3);

  //G4Element* N  = new G4Element("Nitrogen", "N", 7, 14.01*g/mole);
  //G4Element* O  = new G4Element("Oxygen",   "O", 8, 16.00*g/mole);
  //
  //G4int ncomponents; G4double fractionmass;      
  //G4Material* GAGG = new G4Material("GAGG", 6.61*g/cm3, ncomponents=2,
  //                    kStateSolid, 293.*kelvin, 1.*atmosphere);
  //  GAGG->AddElement(N, fractionmass=0.7);
  //  GAGG->AddElement(O, fractionmass=0.3);
  //

  G4Element* N  = new G4Element("Nitrogen", "N", 7, 14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen",   "O", 8, 16.00*g/mole);
  //
  G4int ncomponents; G4double fractionmass;      
  G4Material* Air20 = new G4Material("Air", 1.205*mg/cm3, ncomponents=2,
                      kStateGas, 293.*kelvin, 1.*atmosphere);
    Air20->AddElement(N, fractionmass=0.7);
    Air20->AddElement(O, fractionmass=0.3);
  //
  fWorldMater = Air20;

  G4Element* Gd  = new G4Element("Gadolinium", "Gd", 64, 157.25*g/mole);
  G4Element* Al  = new G4Element("Aluminum", "Al", 13, 26.98*g/mole);
  G4Element* Ga  = new G4Element("Gallium", "Ga", 31, 69.7*g/mole);
  G4Material* GAGG = new G4Material("GAGG", 6.61*g/cm3, ncomponents=4,
                      kStateSolid, 293.*kelvin, 1.*atmosphere);
    GAGG->AddElement(Gd, fractionmass=0.51);
    GAGG->AddElement(Al, fractionmass=0.06);
    GAGG->AddElement(Ga, fractionmass=0.23);
    GAGG->AddElement(O, fractionmass=0.20);
  //

  fDetectorMater = GAGG;
  
  // or use G4 materials data base
  //
  G4NistManager* man = G4NistManager::Instance();  
  fTargetMater = man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
                   
 ///G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  
  // World
  //
  // (re) compute World dimensions if necessary
  //fWorldLength = std::max(fTargetLength,fDetectorLength);
  //fWorldThickness = fTargetRadius + fDetectorThickness;
    
  //G4Tubs*
  //sWorld = new G4Tubs("World",                                 //name
  //               0.,fWorldThickness, 0.5*fWorldLength, 0.,twopi); //dimensions  
  G4VSolid* sWorld = new G4Box("Detector", fWorldLength, fWorldLength, fWorldThickness);   

  G4LogicalVolume*
  lWorld = new G4LogicalVolume(sWorld,                  //shape
                             fWorldMater,               //material
                             "World");                  //name

  fPhysiWorld = new G4PVPlacement(0,                    //no rotation
                            G4ThreeVector(),            //at (0,0,0)
                            lWorld,                     //logical volume
                            "World",                    //name
                            0,                          //mother volume
                            false,                      //no boolean operation
                            0);                         //copy number
                            
  // Target
  //
  G4Tubs* 
  sTarget = new G4Tubs("Target",                                   //name
                  0., fTargetRadius, fTargetLength, 0.,twopi); //dimensions


  fLogicTarget = new G4LogicalVolume(sTarget,           //shape
                             fTargetMater,              //material
                             "Target");                 //name
                               
           new G4PVPlacement(0,                         //no rotation
                           G4ThreeVector(),             //at (0,0,0)
                           fLogicTarget,                //logical volume
                           "Target",                    //name
                           lWorld,                      //mother  volume
                           false,                       //no boolean operation
                           0);                          //copy number

  // Detector
  //
   //G4Tubs* 
  //sDetector = new G4Tubs("Detector",  
  //              fTargetRadius, fWorldThickness, 0.5*fDetectorLength, 0.,twopi);
  G4VSolid* sDetector = new G4Box("Detector", fDetectorLength, fDetectorLength, fDetectorThickness);   


  fLogicDetector = new G4LogicalVolume(sDetector,       //shape
                             fDetectorMater,            //material
                             "Detector");               //name
                               
           new G4PVPlacement(0,                         //no rotation
                           G4ThreeVector(0,0,fDetetectorTargetDistance+fDetectorThickness), //at (0,0,zdet)
                           fLogicDetector,              //logical volume
                           "Detector",                  //name
                           lWorld,                      //mother  volume
                           false,                       //no boolean operation
                           0);                          //copy number

 

  PrintParameters();
  
  //always return the root volume
  //
  return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintParameters()
{
  G4cout << "\n Target : Length = " << G4BestUnit(fTargetLength,"Length")
         << " Radius = " << G4BestUnit(fTargetRadius,"Length")  
         << " Material = " << fTargetMater->GetName();
  G4cout << "\n Detector : Length = " << G4BestUnit(fDetectorLength,"Length")
         << " Tickness = " << G4BestUnit(fDetectorThickness,"Length")  
         << " Material = " << fDetectorMater->GetName() << G4endl;          
  G4cout << "\n" << fTargetMater << "\n" << fDetectorMater << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetMaterial(G4String materialChoice)
{
  // search the material by its name
  G4Material* pttoMaterial =
     G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);   
  
  if (pttoMaterial) { 
    fTargetMater = pttoMaterial;
    if(fLogicTarget) { fLogicTarget->SetMaterial(fTargetMater); }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetTargetMaterial : "
           << materialChoice << " not found" << G4endl;
  }              
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorMaterial(G4String materialChoice)
{
  // search the material by its name
  G4Material* pttoMaterial =
     G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);   
  
  if (pttoMaterial) { 
    fDetectorMater = pttoMaterial;
    if(fLogicDetector) { fLogicDetector->SetMaterial(fDetectorMater); }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetDetectorMaterial : "
           << materialChoice << " not found" << G4endl;
  }              
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetRadius(G4double value)
{
  fTargetRadius = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetLength(G4double value)
{
  fTargetLength = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorThickness(G4double value)
{
  fDetectorThickness = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorLength(G4double value)
{
  fDetectorLength = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetTargetLength()
{
  return fTargetLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetTargetRadius()
{
  return fTargetRadius;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* DetectorConstruction::GetTargetMaterial()
{
  return fTargetMater;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LogicalVolume* DetectorConstruction::GetLogicTarget()
{
  return fLogicTarget;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetDetectorLength()
{
  return fDetectorLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetDetectorThickness()
{
  return fDetectorThickness;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* DetectorConstruction::GetDetectorMaterial()
{
  return fDetectorMater;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LogicalVolume* DetectorConstruction::GetLogicDetector()
{
  return fLogicDetector;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
