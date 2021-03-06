# Copyright CERN and copyright holders of ALICE O2. This software is distributed
# under the terms of the GNU General Public License v3 (GPL Version 3), copied
# verbatim in the file "COPYING".
#
# See http://alice-o2.web.cern.ch/license for full licensing information.
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization or
# submit itself to any jurisdiction.

if(pythia6_FOUND)
  set(pythia6Target ROOT::EGPythia6 MC::Pythia6)
endif()

if(pythia_FOUND)
  set(pythiaTarget MC::Pythia)
endif()

if(HepMC_FOUND)
  set(hepmcTarget MC::HepMC)
endif()

o2_add_library(Generators
               SOURCES src/Generator.cxx
	       	       src/Trigger.cxx
	       	       src/TriggerParticle.cxx
                       src/GeneratorTGenerator.cxx
                       src/GeneratorFromFile.cxx
                       src/PDG.cxx
                       src/PrimaryGenerator.cxx
                       src/InteractionDiamondParam.cxx
                       src/TriggerParticleParam.cxx
                       src/BoxGunParam.cxx
		       src/QEDGenParam.cxx
                       src/GeneratorFactory.cxx
                       $<$<BOOL:${pythia6_FOUND}>:src/GeneratorPythia6.cxx>
                       $<$<BOOL:${pythia6_FOUND}>:src/GeneratorPythia6Param.cxx>
                       $<$<BOOL:${pythia_FOUND}>:src/GeneratorPythia8.cxx>
                       $<$<BOOL:${pythia_FOUND}>:src/DecayerPythia8.cxx>
                       $<$<BOOL:${pythia_FOUND}>:src/GeneratorPythia8Param.cxx>
                       $<$<BOOL:${pythia_FOUND}>:src/DecayerPythia8Param.cxx>
                       $<$<BOOL:${HepMC_FOUND}>:src/GeneratorHepMC.cxx>
               PUBLIC_LINK_LIBRARIES FairRoot::Base O2::SimConfig O2::CommonUtils
                                     O2::SimulationDataFormat ${pythia6Target} ${pythiaTarget} ${hepmcTarget}
                                     FairRoot::Gen
               TARGETVARNAME targetName)

if(pythia6_FOUND)
  target_compile_definitions(${targetName} PUBLIC GENERATORS_WITH_PYTHIA6)
endif()

if(pythia_FOUND)
  target_compile_definitions(${targetName} PUBLIC GENERATORS_WITH_PYTHIA8)
endif()

if(HepMC_FOUND)
  target_compile_definitions(${targetName} PUBLIC GENERATORS_WITH_HEPMC3)
endif()

set(headers
    include/Generators/Generator.h
    include/Generators/Trigger.h
    include/Generators/TriggerParticle.h
    include/Generators/GeneratorTGenerator.h
    include/Generators/GeneratorFromFile.h
    include/Generators/PDG.h
    include/Generators/PrimaryGenerator.h
    include/Generators/InteractionDiamondParam.h
    include/Generators/TriggerParticleParam.h
    include/Generators/ConfigurationMacroHelper.h
    include/Generators/BoxGunParam.h
    include/Generators/QEDGenParam.h)

if (pythia6_FOUND)
   list(APPEND headers include/Generators/GeneratorPythia6.h
   	       include/Generators/GeneratorPythia6Param.h)
endif()

if(pythia_FOUND)
  list(APPEND headers 
              include/Generators/GeneratorPythia8.h
              include/Generators/DecayerPythia8.h
              include/Generators/GeneratorPythia8Param.h
              include/Generators/DecayerPythia8Param.h
              include/Generators/GeneratorFactory.h)
endif()

if(HepMC_FOUND)
  list(APPEND headers include/Generators/GeneratorHepMC.h)
endif()

o2_target_root_dictionary(Generators HEADERS ${headers})

o2_add_test_root_macro(share/external/extgen.C
                       PUBLIC_LINK_LIBRARIES O2::Generators FairRoot::Base
                       LABELS generators COMPILE_ONLY)
if(pythia6_FOUND)
  o2_add_test_root_macro(share/external/pythia6.C
                         PUBLIC_LINK_LIBRARIES O2::Generators
                         LABELS generators COMPILE_ONLY)
endif()

if(doBuildSimulation)
  o2_add_test_root_macro(share/external/QEDLoader.C
                         PUBLIC_LINK_LIBRARIES O2::Generators
                                               O2::SimConfig
                                               LABELS generators)
endif()

o2_add_test_root_macro(share/external/tgenerator.C
                       PUBLIC_LINK_LIBRARIES O2::Generators
                       LABELS generators)

o2_add_test_root_macro(share/external/trigger_multiplicity.C
                       PUBLIC_LINK_LIBRARIES O2::Generators
                       LABELS generators)

if(pythia_FOUND)
o2_add_test_root_macro(share/external/trigger_mpi.C
                       PUBLIC_LINK_LIBRARIES O2::Generators
                       LABELS generators)

o2_add_test_root_macro(share/egconfig/pythia8_userhooks_charm.C
        PUBLIC_LINK_LIBRARIES O2::Generators
                       LABELS generators)
endif()		       
		     
install(FILES share/external/extgen.C share/external/pythia6.C
              share/external/tgenerator.C share/external/QEDLoader.C share/external/QEDepem.C
	      share/external/trigger_multiplicity.C
	      share/external/trigger_mpi.C
        DESTINATION share/Generators/external/)

install(FILES share/egconfig/pythia8_inel.cfg
              share/egconfig/pythia8_hf.cfg
	      share/egconfig/pythia8_hi.cfg
	      share/egconfig/pythia8_userhooks_charm.C
        DESTINATION share/Generators/egconfig/)

install(FILES share/pythia8/decays/base.cfg
              share/pythia8/decays/bjpsidimuon.cfg
        DESTINATION share/Generators/pythia8/decays/)

      
