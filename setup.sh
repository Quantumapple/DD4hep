#!/bin/bash

DD4hepRoot="/afs/cern.ch/work/j/jongho/Project_DD4hep/Test/DD4hep"

source ${DD4hepRoot}/.dd4hep-ci.d/init_x86_64.sh
source ${DD4hepRoot}/bin/thisdd4hep.sh 
source ${DD4hepRoot}/examples/bin/thisDDCodex.sh
export DD4hepExamplesINSTALL="/afs/cern.ch/work/j/jongho/Project_DD4hep/Test/DD4hep/examples"

echo ""
echo "Path to example install = $DD4hepExamplesINSTALL"
echo "Environments for running DDCodex are set"
echo ""
