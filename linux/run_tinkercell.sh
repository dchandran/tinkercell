#!/bin/bash

if [ $1 == "dev" ]
then 
    TINKERCELL_PATH=@TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ/TinkerCell
else
    TINKERCELL_PATH=@TINKERCELL_EXE@@TINKERCELL_BIT@
fi

#library paths
export LD_LIBRARY_PATH=$TINKERCELL_PATH:$TINKERCELL_PATH/plugins:@OCTAVE_LIB_DIR@

#run tinkercell
$TINKERCELL_PATH/@TINKERCELL_EXE@
