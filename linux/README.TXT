-------------------------------------------------------
Follow these steps to run TinkerCell on Ubuntu
-------------------------------------------------------
1. Install Qt, Python, and Octave using the command:

    sudo apt-get install libqt4-core libqt4-gui libqt4-opengl libqt4-xml libxml2 python octave

     optional: download and install PySCeS

2. download TinkerCell tar file and extract it using the command:
                 tar xzvf TinkerCell64.tar.gz  (or TinkerCell32.tar.gz)

3. cd into the TinkerCell folder

4. edit the run_tinkercell.sh script 

    * check that the TINKERCELL_PATH variable is the correct TinkerCell folder
    * check that the Octave path is correct (if you have octave installed)

5. source run_tinkercell

