#!/bin/bash

TINKERCELL_PATH=@TINKERCELL_BINARY_DIR@/_CPack_Packages/Linux/TGZ/TinkerCell

#library paths
export LD_LIBRARY_PATH=$TINKERCELL_PATH:$TINKERCELL_PATH/plugins:@OCTAVE_LIB_DIR@

#run tinkercell
$TINKERCELL_PATH/TinkerCell
