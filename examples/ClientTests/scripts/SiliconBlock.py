#
#
import os, sys, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   DD4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir+'/examples/DDG4/examples';
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/SiliconBlock.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
  DDG4.importConstants(kernel.lcdd(),debug=False)
  simple = DDG4.Simple(kernel,tracker='Geant4TrackerCombineAction')
  simple.printDetectors()
  # Configure UI
  if len(sys.argv)>1:
    simple.setupCshUI(macro=sys.argv[1])
  else:
    simple.setupCshUI()

  # Configure Event actions
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.INFO

  # Configure I/O
  evt_root = simple.setupROOTOutput('RootOutput','SiliconBlock_'+time.strftime('%Y-%m-%d_%H-%M'))

  # Setup particle gun
  gun = simple.setupGun("Gun",particle='mu-',energy=20*GeV,multiplicity=1)
  gun.output_level = generator_output_level

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100*MeV
  part.OutputLevel = Output.INFO #generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 3.0*m
  user.TrackingVolume_Rmax = 3.0*m
  user.enableUI()
  part.adopt(user)

  simple.setupTracker('SiliconBlockUpper')
  simple.setupTracker('SiliconBlockDown')

  # Now build the physics list:
  phys = simple.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel,'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4Geantino')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  simple.execute()

if __name__ == "__main__":
  run()
