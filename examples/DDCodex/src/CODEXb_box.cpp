//==========================================================================
// AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : Jongho.Lee
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens)  {
  xml_det_t  x_det  (e);
  xml_comp_t x_box      = x_det.envelope();
  xml_dim_t  pos        = x_det.position();
  string     det_name   = x_det.nameStr();
  DetElement det(det_name,x_det.id());
  //double     env_dz     .dz()/2.0;
  double     env_width  = x_box.width();
  double     env_length = x_box.length();
  double     env_height = x_box.height();
  // If the cone should be removed and replaced by an assembly,
  // comment the upper to and enable the lower line
  Assembly   env_vol(det_name);
  PlacedVolume pv;
  SensitiveDetector sd = sens;

  sd.setType("tracker");
  // Set visualization, limits and region (if present)
  env_vol.setRegion(description, x_det.regionStr());
  env_vol.setLimitSet(description, x_det.limitsStr());
  env_vol.setVisAttributes(description, x_det.visStr());
  printout(INFO,"CODEX-b box","%s width=%g length=%g height= %g",
           det_name.c_str(), env_width, env_length, env_height  );

  Box b(env_width,env_length,env_height);
  Volume vb(det_name+"_bbb",b,description.material("Si"));
  pv = env_vol.placeVolume(vb, Position(pos.x(),pos.y(),pos.z()));
  printout(INFO,"CODEX-b box","%s X=%g Y=%g Z= %g",
           det_name.c_str(), pos.x(), pos.y(), pos.z()  );
  vb.setSensitiveDetector(sd);
  vb.setVisAttributes(description,x_det.visStr());
  pv.addPhysVolID("type", 0);

  env_vol.setVisAttributes(description, x_det.visStr());
  env_vol.setLimitSet(description, x_det.limitsStr());
  env_vol.setRegion(description, x_det.regionStr());
  

  ///// Test for 1 layer into a CODEXb-box /////
  int num_sensitive = 0;
  for(xml_coll_t i(x_det, _U(station)); i; ++i) {
      xml_comp_t station  = i;
      double    st_width  = width.station();
      double    st_thick  = thickness.station();
      double    st_height = height.station();
      double    st_x      = x.station();
      double    st_y      = y.station();
      double    st_z      = z.station(); 
      string    nam = det_name+"_"+station.nameStr();
      Box       box(st_width, st_thick, st_height);
      Material  mat(description.material(station.attr<string>(_U(material))));
      Volume    vol(nam,box,mat);

      //printout(INFO, "CODEX-b station", "%s Station:
     
      vol.setVisAttributes(description, station.visStr());
      pv = env_vol.placeVolume(vol,Position(st_x, st_y, st_z));
      if( station.isSensitive() )  {
          DetElement st_det(det, "station_"+nam, x_det.id());
          vol.setSensitiveDetector(sd);
          pv.addPhysVolID("type",0);
          pv.addPhysVolID("station", num_sensitive);
          st_det.setPlacement(pv);
          ++num_sensitive;
      }  
  }

  if ( x_det.isSensitive() )   {   
      xml_dim_t sd_typ = x_det.child(_U(sensitive));
      env_vol.setSensitiveDetector(sens);
      sd.setType(sd_typ.typeStr());
  }
  Volume mother = description.pickMotherVolume(det);
  //Transform3D trafo(Position(pos.x(),pos.y(),pos.z()));
  //Direction dir = trafo*Direction(0,0,1.);
  pv = mother.placeVolume(env_vol);
  if ( x_det.hasAttr(_U(id)) )  {
      pv.addPhysVolID("system",x_det.id());
  }
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(DD4hep_CODEXb_layer,create_element)
