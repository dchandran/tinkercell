#include "TC_structs.h"

BEGIN_C_DECLS

/*!\brief An algorithm that does automatically calculates the next set of positions for performing force-based auto-layout. 
                Use this if you want to make updates during each iteration.
 \param tc_matrix matrix with 5 columns - x, y, mass, dx, dy
 \param tc_matrix a square matrix with 1 or 0 indicating a connection form i to j
 \param double spring constant
 \param double charge constant
 \param double damping constant
 \return double total velocity in the system (use this in the stopping criterion)
 \ingroup Get and set position
*/
TCAPIEXPORT double ApplySpringForce(tc_matrix nodes, tc_matrix connections, double spring, double charge, double damping);


END_C_DECLS
