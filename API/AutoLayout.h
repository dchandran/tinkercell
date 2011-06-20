#include "TC_structs.h"

/*!\brief An algorithm that does automatically arranges the positions of the given items.
 \param tc_items list of items
 \param double spring constant
 \param double charge constant
 \param double damping constant
 \param double threshold for stopping
 \ingroup Get and set position
*/
TCAPIEXPORT void Autolayout(tc_matrix positions, double spring, double charge, double damping, double threshold);

/*!\brief An algorithm that does automatically calculates the next set of positions for performing force-based auto-layout. 
                Use this if you want to make updates during each iteration.
 \param tc_matrix matrix with 5 columns - x, y, mass, dx, dy
 \param double spring constant
 \param double charge constant
 \param double damping constant
 \return double total velocity in the system (use this in the stopping criterion)
 \ingroup Get and set position
*/
TCAPIEXPORT double ApplySpringForce(tc_matrix nodes, double spring, double charge, double damping);


