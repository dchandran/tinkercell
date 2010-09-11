FILE(GLOB EnzymeCatalysis_FILES EnzymeCatalysis/*.*)
FILE(GLOB ProteinProduction_FILES ProteinProduction/*.*)
FILE(GLOB TranscriptionActivation_FILES TranscriptionActivation/*.*)
FILE(GLOB TranscriptionRepression_FILES TranscriptionRepression/*.*)
FILE(GLOB DirectGeneActivation_FILES DirectGeneActivation/*.*)
FILE(GLOB DirectGeneRepression_FILES DirectGeneRepression/*.*)

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

