FILE(GLOB EnzymeCatalysis_FILES EnzymeCatalysis/*.xml EnzymeCatalysis/*.sbml EnzymeCatalysis/*.txt EnzymeCatalysis/*.png EnzymeCatalysis/*.tic EnzymeCatalysis/*.ant)
FILE(GLOB ProteinProduction_FILES ProteinProduction/*.xml ProteinProduction/*.sbml ProteinProduction/*.txt ProteinProduction/*.png ProteinProduction/*.tic ProteinProduction/*.ant)
FILE(GLOB TranscriptionActivation_FILES TranscriptionActivation/*.xml TranscriptionActivation/*.sbml TranscriptionActivation/*.txt TranscriptionActivation/*.png TranscriptionActivation/*.tic TranscriptionActivation/*.ant)
FILE(GLOB TranscriptionRepression_FILES TranscriptionRepression/*.xml TranscriptionRepression/*.sbml TranscriptionRepression/*.txt TranscriptionRepression/*.png TranscriptionRepression/*.tic TranscriptionRepression/*.ant)
FILE(GLOB DirectGeneActivation_FILES DirectGeneActivation/*.xml DirectGeneActivation/*.sbml DirectGeneActivation/*.txt DirectGeneActivation/*.png DirectGeneActivation/*.tic DirectGeneActivation/*.ant)
FILE(GLOB DirectGeneRepression_FILES DirectGeneRepression/*.xml DirectGeneRepression/*.sbml DirectGeneRepression/*.txt DirectGeneRepression/*.png DirectGeneRepression/*.tic DirectGeneRepression/*.ant)
FILE(GLOB AllostericActivation_FILES AllostericActivation/*.xml AllostericActivation/*.sbml AllostericActivation/*.txt AllostericActivation/*.png AllostericActivation/*.tic AllostericActivation/*.ant)
FILE(GLOB AllostericInhibition_FILES AllostericInhibition/*.xml AllostericInhibition/*.sbml AllostericInhibition/*.txt AllostericInhibition/*.png AllostericInhibition/*.tic AllostericInhibition/*.ant)
FILE(GLOB Phosphorylation_FILES Phosphorylation/*.xml Phosphorylation/*.sbml Phosphorylation/*.txt Phosphorylation/*.png Phosphorylation/*.tic Phosphorylation/*.ant)

INSTALL( FILES modules.xml
  DESTINATION Modules
  COMPONENT Modules
)

INSTALL( FILES ${EnzymeCatalysis_FILES} 
  DESTINATION Modules/EnzymeCatalysis
  COMPONENT Modules
)

INSTALL( FILES ${ProteinProduction_FILES} 
  DESTINATION Modules/ProteinProduction
  COMPONENT Modules
)

INSTALL( FILES ${TranscriptionActivation_FILES} 
  DESTINATION Modules/TranscriptionActivation
  COMPONENT Modules
)

INSTALL( FILES ${TranscriptionRepression_FILES} 
  DESTINATION Modules/TranscriptionRepression
  COMPONENT Modules
)

INSTALL( FILES ${DirectGeneActivation_FILES} 
  DESTINATION Modules/DirectGeneActivation
  COMPONENT Modules
)

INSTALL( FILES ${DirectGeneRepression_FILES} 
  DESTINATION Modules/DirectGeneRepression
  COMPONENT Modules
)

INSTALL( FILES ${AllostericActivation_FILES} 
  DESTINATION Modules/AllostericActivation
  COMPONENT Modules
)

INSTALL( FILES ${AllostericInhibition_FILES} 
  DESTINATION Modules/AllostericInhibition
  COMPONENT Modules
)

INSTALL( FILES ${Phosphorylation_FILES} 
  DESTINATION Modules/Phosphorylation
  COMPONENT Modules
)

