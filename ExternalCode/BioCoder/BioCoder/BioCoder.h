#ifndef __BIOCODER__
#define __BIOCODER__
#include <iostream>
#include <conio.h>
#include <cstdio>
#include <fstream>
#include "Graph.h"

/*! \mainpage 
 *
 * \section intro_sec Introduction
 *
 * BioCoder is a high-level programming language that enables standardization and automation of
 * biology protocols. Our vision is to change the way that experimental methods are communicated: rather than
 * publishing a written account of the protocols used, researchers will simply publish the code. The code can be automatically
 * converted to human-readable steps for manual execution in the laboratory. When written as a computer
 * program, biology protocols can be parameterized to facilitate reuse in different contexts. They can also be
 * mapped automatically to the setup of a given laboratory, taking into account the equipment and reagents that
 * are available.<br>
 *
 * \section links Some important links:
 * \li \ref page1
 * \li \ref page2
 * \li \ref example
 * \li \ref oww
 *
 * \section notes_sec Notes
 *
 * Before starting to use BioCoder, please read this section for basic information on usage. Doing this will prevent 
 * loss of considerable amount of time you may spend in trying to figure out details by yourself. Thanks!<br>
 *
 *\li \ref sec 
 *\li \ref sec1
 *\li \ref sec2
 *\li \ref sec3
 *\li \ref sec4
 *\li \ref sec5
 *\li \ref sec6
 *\li \ref sec7
 *
 *\subsection sec General Instructions
 * The language is C-based, meaning the general syntax rules of C will have to be obeyed when using BioCoder, like semi-colons at the end of every statement
 * naming rules for identifiers etc. For more information on these syntax rules, please refer to any standard documentation for the C programming language.
 *
 *\subsection sec1 The layout of the protocol
 *
 * The protocol that is generated is an html file that is organized as follows:<br>
 * - The top-most line is the title of the protocol.<br>
 * - Below the title is the column listing the required reagents and solutions.<br>
 * - Adjacent to the reagents/solutions column is the column listing the equipment required.<br>
 * - What follows is the actual protocol listed as a sequence of points under "Steps".<br>
 *
 *\subsection sec2 Containers as the basis of all instructions
 *
 * The design of BioCoder is such that all instructions are carried out with containers as input parameters. The operations
 * themselves, though, are on the contents of the container.
 *
 *\subsection sec3 Declaration of new fluids, solids and columns
 *
 * To keep the "Reagents/Solutions" column at the top, make sure that all the fluids used during the course of a protocol 
 * have been declared using the "new_fluid", "new_solid" or "new_column" command at the start of the protocol. Containers
 * can be declared as and when they are used, but declaring the containers with the rest of the declarations makes it visually
 * pleasing.
 *
 *\subsection sec4 Usage of time(), speed(), vol(), time_range(),...
 *
 * Time durations, speeds and volumes occuring in a protocol have to be as follows:
 * 
 * <b>Time:</b><br>
 * (i) time(int x, enum time_unit unit) - for exact time. Eg: \p time(3, MINS) for "3 mins".<br>
 * (ii) time_range(int x, int y, enum time_unit unit) - for a time range. Eg. \p time_range(3, 5, MINS) for "3 - 5 mins".<br>
 * (iii) min_time(int x, enum time_unit unit) - for the minimum time. Eg. \p min_time(3, MINS) for "at least 3 mins".<br>
 * (iv) max_time(int x, enum time_unit unit) - for the maximum time. Eg. \p max_time(3, MINS) for "at most 3 mins".<br>
 * (v) approx_time(int x, enum time_unit unit) - for approximate time. Eg. \p approx_time(3, MINS) for "~3 mins".<br>
 * All of the above are used in an instruction that requires a \p Time* input in a parameter field.<br>
 *
 * <b>Speed:</b><br>
 * Usage similar to "Time". Just replace "time" with "speed" and use in an instruction that requires a \p Speed* input in a parameter field.<br>
 * 
 * <b>Volume:</b><br>
 * Usage similar to "Time", but no equivalents for "min_time" and "max_time" exist. For the rest, just replace "time" with "vol" and use in an instruction 
 * that requires a \p Volume* input in a parameter field.<br>
 * Another special feature of volume specification is the provision for symbolic volume. More about symbolic volumes in the \ref sec5 "next section" .
 *
 *\subsection sec5 Symbolic volumes
 * In BioCoder, you can define symbolic volumes at the start of a protocol, after having declared the Solutions/Reagents using
 * the "new_symbol" instruction. Subsequent references to that symbolic volume can be made as follows in a parameter field that 
 * requires a \p Symbolic_volume* input:<br>
 * \p s_vol(Symbol s) 
 * 
 *\subsection sec6 Compound instructions
 * In addition to the basic set of orthogonal primitives, BioCoder possesses instructions that are combinations of these basic primitives and these are
 * referred to as the 'compound instructions'. The only purpose of these compound instructions is to make the text-output of a BioCoder protocol read 
 * better. Examples of compound primitives are \p measure_fluid(), \p combine_and_mix(), \p incubate() etc.
 *
 *\subsection sec7 Definitions and enumerations
 *
 * Take a while to go through the definitions and enumerations offered by BioCoder to make your job of coding up a protocol much easier. In the definitions, 
 * quantities like \p ON_ICE, \p SPEED_MAX have been defined, while in the enumeration section, \p speed_unit, \p vol_unit, \p container_type have been designated values.
 * One thing to note here is that all definitions and enumerations are in the upper case to help differentiate them from the rest of the protocol.
 *
 */
 /*! \page page1 Instructions classified based on the functions performed
 * Please click on the required functionality to access the corresponding instructions.
 *
 -\subpage class1 <br>
 -\subpage class2 <br>
 -\subpage class3 <br>
 -\subpage class4 <br>
 -\subpage class5 <br>
 -\subpage class6 <br>
 -\subpage class7 <br>
 -\subpage class8 <br>
 -\subpage class9 <br>
 -\subpage class10 <br>
 -\subpage class11 <br>
 -\subpage class12 <br>
 -\subpage class13 <br>
 -\subpage class14 <br>
 -\subpage class15 <br>
 -\subpage class16 <br>
 -\subpage class17 <br>
 *
 *\page class1 1. Writing a new protocol
 * 1. \p start_protocol()<br>
 * 2. \p end_protocol()<br>
 * 3. \p first_step()<br>
 * 4. \p next_step()<br>
 * 5. \p optional_step()<br>
 * 6. \p first_option()<br>
 * 7. \p next_option()<br>
 * 8. \p end_option()<br>
 * 9. \p first_sub_step()<br>
 * 10. \p next_sub_step()<br>
 * 11. \p parallel_step()<br>
 * 12. \p comment()<br>
 * 13. \p to_do()<br>
 * 14. \p repeat()<br> <br>
 * Back to \ref page1 
 *
 *\page class2 2. Declaration of new items
 * 1. \p new_fluid()<br>
 * 2. \p new_solid()<br>
 * 3. \p new_container()<br>
 * 4. \p new_plate()<br>
 * 5. \p new_slide()<br>
 * 6. \p new_symbol()<br>
 * 7. \p new_column()<br><br>
 * Back to \ref page1 
 *
 *\page class3 3. Measuring out materials
 * 1. \p measure_fluid()<br>
 * 2. \p measure_solid()<br>
 * 3. \p measure_prop()<br>
 * 4. \p add_to_column()<br>
 * 5. \p add_to_slide()<br>
 * 6. \p collect_tissue()<br>
 * 7. \p plate_out()<br><br>
 * Back to \ref page1 
 *
 *\page class4 4. Combination/mixing
 * 1. \p combine()<br>
 * 2. \p combine_and_mix()<br>
 * 3. \p dissolve()<br>
 * 4. \p invert()<br>
 * 5. \p pipet()<br>
 * 6. \p resuspend()<br>
 * 7. \p tap()<br>
 * 8. \p vortex()<br>
 * 9. \p vortex_column()<br>
 * 9. \p incubate_and_mix()<br>
 * 10. \p mixing_table()<br>
 * 11. \p mixing_table_pcr()<br><br>
 * Back to \ref page1 
 *
 *\page class5 5. Temperature changes
 * 1. \p incubate()<br>
 * 2. \p incubate_and_mix()<br>
 * 3. \p store_for()<br>
 * 4. \p store_until()<br>
 * 5. \p set_temp()<br>
 * 6. \p invert_dry()<br>
 * 7. \p store_plate()<br>
 * 8. \p thermocycler()<br>
 * 9. \p thermocycler_anneal()<br>
 * 10. \p pcr_init_denat()<br>
 * 11. \p pcr_final_ext()<br>
 * 12. \p use_or_store()<br>
 * 13. \p inoculation()<br>
 * 14. \p incubate_plate()<br><br>
 * Back to \ref page1 
 *
 *\page class6 6. Storage 
 * 1. \p wait()<br>
 * 2. \p store()<br>
 * 3. \p store_for()<br>
 * 4. \p store_until()<br>
 * 5. \p incubate()<br>
 * 6. \p incubate_and_mix()<br>
 * 7. \p inoculation()<br>
 * 8. \p incubate_plate()<br>
 * 9. \p invert_dry()<br>
 * 10. \p dry_pellet()<br>
 * 11. \p dry_slide()<br>
 * 12. \p use_or_store()<br>
 * 13. \p time_constraint()<br>
 * 14. \p store_plate()<br><br>
 * Back to \ref page1 
 *
 *\page class7 7. Centrifugation
 * 1. \p centrifuge()<br>
 * 2. \p centrifuge_pellet()<br>
 * 3. \p centrifuge_phases_top()<br>
 * 4. \p centrifuge_phases_bottom()<br>
 * 5. \p centrifuge_column()<br>
 * 6. \p centrifuge_flow_through()<br><br>
 * Back to \ref page1 
 *
 *\page class8 8. Disposal
 * 1. \p discard()<br>
 * 2. \p drain()<br><br>
 * Back to \ref page1 
 *
 *\page class9 9. Transfer of fluid and columns
 * 1. \p transfer()<br>
 * 2. \p transfer_column()<br><br>
 * Back to \ref page1 
 *
 *\page class10 10. Renaming materials
 * 1. \p name_container()<br>
 * 2. \p name_sample()<br>
 * 3. \p name_plate()<br>
 * 4. \p name_container()<br><br>
 * Back to \ref page1 
 *
 *\page class11 11. Detection/ separation/ techniques
 * 1. \p ce_detect()<br>
 * 2. \p electrophoresis()<br>
 * 3. \p facs()<br>
 * 4. \p measure_fluorescence()<br>
 * 5. \p mount_observe_slide()<br>
 * 6. \p sequencing()<br>
 * 7. \p electroporate()<br>
 * 8. \p weigh()<br>
 * 9. \p cell_culture()<br>
 * 10. \p transfection()<br>
 * Back to \ref page1 
 *
 *\page class12 12. Column-specific instructions
 * 1. \p add_to_column()<br>
 * 2. \p centrifuge_column()<br>
 * 3. \p centrifuge_flow_through()<br>
 * 4. \p vortex_column()<br>
 * 5. \p new_column()<br>
 * 6. \p name_column()<br><br>
 * Back to \ref page1 
 *
 *\page class13 13. Slide-specific instructions
 * 1. \p add_to_slide()<br>
 * 2. \p dry_slide()<br>
 * 3. \p remove_slide()<br>
 * 4. \p new_slide()<br>
 * 5. \p immerse_slide()<br>
 * 6. \p immerse_slide_and_incubate()<br>
 * 7. \p wash_slide()<br>
 * 8. \p mount_observe_slide()<br><br>
 * Back to \ref page1 
 * 
 *\page class14 14. Tissue-specific instructions
 * 1. \p collect_tissue()<br>
 * 2. \p homogenize_tissue()<br>
 * 3. \p wash_tissue()<br><br>
 * Back to \ref page1 
 *
 *\page class15 15. Plate-specific instructions
 * 1. \p new_plate()<br>
 * 2. \p name_plate()<br>
 * 3. \p patch()<br>
 * 4. \p plate_out()<br>
 * 5. \p incubate_plate()<br>
 * 6. \p store_plate()<br><br>
 * Back to \ref page1 
 *
 *\page class16 16. Cell culture-specific instructions
 * 1. \p cell_culture()<br>
 * 2. \p transfection()<br>
 * 3. \p wash_cells()<br>
 * 4. \p drain()<br><br>
 * Back to \ref page1 
 *
 *\page class17 17. Symbolic volume-specific instructions
 * 1. \p new_symbol()<br>
 * 2. \p set_value()<br>
 * 3. \p assign()<br>
 * 4. \p add()<br>
 * 5. \p divide()<br>
 * 6. \p subtract()<br>
 * 7. \p multiply()<br><br>
 * Back to \ref page1 
 */
using namespace std;

/*! Types of containers. For use in new_container(). */
typedef enum container_type{STERILE_MICROFUGE_TUBE/*!< Sterile 1.5-ml microfuge tube */, CENTRIFUGE_TUBE_15ML/*!< 15ml centrifuge tube */, FLASK/*!< Flask */, CENTRIFUGE_BOTTLE/*!< Centrifuge bottle */, GRADUATED_CYLINDER/*!< Graduated cylinder */, HUMIDIFIED_CHAMBER/*!< container that can be placed in a humidified chamber */, RXN_TUBE/*!< Reaction tube */, FRESH_COLL_TUBE/*!< Fresh collection tube */, LIQUID_NITROGEN/*!< container with liquid nitrogen */, PLG/*!< 50ml PLG tube */, OAKRIDGE/*!< Oakridge tube */, QIA_CARTRIDGE/*!< Qiacartridge */, CUVETTE_ICE/*!< Cuvette stored on ice */, SPEC_CUVETTE/*!< spectrometry cuvette */, STOCK_PLATE_96/*!< 96-well stock plate */, WELL_BLOCK_96/*!< 96-well block */, PCR_PLATE/*!< 96-well PCR plate */, LIQUID_BLOCK/*!< 96-well liquid block */, CELL_CULT_CHAMBER/*!< Cell culture chamber */, EPPENDORF/*!< Eppendorf tube */, STERILE_MICROFUGE_TUBE2ML/*!< Sterile 2 ml microcentrifuge tube */, STERILE_PCR_TUBE/*!< Sterile 0.6-ml tube */, CENTRI_TUBE_50ML/*!< 50-ml centrifuge tube */, CRYO_VIAL/*!< screw-topped cryo vial */, SCREW_CAP_TUBE/*!< Screw-cap tube of appropriate volume */};
/*!Specifies whether drying has to be performed in air or in vacuum. */
typedef enum drying{IN_AIR, IN_VACUUM};
/// @cond
/*! Used internally for conversion between volume units. */
typedef enum factor{MICRO=1/*!< Conversion value = 1 */, MILLI=1000/*!< Conversion value = 1000 */, LITRE=1000000/*!< Conversion value = 1000000 */};
/*! Specifies the type of the subtance. Used internally. */
typedef enum fluid_type {FLUID, SOLID, CONTAINER, OPERATION} ;
/// @endcond
/*!Specifies the kind of operation performed by a given instruction. */
typedef enum func{DEWAX/*!< dewaxing */, DENATURE/*!< denaturation */, ENZYME_INAC/*!< enzyme inactivation */};
/*! Types of mixing. For use in incubate_and_mix(). */
typedef enum mixing {TAPPING, STIRRING, INVERTING, VORTEXING, RESUSPENDING, DISSOLVING, PIPETTING};
/*! Specifies the type of PCR being performed. For use in thermocycler(). */ 
typedef enum pcr_type{NORMAL/*!< Regular PCR */, GRADIENT/*!< Gradient PCR */, COLONY/*!< Colony PCR */};
/*! Specifies the unit of the speed of the rotor in a centrifuge instruction. */
typedef enum speed_type{RPM, G};
/*! Specifies the type of time constraint being employed. For use in time_constraint(). */
typedef enum time_constraint_type{CURRENT_FLUID/*!< if \c time_constraint() is applicable to the subsequent use of the contents of a particular container. */, NEXTSTEP/*!< if \c time_constraint() is applicable between a pair of successive steps that use the contents of a container. */};
/*! Time units supported. */
typedef enum time_unit{SECS/*!< seconds */, MINS/*!< minutes */, HRS/*!< hours */};
/*! Specifies certain conditions that might have to be satisfied for the completion of a step. For use in store_until(), vortex(), etc. */
typedef enum until{ETHANOL_EVAP/*!< "until all the ethanol has evaporated and no fluid is visible in the tube".*/, OD/*!< "until the O.D.600 reaches 0.6". */, THAW/*!< "until the sample has thawed". */, COOLED/*!< "until cooled". */, COLOUR_DEVELOPS/*!< "until the colour develops". */, PPT_STOPS_STICKING/*!< "until the precipitate stops sticking to the walls of the tube". */, PELLET_DISLODGES/*!< "until the pellet dislodges". */, THAW_ICE/*!< "keep on ice until the sample has thawed" */};
/*! Volume units supported. */
typedef enum vol_unit{UL/*!< microliter */, ML/*!< milliliter */, L/*!< liter */};
/*! Types of washing. For use in wash_slide(). */
typedef enum wash_type{WASHING,RINSING};
/*! Weight units supported. For use in measure_solid(). */
typedef enum weight_unit{UG/*!< microgram */, MG/*!< milligram */, GR/*!< gram */};

/// @cond 
typedef struct {
	Node* node;/*!< The current graph node. */
	char* original_name;/*!< The name of the fluid. */
	char* new_name;/*!< The name of the fluid after renaming. Initially, original_name = new_name */
	enum container_type container;/*!< The container of the fluid. */
	char* state;/*!< The state of the fluid. */
	float volume;/*!< The volume of the fluid. */
	enum vol_unit unit;/*!< The unit of volume. */
	fluid_type type;/*!< The type of this resource. */
	int used;/*!< Used internally to track the usage of the fluid. */
	int usage_index;/*!< Used internally to track the usage of the fluid. */

} Fluid;/// @endcond 
/*! \struct Fluid
* \brief Defines a fluid.
*/

/// @cond 
typedef struct {
	Node* node;/*!< The current graph node. */
	enum container_type id;/*!< The container's id. */
	Fluid contents;/*!< The contents of the container. */
	float volume;/*!< The volume of the contents of the container. Note: This is different from the content's volume i. e. container.volume != container.contents.volume */
	char* name;/*!< The container's label. */
	fluid_type type;/*!< The type of this resource. */
	int used;/*!< Used internally to track the usage of the container. */
	int usage_index;/*!< Used internally to track the usage of the container. */

} Container;/// @endcond 
/*! \struct Container
*	\brief Defines a container.
*/


typedef Fluid Solid;
/*! \struct Solid
*	\brief Defines a solid.
*/
typedef Container Column;
/*! \struct Column
*	\brief Defines a column.
*/
typedef Solid Plate;
/*! \struct Plate
*	\brief Defines a plate.
*/
typedef Container Slide;
/*! \struct Slide
*	\brief Defines a slide.
*/
typedef Solid Tissue;
/*! \struct Tissue
*	\brief Defines a tissue.
*/

/// @cond 
class Time 
{		
public: 
	float value;
	int unit_choice;
	char* unit;
	int mul;
	Time(){}
	Time (float x, enum time_unit unit);
	virtual void display_time ();
	virtual ~Time(){}
};

class Minimum_time : public Time
{
public:
	Minimum_time(float x, enum time_unit unit);
	void display_time();
	~ Minimum_time(){}
};

class Maximum_time : public Time
{
public:
	Maximum_time(float x, enum time_unit unit);
	void display_time();
	~ Maximum_time(){}
}; 
class Approx_time : public Time
{
public:
	Approx_time(float x, enum time_unit unit);
	void display_time();
	~ Approx_time(){}
};

class Time_range : public Time
{
public:
	float value1;
	Time_range(float x, float y, enum time_unit unit);
	void display_time();
	~ Time_range(){}
}; 

typedef struct 
{
	char* symbol;
	float value;
	enum time_unit unit;
}Symbol_t;

class Symbolic_time : public Time
{
public:
	Symbol_t s1;
	Symbolic_time(){}
	Symbolic_time(Symbol_t& s);
	void display_time();
};


class Speed
{		
public: 
	float value;
	int unit_choice;
	char* unit;
	Speed(){}
	Speed (float x, enum speed_type unit);
	virtual void display_speed ();
	virtual ~Speed(){}
};
class Minimum_speed : public Speed
{
public:
	Minimum_speed(float x, enum speed_type unit);
	void display_speed();
	~ Minimum_speed(){}
};

class Maximum_speed : public Speed
{
public:
	Maximum_speed(float x, enum speed_type unit);
	void display_speed();
	~ Maximum_speed(){}
}; 
class Approx_speed : public Speed
{
public:
	Approx_speed(float x, enum speed_type unit);
	void display_speed();
	~ Approx_speed(){}
}; 

class Speed_range : public Speed
{
public:
	float value1;
	Speed_range(float x, float y, enum speed_type unit);
	void display_speed();
	~ Speed_range(){}
}; 

Time *time (float x, enum time_unit unit);
Time_range *time_range (float x, float y, enum time_unit unit);
Minimum_time *min_time(float x, enum time_unit unit);
Maximum_time *max_time(float x, enum time_unit unit);
Symbolic_time* s_time(Symbol_t s);
Approx_time * approx_time (float x, enum time_unit unit);
Speed *speed (float x, enum speed_type unit);
Speed_range *speed_range (float x, float y, enum speed_type unit);
Minimum_speed *min_speed(float x, enum speed_type unit);
Maximum_speed *max_speed(float x, enum speed_type unit);
Approx_speed* approx_speed(float x, enum speed_type unit);
typedef struct 
{
	char* symbol;
	float value;
	enum vol_unit unit;
}Symbol;

class Volume
{
public:
	float value;
	char* unit;
	enum factor mul;
	enum vol_unit unit_choice;
	Volume(){}
	Volume(float x, enum vol_unit unit);
	virtual void display_vol(); 
	~Volume(){}	
};

class Symbolic_vol : public Volume
{
public:
	Symbol s1;
	Symbolic_vol(){}
	Symbolic_vol(Symbol& s);
	void display_vol();
};
class Volume_range : public Volume
{
public:
	float value1;
	Volume_range(){}
	Volume_range(float x, float y, enum vol_unit unit);
	void display_vol();
};
class Approx_volume : public Volume
{
public:
	Approx_volume(){}
	Approx_volume(float x, enum vol_unit unit);
	void display_vol();
};
Volume* vol(float x, enum vol_unit unit);
Symbolic_vol* s_vol(Symbol s);
Volume_range* vol_range(float x, float y, enum vol_unit unit);
Approx_volume* approx_vol(float x, enum vol_unit unit);

/// @endcond

//! Adds a symbolic volume with another symbolic volume.
/*!
\param vol1 the symbolic volume.
\param vol2 the other symbolic volume.
\par Example:
\code Symbol z = add(s_vol(x), s_vol(y)); \endcode
\par Output:
Internally, \c z is assigned the value \c x+y.
\sa subtract(), multiply() and divide()
*/
extern Symbol add(Symbolic_vol* vol1, Symbolic_vol* vol2);

//! Adds a symbolic volume with an actual volume.
/*!
\param vol1 the symbolic volume.
\param vol2 the actual volume.
\par Example:
\code Symbol y = add(s_vol(x), vol(10, UL)); \endcode
\par Output:
Internally, \c y is assigned the value \c x+10. 
\sa subtract(), multiply() and divide()
*/
extern Symbol add(Symbolic_vol* vol1, Volume* vol2);

//! Adds the contents of \c container1 to the specified column.
/*!
\param column1 the column to which contents of \c container1 are added.
\param container1 the container with the fluid that needs to be added to the column.
\par Example:
\code add_to_column(spin_column, eppendorf1); \endcode
\par Output:
\htmlonly Add <font color=#357EC7>(eppendorf1.contents.name)</font> to (spin_column.name). \endhtmlonly
\sa measure_fluid() and measure_prop()
*/
extern void add_to_column(Column& column1, Container& container1);

//! Adds the given volume of contents of \c container1 to the specified column.
/*!
\param column1 the column to which contents of \c container1 are added.
\param container1 the container with the fluid that needs to be added to the column.
\param volume1 the volume of \c container1 's contents that needs to be added to \c column1.
\par Example:
\code add_to_column(spin_column, eppendorf1, vol(1, ML)); \endcode
\par Output:
\htmlonly Add <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(eppendorf1.contents.name)</font> to (spin_column.name). \endhtmlonly
\sa measure_fluid() and measure_prop() 
*/
extern void add_to_column(Column& column1, Container& container1, Volume* volume1);

//! Adds the given volume of \c fluid1 to the specified column.
/*!
\param column1 the column to which contents of \c container1 are added.
\param fluid1 the fluid that needs to be added to the column.
\param volume1 the volume of \c fluid1 that needs to be added to \c column1.
\par Example:
\code add_to_column(spin_column, sola, vol(1, ML)); \endcode
\par Output:
\htmlonly Add <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(sola.name)</font> to (spin_column.name). \endhtmlonly
\sa measure_fluid() and measure_prop() 
*/
extern void add_to_column(Column& column1, Fluid& fluid1, Volume* volume1);

//! Adds the given volume of \c sample1 to the specified slide.
/*!
\param slide1 the slide to which contents of \c slide1 are added.
\param sample1 the fluid that needs to be added to the slide.
\param volume1 the volume of \c sample1 that needs to be added to \c slide1.
\par Example:
\code add_to_slide(slides, ethanol, vol(1, ML)); \endcode
\par Output:
\htmlonly Add <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(ethanol.name)</font> to each of the (slides.name). \endhtmlonly
\sa add_to_column(), measure_fluid() and measure_prop()
*/
extern void add_to_slide(Slide& slide1, Fluid& sample1, Volume* volume1);

//! Sets symbolic volume \c s1 to be equal to symbolic volume \c s2..
/*!
\param s1 the symbolic volume that is assigned a value.
\param s2 the symbolic volume that \c s1 is equal to.
\par Example:
\code assign(s_vol(x), s_vol(y)); \endcode
\par Output:
\htmlonly Let (x.name) = (y.name). \endhtmlonly
\sa new_symbol() and set_value()
*/
extern void assign(Symbol& s1, Symbol& s2);

//! Sets symbolic volume \c s1 to be equal to symbolic volume \c s2..
/*!
\param s1 the symbolic time that is assigned a value.
\param s2 the symbolic time that \c s1 is equal to.
\par Example:
\code assign(s_time(x), s_time(y)); \endcode
\par Output:
\htmlonly Let (x.name) = (y.name). \endhtmlonly
\sa new_symbol() and set_value()
*/
extern void assign(Symbol_t& s1, Symbol_t& s2);

//! Sends the contents of the given container to a capillary electrophoresis(CE) unit for separation/detection at the given settings.
/*!
\param container1 the container whose contents need to be detected/separated with CE.
\param length length of the CE column.
\param volt_per_cm voltage setting.
\param fluid1 the separation buffer.
\par Example:
\code ce_detect(eppendorf1, 23, 200, sep_buffer); \endcode
\par Output:
\htmlonly Detect/separate (eppendorf1.contents.name) by capillary electrophoresis with the following settings - <b><font color=#357EC7>23</font></b> cm at <b><font color=#357EC7>200</font></b> V/cm using (sep_buffer.name).\endhtmlonly.
\sa nanodrop()
*/
extern void ce_detect (Container& container1, float length, float volt_per_cm, Fluid& fluid1);

//! Sends the contents of the given container to a capillary electrophoresis(CE) unit for separation/detection at the given settings.
/*!
\param container1 the container whose contents need to be detected/separated with CE.
\param length length of the CE column.
\param volt_per_cm voltage setting.
\param fluid1 the separation buffer.
\param time1 the duration of capillary electrophoresis at the given settings.
\par Example:
\code ce_detect(eppendorf1, 23, 200, sep_buffer, time(3, MINS)); \endcode
\par Output:
\htmlonly Detect/separate (eppendorf1.contents.name) by capillary electrophoresis with the following settings - <b><font color=#357EC7>23</font></b> cm at <b><font color=#357EC7>200</font></b> V/cm using (sep_buffer.name) for <b><font color=#357EC7>3 mins</font></b>.\endhtmlonly.
\sa nanodrop()
*/
extern void ce_detect (Container& container1, float length, float volt_per_cm, Fluid& fluid1, Time* time1);

//! Performs cell culture of the cells present in the given container with the specified parameters.
/*!
\param cells the container with the cells that need to be cultured.
\param medium the cell culture medium.
\param centri_speed the speed of centrifugation to retrieve the cells. (in rpm)
\param temp the temperature of cell culture.
\param time the duration of cell culture.
\param percent_CO2 the percent of carbon dioxide in the cell culture chamber.
\param for_wash_valves the fluid used to wash the valves in the cell culture chamber.
\param for_wash_chambers the fluid used to wash the chamber.
\param for_trypsinization the fluid used for trypsinization.
\param for_feeding the time duration between two consecutive feeds.
\par Example:
\code cell_culture(cult_chamber, medium, 200, RT, 48, 30, fluid1, fluid2, fluid3, 20); \endcode
\par Output:
\htmlonly Perform cell culture with the specified parameters. \endhtmlonly
*/
extern void cell_culture(Container& cells, Fluid& medium, int centri_speed, float temp, float time, float percent_CO2, Fluid& for_wash_valves, Fluid& for_wash_chambers, Fluid& for_trypsinization, float for_feeding);

//! Performs centrifugation of given container at the specified temperature, speed and time.
/*!
\param container1 the container that is to be centrifuged.
\param speed1 speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 duration of centrifugation.
\par Example:
\code centrifuge(microfuge_tube1, speed(SPEED_MAX, RPM), 4, time(5, MINS)); \endcode
\par Output:
\htmlonly Centrifuge at <font color=#357EC7>maximum speed</font> for <b><font color=#357EC7>5 mins</font></b> at <b><font color=#357EC7>4 C</font></b>. \endhtmlonly
\sa centrifuge_pellet(), centrifuge_phases_top() and centrifuge_phases_bottom()
*/
extern void centrifuge(Container &container1, Speed* speed1, float temp, Time* time1);

//! Performs centrifugation of given column at the specified temperature and for the specified duration of time. The flow-through is discarded.
/*!
\param column1 the column that is to be centrifuged.
\param temp the temperature of centrifugation.
\param time1 the duration of centrifugation.
\par Example:
\code centrifuge_column(column1, 4, time(5, MINS)); \endcode
\par Output:
\htmlonly Microfuge column for <b><font color=#357EC7>5 mins</font></b> at <b><font color=#357EC7>4 C</font></b>. Discard the flow-through. \endhtmlonly
\sa centrifuge_flow_through() 
*/
extern void centrifuge_column(Column& column1, float temp, Time* time1);

//! Performs centrifugation of given column at the specified temperature, speed and for the specified duration of time. The flow-through is discarded.
/*!
\param column1 the column that is to be centrifuged.
\param speed1 the speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 the duration of centrifugation.
\par Example:
\code centrifuge_column(column1, speed(10000, RPM), 4, time(5, MINS)); \endcode
\par Output:
\htmlonly Centrifuge at a speed of <font color=#357EC7>10000 rpm</font> for <b><font color=#357EC7>5 mins</font></b> at <b><font color=#357EC7>4 C</font></b>. Discard the flow-through. \endhtmlonly
\sa centrifuge_flow_through()
*/
extern void centrifuge_column(Column& column1, Speed* speed1, float temp, Time* time1);

//! Performs centrifugation of given column at the specified temperature and for the specified duration of time. The column is discarded and the flow-through is left in the collection tube, \c container1.
/*!
\param column1 the column that is to be centrifuged.
\param temp the temperature of centrifugation.
\param time1 the duration of centrifugation.
\param container1 the container that collects the flow-through
\par Example:
\code centrifuge_flow_through(column1, 4, time(5, MINS)); \endcode
\par Output:
\htmlonly Microfuge column for <b><font color=#357EC7>5 mins</font></b> at <b><font color=#357EC7>4 C</font></b>. Discard the column.\endhtmlonly
\sa centrifuge_column()
*/
extern void centrifuge_flow_through(Column& column1, float temp, Time* time1, Container& container1);

//! Performs centrifugation of given column at the specified temperature, speed and for the specified duration of time. The column is discarded and the flow-through is left in the collection tube, \c container1.
/*!
\param column1 the column that is to be centrifuged.
\param speed1 the speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 the duration of centrifugation.
\param container1 the container that collects the flow-through.
\par Example:
\code centrifuge_flow_through(column1, speed(10000, RPM), 4, time(5, MINS)); \endcode
\par Output:
\htmlonly Centrifuge at a speed of <font color=#357EC7>10000 rpm</font> for <b><font color=#357EC7>5 mins</font></b> at <b><font color=#357EC7>4 C</font></b>. Discard the column.\endhtmlonly
\sa centrifuge_column()
*/
extern void centrifuge_flow_through(Column& column1, Speed* speed1, float temp, Time* time1, Container& container1);

//! Performs centrifugation of given container at the specified temperature, speed and time and yields a pellet. The supernatant is discarded.
/*!
\param container1 the container that is to be centrifuged.
\param speed1 speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 duration of centrifugation.
\par Example:
\code centrifuge_pellet(eppendorf1, speed(SPEED_MAX, RPM), RT, time(2, MINS)); \endcode
\par Output:
\htmlonly Centrifuge <font color=#357EC7>maximum speed</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>room temperature</font></b>, gently aspirate the supernatant and discard it.\endhtmlonly
\sa centrifuge_phases_top()
*/
extern void centrifuge_pellet(Container& container1, Speed* speed1, float temp, Time* time1);

//! Performs centrifugation of given container at the specified temperature, speed and time and yields a pellet. The expected volume of supernatant is specified and the supernatant is discarded.
/*!
\param container1 the container that is to be centrifuged.
\param speed1 speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 duration of centrifugation.
\param expected_vol the expected volume of supernantant.
\par Example:
\code centrifuge_pellet(eppendorf1, speed(SPEED_MAX, RPM), RT, time(2, MINS), vol(100, UL)); \endcode
\par Output:
\htmlonly Centrifuge at <font color=#357EC7>maximum speed</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>room temperature</font></b>, gently aspirate out the supernatant and discard it. The expected volume 
of supernatant is <b><font color=#357EC7>100 ul</font></b>.\endhtmlonly
\sa centrifuge_phases_top() and enum vol_unit
*/
extern void centrifuge_pellet(Container& container1, Speed* speed1, float temp, Time* time1, Volume* expected_vol);

//! Performs centrifugation of given container for the specified time, speed and temp. The top(aqueous) phase is transferred to a new container and the bottom layer is discarded.
/*!
\param container1 the container that is to be centrifuged.
\param speed1 the speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 the duration of centrifugation.
\param container2 the container to which the supernatant is tranferred.
\par Example:
\code centrifuge_phases_top(eppendorf1, speed(SPEED_MAX, RPM), RT, time(2, MINS), eppendorf1); \endcode
\par Output:
\htmlonly Centrifuge at <font color=#357EC7>maximum speed</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>room temperature</font></b> and aspirate out the top layer. Transfer the top layer to (eppendorf1.name). \endhtmlonly
\sa centrifuge_pellet()
*/
extern void centrifuge_phases_top(Container& container1, Speed* speed1, float temp, Time* time1, Container& container2);

//! Performs centrifugation of given container for the specified time, speed and temp. The specified proportion of top(aqueous) phase is transferred to a new container and the bottom layer is discarded. 
/*!
\param container1 the container that is to be centrifuged.
\param speed1 the speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 the duration of centrifugation.
\param aspirate_vol the volume of \c container1 's contents that has to be aspirated out into a new container.
\param container2 the container to which the supernatant is tranferred.
\par Example:
\code centrifuge_phases_top(eppendorf1, speed(SPEED_MAX, RPM), RT, time(2, MINS), vol(200, UL), eppendorf1); \endcode
\par Output:
\htmlonly Centrifuge at <font color=#357EC7>maximum speed</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>room temperature</font></b> and aspirate out <b><font color=#357EC7>200 ul</font></b> of the top layer. Transfer the top layer to (eppendorf1.name).  \endhtmlonly
\sa centrifuge_pellet() 
*/
extern void centrifuge_phases_top(Container& container1, Speed* speed1, float temp, Time* time1, Volume* aspirate_vol, Container& container2);

//! Performs centrifugation of given container for the specified time, speed and temp. The specified volume of top(aqueous) phase is discarded and the remaining portion is left behind in the container.
/*!
\param container1 the container that is to be centrifuged.
\param speed1 the speed of centrifugation.
\param temp the temperature of centrifugation.
\param time1 duration of centrifugation.
\param aspirate_vol the volume of \c container1 's contents that has to be aspirated out and discarded.
\param leave_behind the volume of \c container1 's contents that has to be left behind.
\par Example:
\code centrifuge_phases_bottom(eppendorf1, speed(SPEED_MAX, RPM), RT, time(2, MINS), vol(800, UL), vol(200, UL)); \endcode
\par Output:
\htmlonly Centrifuge at <font color=#357EC7>maximum speed</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>room temperature</font></b> and gently aspirate out <b><font color=#357EC7>800 ul</font></b> of the supernantant and discard it.
Leave behind <b><font color=#357EC7>200 ul</font></b>. \endhtmlonly
\sa centrifuge_pellet()and centrifuge_phases_top()
*/
extern void centrifuge_phases_bottom(Container& container1, Speed* speed1, float temp, Time* time1, Volume* aspirate_vol, Volume* leave_behind);

//! Collects the specified tissue in the given container.
/*!
\param sample1 the tissue that has to be collected.
\param container1 the container in which \c sample1 has to be collected.
\par Example:
\code collect_tissue(plant_tissue, cryo_tube); \endcode
\par Output:
\htmlonly Collect <font color=#357EC7>(plant_tissue.name)</font> into (cryo_tube.name) and snap-freeze in liquid nitrogen. \endhtmlonly
\sa homogenize_tissue()
*/
extern void collect_tissue(Tissue& sample1, Container& container1);

//! Collects the specified amount of tissue in the given container.
/*!
\param sample1 the tissue that has to be collected.
\param wt the weight of \c sample1 that has to be collected.
\param unit the unit of \c wt. (See enum weight_unit)
\param container1 the container in which \c sample1 has to be collected.
\par Example:
\code collect_tissue(plant_tissue, 2, GR, sterile_microfuge_tube1); \endcode
\par Output:
\htmlonly Collect <b><font color=#357EC7>2 g</font></b> of <font color=#357EC7>(plant_tissue.name)</font> in (sterile_microfuge_tube1.name). \endhtmlonly
\sa homogenize_tissue()
*/
extern void collect_tissue(Tissue& sample1, float wt, enum weight_unit unit, Container& container1);

//! Combines the contents of the given containers.
/*!
\param count the number of containers whose contents have to be mixed.
\param container1 the first container of \c count number of containers.
\par Example:
\code combine(3, tube1, tube2, tube3); \endcode
\par Output:
\htmlonly Combine the (tube1.contents.name), (tube2.contents.name), (tube3.contents.name). \endhtmlonly
\sa combine_and_mix()
*/
extern void combine (int count, Container& container1, ...);

//! Combines the given list of containers' contents and mixes them with the specified setting for a few seconds.
/*!
\param type the type of mixing. (See enum mixing)
\param count the number of containers whose contents have to be mixed.
\param container1 the first of the list of containers whose contents have to be mixed.
\par Example:
\code combine_and_mix(VORTEXING, 2, tube1, tube2); \endcode
\par Output:
\htmlonly Combine the (tube1.contents.name), (tube2.contents.name).
Vortex the mixture for a few secs. \endhtmlonly
\sa combine(), tap(), stir(), invert(), vortex(), resuspend(), dissolve(), pipet() and enum mixing
*/
extern void combine_and_mix (enum mixing type, int count, Container& container1, ...);

//! Combines the given list of containers' contents and mixes them for the specified duration of time and setting.
/*!
\param type the type of mixing. (See enum mixing)
\param time1 the duration of mixing.
\param count the number of containers whose contents have to be mixed.
\param container1 the first of the list of containers whose contents have to be mixed.
\par Example:
\code combine_and_mix(VORTEXING, time_range(2, 3, MINS), 2, tube1, tube2); \endcode
\par Output:
\htmlonly Combine the (tube1.contents.name), (tube2.contents.name).
Vortex the mixture for <b><font color=#357EC7>2 - 3 mins</font></b>. \endhtmlonly
\sa combine(), tap(), stir(), invert(), vortex(), resuspend(), dissolve(), pipet() and enum mixing 
*/
extern void combine_and_mix (enum mixing type, Time* time1, int count, Container& container1, ...);

//! Used to specify a hint/ caution type message that supplements a step in a protocol.
/*!
\par Example:
\code comment("Take care with this step as the pellet may not stick to the tube completely.");\endcode
\par Output:
\htmlonly <font color=#800517><i>Take care with this step as the pellet may not stick to the tube completely.</i></font> \endhtmlonly 
\sa to_do()
*/
extern void comment(char* comm);

//! Discards the contents of the specified container.
/*!
\par Example:
\code discard(epepndorf1);\endcode
\par Output:
\htmlonly Discard (eppendorf1.contents.name). \endhtmlonly
\param container1 the container whose contents have to be discarded.
*/
extern void discard(Container& container1);

//! Dissolves the contents of the solution in tube.
/*!
\param container1 the container whose contents have to be dissolved.
\par Example:
\code dissolve(eppendorf1);\endcode
\par Output:
\htmlonly Dissolve the pellet in the solution. \endhtmlonly
\sa tap(), stir(), invert(), vortex(), resuspend(), pipet() and combine_and_mix()
*/
extern void dissolve (Container& container1);

//! Divides a symbolic volume by an actual volume.
/*!
\param vol1 the symbolic volume (dividend).
\param vol2 the actual volume (divisor).
\par Example:
\code y = divide(s_vol(x), vol(10, UL)); \endcode
\par Output:
Internally, \c y is assigned the value \c x/10.
\sa add(), multiply() and subtract()
*/
extern Symbol divide(Symbolic_vol* vol1, Volume* vol2);

//! Divides an actual volume by a symbolic volume.
/*!
\param vol1 the actual volume (dividend).
\param vol2 the symbolic volume (divisor).
\par Example:
\code y = divide(vol(10, UL), s_vol(x)); \endcode
\par Output:
 Internally, \c y is assigned the value \c 10/x.
\sa add(), multiply() and subtract()
*/
extern Symbol divide(Volume* vol1, Symbolic_vol*  vol2);

//! Divides a symboic volume by another symbolic volume.
/*!
\param vol1 the dividend.
\param vol2 the divisor.
\par Example:
\code z = divide(s_vol(x), s_vol(y)); \endcode
\par Output:
 Internally, \c z is assigned the value \c x/y.
\sa add(), multiply() and subtract()
*/
extern Symbol divide(Symbolic_vol* vol1, Symbolic_vol*  vol2);

//! Drains the specified container.
/*!
\param container1 the container that has to be drained.
\par Example:
\code drain(cult_chamber); \endcode
\par Output:
\htmlonly Drain (cult_chamber.name). \endhtmlonly
\sa discard()
*/
extern void drain(Container& container1);

//! Dries the contents(pellet) of the specified container with specified conditions.
/*!
\param container1 the container whose contents(pellet) need to be dried.
\param type the condition for drying.
\par Example:
\code dry_pellet(microfuge_tube1, "in vacuum");\endcode
\par Output:
\htmlonly Dry the pellet in vacuum. \endhtmlonly
\sa invert_dry()
*/
extern void dry_pellet(Container& container1, drying type);

//! Dries the contents(pellet) of the specified container with specified conditions and time.
/*!
\param container1 the container whose contents(pellet) need to be dried.
\param type the condition for drying.
\param time1 the duration of drying.
\par Example:
\code dry_pellet(microfuge_tube1, "in air", max_time(15, MINS)); \endcode
\par Output:
\htmlonly Dry the pellet in air for at most <b><font color=#357EC7>15 mins</font></b>. \endhtmlonly
\sa invert_dry()
*/
extern void dry_pellet(Container& container1, drying type, Time* time1);

//! Dries the specified slide in air.
/*!
\param slide1 the slide that needs to be dried.
\par Example:
\code dry_slide(slide1); \endcode
\par Output:
\htmlonly Dry (slide1.name). \endhtmlonly
*/
extern void dry_slide(Slide& slide1);

//! Performs agarose gel elecrophoresis of the contents of \c container1.
/*!
\param container1 the container whose contents have to be subjected to agarose gel electrophoresis.
\par Example:
\code electrophoresis(eppendorf1);\endcode
\par Output:
\htmlonly Perform agarose gel electrophoresis of appropriate quantity of (eppendorf1.contents.name)mixed with ethidium bromide
and visualize with UV transilluminator to confirm the presence of required product. \endhtmlonly
\sa nanodrop()
*/
extern void electrophoresis(Container& container1);

//! Performs agarose gel elecrophoresis of the contents of \c container1.
/*!
\param container1 the container whose contents have to be subjected to agarose gel electrophoresis.
\param agar_conc the concentration of agarose in the gel.
\par Example:
\code electrophoresis(eppendorf1, 0.8);\endcode
\par Output:
\htmlonly Perform 0.8% agarose gel electrophoresis of appropriate quantity of (eppendorf1.contents.name)mixed with ethidium bromide
and visualize with UV transilluminator to confirm the presence of required product. \endhtmlonly
\sa nanodrop()
*/
extern void electrophoresis(Container& container1, float agar_conc);

//! Performs agarose gel elecrophoresis of the contents of \c container1.
/*!
\param no_samples the number of samples loaded on to the gel.
\param container_array the containers whose contents have to be subjected to agarose gel electrophoresis.
\param agar_conc the concentration of agarose in the gel.
\param pdt_vol the volume of sample used for electrophoresis.
\param loading_buff_vol the volume of loading buffer used.
\par Example:
\code electrophoresis(2, array1, 0.8, vol(5, UL), vol(1, UL));\endcode
\par Output:
\htmlonly Perform 0.8% agarose gel electrophoresis of (array1[0].contents.name) and (array1[1].contents.name)(5µl sample and 1µl of loading buffer), mixed with ethidium bromide
and visualize with UV transilluminator to confirm the presence of required product. \endhtmlonly
\sa nanodrop()
*/
void electrophoresis(int no_samples, Container container_array[], float agar_conc, Volume* pdt_vol, Volume* loading_buff_vol);

//! Electroporates the contents of the specified container with the given settings.
/*!
\param container1 the container whose contents have to be subjected to electroporation.
\param voltage the electroporation voltage.
\param no_pulses the number of voltage pulses.
\par Example:
\code electroporate(tube1, 220, 5); \endcode
\par Output:
\htmlonly Set the electroporator to deliver <b><font color=#357EC7>220 V</font></b>, and then press the PULSE button <b><font color=#357EC7>5 times</font></b>.\endhtmlonly
*/
extern void electroporate (Container& container1, float voltage, int no_pulses);

//! Marks the last option of a step of a protocol.
/*!
\par Example:
\code end_option(); \endcode
\par Output:
Internally, ends the list of options.
\sa first_option(), next_option()
*/
extern void end_option();

//! Marks the end of the protocol (calls display_equipment, closes the file and resets all the variables).
/*!
\par Example:
\code end_protocol; \endcode
\par Output:
Internally, closes the html file.
\sa start_protocol()
*/
extern void end_protocol();

//! Peforms fluorescence activated cell sorting (FACS) of the contents of \c container1.
/*!
\param container1 the container whose contents have to be sorted using FACS.
\par Example:
\code facs(eppendorf1); \endcode
\par Output:
\htmlonly FACS: Sort (eppendor1.contents.name) based on fluorescence. \endhtmlonly
*/
extern void facs(Container& container1);

//! Marks the first option of a step of a protocol.
/*!
\par Example:
\code first_option(); \endcode
\par Output:
\htmlonly <b>Option 1:</b> \endhtmlonly
\sa next_option(), end_option()
*/
extern void first_option();

//! Marks the first option of a step of a protocol and gives the step a name.
/*!
\param name the name of the step.
\par Example:
\code first_option(); \endcode
\par Output:
\htmlonly <b>Option 1:</b> \endhtmlonly
\sa next_option(), end_option()
*/
extern void first_option(char* name);

//! Marks the first step of a protocol.
/*!
\par Example:
\code first_step(); \endcode
\par Output:
\htmlonly 1. \endhtmlonly
\sa next_step(), optional_step() and first_sub_step()
*/
extern void first_step();

//! Marks the first step of a protocol. Also specifies the name of the step.
/*!
\param step_name the name of the step.
\par Example:
\code first_step(DNA extraction); \endcode
\par Output:
\htmlonly <h2>Steps:</h2>
1. <b>DNA extraction</b> \endhtmlonly
\sa next_step(), optional_step() and first_sub_step()
*/
extern void first_step(char* step_name);

//! Marks the first sub-step of a step in the protocol.
/*!
\par Example:
\code first_sub_step(); \endcode
\par Output:
\htmlonly a. \endhtmlonly
\sa next_step(), optional_step() and next_sub_step()
*/
extern void first_sub_step();

//! Homogenizes the contents (tissue) of the specified container using a mortor and a pestle with liquid nitrogen.
/*!
\param container1 the container with the tissue to be homogenized.
\par Example:
\code homogenize_tissue(microfuge_tube);\endcode
\par Output:
\htmlonly Pre-cool the mortars by pouring liquid nitrogen into them.
Add (microfge_tube.contents.name) into the mortar containing liquid nitrogen and grind with a pestle until it is a fine powder.
Use gloves! \endhtmlonly
\sa collect_tissue()
*/
extern void homogenize_tissue(Container& container1);

//! Homogenizes the contents (tissue) of the specified container in the given fluid.
/*!
\param container1 the container with the tissue to be homogenized.
\param fluid1 the fluid used to homogenize the tissue in \c container1.
\par Example:
\code homogenize_tissue(microfuge_tube, solution1); \endcode
\par Output:
\htmlonly Homogenize (microfuge_tube.contents.name) in  <font color=#357EC7>(solution1.name)</font>. \endhtmlonly
\sa collect_tissue()
*/
extern void homogenize_tissue(Container& container1, Fluid& fluid1);

//! Immerses the given slide in the specified fluid for the specified number of times.
/*!
\param slide1 the slide to be immersed.
\param sample1 the fluid in which the slide has to be immersed.
\param no_of_times the number of times that the slide has to be immersed in the given fluid.
\par Example:
\code immerse_slide(slide1, ethanol, 2); \endcode
\par Output:
\htmlonly Immerse <font color=#357EC7>(slide1.name)</font> in <font color=#357EC7>(ethanol.name)</font> twice. \endhtmlonly
\sa immerse_slide_and_incubate()
*/
extern void immerse_slide(Slide& slide1, Fluid& sample1, int no_of_times);

//! Immerses the given slide in the specified fluid for the specified duration and number of times.
/*!
\param slide1 the slide to be immersed.
\param sample1 the fluid in which the slide has to be immersed.
\param time1 the duration of immersion.
\param no_of_times the number of times that the slide has to be immersed in the given fluid.
\par Example:
\code immerse_slide(slide1, ethanol, time(2, MINS), 1); \endcode
\par Output:
\htmlonly Immerse <font color=#357EC7>(slide1.name)</font> in <font color=#357EC7>(ethanol.name)</font> for <b><font color=#357EC7>2 mins</font></b>. \endhtmlonly 
\sa immerse_slide_and_incubate() 
*/
extern void immerse_slide(Slide& slide1, Fluid& sample1, Time* time1, int no_of_times);

//! Immerses the given slide in the specified fluid for the specified duration and number of times, thus performing a given function.
/*!
\param slide1 the slide to be immersed.
\param sample1 the fluid in which the slide has to be immersed.
\param time1 the duration of immersion.
\param function the function being performed as a result of the immersion operation. (See enum func)
\param no_of_times the number of times that the slide has to be immersed in the given fluid.
\par Example:
\code immerse_slide(slide1, ethanol, time(2, MINS), DEWAX, 2); \endcode
\par Output:
\htmlonly Dewax <font color=#357EC7>(slide1.name)</font> in <font color=#357EC7>(ethanol.name)</font> for <b><font color=#357EC7>2 mins</font></b> twice. \endhtmlonly 
\sa immerse_slide_and_incubate() and enum func
*/
extern void immerse_slide(Slide& slide1, Fluid& sample1, Time* time1, enum func function, int no_of_times);

//! Incubates the given slide in the specified fluid for the specified duration.
/*!
\param slide1 the slide to be incubated.
\param sample1 the fluid in which the slide has to be immersed and incubated.
\param temp the temperature of incubation.
\param time1 the duration of incubation.
\par Example:
\code immerse_slide_and_incubate(slide1, ethanol, RT, time(2, MINS)); \endcode
\par Output:
\htmlonly Incubate <font color=#357EC7>(slide1.name)</font> in <font color=#357EC7>(ethanol.name)</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>room temperature</font></b>. \endhtmlonly 
\sa immerse_slide()
*/
extern void immerse_slide_and_incubate(Slide& slide1, Fluid& sample1, float temp, Time* time1);

//! Incubates the given slide in the specified fluid for the specified duration in the given environment.
/*!
\param slide1 the slide to be incubated.
\param sample1 the fluid in which the slide has to be immersed and incubated.
\param temp the temperature of incubation.
\param time1 the duration of incubation.
\param type the environment in which the slide is incubated. (See enum container_type)
\par Example:
\code immerse_slide_and_incubate(slide1, ethanol, 37, time(2, MINS), humidified_chamber); \endcode
\par Output:
\htmlonly Incubate <font color=#357EC7>(slide1.name)</font> in <font color=#357EC7>(ethanol.name)</font> for <b><font color=#357EC7>2 mins</font></b> at <b><font color=#357EC7>37 C</font></b> in (humidified_chamber.name). \endhtmlonly 
\sa immerse_slide() and enum container_type 
*/
extern void immerse_slide_and_incubate(Slide& slide1, Fluid& sample1, float temp, Time* time1, enum container_type type);

//! Incubates the given container at the specified temperature for the specified duration of time, without shaking.
/*!
\param container1 the container whose contents need to be incubated.
\param temp the temperature of incubation.
\param time1 the duration of incubation.
\par Example:
\code incubate(eppendorf, ON_ICE, time_range(3, 5, MINS)); \endcode
\par Output:
\htmlonly Incubate <b><font color=#357EC7>on ice</font></b> for <b><font color=#357EC7>3 - 5 mins</font></b>. \endhtmlonly
\sa store(), store_for(), store_until() and inoculation() 
*/
extern void incubate(Container& container1, float temp, Time* time1);

//! Incubates the given container at the specified temperature for the specified duration of time, with shaking at the given rpm.
/*!
\param container1 the container whose contents need to be incubated.
\param temp the temperature of incubation.
\param time1 the duration of incubation.
\param rpm the speed of shaking while incubating (in rpm).
\par Example:
\code incubate(flask, RT, time(3, HRS), 200); \endcode
\par Output:
\htmlonly Incubate at <b><font color=#357EC7>room temperature</font></b> for <b><font color=#357EC7>3 hrs</font></b> with shaking at <font color=#357EC7>200</font> rpm. \endhtmlonly
\sa store(), store_for(), store_until() and inoculation() 
*/
extern void incubate(Container& container1, float temp, Time* time1, int rpm);

//! Incubates the given container at the specified temperature for the specified duration of time, with shaking at the given rpm and stirring during the incubation as specified.
/*!
\param container1 the container whose contents need to be incubated.
\param temp the temperature of incubation.
\param time1 the duration of incubation.
\param time_mix the time intervals at which the contents of the container have to be mixed during incubation.
\param type the type of mixing.
\par Example:
\code incubate_and_mix(flask, RT, time(3, HRS), 200, time(10, MINS)); \endcode
\par Output:
\htmlonly Incubate at <b><font color=#357EC7>room temperature</font></b> for <b><font color=#357EC7>3 hrs</font></b> with shaking at <font color=#357EC7>200 rpm</font>, mixing gently by stirring the container every <b><font color=#357EC7>10 mins</font></b>. \endhtmlonly
\sa store(), store_for(), store_until(), incubate(), inoculation() and enum ranges
*/
extern void incubate_and_mix(Container& container1, float temp, Time* time1, Time* time_mix, enum mixing type);

//! Incubates the given plate at the specified temperature for the specified duration of time.
/*!
\param plate1 the plate that needs to be incubated.
\param temp the temperature of incubation.
\param time1 duration of incubation.
\par Example:
\code incubate_plate(lb_plate, 37, time(OVERNIGHT, HRS));\endcode
\par Output:
\htmlonly Incubate at <b><font color=#357EC7>37 C</font></b> for <b><font color=#357EC7>12 hrs(overnight)</font></b>. \endhtmlonly
\sa incubate(), inoculation() and enum ranges
*/
extern void incubate_plate(Plate& plate1, float temp, Time* time1);

//! Inoculates a medium in the given container with a specified solid colony of culture and performs incubation at the given temperature for the specified duration on a rotary incubator.
/*!
\param medium the container that contains the medium for inoculation.
\param sample1 the colony used for inoculating the medium.
\param temp the temperature of incubation after inoculation.
\param time1 the duration of incubation after inoculation.
\param shaking_type set to 1 if incubation with shaking is required, else set to 0.
\par Example:
\code inoculation(flask, colony, 37, time(OVERNIGHT, HRS), 1); \endcode
\par Output:
\htmlonly Inoculate <font color=#357EC7>(flask.contents.name)</font> with <font color=#357EC7>(colony.name)</font> and incubate with shaking for <b><font color=#357EC7>12 hrs(overnight)</font></b> at <b><font color=#357EC7>37 C</font></b>. \endhtmlonly
\sa incubate() and store_until() 
*/
extern void inoculation(Container& medium, Solid& sample1, float temp, Time* time1, int shaking_type);

//! Inoculates a medium in the given container with a specified solid colony of culture and performs incubation at the given temperature for the specified duration on a rotary incubator until the desired cell density(X10 \htmlonly <sup>6</sup> \endhtmlonly) is reached.
/*!
\param medium the container that contains the medium for inoculation.
\param sample1 the colony used for inoculating the medium.
\param temp the temperature of incubation after inoculation.
\param time1 the duration of incubation after inoculation.
\param cell_density the required cell density after incubation.
\param shaking_type set to 1 if incubation with shaking is required, else set to 0.
\par Example:
\code inoculation(flask, colony, 37, time(OVERNIGHT, HRS), 6, 1); \endcode
\par Output:
\htmlonly Inoculate <font color=#357EC7>(flask.contents.name)</font> with <font color=#357EC7>(colony.name)</font> and incubate with shaking for <b><font color=#357EC7>12 hrs(overnight)</font></b> at <b><font color=#357EC7>37 C</font></b>.<br>Calibrate the spectrophotometer and make sure that the cell density of the culture has reached <b><font color=#357EC7>6 X 10<sup>6</sup> cells/ml</font></b> before continuing. \endhtmlonly
\sa incubate() and store_until() 
*/
extern void inoculation(Container& medium, Solid& sample1, float temp, Time* time1, float cell_density, int shaking_type);

//! Inoculates a medium in the given container with a specified volume of culture and performs incubation at the given temperature for the specified duration on a rotary incubator.
/*!
\param medium the container that contains the medium for inoculation.
\param sample1 the culture used for inoculating the medium.
\param volume1 the volume of \c sample1 that is used for inoculation.
\param temp the temperature of incubation after inoculation.
\param time1 the duration of incubation after inoculation.
\param shaking_type set to 1 if incubation with shaking is required, else set to 0.
\par Example:
\code inoculation(flask, subculture, vol(200, UL), 37, time(OVERNIGHT, HRS), 0); \endcode
\par Output:
\htmlonly Inoculate <font color=#357EC7>(flask.contents.name)</font> with <b><font color=#357EC7>200 ul</font></b> of <font color=#357EC7>(sub_culture.name)</font> and incubate without shaking for <b><font color=#357EC7>12 hrs(overnight)</font></b> at <b><font color=#357EC7>37 C</font></b>. \endhtmlonly
\sa incubate() and store_until()
*/
extern void inoculation(Container& medium, Fluid& sample1, Volume* volume1, float temp, Time* time1, int shaking_type);

//! Inoculates a medium in the given container with a specified volume of culture and performs incubation at the given temperature for the specified duration on a rotary incubator until the desired cell density(X10 \htmlonly <sup>6</sup> \endhtmlonly) is reached.
/*!
\param medium the container that contains the medium for inoculation.
\param sample1 the culture used for inoculating the medium.
\param volume1 the volume of \c sample1 that is used for inoculation.
\param temp the temperature of incubation after inoculation.
\param time1 the duration of incubation after inoculation.
\param cell_density the required cell density after incubation.
\param shaking_type set to 1 if incubation with shaking is required, else set to 0.
\par Example:
\code inoculation(flask, subculture, vol(200, UL), 37, time(OVERNIGHT, HRS), 6, 0); \endcode
\par Output:
\htmlonly Inoculate <font color=#357EC7>(flask.contents.name)</font> with <b><font color=#357EC7>200 ul</font></b> of <font color=#357EC7>(sub_culture.name)</font> and incubate without shaking for <b><font color=#357EC7>12 hrs(overnight)</font></b> at <b><font color=#357EC7>37 C</font></b>.<br>Calibrate the spectrophotometer and make sure that the cell density of the culture has reached <b><font color=#357EC7>6 X 10<sup>6</sup> cells/ml</font></b> before continuing.\endhtmlonly
\sa incubate() and store_until()
*/
extern void inoculation(Container& medium, Fluid& sample1, Volume* volume1, float temp, Time* time1, float cell_density, int shaking_type);

//! Mixes the contents of the given container by inverting the container for a few seconds.
/*!
\param container1 the container whose contents have to be mixed.
\par Example:
\code invert(eppendorf);\endcode
\par Output:
\htmlonly Close the tube tightly and gently mix the contents by inverting the tube. \endhtmlonly
\sa tap(), stir(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void invert(Container& container1);

//! Mixes the contents of the given container by inverting it for the specified number of times.
/*!
\param container1 the container whose contents have to be mixed.
\param times the number of times that the container has to be inverted.
\par Example:
\code invert(eppendorf, 5);\endcode
\par Output:
\htmlonly Close the tube tightly and gently mix the contents by inverting the tube <b><font color=#357EC7>5 times</font></b>. \endhtmlonly
\sa tap(), stir(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void invert(Container& container1, int times);

//! Mixes the contents of the given container by inverting it for the specified number of times.
/*!
\param container1 the container whose contents have to be mixed.
\param min_times the minimum number of times that the container has to be inverted.
\param max_times the maximum number of times that the container has to be inverted.
\par Example:
\code invert(eppendorf, 3, 5);\endcode
\par Output:
\htmlonly Close the tube tightly and gently mix the contents by inverting the tube <b><font color=#357EC7>3 - 5 times</font></b>. \endhtmlonly
\sa tap(), stir(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void invert(Container& container1, int min_times, int max_times);

//! Mixes the contents of the given container by inverting it until the specified result is obtained.
/*!
\param container1 the container whose contents have to be mixed.
\param event1 the event until which the container has to be mixed. (See enum until)
\par Example:
\code invert(eppendorf, PPT_STOPS_STICKING);\endcode
\par Output:
\htmlonly Close the tube tightly and gently mix the contents by inverting the tube until the precipitate stops sticking to the walls of the tube. \endhtmlonly
\sa tap(), stir(), vortex(), dissolve(), resuspend(), pipet(), combine_and_mix() and enum until
*/
extern void invert(Container& container1, enum until event1);

//! Inverts the container containing a pellet to dry it.
/*!
\param container1 the container whose contents have to be dried.
\param temp the temperature of drying.
\par Example:
\code invert_dry(tube1, RT); \endcode
\par Output:
\htmlonly Stand the tube containing (tube1.contents.name) in an inverted position on a paper towel to allow all of the fluid to drain away. \endhtmlonly
\sa store(), store_for(), store_until, incubate(), enum ranges and enum time_unit
*/
extern void invert_dry(Container& container1, float temp);

//! Inverts the container containing a pellet to dry it. The approximate time taken for drying the pellet is also specified.
/*!
\param container1 the container whose contents have to be dried.
\param temp the temperature of drying.
\param time1 the duration of drying.
\par Example:
\code invert_dry(tube1, 37, time(15, MINS)); \endcode
\par Output:
\htmlonly Stand the tube containing (tube1.contents.name) for <b><font color=#357EC7>15 mins</font></b> in an inverted position on a paper towel to allow all of the fluid to drain away.
Dry at <b><font color=#357EC7>37 C</font></b>.\endhtmlonly
\sa store(), store_for(), store_until and incubate()
*/
extern void invert_dry(Container& container1, float temp, Time* time1);

//! Measures out \c container's into another specified container. 
/*!
\param container the container whose contents have to be measured out.
\param container1 the container into which \c fluid1 has to be measured.
\par Example:
\code measure_fluid(rxn_tube, eppendorf); \endcode
\par Output:
\htmlonly Measure out <font color=#357EC7>(rxn_tube.contents.name)</font> into (eppendorf.name).<br>\endhtmlonly
\sa measure_solid() and measure_prop()
*/
extern void measure_fluid(Container& container, Container& container1);

//! Measures out \c fluid1 into the specified container. 
/*!
\param fluid1 the fluid that has to be measured out.
\param container1 the container into which \c fluid1 has to be measured.
\par Example:
\code measure_fluid(solution1, eppendorf);
 measure_fluid(solution2, eppendorf);\endcode
\par Output:
\htmlonly Measure out <font color=#357EC7>(solution1.name)</font> into (eppendorf.name).<br>
Add <font color=#357EC7>(solution2.name)</font> to (solution1.name).\endhtmlonly
\sa measure_solid() and measure_prop()
*/
extern void measure_fluid(Fluid& fluid1, Container& container1);

//! Measures out a specified volume of \c fluid1.
/*!
\param fluid1 the fluid that has to be measured out.
\param volume1 the volume of \c fluid1 required.
\par Example:
\code measure_fluid(solution1, vol(1, ML));\endcode
\par Output:
\htmlonly Measure out <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(solution1.name)</font>.\endhtmlonly
\sa measure_solid() and measure_prop()
*/
extern void measure_fluid(Fluid& fluid1, Volume* volume1);

//! Measures out a specified volume of \c fluid1 into the given container.
/*!
\param fluid1 the fluid that has to be measured out.
\param volume1 the volume of \c fluid1 required.
\param container1 the container into which \c fluid1 has to be measured.
\par Example:
\code measure_fluid(solution1, vol(1, ML), tube1);
 measure_fluid(solution2, vol(1, ML), tube1);\endcode
\par Output:
\htmlonly Measure out <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(solution1.name)</font> into (tube1.name).<br>
Add <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(solution2.name)</font>.\endhtmlonly
\sa measure_solid() and measure_prop
*/
extern void measure_fluid(Fluid& fluid1, Volume* volume1, Container& container1);

//! Measures out a specified volume of contents of container \c con into another container, \c container1. 
/*!
\param con the container whose contents have to be measured out.
\param volume1 the volume of \c con 's contents required.
\param container1 the destination container of the measured contents of \c con.
\par Example:
\code measure_fluid(eppendorf1, vol(1, ML), tube1);\endcode
\par Output:
\htmlonly Measure out <b><font color=#357EC7>1 ml</font></b> of <font color=#357EC7>(eppendorf1.contents.name)</font> into (tube1.name).\endhtmlonly
\sa measure_solid() and measure_prop
*/
extern void measure_fluid(Container& con, Volume* volume1, Container& container1);

//! Measures the fluorescence of the contents of the specified container.
/*!
\param container1 the container whose contents need to be measured for fluorescence.
\par Example:
\code measure_fluorescence(eppendorf1);\endcode
\par Output:
\htmlonly Measure the fluorescence of (eppendorf1.contents.name).\endhtmlonly
*/
extern void measure_fluorescence (Container& container1);

//! Measures out a specified weight of \c solid1 into the given container.
/*!
\param solid1 the solid that has to be measured out.
\param container1 the container into which \c solid1 has to be measured.
\par Example:
\code measure_solid(sds, tube1); \endcode
\par Output:
\htmlonly Measure out <font color=#357EC7>(sds.name)</font> into (tube1.name). \endhtmlonly
\sa measure_fluid() and measure_prop()
*/
extern void measure_solid(Solid& solid1, Container& container1);

//! Measures out a specified weight of \c solid1 into the given container.
/*!
\param solid1 the solid that has to be measured out.
\param weight the weight of \c solid1 required.
\param unit the unit of \c weight.
\param container1 the container into which \c solid1 has to be measured.
\par Example:
\code measure_solid(sds, 2, GR, tube1); \endcode
\par Output:
\htmlonly Measure out <b><font color=#357EC7>2 g</font></b> of <font color=#357EC7>(sds.name)</font> into (tube1.name). \endhtmlonly
\sa measure_fluid(), enum weight_unit and measure_prop()
*/
extern void measure_solid(Solid& solid1, float weight, enum weight_unit unit, Container& container1);

//! Measures a specified proportion of fluid \c source and adds it to the \c dest container. Combination of measure_fluid() and combine() instructions.
/*!
\param dest the container to which \c source is added.
\param source the fluid that has to be measured out.
\param prop the proportion of \c dest 's contents that has to be measured out of \c source.
\par Example:
\code measure_prop(tube1, phenol, 2); \endcode
\par Output:
\htmlonly Add <b><font color=#357EC7>2</font></b> volumes <font color=#357EC7>(phenol.name)</font>. \endhtmlonly
\sa measure_fluid and measure_fluid()
*/
extern void measure_prop(Container& dest, Fluid& source, float prop);

//! Measures a specified proportion of \c source container's contents and adds it to the \c dest container. Combination of measure_fluid() and combine() instructions.
/*!
\param dest the container to which \c source 's contents are added.
\param source the container whose contents have to be measured out.
\param prop the proportion of \c dest 's contents that has to be measured out of \c source.
\par Example:
\code measure_prop(tube1, tube2, 2); \endcode
\par Output:
\htmlonly Add <b><font color=#357EC7>2</font></b> volumes <font color=#357EC7>(tube2.contents.name)</font>. \endhtmlonly
\sa measure_fluid and measure_fluid()
*/
extern void measure_prop(Container& dest, Container& source, float prop);

//! Prints a table with the given fluid array as the column elements and the given tubes (reaction names) as the row elements.
/*!
\param no_rows the number of rows in the table(= Number of members in \c array1 + 1).
\param no_columns the number of columns in tht table(= Number of members in array \c tube  + 1).
\param rxn_vol the final volume of the reaction in each tube.
\param array1 the array of fluids that make up the reaction in each tube.
\param tube the array of unique tubes/ reaction names.
\param vol the array (multidimensional) of volumes of each fluid in \c array1 that make up each reaction in \c tube.
\param container1 the container that contains the final reaction mix.
\par Example:
\code 
{
	Fluid fluid_array[7] = {dl_water, neb2, bsa, dna, x, s, p};
	char*	tube[2] = {"R","E"};
	float volumes[2][7]={{38.5,5,0.5,4,0,1,1},{38.5,5,0.5,4,1,0,1}};
	mixing_table(3, 8, vol(50, UL), fluid_array, tube, *volumes, rxn_tube);
}
\endcode
\par Output:
\htmlonly Use the following table as a checklist for preparing the reactions:<br><br><table border cellpadding=5 rules=all frame=void bordercolor=#357EC7><thead><tr><td>&nbsp;</td><td><font color=#357EC7>distilled water(dl H<sub>2</sub>O)</font></td><td><font color=#357EC7>NEB2 buffer(NEB2)</font></td><td><font color=#357EC7>100X BSA(BSA)</font></td><td><font color=#357EC7>DNA</font></td><td><font color=#357EC7>Xba I(Xba)</font></td><td><font color=#357EC7>Spe I(Spe)</font></td><td><font color=#357EC7>Pst I(Pst)</font></td></tr></thead><tbody><tr><td><font color=#357EC7>R</font></td><td><b><font color=#357EC7>38.5</font></b></td><td><b><font color=#357EC7>5</font></b></td><td><b><font color=#357EC7>0.5</font></b></td><td><b><font color=#357EC7>4</font></b></td><td><b><font color=#357EC7>0</font></b></td><td><b><font color=#357EC7>1</font></b></td><td><b><font color=#357EC7>1</font></b></td></tr></body><tbody><tr><td><font color=#357EC7>E</font></td><td><b><font color=#357EC7>38.5</font></b></td><td><b><font color=#357EC7>5</font></b></td><td><b><font color=#357EC7>0.5</font></b></td><td><b><font color=#357EC7>4</font></b></td><td><b><font color=#357EC7>1</font></b></td><td><b><font color=#357EC7>0</font></b></td><td><b><font color=#357EC7>1</font></b></td></tr></body></table> \endhtmlonly
\sa mixing_table_pcr()
*/
extern void mixing_table(int no_rows, int no_columns, Volume* rxn_vol, Fluid array1[], char* tube[], float *vol, Container& container1);

//! Prints a table with the given fluid array as the column elements and the given tubes (reaction names) as the row elements.
/*!
\param no_rows the number of rows in the table(= Number of members in \c array1 + 1).
\param no_columns the number of columns in tht table(= Number of members in array \c tube  + 1).
\param array1 the array of fluids that make up the reaction in each tube.
\param tube the array of unique tubes/ reaction names.
\param vol the array of volumes of each fluid in \c array1 that make up each reaction in \c tube.
\param rxn_vol the final volume of the reaction in each tube.
\param container1 the container that contains the final reaction mix.
\par Example:
\code
{
	Fluid fluid_array[6] = {dna, bsa, buffer, enzyme1, enzyme2, water};
	char* tubes[1] = {"Restriction Digest - PCR product"};
	Volume* volume[6] = {s_vol(v), vol(3.5, UL), vol(3.5, UL), vol(0.2, UL), vol(0.2, UL), vol(XVAL, UL)};
	mixing_table(2, 7, fluid_array, tubes, volume, vol(35, UL), rxn_tube1);
}
\endcode
\par Output:
\htmlonly Use the following table as a checklist for preparing the reaction in (rxn_tube1.name):<br><br><table border cellpadding=5 rules=all frame=void bordercolor=#357EC7><thead><tr><td>&nbsp;</td><td><font color=#357EC7>PCR product DNA</font></td><td><font color=#357EC7>10X BSA</font></td><td><font color=#357EC7>10X restriction buffer</font></td><td><font color=#357EC7>restriction enzyme1</font></td><td><font color=#357EC7>restriction enzyme2</font></td><td><font color=#357EC7>distilled H2O</font></td></tr></thead><tbody><tr><td><font color=#357EC7>Restriction Digest - PCR product</font></td><td><b><b><font color=#357EC7>V</font></b></td><td><b><b><font color=#357EC7>3.5 ul</font></b></td><td><b><b><font color=#357EC7>3.5 ul</font></b></td><td><b><b><font color=#357EC7>0.2 ul</font></b></td><td><b><b><font color=#357EC7>0.2 ul</font></b></td><td><b><font color=#357EC7>Make up the volume to <b><font color=#357EC7>35 ul</font></b></font></b></td></tr></body></table> \endhtmlonly
\sa mixing_table_pcr()
*/
extern void mixing_table(int no_rows, int no_columns, Fluid array1[],  char* tube[], Volume* vol[], Volume* rxn_vol, Container& container1);

//! Prints a table with the given fluid array, initial concentration and final concentration values as parameters of the columns and their corresponding values as row values.
/*!
\param no_rows the number of rows in the table(= Number of members in \c array1 + 1).
\param rxn_vol the final volume of the reaction in each tube.
\param array1 the array of fluids that make up the reaction in each tube.
\param initial_conc the initial concentration of the fluids in \c array1.
\param final_conc the final concentration of the fluids in \c array1
\param vol the array of volumes of each fluid in \c array1 that make up each reaction in \c tube.
\param container1 the container that contains the final reaction mix.
\par Example:
\code
{
	Fluid fluid_array[5]={dna, sau3a1, neb, bsa, water};
	char* (initial_conc)[5]={"--","20 U ul<sup>-1</sup>","10X","10 mg ml<sup>-1</sup>","--"};
	char* (final_conc)[5]={"2 ug","40 U","1X","0.33 ug ul<sup>-1</sup>","--"};
	Volume* volumes[5]={vol(1, UL), vol(2, UL), vol(3, UL), vol(1, UL), vol(23, UL)};
	mixing_table_pcr(6, vol(30, UL), fluid_array, initial_conc, final_conc, volumes, sterile_microfuge_tube);
}
\endcode
\par Output:
\htmlonly Set up a reaction in (sterile_microfuge_tube.name) as follows on ice:<br><br><table border cellpadding=5 rules=all frame=void bordercolor=#357EC7><thead><tr><td>&nbsp;</td><td><font color=#357EC7>Initial concentration</font></td><td><font color=#357EC7>Final concentration</font></td><td><font color=#357EC7>Final volume per 30 ul reaction</font></td></tr></thead><tbody><tr><td><font color=#357EC7>Genomic DNA</font></td><td><b><font color=#357EC7>--</font></b></td><td><b><font color=#357EC7>2 ug</font></b></td><td><b><font color=#357EC7>1 ul</font></b></td></tr></body><tbody><tr><td><font color=#357EC7>Sau3A1 enzyme</font></td><td><b><font color=#357EC7>20 U ul<sup>-1</sup></font></b></td><td><b><font color=#357EC7>40 U</font></b></td><td><b><font color=#357EC7>2 ul</font></b></td></tr></body><tbody><tr><td><font color=#357EC7>NEB buffer 1</font></td><td><b><font color=#357EC7>10X</font></b></td><td><b><font color=#357EC7>1X</font></b></td><td><b><font color=#357EC7>3 ul</font></b></td></tr></body><tbody><tr><td><font color=#357EC7>Acetylated BSA</font></td><td><b><font color=#357EC7>10 mg ml<sup>-1</sup></font></b></td><td><b><font color=#357EC7>0.33 ug ul<sup>-1</sup></font></b></td><td><b><font color=#357EC7>1 ul</font></b></td></tr></body><tbody><tr><td><font color=#357EC7>Double-distilled water</font></td><td><b><font color=#357EC7>--</font></b></td><td><b><font color=#357EC7>--</font></b></td><td><b><font color=#357EC7>23 ul</font></b></td></tr></body></table> \endhtmlonly
\sa mixing_table()
*/
extern void mixing_table_pcr(int no_rows, Volume* rxn_vol, Fluid array1[], char*initial_conc[], char* final_conc[], Volume* vol[], Container& container1);

//! Prompts the used to mount the specified slide using the given liquid and to view it under the given device.
/*!
\param slide1 the slide that has to be mounted and observed.
\param sample1 the fluid usen to mount \c slide1.
\param device the device used to observe the mounted slide.
\par Example:
\code mount_observe_slide(slide, entellan, "bright-field microscope"); \endcode
\par Output:
\htmlonly Mount (slide.name) with <b><font color=#357EC7>(entellan.name)</font></b>, let it solidify and view under bright-field microscope.\endhtmlonly
*/
extern void mount_observe_slide(Slide& slide1, Fluid& sample1, char* device);

//! Multiplies a symbolic volume with an actual volume.
/*!
\param vol1 the symbolic volume.
\param vol2 the actual volume.
\par Example:
\code y = multiply(s_vol(x), vol(10, UL)); \endcode
\par Output:
Internally, \c y is assigned the value \c x*10.
\sa add(), subtract() and divide()
*/
extern Symbol multiply(Symbolic_vol* vol1, Volume* vol2);

//! Multiplies a symbolic volume with another symbolic volume.
/*!
\param vol1 the symbolic volume.
\param vol2 the other symbolic volume.
\par Example:
\code z = multiply(s_vol(x), s_vol(y)); \endcode
\par Output:
Internally, \c z is assigned the value \c x*y.
\sa add(), subtract() and divide()
*/
extern Symbol multiply(Symbolic_vol* vol1, Symbolic_vol* vol2);

//! Labels the column with the \c new_name.
/*!
\param column1 the column that needs to be named.
\param new_name the desired name of \c column1.
\par Example:
\code name_column(column1, "used column"); \endcode
\par Output:
Internally, \c column1 's name is set to "used column".
\sa name_container() and name_sample()
*/
extern void name_column(Column& column1, char* new_name);

//! Labels a container with the given name.
/*!
\param container1 the container that has to be labeled.
\param name the name with which \c container1 has to be labeled.
\par Example:
\code name_container(eppendorf1, "Reaction Tube"); \endcode
\par Output:
\htmlonly Set aside a fresh (eppendorf1.name). Call it Reaction Tube. \endhtmlonly
\sa name_sample()
*/
extern void name_container(Container& container1, char* name);

//! Labels the specified plate with the \c new_name.
/*!
\param plate1 the plate that needs to be named.
\param new_name the desired name of the \c plate1.
\par Example:
\code name_plate(plate1, "Reaction Plate");\endcode
\par Output:
Internally, \c plate1 's name is set to "Reaction Plate". 
\sa name_container(), name_sample()
*/
extern void name_plate(Plate& plate1, char* new_name);

//! Names the contents of the specified container with the \c new_name.
/*!
\param container1 the container whose contents need to be named.
\param new_name the desired name of the contents of \c container1.
\par Example:
\code name_sample(tube1, "lysate"); \endcode
\par Output:
Internally, the name of \c tube1 's contents is set to "lysate".  
\sa name_container()
*/
extern void name_sample(Container& container1, char* new_name);

//! Measures the Nanodrop reading of the contents of the specified container.
/*!
\param container1 the container whose contents need to be analyzed using the Nanodrop.
\par Example:
\code nanodrop(eppendorf1); \endcode
\par Output:
\htmlonly Measure the concentration of (eppendorf1.contents.name) in your final samples with the Nanodrop. \endhtmlonly
\sa ce_detect()
*/
extern void nanodrop(Container& container1);

//! Used to declare a new column with the given name.
/*!
\param name the name of the column.
\return Column
\par Example:
\code Column spin_column = new_column("Qiagen Spin Column"); \endcode
\par Output:
Internally, sets aside a new column with the name "Qiagen Soin Column".
\sa new_container(), new_fluid(), new_solid() and new_plate()
*/
extern Column new_column(char*name);

//! Used to declare a new container of the type specified by \c cont_id.
/*!
\param cont_id the type of container.
\return Container
\par Example:
\code Container microfuge_tube1 = new_container(STERILE_MICROFUGE_TUBE);\endcode
\par Output:
Internally, sets aside a sterile microfuge tube with the name "a sterile microfuge tube".
\sa enum container_type, new_fluid(), new_solid() and new_plate()
*/
extern Container new_container(enum container_type cont_id);

//! Used to declare a new container of the type specified by \c cont_id that contains a fluid at the start of the protocol.
/*!
\param cont_id the type of container.
\param fluid1 the contents of the container.
\return Container
\par Example:
\code Container microfuge_tube1 = new_container(STERILE_MICROFUGE_TUBE, comp_cells);\endcode
\par Output:
Internally, sets aside a sterile microfuge tube with the name "a sterile microfuge tube" and \c comp_cells as contents.
\sa enum container_type, new_fluid(), new_solid() and new_plate()
*/
extern Container new_container(enum container_type cont_id, Fluid& fluid1);

//! Used to declare a new fluid with the given name.
/*!
\param name the name of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1"); \endcode
\par Output:
\htmlonly <ul type="circle"><li>Solution 1</li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern Fluid new_fluid(char* name);

//! Used to declare a new fluid with the given name.
/*!
\param name the name of the fluid.
\param volume1 the initial volume of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", vol(10, ML)); \endcode
\par Output:
\htmlonly <ul type="circle"><li>10 ml Solution 1</li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern Fluid new_fluid(char* name, Volume* volume1);

//! Used to declare a new fluid with the given name. The solid has been stored at the specified temperature.
/*!
\param name the name of the fluid.
\param temp the temperature of storage of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", ICE_COLD, vol(10, ML)); \endcode
\par Output:
\htmlonly <ul type="circle"><li>10 ml ice-cold Solution 1</li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern 	Fluid new_fluid(char*name, float temp);

//! Used to declare a new fluid with the given name. The solid has been stored at the specified temperature.
/*!
\param name the name of the fluid.
\param temp the temperature of storage of the fluid.
\param volume1 the initial volume of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", ICE_COLD); \endcode
\par Output:
\htmlonly <ul type="circle"><li>ice-cold Solution 1</li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern 	Fluid new_fluid(char*name, float temp, Volume* volume1);

//! Used to declare a new fluid with the given name and characteristics.
/*!
\param name the name of the fluid.
\param state the characteristics of the fluid (Eg: "freshly prepared" or composition of a solution).
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", "50mM SDS, 20mM TE"); \endcode
\par Output:
\htmlonly <ul type="circle"><li>Solution 1<br><i>(50mM SDS, 20mM TE)</i></li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern 	Fluid new_fluid(char*name, char* state);

//! Used to declare a new fluid with the given name and characteristics.
/*!
\param name the name of the fluid.
\param state the characteristics of the fluid (Eg: "freshly prepared" or composition of a solution).
\param volume1 the initial volume of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", "50mM SDS, 20mM TE", vol(10, ML)); \endcode
\par Output:
\htmlonly <ul type="circle"><li>10 ml Solution 1<br><i>(50mM SDS, 20mM TE)</i></li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern 	Fluid new_fluid(char*name, char* state, Volume* volume1);

//! Used to declare a new fluid with the given name and characteristics. The fluid has been stored at the specified temperature.
/*!
\param name the name of the fluid.
\param state the characteristics of the fluid (Eg: "freshly prepared" or composition of a solution).
\param temp the temperature of storage of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", "50mM SDS, 20mM TE", RT); \endcode
\par Output:
\htmlonly <ul type="circle"><li>Solution 1 at room temperature<br><i>(50mM SDS, 20mM TE)</i></li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern 	Fluid new_fluid (char* name, char*state, float temp);

//! Used to declare a new fluid with the given name and characteristics. The fluid has been stored at the specified temperature.
/*!
\param name the name of the fluid.
\param state the characteristics of the fluid (Eg: "freshly prepared" or composition of a solution).
\param temp the temperature of storage of the fluid.
\param volume1 the initial volume of the fluid.
\return Fluid
\par Example:
\code Fluid solution1 = new_fluid("Solution 1", "50mM SDS, 20mM TE", RT, vol(10, ML)); \endcode
\par Output:
\htmlonly <ul type="circle"><li>10 ml Solution 1 at room temperature<br><i>(50mM SDS, 20mM TE)</i></li></ul> \endhtmlonly
\sa new_solid(), new_container() and new_plate()
*/
extern 	Fluid new_fluid (char* name, char*state, float temp, Volume* volume1);

//! Used to declare a new parameter (symbolic volume) with the given name and definition. 
/*!
\param symbol the symbol used to denote the parameter(symbolic volume).
\param def the definition of the parameter.
\return Plate
\par Example:
\code Symbol x = new_symbol("X", "concentration of vector DNA");\endcode
\par Output:
\htmlonly X - concentration of vector DNA \endhtmlonly
\sa new_fluid(), new_solid(), new_plate() and new_container()
*/
extern Symbol new_symbol(char* symbol, char* def);

//! Used to declare a new parameter (symbolic time) with the given name and definition. 
/*!
\param symbol the symbol used to denote the parameter(symbolic volume).
\param def the definition of the parameter.
\return Plate
\par Example:
\code Symbol_t x = new_symbol_t("X", "time taken for elongation");\endcode
\par Output:
\htmlonly X - time taken for elongation \endhtmlonly
\sa new_fluid(), new_solid(), new_plate() and new_container()
*/
extern Symbol_t new_symbol_t(char* symbol, char* def);

//! Used to declare a new plate with the given name.
/*!
\param name the name of the plate.
\return Plate
\par Example:
\code Plate lb_plate = new_plate("LB plate");\endcode
\par Output:
\htmlonly <ul type="circle"><li>LB plate</li></ul> \endhtmlonly
\sa new_fluid(), new_solid() and new_container()
*/
extern Plate new_plate(char*name);

//! Used to declare a new plate with the given name and characteristics.
/*!
\param name the name of the plate.
\param state the characteristics of the fluid (Eg: composition of a plate).
\return Plate
\par Example:
\code Plate lb_plate = new_plate("LB plate", "containing appropriate antibiotic");\endcode
\par Output:
\htmlonly <ul type="circle"><li>LB plate<br><i>(containing appropriate antibiotic)</i></li></ul> \endhtmlonly
\sa new_fluid(), new_solid() and new_container()
*/
extern Plate new_plate(char*name, char* state);

//! Used to declare a new slide with the given name.
/*!
\param name the name of the slide.
\return Slide
\par Example:
\code Slide tissue_slide = new_slide("slide containing tissue sample");\endcode
\par Output:
\htmlonly <ul type="circle"><li>slide containing tissue sample</li></ul> \endhtmlonly
\sa new_fluid(), new_solid(), new_plate() and new_container()
*/
extern Slide new_slide(char* name);

//! Used to declare a new solid with the given name.
/*!
\param name the name of the solid.
\return Solid
\par Example:
\code Solid colony = new_solid("a single colony of plasmid-containing bacteria");\endcode
\par Output:
\htmlonly <ul type="circle"><li>a single colony of plasmid-containing bacteria</li></ul> \endhtmlonly
\sa new_fluid(), new_container() and new_plate()
*/
extern Solid new_solid(char* name);

//! Used to declare a new solid with the given name. The solid has been stored at the specified temperature.
/*!
\param name the name of the solid.
\param temp the temperature of storage of the solid.
\return Solid
\par Example:
\code Tissue plant_tissue = new_solid("wet plant tissue", 4);\endcode
\par Output:
\htmlonly <ul type="circle"><li>wet plant tissue at 4 C</li></ul> \endhtmlonly
\sa new_fluid(), new_container() and new_plate()
*/
extern 	Solid new_solid(char*name, float temp);

//! Used to declare a new solid with the given name and characteristics.
/*!
\param name the name of the solid.
\param state the characteristics of the solid (Eg: composition of a solid).
\return Solid
\par Example:
\code Tissue plant_tissue = new_solid("wet plant tissue", "freshly collected");\endcode
\par Output:
\htmlonly <ul type="circle"><li>wet plant tissue<br><i>(freshly collected)</i></li></ul> \endhtmlonly
\sa new_fluid(), new_container() and new_plate()
*/
extern 	Solid new_solid(char*name, char* state);

//! Used to declare a new solid with the given name and characteristics. The solid has been stored at the specified temperature.
/*!
\param name the name of the solid.
\param state the characteristics of the solid (Eg: composition of a solid).
\param temp the temperature of storage of the solid.
\return Solid
\par Example:
\code Tissue plant_tissue = new_solid("wet plant tissue", "freshly collected", 4); \endcode
\par Output:
\htmlonly <ul type="circle"><li>wet plant tissue at 4 C<br><i>(freshly collected)</i></li></ul> \endhtmlonly
\sa new_fluid(), new_container() and new_plate()
*/
extern 	Solid new_solid (char* name, char*state, float temp);

//! Marks the next option of a step of a protocol.
/*!
\par Example:
\code next_option();\endcode
\par Output:
\htmlonly <b>Option 2:</b> \endhtmlonly
\sa first_option(), end_option()
*/
extern void next_option();

//! Marks the next option of a step of a protocol and gives it a name.
/*!
\param name the name of the step.
\par Example:
\code next_option();\endcode
\par Output:
\htmlonly <b>Option 2:</b> \endhtmlonly
\sa first_option(), end_option()
*/
extern void next_option(char* name);

//! Marks the ending of a step and the beginning of the next step of a protocol.
/*!
\par Example:
\code next_step();\endcode
\par Output:
\htmlonly 2. \endhtmlonly
\sa first_step(), optional_step() and next_sub_step()
*/
extern void next_step();

//! Marks the ending of a step and the beginning of the next step of a protocol. Also specifies the name of the step.
/*!
\param step_name the name of the step.
\par Example:
\code next_step("DNA lysis");\endcode
\par Output:
\htmlonly 2.<b>DNA lysis</b> \endhtmlonly
\sa first_step(), optional_step() and next_sub_step()
*/
extern void next_step(char* step_name);

//! Marks the end of a sub-step and the start of the subsequent sub-step of a step in the protocol.
/*!
\par Example:
\code next_sub_step();\endcode
\par Output:
\htmlonly b. \endhtmlonly
\sa next_step(), optional_step() and next_sub_step()
*/
extern void next_sub_step();

//! Marks an optional step in the protocol.
/*!
\par Example:
\code optional_step(); \endcode
\par Output:
\htmlonly <b>(Optional)</b> \endhtmlonly
\sa first_step(), next_step(), first_sub_step() and next_sub_step()
*/
extern void optional_step();

//! Marks an optional step in the protocol and gives it a name.
/*!
\param name the name of the step.
\par Example:
\code optional_step(); \endcode
\par Output:
\htmlonly <b>(Optional)</b> \endhtmlonly
\sa first_step(), next_step(), first_sub_step() and next_sub_step()
*/
extern void optional_step(char* name);

//! Marks a parallel step in the protocol.
/*!
\par Example:
\code parallel_step(); \endcode
\par Output:
\htmlonly 3. <b> Meanwhile: </b> \endhtmlonly
\sa first_step(), next_step(), first_sub_step() and next_sub_step()
*/
extern void parallel_step();

//! Patches plate with colonies onto a fresh plate \c plate1.
/*!
\param plate1 the fresh plate.
\param colony the plate with the colonies that need to be patched.
\par Example:
\code patch(lb_plate, colony_plate); \endcode
\par Output:
\htmlonly Patch <font color=#357EC7>(colony_plate.name)</font> onto (lb_plate.name). \endhtmlonly
\sa plate_out(), streak_plate()
*/
extern void patch(Plate& plate1,Plate& colony);

//! Performs final extension of the contents of the given container in a thermocycler.
/*!
\param container1 the container whose contents have to be subject to initial denaturation.
\param temp_ext the temperature of final extension.
\param time_ext the duration of final extension.
\param temp_hold the temperature at which the contents are held until they are removed from the thermocycler.
\par Example:
\code pcr_final_ext(rxn_tube, 72, time(15, MINS), 4);\endcode
\par Output:
\htmlonly Termination<ul><li>Elongate: <b><font color=#357EC7>72 C</font></b>, <b><font color=#357EC7>15 mins</font></b></li><li>Hold: <b><font color=#357EC7>4 C</font></b>, until removed from machine </li></ul> \endhtmlonly
\sa pcr_initial_denat() and thermocycler()
*/
extern void pcr_final_ext(Container& container1, float temp_ext, Time* time_ext, float temp_hold);

//! Performs final extension of the contents of the given container in a thermocycler.
/*!
\param container1 the container whose contents have to be subject to initial denaturation.
\param temp_ext the temperature of final extension.
\param time_ext the duration of final extension.
\param temp_halt the temperature at which the PCR reaction is halted.
\param time_halt the duration of time required for the halt.
\param temp_hold the temperature at which the contents are held until they are removed from the thermocycler.
\par Example:
\code pcr_final_ext(rxn_tube, 72, time(15, MINS), 4, time(10, MINS), 4);\endcode
\par Output:
\htmlonly Termination<ul><li>Elongate: <b><font color=#357EC7>72 C</font></b>, <b><font color=#357EC7>15 mins</font></b></li><li>Halt reaction: <b><font color=#357EC7>4 C</font></b>, <b><font color=#357EC7>10 mins</font></b></li><li>Hold: <b><font color=#357EC7>4 C</font></b>, until removed from machine </li></ul> \endhtmlonly
\sa pcr_initial_denat() and thermocycler()
*/
extern void pcr_final_ext(Container& container1, float temp_ext, Time* time_ext, float temp_halt, Time* time_halt, float temp_hold);

//! Performs initial denaturation of the contents of the given container in a thermocycler.
/*!
\param container1 the container whose contents have to be subject to initial denaturation.
\param temp the temperature of initial denaturation.
\param time1 the duration of initial denaturation.
\par Example:
\code pcr_init_denat(rxn_tube, 94, time(15,MINS));\endcode
\par Output:
\htmlonly Program a standard thermocycler to run the reaction using the following parameters:<br> Initial denaturation<ul><li>Denature: <b><font color=#357EC7>94 C</font></b>, <b><font color=#357EC7>15 mins</font></b></li></ul>\endhtmlonly
\sa pcr_final_ext() and thermocycler()
*/
extern void pcr_init_denat(Container& container1, float temp, Time* time1);

//! Resuspends the contents of the solution in tube by pipetting up and down for a few seconds.
/*!
\param container1 the container whose contents have to be resuspended.
\par Example:
\code pipet(tube1);\endcode
\par Output:
\htmlonly Mix solution by pipetting up and down several times. \endhtmlonly
\sa tap(), stir(), invert(), vortex(), dissolve(), resuspend() and combine_and_mix()
*/
extern void pipet (Container& container1);

//! Plates out the contents of the specified container onto the given plate.
/*!
\param plate1 the plate onto which contents of \c container1 have to be plated out.
\param container1 the container whose contents have to plated out.
\par Example:
\code plate_out(lb_plate, culture_eppendorf);\endcode
\par Output:
\htmlonly Plate out (culture_eppendorf.contents.name) onto <font color=#357EC7>(lb_plate.name)</font>. \endhtmlonly
\sa new_plate()
*/
extern void plate_out(Plate& plate1, Container& container1);

//! Plates out the specified volume of contents of the specified container onto the given plate.
/*!
\param plate1 the plate onto which contents of \c container1 have to be plated out.
\param container1 the container whose contents have to plated out.
\param volume1 the volume of contents of \c container1 that has to be plated out.
\par Example:
\code plate_out(lb_plate, culture_eppendorf, vol(100, UL));\endcode
\par Output:
\htmlonly Plate out <b><font color=#357EC7>100 ul</font></b> of (culture_eppendorf.contents.name) onto <font color=#357EC7>(lb_plate.name)</font>. \endhtmlonly
\sa new_plate() and enum vol_unit
*/
extern void plate_out(Plate& plate1, Container& container1, Volume* volume1);

//! Removes the slide \c slide1 from the specified fluid.
/*!
\param slide1 the slide that has to be removed.
\param fluid1 the fluid from which \c slide1 has to be removed.
\par Example:
\code remove_slide(slide1, solutiona); \endcode
\par Output:
\htmlonly Take out (slide1.name) from (solutiona.name). \endhtmlonly
\sa immerse_slide() and immerse_slide_and_incubate
*/
extern void remove_slide(Slide& slide1, Fluid& fluid1);

//! Used to prompt the user to repeat a given step of a protocol.
/*!
\param step_no the step number of the protocol that should be repeated.
\par Example:
\code repeat(3);\endcode
\par Output:
\htmlonly Repeat Step 3. \endhtmlonly
*/
extern void repeat(int step_no);

//! Resuspends the contents of the solution in tube.
/*!
\param container1 the container whose contents have to be resuspended.
\par Example:
\code resuspend(eppendorf);\endcode
\par Output:
\htmlonly Resuspend the (eppendorf.contents.name) by vortexing/shaking vigorously. \endhtmlonly
\sa tap(), stir(), invert(), vortex(), dissolve(), pipet() and combine_and_mix()
*/
extern void resuspend(Container& container1);

//! Sets the specified container as the working container for the specified sample.
/*!
\param fluid1 the sample whose container is being set.
\param container1 the container which contains \c sample1.
\par Example:
\code set_container(culture, flask);\endcode
\par Output:
Internally, set the contents of \c flask to \c culture.
\sa new_container(), enum container_type
*/
extern void set_container(Fluid& fluid1, Container& container1);

//! Sets the value of the specified symbolic volume.
/*!
\param symbol1 the symbol whose value is being set.
\param value the value of the symbol.
\param unit the unit of \c value.
\par Example:
\code set_value(x, 4, UL); \endcode
\par Output:
Internally, \c x 's value is set to '4' and \c x 's unit is set to 'UL'.
\sa new_container(), enum container_type
*/
extern void set_value(Symbol& symbol1, float value, enum vol_unit unit);

//! Sets the value of the specified symbolic volume.
/*!
\param symbol1 the symbol whose value is being set.
\param value the value of the symbol.
\param unit the unit of \c value.
\par Example:
\code set_value(x, 1, MINS); \endcode
\par Output:
Internally, \c x 's value is set to '4' and \c x 's unit is set to 'MINS'.
\sa new_container(), enum container_type
*/
extern void set_value(Symbol_t& symbol1, float value, enum time_unit unit);

//! Prompts the user to send the sample for sequencing after diluting to appropriate concentration.
/*!
\param container1 the container whose contents have to be sent for sequencing.
\par Example:
\code sequencing(eppendorf); \endcode
\par Output:
\htmlonly Dilute to <font color=#357EC7>100ng/ µL</font> and send <font color=#357EC7>1 µg (10 µL)</font> for sequencing. \endhtmlonly
\sa electrophoresis(), nanodrop()
*/
extern void sequencing(Container& container1);

//! Sets the temperature of the contents of the given container as specified.
/*!
\param container1 the container whose contents have to be set to a particular temperature.
\param temp the temperature setting.
\par Example:
\code set_temp(eppendorf1, 37); \endcode
\par Output:
\htmlonly Set the temperature of (eppendorf1.contents.name) to <b><font color=#357EC7>37 C</font></b>. \endhtmlonly
\sa store(), store_for(), store_until(), incubate()
*/
extern void set_temp(Container& container1, float temp);

//! Creates an html file with the given name and opens it for writing the protocol.
/*!
\param name the name of the protocol.
\par Example:
\code start_protocol("DNA Miniprep");\endcode
\par Output:
Internally, opens a new html file named "DNA Miniprep" for writing. The title of the protocol "DNA Miniprep" is also printed on the top left corner of the file.
\sa end_protocol()
*/
extern void start_protocol(char* name);

//! Mixes the contents of the given container by stirring the container for a few seconds.
/*!
\param container1 the container whose contents have to be mixed.
\par Example:
\code stir(eppendorf);\endcode
\par Output:
\htmlonly Gently stir the mixture for a few secs. \endhtmlonly
\sa tap(), invert(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void stir(Container& container1);

//! Mixes the contents of the given container by stirring the container for the specified duration of time.
/*!
\param container1 the container whose contents have to be mixed.
\param time1 the duration of tapping.
\par Example:
\code stir(eppendorf, time(10, SECS));\endcode
\par Output:
\htmlonly Gently stir the mixture for <b><font color=#357EC7>10 secs</font></b>. \endhtmlonly
\sa tap(), invert(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void stir(Container& container1, Time* time1);

//! Stores the specified container at a given temperature.
/*!
\param container1 the container whose contents have to be stored.
\param temp the temperature of storage.
\par Example:
\code store(tube1, ON_ICE);\endcode
\par Output:
\htmlonly Store the tube <b><font color=#357EC7>on ice</font></b>. \endhtmlonly
\sa  store_for(), store_until(), incubate()
*/
extern void store(Container& container1, float temp);

//! Stores the specified container at a given temperature and given duration of time. Combination of set_temp() and wait().
/*!
\param container1 the container whose contents have to be stored.
\param temp the temperature of storage.
\param time1 the duration of storage.
\par Example:
\code store_for(tube1, 37, min_time(5, MINS));\endcode
\par Output:
\htmlonly Store at <b><font color=#357EC7>37 c</font></b> for at most <b><font color=#357EC7>5 mins</font></b>. \endhtmlonly
\sa  store(), store_until() and incubate()
*/
extern void store_for(Container& container1, float temp, Time* time1);

//! Stores the specified container at a given temperature and given duration of time. Combination of set_temp() and wait().
/*!
\param container1 the container whose contents have to be stored.
\param temp the temperature of storage.
\param time1 the duration of storage.
\param function the function being performed as a result of the store_for().
\par Example:
\code store_for(eppendorf, 80, time(30, SECS), ENZYME_INAC);\endcode
\par Output:
\htmlonly Perform enzyme inactivation by storing at <b><font color=#357EC7>80 C</font></b> for <b><font color=#357EC7>30 secs</font></b>. \endhtmlonly 
\sa store(), store_until() and incubate()
*/
extern void store_for(Container& container1, float temp, Time* time1, enum func function);

//! Stores the specified plate at a given temperature.
/*!
\param plate1 the plate that needs to be stored.
\param temp the temperature of storage.
\par Example:
\code store_plate(plate1, 37);\endcode
\par Output:
\htmlonly Store the plate at <b><font color=#357EC7>37 C</font></b>. \endhtmlonly
\sa store(), store_for(), store_until() and incubate()
*/
extern void store_plate(Plate& plate1, float temp);

//! Stores the specified container at a given temperature until the occurence of a specified event.
/*!
\param container1 the container whose contents have to be stored.
\param temp the temperature of storage.
\param type the event until whose occurence \c container1 has to be stored.
\par Example:
\code store_until(tube1, RT, ETHANOL_EVAP);\endcode
\par Output:
\htmlonly Store (tube1.contents.name) at <b><font color=#357EC7>room temperature</font></b> until the ethanol has evaporated and no fluid is visible in the tube.\endhtmlonly
\sa store(), store_for(), incubate(), and enum time_unit
*/
extern void store_until(Container& container1, float temp, enum until type);

//! Stores the specified container at a given temperature until the occurence of a specified event. The approximate time taken for the occurence of the event is also specified.
/*!
\param container1 the container whose contents have to be stored.
\param temp the temperature of storage.
\param type the event until whose occurence \c container1 has to be stored.
\param time1 the approximate time taken for the occurence of the event.
\par Example:
\code store_until(tube1, RT, ETHANOL_EVAP, time_range(10, 15, MINS));\endcode
\par Output:
\htmlonly Store (tube1.contents.name) at <b><font color=#357EC7>room temperature</font></b> until the ethanol has evaporated and no fluid is visible in the tube(~ <b><font color=#357EC7>10-15 mins</font></b>).\endhtmlonly
\sa store(), store_for() and incubate()
*/
extern void store_until(Container& container1, float temp, enum until type, Time* time1);

//! Streaks the plate with the required colony.
/*!
\param plate1 the fresh plate.
\param colony the colony that needs to be streaked.
\par Example:
\code streak_plate(lb_plate, colony); \endcode
\par Output:
\htmlonly Use a sterile toothpick or disposable loop to transfer a small segement of (colony.name) to a streak or patch on (lb_plate.name). \endhtmlonly
\sa plate_out(), patch()
*/
extern void streak_plate(Plate& plate1, Plate& colony);

//! Subtracts an actual volume from a symbolic volume.
/*!
\param vol1 the symbolic volume.
\param vol2 the actual volume.
\par Example:
\code y = subtract(s_vol(x), vol(10, UL));\endcode
\par Output:
Internally, \c y is assigned the value \c x-10.
\sa add(), multiply() and divide()
*/
extern Symbol subtract(Symbolic_vol* vol1, Volume* vol2);

//! Subtracts a symbolic volume from an actual volume.
/*!
\param vol1 the actual volume.
\param vol2 the symbolic volume.
\par Example:
\code y = subtract(vol(10, UL), s_vol(x));\endcode
\par Output:
Internally, \c y is assigned the value \c 10-x.
\sa add(), multiply() and divide()
*/
extern Symbol subtract(Volume* vol1,Symbolic_vol*  vol2);

//! Subtracts a symbolic volume from another symbolic volume.
/*!
\param vol1 the first symbolic volume.
\param vol2 the other symbolic volume.
\par Example:
\code z = subtract(s_vol(x), s_vol(y)));\endcode
\par Output:
Internally, \c z is assigned the value \c x-y.
\sa add(), multiply() and divide()
*/
extern Symbol subtract(Symbolic_vol* vol1, Symbolic_vol* vol2);

//! Mixes the contents of the given container by tapping the container for a few seconds.
/*!
\param container1 the container whose contents have to be mixed.
\par Example:
\code tap(eppendorf);\endcode
\par Output:
\htmlonly Gently tap the mixture for a few secs. \endhtmlonly
\sa stir(), invert(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void tap(Container& container1);

//! Mixes the contents of the given container by tapping until the specified result is obtained.
/*!
\param container1 the container whose contents have to be mixed.
\param event1 the event until which the container has to be mixed. (See enum until)
\par Example:
\code tap(eppendorf, PELLET_DISLODGES); \endcode
\par Output:
\htmlonly Close the tube tightly and gently tap the mixture for a few secs until the pellet dislodges. \endhtmlonly
\sa stir(), invert(), vortex(), dissolve(), resuspend(), pipet(), combine_and_mix() and enum until
*/
extern void tap (Container& container1, enum until event1);

//! Mixes the contents of the given container by tapping the container for the specified duration of time.
/*!
\param container1 the container whose contents have to be mixed.
\param time1 the duration of tapping.
\par Example:
\code tap(eppendorf, time(10, SECS));\endcode
\par Output:
\htmlonly Gently tap the mixture for <b><font color=#357EC7>10 secs</font></b>. \endhtmlonly
\sa invert(), vortex(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void tap(Container& container1, Time* time1);

//! Programs the thermocycler with the appropriate values to carry out a specific type of PCR.
/*!
\param plate1 the plate that contains the sample for thermocycling.
\param type the type of PCR to be performed.
\par Example:
\code thermocycler(lb_plate, COLONY);\endcode
\par Output:
\htmlonly Carry out colony PCR on (lb_plate.name). \endhtmlonly
\sa enum pcr_type 
*/
extern void thermocycler (Plate& plate1, enum pcr_type type);

//! Programs the thermocycler with the appropriate values for temperature 1, time 1 and sends the contents of the specified container for thermocycling.
/*!
\param container1 the container that contains the sample for PCR.
\param temp1 temperature 1.
\param time1 time 1.
\par Example:
\code thermocycler(rxn_tube, 80, time(20, SECS));\endcode
\par Output:
\htmlonly Set the thermocycler to run the following program:<br><ul><li><b><font color=#357EC7>80 C</font></b>, <b><font color=#357EC7>20 secs</font></b></li></ul> \endhtmlonly
\sa pcr_init_denat(), pcr_final_ext() and thermocycler_anneal()
*/
extern void thermocycler(Container& container1, float temp1, Time* time1);

//! Programs the thermocycler with the appropriate values for temperature 1, time 1, temperature 2, time 2 and sends the contents of the specified container for thermocycling.
/*!
\param master_mix the container that contains the sample for thermocycling.
\param cycles the number of cycles required
\param temp1 temperature 1.
\param time1 time 1.
\param temp2 temperature 2.
\param time2 time 2.
\par Example:
\code thermocycler(rxn_tube, 30, 94, time(30, SECS), 65, time(1, MINS));\endcode
\par Output:
\htmlonly Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>30</font></b></li><li>Denature: <b><font color=#357EC7>94 C</font></b>, <b><font color=#357EC7>30 secs</font></b></li> <li> Anneal: <b><font color=#357EC7>65 C</font></b>, <b><font color=#357EC7>1 min</font></b> </li></ul> \endhtmlonly
\sa pcr_init_denat(), pcr_final_ext and thermocycler_anneal()
*/
extern void thermocycler (Container& master_mix, int cycles, float temp1, Time* time1, float temp2, Time* time2);

//! Programs the thermocycler with the appropriate values for temperature 1, time 1, temperature 2, time 2 and sends the contents of the specified container for thermocycling.
/*!
\param master_mix the container that contains the sample for thermocycling.
\param temp1 temperature 1.
\param time1 time 1.
\param temp2 temperature 2.
\param time2 time 2.
\par Example:
\code thermocycler(rxn_tube, 94, time(30, SECS), 65, time(1, MINS));\endcode
\par Output:
\htmlonly Set the thermocycler to run the following program:<br><ul><li><b><font color=#357EC7>94 C</font></b>, <b><font color=#357EC7>30 secs</font></b></li><li> <b><font color=#357EC7>65 C</font></b>, <b><font color=#357EC7>1 min</font></b> </li></ul> \endhtmlonly
\sa pcr_init_denat(), pcr_final_ext() and thermocycler_anneal()
*/
extern void thermocycler (Container& master_mix, float temp1, Time* time1, float temp2, Time* time2);

//! Programs the thermocycler with the appropriate values for number of cycles, temperature of denaturation, time of denaturation, temperature of annealing, time of annealing, temperature of elongation, time of elongation and type of PCR and sends the contents of the specified container for thermocycling.
/*!
\param master_mix the container that contains the sample for thermocycling.
\param cycles the number of cycles required.
\param temp1 the temperature of denaturation.
\param time1 the duration of denaturation.
\param temp2 the temperature of annealing.
\param time2 the duration of annealing.
\param temp3 the temperature of elongation.
\param time3 the duration of elongation.
\param type the type of PCR. (See enum pcr_type)
\par Example:
\code thermocycler(rxn_tube, 94, time(30, SECS), 58, time(1, MINS), 72, time(45, SECS), NORMAL);\endcode
\par Output:
\htmlonly Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>30</font></b></li><li>Denature: <b><font color=#357EC7>94 C</font></b>, <b><font color=#357EC7>30 secs</font></b></li><li>Anneal: <b><font color=#357EC7>58 C</font></b>, <b><font color=#357EC7>1 min</font></b> </li><li>Elongate: <b><font color=#357EC7>72 C</font></b>, <b><font color=#357EC7>45 secs</font></b> </li></ul> \endhtmlonly
\sa pcr_init_denat(), pcr_final_ext(), thermocycler_anneal() and enum pcr_type
*/
extern void thermocycler (Container& master_mix, int cycles, float temp1, Time* time1, float temp2, Time* time2, float temp3, Time* time3, enum pcr_type type);

//! Programs the thermocycler with the appropriate values for number of cycles, temperature of denaturation, time of denaturation, temperature of annealing, time of annealing, temperature of elongation, time of elongation, type of PCR and gradient temperature (for gradient PCR) and sends the contents of the specified container for thermocycling.
/*!
\param master_mix the container that contains the sample for thermocycling.
\param cycles the number of cycles required.
\param temp1 the temperature of denaturation.
\param time1 the duration of denaturation.
\param temp2 the temperature of annealing.
\param time2 the duration of annealing.
\param temp3 the temperature of elongation.
\param time3 the duration of elongation.
\param type the type of PCR. (See enum pcr_type)
\param grad_temp the maximum temperature of annealing for performing gradient PCR.
\par Example:
\code thermocycler(rxn_tube, 94, time(30, SECS), 58, time(1, MINS), 72, time(45, SECS), GRADIENT, 65);\endcode
\par Output:
\htmlonly Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>30</font></b></li><li>Denature: <b><font color=#357EC7>94 C</font></b>, <b><font color=#357EC7>30 secs</font></b></li><li>Anneal: <b><font color=#357EC7>58 - 65 C (gradient PCR)</font></b>, <b><font color=#357EC7>1 min</font></b> </li><li>Elongate: <b><font color=#357EC7>72 C</font></b>, <b><font color=#357EC7>45 secs</font></b> </li></ul> \endhtmlonly
\sa pcr_init_denat(), pcr_final_ext(), thermocycler_anneal() and enum pcr_type
*/
extern void thermocycler (Container& master_mix, int cycles, float temp1, Time* time1, float temp2, Time* time2, float temp3, Time* time3, enum pcr_type type, float grad_temp);

//! Programs the thermocycler with the appropriate values for annealing the primers with the template according to the specified gradient and sends the contents of the specified container for thermocycling.
/*!
\param container1 the container that contains the sample for thermocycling.
\param cool_to_temp the temperature of annealing.
\param gradient the gradient according to whichthe products are cooled.
\param time1 the duration of annealing.
\par Example;
\code thermocycler_anneal(tube1, 45, 2, time(1, MINS));\endcode
\par Output:
\htmlonly Cool (tube1.contents.name) to <b><font color=#357EC7>45 C</font></b> at the rate of <b><font color=#357EC7>2 C</font></b> every <b><font color=#357EC7>1 min</font></b>. \endhtmlonly
\sa pcr_init_denat(), pcr_final_ext() and thermocycler()
*/
extern void thermocycler_anneal(Container& container1, float cool_to_temp, float gradient, Time* time1);

//! Specifies the time between the completion of one operation on a sample and the commencement of the next.
/*!
\param container1 the container whose contents are subject to the time constraint.
\param time1 the duration of time constraint.
\param type the type of time constraint. (See enum time_constraint_type)
\par Example:
\code time_constraint(tube1, time(0, SECS), NEXTSTEP); \endcode
\par Output:
\htmlonly <font color=red>NOTE: Proceed to the next step immediately!</font> \endhtmlonly
\sa use_or_store() and enum time_constraint_type
*/
extern void time_constraint(Container& container1, Time* time1, enum time_constraint_type type );

//! Marks the steps in the protocol that need further clarification for coding with BioCoder.
/*!
\par Example:
\code to_do("Add the samples to appropriate wells."); \endcode
\par Output:
\htmlonly <font color=red><i>Add the samples to appropriate wells.</i></font> \endhtmlonly
\sa comment()
*/
extern void to_do(char* clarify);

//! Transfects the cells contained in \c container1 with DNA.
/*!
\param container1 the container with the cells to be transfected.
\param medium the medium for transfection.
\param dna the fluid with the DNA that will transfect the cells in \c container1.
\par Example:
\code transfection(flask, growth_medium, dna); \endcode
\par Output:
\htmlonly Transfect (flask.contents.name) with (dna.name). \endhtmlonly
*/
extern void transfection(Container& container1, Fluid& medium, Fluid& dna);

//! Transfers the contents of a container \c container1 to another specified container \c con2.
/*!
\param container1 the container whose contents have to be transferred.
\param con2 the container to which \c container1 's contents have to be transferred.
\par Example:
\code transfer(tube1, tube2);\endcode
\par Output:
\htmlonly Transfer (tube1.contents.name) into (tube2.name). \endhtmlonly
\sa combine_and_mix()
*/
extern void transfer(Container& container1, Container& con2);

//! Transfers the column to a container \c container1.
/*!
\param column1 the column that needs to be transferred.
\param container1 the container to which \c column1 has to be transferred.
\par Example:
\code transfer_column(spin_col, fresh_eppendorf); \endcode
\par Output:
\htmlonly Transfer (spin_col.name) into (fresh_eppendorf.name). \endhtmlonly
\sa combine_and_mix()
*/
extern void transfer_column(Column& column1, Container& container1);

//! The contents of the specified container are either used within the duration \c time after setting the temp to be \c temp_use or if used later than \c time, the container is stored at \c temp_storage.
/*!
\param container1 the container whose contents are to be used or stored.
\param time1 the time within which contents of \c container1 are to be used.
\param temp_use the temperature of usage of contents of \c container1.
\param temp_store the temperature of storage of contents of \c container1.
\par Example:
\code use_or_store(tube1, time(10, MINS), 4, -20); \endcode
\par Output:
\htmlonly NOTE: If (tube1.contents.name) is/are used within <b><font color=#357EC7>10 mins</font></b>, set temperature to <b><font color=#357EC7>4 C</font></b> and use, else store it at <b><font color=#357EC7>-20 C</font></b>. \endhtmlonly
\sa time_constraint()
*/
extern void use_or_store(Container& container1, Time* time1, float temp_use, float temp_store);

//! Mixes the contents of the given container by vortexing the container for a few seconds.
/*!
\param container1 the container whose contents have to be mixed.
\par Example:
\code vortex(eppendorf);\endcode
\par Output:
\htmlonly Vortex the mixture for a few seconds. \endhtmlonly
\sa tap(), stir(), invert(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void vortex(Container& container1);

//! Mixes the contents of the given container by vortexing the container for the specified duration of time.
/*!
\param container1 the container whose contents have to be mixed.
\param time1 the duration of vortexing.
\par Example:
\code vortex(eppendorf, time(1, MINS));\endcode
\par Output:
\htmlonly Vortex the mixture for <b><font color=#357EC7>1 min</font></b>. \endhtmlonly
\sa tap(), stir(), invert(), dissolve(), resuspend(), pipet() and combine_and_mix()
*/
extern void vortex(Container& container1, Time* time1);

//! Vortexes the given column for a few seconds.
/*!
\param column1 the column that has to be vortexed.
\par Example:
\code vortex_column(spin_col); \endcode
\par Output:
\htmlonly Vortex (spin_col.name) gently. \endhtmlonly
\sa vortex()
*/
extern void vortex_column(Column& column1);

//! Holds the given container for the specified unit of time.
/*!
\param container1 the container whose contents have to be held.
\param time1 the duration for which \c container1 has to be held.
\par Example:
\code wait(tube1, time(5, MINS)); \endcode
\par Output:
\htmlonly Keep (tube1.contents.name) aside for <b><font color=#357EC7>5 mins</font></b>. \endhtmlonly
\sa store(), store_for(), store_until(), incubate()
*/
extern void wait (Container& container1, Time* time1);

//! Washes the cells in the given container with the specified fluid.
/*!
\param container1 the container whose contents(cells) have to be washed.
\param fluid1 the fluid used to wash the cells in \c container1.
\par Example:
\code wash_cells(cult_chamber, medium); \endcode
\par Output:
\htmlonly Wash cells with (medium.name). \endhtmlonly
*/
extern void wash_cells(Container& container1, Fluid& fluid1);

//! Washes the slide with the specified fluid.
/*!
\param slide1 the slide that has to be washed.
\param sample1 the fluid used to wash \c slide1.
\param type the type of washing. (See enum wash_type)
\par Example:
\code wash_slide(slides, ethanol, WASHING); \endcode
\par Output:
\htmlonly Wash (slides.name) in <font color=#357EC7>(ethanol.name)</font>. \endhtmlonly
\sa remove_slide() and enum wash_type
*/
extern void wash_slide(Slide& slide1, Fluid& sample1, enum wash_type type);

//! Washes the slide with the specified fluid for the specified duration and number of times.
/*!
\param slide1 the slide that has to be washed.
\param sample1 the fluid used to wash \c slide1.
\param type the type of washing. (See enum wash_type)
\param time1 the duration of washing.
\param no_of_times the number of times that the slide has to be washed.
\par Example:
\code wash_slide(slides, ethanol, RINSING, time(30, SECS), 2); \endcode
\par Output:
\htmlonly Rinse (slides.name) in <font color=#357EC7>(ethanol.name)</font>, <b><font color=#357EC7>twice</font></b> for <b><font color=#357EC7>30 secs</font></b> each. \endhtmlonly
\sa enum wash_type 
*/
extern void wash_slide(Slide& slide1, Fluid& sample1, enum wash_type type, Time* time1, int no_of_times);

//! Washes the tissue with the specified fluid.
/*!
\param tissue1 the tissue that has to be washed.
\param sample1 the fluid used to wash \c slide1.
\param type the type of washing. (See enum wash_type)
\param temp the temperature of washing.
\param shaking indicates if shaking during wash is required.(1 if required, 0 if not required.)
\par Example:
\code wash_tissue(plant_tissue, fluida, RINSING, RT, 1); \endcode
\par Output:
\htmlonly Rinse (plant_tissue.name) in <font color=#357EC7>(fluida.name)</font> at <b><font color=#357EC7>room temperature</font></b> with shaking.\endhtmlonly
\sa enum wash_type
*/
extern void wash_tissue(Tissue& tissue1, Fluid& sample1, enum wash_type type, float temp, int shaking);

//! Washes the tissue with the specified fluid for the specified duration and number of times.
/*!
\param tissue1 the tissue that has to be washed.
\param sample1 the fluid used to wash \c slide1.
\param type the type of washing. (See enum wash_type)
\param time1 the duration of washing.
\param no_of_times the number of times that the slide has to be washed.
\param temp the temperature of washing.
\param shaking indicates if shaking during wash is required.(1 if required, 0 if not required.)
\par Example:
\code wash_tissue(plant_tissue, fluida, WASHING, time(45, SECS), 3, RT, 0); \endcode
\par Output:
\htmlonly Wash (plant_tissue.name) in <font color=#357EC7>(fluida.name)</font>, <b><font color=#357EC7>thrice</font></b> each for <b><font color=#357EC7>45 secs</font></b> at <b><font color=#357EC7>room temperature</font></b> without shaking.\endhtmlonly
\sa enum wash_type 
*/
extern void wash_tissue(Tissue& tissue1, Fluid& sample1, enum wash_type type, Time* time1, int no_of_times, float temp, int shaking);

//! Prompts the user to weigh the amount of solid present in the given container.
/*!
\param container1 the container with the solid whose weight needs to be determined.
\par Example:
\code weigh(eppendorf);\endcode
\par Output:
\htmlonly Weigh the amount of (eppendorf.contents.name) present. \endhtmlonly
*/
extern void weigh(Container& container1);

#define BOILING_WATER 101
/*! \def BOILING_WATER 
* \brief For use in store(), store_for() and store_until().
*/
#define ICE_COLD 3
/*! \def ICE_COLD 
* \brief For use in declaring the state of a fluid in new_fluid().
*/
#define FOREVER -100
/*! \def FOREVER 
* \brief For use in pcr_final_ext().
*/
#define NA -1
/*! \def NA 
* \brief "Not Applicable". For use in mixing_table() and mixing_table_pcr() where "--" needs to be printed.
*/
#define ON_ICE 3
/*! \def ON_ICE 
* \brief For use in store(), store_for() and store_until().
*/
#define OVERNIGHT 12
/*! \def OVERNIGHT 
* \brief For use in incubate(), incubate_and_mix(), inoculation(), store(), store_for() and store_until().
*/
#define RT 28
/*! \def RT 
* \brief Room Temperature.
*/
#define SPEED_MAX 20000
/*! \def SPEED_MAX 
* \brief Maximum speed value to be used in centrifugation instructions.
*/
#define XVAL -2
/*! \def XVAL 
* \brief 'X'(unknown) value. For use in mixing_table() and mixing_table_pcr() where "X" needs to be printed. 
*/

/**\page page2 Instructions sorted alphabetically
* Click on the link below to view the list of functions sorted in the alphabetical order: <br>
* file:///C:/Users/t-vaisha/Desktop/BioCoder%20Documentation/globals_func.html
*/

/**\page example Example protocol
 * The protocol from <em>A Molecular Cloning Handbook</em> by Sambrook and Russel, <b>Preparation of Plasmid DNA by Alkaline Lysis with SDS - Minipreparation</b> was coded up in BioCoder. Following are
 * the source code and the corresponding auto-generated text output for the protocol.<br>
 *-\subpage source_code<br>
 *-\subpage text_output<br>
 *\page source_code Preparation of Plasmid DNA by Alkaline Lysis with SDS - Minipreparation : Souce code 
 * The auto-generated text output of this protocol can be found \ref text_output "here".
 * \verbinclude Miniprep.cpp
 *-\page text_output Preparation of Plasmid DNA by Alkaline Lysis with SDS - Minipreparation : Auto-generated text output
 *Click on the link below to view the text-output of the 'Preparation of Plasmid DNA by Alkaline Lysis with SDS - Minipreparation' \ref source_code "source code".<br>
 * file:///C:/Users/t-vaisha/Desktop/BioCoder%20Documentation/Preparation%20of%20Plasmid%20DNA%20by%20Alkaline%20Lysis%20with%20SDS-%20Minipreparation.htm
 */

/*!\page oww BioCoder on OpenWetWare
 * A number of protocols on OpenWetWare (www.openwetware.org) have been coded up in BioCoder. Here is an example protocol:<br>
 * http://openwetware.org/wiki/Transforming_chemically_competent_cells#Text_Output
 * \sa http://www.google.co.in/search?hl=en&client=firefox-a&rls=org.mozilla%3Aen-GB%3Aofficial&hs=XE7&as_q=&as_epq=&as_oq=BioCoder+BioStream&as_eq=&num=10&lr=&as_filetype=&ft=i&as_sitesearch=openwetware.org&as_qdr=all&as_rights=&as_occt=any&cr=&as_nlo=&as_nhi=&safe=images
 */

///@cond
extern float get_ttr();
///@endcond
#endif

