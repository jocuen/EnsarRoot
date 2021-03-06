// --------------------------------------------------------------------
// -----                  EnsarMultiView source file                -----
// --------------------------------------------------------------------

#include "EnsarMultiView.h"


//______________________________________________________________________________
// Full description of EnsarMultiView
//

ClassImp(EnsarMultiView)

EnsarMultiView* EnsarMultiView::fgInstance = 0;

EnsarMultiView* EnsarMultiView::Instance()
{
  // Return static instance.

  return fgInstance;
}

EnsarMultiView::EnsarMultiView() :
  fRPhiMgr(0), fRhoZMgr(0),
  f3DView(0), fRPhiView(0), fRhoZView(0),
  fRPhiGeomScene(0), fRhoZGeomScene(0),
  fRPhiEventScene(0), fRhoZEventScene(0),
  fGeomGentle(0), fGeomGentleRPhi(0), fGeomGentleRhoZ(0)
{
  // Constructor --- creates required scenes, projection managers
  // and GL viewers.

  if (fgInstance)
    throw TEveException("EnsarMultiView::EnsarMultiView already instantiated.");
  fgInstance = this;

  // Scenes
  //========

  fRPhiGeomScene  = gEve->SpawnNewScene("RPhi Geometry",
                                        "Scene holding projected geometry for the RPhi view.");
  fRhoZGeomScene  = gEve->SpawnNewScene("RhoZ Geometry",
                                        "Scene holding projected geometry for the RhoZ view.");
  fRPhiEventScene = gEve->SpawnNewScene("RPhi Event Data",
                                        "Scene holding projected event-data for the RPhi view.");
  fRhoZEventScene = gEve->SpawnNewScene("RhoZ Event Data",
                                        "Scene holding projected event-data for the RhoZ view.");

  // Projection managers
  //=====================

  fRPhiMgr = new TEveProjectionManager();
  fRPhiMgr->SetProjection(TEveProjection::kPT_RPhi);
  gEve->AddToListTree(fRPhiMgr, kFALSE);
  {
    TEveProjectionAxes* a = new TEveProjectionAxes(fRPhiMgr);
    a->SetMainColor(kWhite);
    a->SetTitle("R-Phi");
    a->SetTitleSize(0.05);
    a->SetTitleFont(102);
    a->SetLabelSize(0.025);
    a->SetLabelFont(102);
    fRPhiGeomScene->AddElement(a);
  }
  fRPhiMgr->ImportElements(gEve->GetGlobalScene(),fRPhiGeomScene);
  fRPhiGeomScene->AddElement(fRPhiMgr);

  fRhoZMgr = new TEveProjectionManager();
  fRhoZMgr->SetProjection(TEveProjection::kPT_RhoZ);
  gEve->AddToListTree(fRhoZMgr, kFALSE);
  {
    TEveProjectionAxes* a = new TEveProjectionAxes(fRhoZMgr);
    a->SetMainColor(kWhite);
    a->SetTitle("Rho-Z");
    a->SetTitleSize(0.05);
    a->SetTitleFont(102);
    a->SetLabelSize(0.025);
    a->SetLabelFont(102);
    fRhoZGeomScene->AddElement(a);
  }

  // Viewers
  //=========

  TEveWindowSlot *slot = 0;
  TEveWindowPack *pack = 0;

  slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  pack = slot->MakePack();
  pack->SetElementName("Multi View");
  pack->SetHorizontal();
  pack->SetShowTitleBar(kFALSE);
  pack->NewSlot()->MakeCurrent();
  f3DView = gEve->SpawnNewViewer("3D View", "");
  f3DView->AddScene(gEve->GetGlobalScene());
  f3DView->AddScene(gEve->GetEventScene());

  pack = pack->NewSlot()->MakePack();
  pack->SetShowTitleBar(kFALSE);
  pack->NewSlot()->MakeCurrent();
  fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
  fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  //fRPhiView->AddScene(fRPhiGeomScene);
  //fRPhiView->AddScene(fRPhiEventScene);
  fRPhiView->AddScene(gEve->GetGlobalScene());
  fRPhiView->AddScene(gEve->GetEventScene());

  pack->NewSlot()->MakeCurrent();
  fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
  fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  //fRhoZView->AddScene(fRhoZGeomScene);
  //fRhoZView->AddScene(fRhoZEventScene);
  fRhoZView->AddScene(gEve->GetGlobalScene());
  fRhoZView->AddScene(gEve->GetEventScene());

}

//-------------------------------------------------------------------------

void EnsarMultiView::InitGeomGentle(TEveGeoShape* g3d, TEveGeoShape* grphi, TEveGeoShape* grhoz)
{
  // Initialize gentle geometry.

  fGeomGentle     = g3d;
  fGeomGentleRPhi = grphi; fGeomGentleRPhi->IncDenyDestroy();
  fGeomGentleRhoZ = grhoz; fGeomGentleRhoZ->IncDenyDestroy();

  ImportGeomRPhi(fGeomGentleRPhi);
  ImportGeomRhoZ(fGeomGentleRhoZ);
}

//-------------------------------------------------------------------------

void EnsarMultiView::SetDepth(Float_t d)
{
  // Set current depth on all projection managers.

  fRPhiMgr->SetCurrentDepth(d);
  fRhoZMgr->SetCurrentDepth(d);

}

//-------------------------------------------------------------------------

void EnsarMultiView::ImportGeomRPhi(TEveElement* el)
{ 
  // Import el into r-phi geometry scene.

  fRPhiMgr->ImportElements(el, fRPhiGeomScene);
}

void EnsarMultiView::ImportGeomRhoZ(TEveElement* el)
{ 
  // Import el into rho-z geometry scene.

  fRhoZMgr->ImportElements(el, fRhoZGeomScene);
}

void EnsarMultiView::ImportEventRPhi(TEveElement* el)
{ 
  // Import el into r-phi event scene.

  fRPhiMgr->ImportElements(el, fRPhiEventScene);
}

void EnsarMultiView::ImportEventRhoZ(TEveElement* el)
{ 
  // Import el into rho-z event scene.

  fRhoZMgr->ImportElements(el, fRhoZEventScene);
}

void EnsarMultiView::DestroyEventRPhi()
{
  // Destroy all elements in r-phi event scene.

  fRPhiEventScene->DestroyElements();
}

void EnsarMultiView::DestroyEventRhoZ()
{
  // Destroy all elements in rho-z event scene.

  fRhoZEventScene->DestroyElements();
}

//-------------------------------------------------------------------------

void EnsarMultiView::SetCenterRPhi(Double_t x, Double_t y, Double_t z)
{
  // Set center of r-phi manager.

  fRPhiMgr->SetCenter(x, y, z);
}

void EnsarMultiView::SetCenterRhoZ(Double_t x, Double_t y, Double_t z)
{
  // Set center of rho-z manager.

  fRhoZMgr->SetCenter(x, y, z);
}

void EnsarMultiView::DestroyAllGeometries()
{
  // Destroy 3d, r-phi and rho-z geometries.

  fGeomGentle->DestroyElements();
  fGeomGentleRPhi->DestroyElements();
  fGeomGentleRhoZ->DestroyElements();

}

