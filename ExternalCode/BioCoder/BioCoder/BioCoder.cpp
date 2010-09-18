#include <iostream>
#include <conio.h>
#include <cstdio>
extern "C"
	{
#include <stdarg.h>
	}
#include "BioCoder.h"

using namespace std;
#define DEFAULT 25000000

FILE* fp;

//initiallization of flags
int print_reagents = 1;
int print_parameters = 1;
int first = 0;
int options_flag = 0;
int option_no = 1;
int sub_step_flag = 0;
int prev_cont = 1;

//initiallization of number of containers, equipment, etc.
int equip_no = 0;
int microfuge_no = 1;
int centrifuge_no = 1;
int incubator_no = 1;
int electrophoresis_no = 1;
int mortar_no = 1;
int thermocycler_no = 1;
int electro_no = 1;
int shaker_no = 1;
int spectrophotometer_no = 1;
int cont_no = 0;
int container_count[24] = {0};
char* containers[20];
char* equipments[20];
char* filename;

//variables required for keeping track of usage of fluids and containers
Fluid usage_list_fluids[200];
Container usage_list_containers[100];
int list_fluid_no = 0;
int list_container_no = 0;
char* prev_container;

//variable required for keeping track of time required for completion of the protocol
int total_time_required = 0;

//operation typedef-ed to Fluid
typedef Fluid Operation;
Fluid new_operation(char* name) {
	Operation result = {0};
	result.new_name = name;
	result.original_name = name;
	result.type = OPERATION;
	return result;
	}

//class Time
Time::Time(float x, enum time_unit unit){value = x; unit_choice = unit;}

void Time::display_time()
	{
	float max_time_value = 0;
	switch(unit_choice)
		{
		case SECS:if (value == 1) unit = "sec"; else unit = "secs";mul = 1;break;
		case MINS:if (value == 1) unit = "min"; else unit = "mins";mul = 60;break;
		case HRS:if (value == 1) unit = "hr"; else unit = "hrs";mul = 3600;break;
		default:break;
		}
	if ((value == OVERNIGHT)&&(unit == "hrs"))
		fprintf (fp, "<b><font color=#357EC7>12 hrs</font></b>(overnight)");
	else if (value == 0)
		fprintf(fp, "<b>immediately</b>");
	else if (value == FOREVER)
		fprintf(fp, "<b><font color=#357EC7>forever</font></b>(until taken out from the thermocycler)");
	else if (value == XVAL)
		fprintf(fp, "<b><font color=#357EC7>X</font></b>");
	else 
		fprintf (fp, "<b><font color=#357EC7>%g %s</font></b>", value, unit);
	if((option_no == 2) && (options_flag == 1))
		max_time_value = value * mul;
	else if(option_no > 2)
		{
		if(max_time_value > value*mul)
			total_time_required = total_time_required + max_time_value;
		else 
			{
			total_time_required = total_time_required + value * mul;
			max_time_value = value*mul;
			}
		}
	else 
		total_time_required = total_time_required + value * mul;
	};

Minimum_time::Minimum_time (float x, enum time_unit unit){value = x; unit_choice = unit;}
void Minimum_time::display_time()
	{
	float max_time_value = 0;
	switch(unit_choice)
		{
		case SECS:if (value == 1) unit = "sec"; else unit = "secs";mul = 1;break;
		case MINS:if (value == 1) unit = "min"; else unit = "mins";mul = 60;break;
		case HRS:if (value == 1) unit = "hr"; else unit = "hrs";mul = 3600;break;
		default:break;
		}
	fprintf(fp, "at least <b><font color=#357EC7>%g %s</font></b>", value, unit);
	if((option_no == 2) && (options_flag == 1))
		max_time_value = value * mul;
	else if(option_no > 2)
		{
		if(max_time_value > value*mul)
			total_time_required = total_time_required + max_time_value;
		else 
			{
			total_time_required = total_time_required + value * mul;
			max_time_value = value*mul;
			}
		}
	else 
		total_time_required = total_time_required + value * mul;
	};

Maximum_time::Maximum_time(float x, enum time_unit unit){value = x; unit_choice = unit;}
void Maximum_time::display_time()
	{	
	float max_time_value = 0;
	switch(unit_choice)
		{
		case SECS:if (value == 1) unit = "sec"; else unit = "secs";mul = 1;break;
		case MINS:if (value == 1) unit = "min"; else unit = "mins";mul = 60;break;
		case HRS:if (value == 1) unit = "hr"; else unit = "hrs";mul = 3600;break;
		default:break;
		}
	fprintf(fp, "at most <b><font color=#357EC7>%g %s</font></b>", value, unit);
	if((option_no == 2) && (options_flag == 1))
		max_time_value = value * mul;
	else if(option_no > 2)
		{
		if(max_time_value > value*mul)
			total_time_required = total_time_required + max_time_value;
		else 
			{
			total_time_required = total_time_required + value * mul;
			max_time_value = value*mul;
			}
		}
	else 
		total_time_required = total_time_required + value * mul;
	};

Approx_time::Approx_time(float x, enum time_unit unit){value = x; unit_choice = unit;}
void Approx_time::display_time()
	{	
	float max_time_value = 0;
	switch(unit_choice)
		{
		case SECS:if (value == 1) unit = "sec"; else unit = "secs";mul = 1;break;
		case MINS:if (value == 1) unit = "min"; else unit = "mins";mul = 60;break;
		case HRS:if (value == 1) unit = "hr"; else unit = "hrs";mul = 3600;break;
		default:break;
		}
	fprintf(fp, "~<b><font color=#357EC7>%g %s</font></b>", value, unit);
	if((option_no == 2) && (options_flag == 1))
		max_time_value = value * mul;
	else if(option_no > 2)
		{
		if(max_time_value > value*mul)
			total_time_required = total_time_required + max_time_value;
		else 
			{
			total_time_required = total_time_required + value * mul;
			max_time_value = value*mul;
			}
		}
	else 
		total_time_required = total_time_required + value * mul;
	};

Time_range::Time_range(float x, float y, enum time_unit unit){value1 = x; value = y; unit_choice = unit;}
void Time_range::display_time ()
	{
	float max_time_value = 0;
	switch(unit_choice)
		{
		case SECS:if (value1 == 1) unit = "sec"; else unit = "secs";mul = 1;break;
		case MINS:if (value1 == 1) unit = "min"; else unit = "mins";mul = 60;break;
		case HRS:if (value1 == 1) unit = "hr"; else unit = "hrs";mul = 3600;break;
		default:break;
		}
	if ((value1 == 12) && (unit == "hrs"))
		fprintf (fp, "<b><font color=#357EC7>%g(overnight) - %g %s</font></b>", value1, value, unit);
	else if ((value == 12) && (unit == "hrs"))
		fprintf (fp, "<b><font color=#357EC7>%g - %g %s(overnight)</font></b>", value1, value, unit);
	else
		fprintf (fp, "<b><font color=#357EC7>%g - %g %s</font></b>", value1, value, unit);
	if((option_no == 2) && (options_flag == 1))
		max_time_value = value * mul;
	else if(option_no > 2)
		{
		if(max_time_value > value*mul)
			total_time_required = total_time_required + max_time_value;
		else 
			{
			total_time_required = total_time_required + value * mul;
			max_time_value = value*mul;
			}
		}
	else 
		total_time_required = total_time_required + value * mul;
	}; 

Symbolic_time::Symbolic_time(Symbol_t& s) {s1.symbol = s.symbol; s1.unit = s.unit; s1.value = s.value; value = s1.value;}
void Symbolic_time:: display_time()
	{
	Time* time1;
	if (s1.value == DEFAULT)
		fprintf(fp,"<b><font color=#357EC7>%s secs</font></b>", s1.symbol);	
	else 
		{
		time1 = time(s1.value, s1.unit);
		time1->display_time();
		}
	};

//class Speed
Speed::Speed(float x, enum speed_type unit){value = x; unit_choice = unit;}
void Speed::display_speed()
	{	
	switch(unit_choice)
		{
		case RPM: unit = "rpm";break;
		case G: unit = "Xg";break;
		default:break;
		}
	if (value == SPEED_MAX)
		fprintf (fp, "<font color=#357EC7>maximum speed</font>");
	else
		fprintf (fp, "a speed of <font color=#357EC7>%g %s</font>", value, unit);
	};

Minimum_speed::Minimum_speed (float x, enum speed_type unit){value = x; unit_choice = unit;}
void Minimum_speed::display_speed()
	{
	switch(unit_choice)
		{
		case RPM: unit = "rpm";break;
		case G: unit = "Xg";break;
		default:break;
		}
	fprintf(fp, "a speed of at least <font color=#357EC7>%g %s</font>", value, unit);
	};

Maximum_speed::Maximum_speed(float x, enum speed_type unit){value = x; unit_choice = unit;}
void Maximum_speed::display_speed()
	{
	switch(unit_choice)
		{
		case RPM: unit = "rpm";break;
		case G: unit = "Xg";break;
		default:break;
		}
	fprintf(fp, "a speed of at most <font color=#357EC7>%g %s</font>", value, unit); 
	};
Approx_speed::Approx_speed(float x, enum speed_type unit){value = x; unit_choice = unit;}
void Approx_speed::display_speed()
	{
	switch(unit_choice)
		{
		case RPM: unit = "rpm";break;
		case G: unit = "Xg";break;
		default:break;
		}
	fprintf(fp, "a speed of ~<font color=#357EC7>%g %s</font>", value, unit); 
	};

Speed_range::Speed_range(float x, float y, enum speed_type unit){value1 = x; value = y; unit_choice = unit;}
void Speed_range::display_speed ()
	{
	switch(unit_choice)
		{
		case RPM: unit = "rpm";break;
		case G: unit = "Xg";break;
		default:break;
		}
	fprintf (fp, "a speed of <font color=#357EC7>%g - %g %s</font>", value1, value, unit);
	}; 

//class Volume
Volume::Volume(float x, enum vol_unit unit1)
	{
	value = x; 
	unit_choice = unit1;
	switch(unit_choice)
		{
		case UL: unit = "µl";mul = MICRO;break;
		case ML: unit = "ml";mul = MILLI;break;
		case L: unit = "l";mul = LITRE;break;
		default : unit = "µl";mul = MICRO;break;
		}
	}
void Volume::display_vol() 
	{
	fprintf(fp, "<b><font color=#357EC7>%g %s</font></b>", value, unit);
	};

Symbolic_vol::Symbolic_vol(Symbol& s) {s1.symbol = s.symbol; s1.unit = s.unit; s1.value = s.value; value = s1.value;}

void Symbolic_vol:: display_vol()
	{
	Volume* vol1;
	if (s1.value == DEFAULT)
		fprintf(fp,"<b><font color=#357EC7>%s µl</font></b>", s1.symbol);	
	else 
		{
		vol1 = vol(s1.value, s1.unit);
		vol1->display_vol();
		}
	};

Volume_range::Volume_range(float x, float y, enum vol_unit unit1)
	{
	value1 = x;
	value = y;
	unit_choice = unit1;
	switch(unit_choice){
	case UL: unit = "µl";mul = MICRO;break;
	case ML: unit = "ml";mul = MILLI;break;  
	case L: unit = "l";mul = LITRE;break;
	default : unit = "µl";mul = MICRO;break;
		}
	}
void Volume_range:: display_vol()
	{
	fprintf(fp, "<b><font color=#357EC7>%g - %g %s</font></b>", value1, value, unit);
	};
Approx_volume::Approx_volume(float x, enum vol_unit unit1)
	{
	value = x; 
	unit_choice = unit1;
	switch(unit_choice){
	case UL: unit = "µl";mul = MICRO;break;
	case ML: unit = "ml";mul = MILLI;break;
	case L: unit = "l";mul = LITRE;break;
	default : unit = "µl";mul = MICRO;break;
		}
	}
void Approx_volume::display_vol() 
	{
	fprintf(fp, "<b><font color=#357EC7>~%g %s</font></b>", value, unit);
	};

//functions used to call time, speed and volume
Time* time (float x, enum time_unit unit)
	{
	Time *temp = new Time(x, unit);
	return temp;
	}
Time_range* time_range (float x, float y, enum time_unit unit)
	{
	Time_range *temp = new Time_range(x, y, unit);
	return temp;
	}

Minimum_time* min_time(float x, enum time_unit unit)
	{
	Minimum_time *temp = new Minimum_time(x, unit);
	return temp;
	}
Maximum_time* max_time(float x, enum time_unit unit)
	{
	Maximum_time *temp = new Maximum_time(x, unit);
	return temp;
	}
Approx_time* approx_time(float x, enum time_unit unit)
	{
	Approx_time *temp = new Approx_time(x, unit);
	return temp;
	}

Speed* speed (float x, enum speed_type unit)
	{
	Speed *temp = new Speed(x, unit);
	return temp;
	}
Speed_range* speed_range(float x, float y, enum speed_type unit)
	{
	Speed_range *temp = new Speed_range(x, y, unit);
	return temp;
	}

Minimum_speed* min_speed(float x, enum speed_type unit)
	{
	Minimum_speed *temp = new Minimum_speed(x, unit);
	return temp;
	}
Maximum_speed* max_speed(float x, enum speed_type unit)
	{
	Maximum_speed *temp = new Maximum_speed(x, unit);
	return temp;
	}
Approx_speed* approx_speed(float x, enum speed_type unit)
	{
	Approx_speed* temp = new Approx_speed(x, unit);
	return temp;
	}

Volume* vol(float x, enum vol_unit unit)
	{
	Volume* result = new Volume(x, unit);
	return result;
	}

Symbolic_vol* s_vol(Symbol s)
	{
	Symbolic_vol* result = new Symbolic_vol(s);
	return result;
	}

Symbolic_time* s_time(Symbol_t s)
	{
	Symbolic_time* result = new Symbolic_time(s);
	return result;
	}

Volume_range* vol_range(float x, float y, enum vol_unit unit)
	{
	Volume_range* result = new Volume_range(x, y, unit);
	return result;
	}
Approx_volume* approx_vol(float x, enum vol_unit unit)
	{
	Approx_volume* result = new Approx_volume(x, unit);
	return result;
	}

void start_protocol(char* name)
	{
	filename = name;
	char* temp1 = "";
	temp1 = (char *)calloc(strlen(name)+ 10, sizeof(char));
	temp1 = strcat(temp1, name);
	temp1 = strcat(temp1, ".htm");
	fp = fopen(temp1, "w");
	fprintf(fp, "<h1 style=\"font-size = 25px;\">%s</h1>", name);
	}

void first_step()
	{
	first = 1;
	fprintf(fp, "</ul><h2>Steps:</h2><ol>");
	fprintf(fp, "<p><li>");
	}

void first_step(char* step_name)
	{
	first = 1;
	fprintf(fp, "</ul><h2>Steps:</h2><ol>");
	fprintf(fp, "<p><li><b><font size=3>%s</font></b><br>", step_name);
	}

void next_step(char* step_name)
	{
	first = 0;
	if (sub_step_flag == 1)
		{
		fprintf(fp,"</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp, "</li></p><p><li><b><font size=3>%s</font></b><br>", step_name);
	}

void next_step()
	{
	first = 0;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp, "</li></p><p><li>");
	}

void optional_step()
	{
	first = 0;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp, "</li></p><p><b><font size=3>(Optional)</font></b><br>");
	}

void optional_step(char* name)
	{
	first = 0;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp, "</li></p><p><b><font size=3>%s (Optional)</font></b><br>", name);
	}

void parallel_step()
	{
	first = 0;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp, "</li></p><p><li><b>Meanwhile:</b><br>");
	}

void first_sub_step()
	{
	first = 0;
	fprintf(fp, "<ol type=\"a\"><p><li>");
	sub_step_flag = 1;
	}

void next_sub_step()
	{
	fprintf(fp, "</li></p><p><li>");
	sub_step_flag = 1;
	}

void repeat(int step_no)
	{
	fprintf(fp, "Repeat Step %d. <br>", step_no);
	}

void first_option()
	{
	options_flag = 1;
	fprintf(fp,"<p><b>Option %d: </b>", option_no);
	option_no++;
	}

void first_option(char* name)
	{
	options_flag = 1;
	fprintf(fp,"<p><b>Option %d: %s</b>", option_no, name);
	option_no++;
	}

void next_option()
	{
	options_flag = 1;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp,"(or)<br><b>Option %d: </b>",option_no);
	option_no++;
	}

void next_option(char* name)
	{
	options_flag = 1;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp,"(or)<p><b>Option %d: %s</b>",option_no, name);
	option_no++;
	}

void end_option()
	{
	options_flag = 0;
	if (sub_step_flag == 1)
		{
		fprintf(fp, "</li></p></ol>");
		sub_step_flag = 0;
		}
	fprintf(fp,"</p><p>");
	option_no=1;
	}

void display_equip()
	{
	int i;
	fprintf(fp, "<div style=\"top: 25px; margin-top: 50px; margin-left: 700px; position: absolute; z-index: 1; visibility: show;\">");
	fprintf(fp, "<h2>Equipment:</h2><ul type=\"circle\">");
	for(i=0; i<equip_no; i++)
		fprintf(fp, "<li>%s</li>", equipments[i]);
	for(i=0; i<cont_no; i++)
		fprintf(fp, "<li>%s</li>", containers[i]);
	fprintf(fp, "</ul></div>");
	}

void usage_details()
	{
	int i;
	int count1 = 0;
	int count2 = 0;
	for(i=0;i<list_fluid_no;i++)
		{
		if (usage_list_fluids[i].used == 1)
			continue;
		if (count1 == 0)
			{
			fprintf(fp, "<font color = red>Warning: The following reagents/materials were declared but not used in the protocol:</font><ul>");
			fprintf(fp, "<li>%s</li>", usage_list_fluids[i].original_name);
			count1++;
			}
		else
			fprintf(fp, "<li>%s</li>", usage_list_fluids[i].original_name);
		}
	fprintf(fp, "</ul>");
	for(i=0;i<list_container_no;i++)
		{
		if (usage_list_containers[i].used == 1)
			continue;
		if (count2 == 0)
			{
			fprintf(fp, "<font color = red>Warning: The following equipment were declared but not used in the protocol:</font><ul>");
			fprintf(fp, "<li>%s</li>", usage_list_containers[i].name);
			count1++;
			}
		else
			fprintf(fp, "<li>%s</li>", usage_list_containers[i].name);
		}
	fprintf(fp, "</ul>");
	}

void timing_details()
	{
	int time_hrs = total_time_required/3600;
	int time_mins = (total_time_required%3600)/60;
	if (time_hrs == 0)
		fprintf(fp, "<p><b>TOTAL TIME REQUIRED FOR THE COMPLETION OF THE PROTOCOL :<font color=#357EC7>~ %d mins</font></b></p>", time_mins);
	else if (time_hrs == 1)
		fprintf(fp, "<p><b>TOTAL TIME REQUIRED FOR THE COMPLETION OF THE PROTOCOL :<font color=#357EC7>~ %d hr, %d mins</font></b></p>", time_hrs, time_mins);
	else
		fprintf(fp, "<p><b>TOTAL TIME REQUIRED FOR THE COMPLETION OF THE PROTOCOL :<font color=#357EC7>~ %d hrs, %d mins</font></b></p>", time_hrs, time_mins);
	total_time_required = 0;
	}

void end_protocol()
	{
	int i;
	export_graph(filename);
	fprintf(fp, "</li></p></ol>");
	display_equip();
	usage_details();
	//timing_details();
	print_reagents = 1;
	print_parameters = 1;
	option_no = 1;
	equip_no = 0;
	microfuge_no = 1;
	centrifuge_no = 1;
	incubator_no = 1;
	electrophoresis_no = 1;
	mortar_no = 1;
	thermocycler_no = 1;
	electro_no = 1;
	shaker_no = 1;
	spectrophotometer_no = 1;
	cont_no = 0;
	prev_cont = 1;
	list_fluid_no = 0;
	total_time_required = 0;
	list_container_no = 0;
	fclose(fp);
	}

void check_container(Container& container1)
	{
	if ((first == 1) || (prev_cont == 1)||(prev_container != container1.name))
		{
		prev_container = container1.name;
		fprintf(fp, "%s", container1.name);
		}
	}

void to_do(char* clarify)
	{
	fprintf(fp, "<font color = \"red\"><i>%s</i></font><br>", clarify);
	}

void comment(char* comm)
	{
	fprintf(fp, "<font color = \"#800517\"><i>%s</i></font><br>", comm);
	}

Fluid new_fluid(char *name)
	{
	Fluid result = {0};
	result.state = "";
	result.original_name = name;
	result.new_name = name;
	result.type = FLUID;
	result.volume = DEFAULT;
	result.unit = L;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	fprintf(fp, "<li>%s</li>", name);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid(char*name, float temp)
	{	
	char* t1="";
	Fluid result = {0};
	result.original_name = name;
	result.new_name = name;
	result.state="";
	result.type = FLUID;
	result.volume = DEFAULT;
	result.unit = L;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	if(temp == ICE_COLD)
		{
		fprintf(fp, "<li>ice-cold %s</li>", name, temp);
		t1 = (char *)calloc(strlen(t1) + strlen(name) + 10,	sizeof(char));
		strcat(t1, "ice-cold ");
		strcat(t1, name);
		result.new_name = t1;
		}
	else if(temp == RT)
		fprintf(fp, "<li>%s stored at room temperature</li>", name);
	else
		fprintf(fp, "<li>%s stored at %g%cC</li>", name, temp, 0x00B0);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid(char*name, char* state)
	{
	char* t1 = "";
	Fluid result = {0};
	result.original_name = name;
	result.new_name = name;
	result.state= state;
	result.type = FLUID;
	result.volume = DEFAULT;
	result.unit = L;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	fprintf(fp, "<li> <a name=\"%s\">%s <i><br><tab><div style=\"margin-right: 600px;\">(%s)</div></i></a></li>", name, name, state);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid (char* name, char* state, float temp)
	{
	char* t1="";
	Fluid result = {0};
	result.original_name = name;
	result.new_name = name;
	result.state= state;
	result.type = FLUID;
	result.volume = DEFAULT;
	result.unit = L;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	if(temp == ICE_COLD)
		{
		t1 = (char *)calloc(strlen(t1) + strlen(name) + 10, sizeof(char));
		strcat(t1, "ice-cold ");
		strcat(t1, name);
		result.new_name = t1;
		fprintf(fp, "<li> <a name=\"%s\"> %s <i><br><tab><div style=\"margin-right: 600px;\">(%s)</div></i></a></li>", result.new_name, result.new_name, state);
		}
	else if(temp >= 35)
		fprintf(fp, "<li> <a name=\"%s\">%s preheated in a water bath set at <b><font color=#357EC7>%g%cC</font></b> <i>%s</i></a></li>", name, name, temp, 0x00B0, state);
	else
		fprintf(fp, "<li> <a name=\"%s\">%s <i>%s</i> </a> at <b><font color=#357EC7>%g%cC</font></b></li>", name, name, state, temp, 0x00B0);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid(char *name, Volume* volume1)
	{
	char* t1 = "";
	char* t2 = "";
	Fluid result = {0};
	result.state = "";
	result.type = FLUID;
	result.unit = vol_unit(volume1->unit_choice);
	result.volume = volume1->value * volume1->mul;
	result.original_name = name;
	result.new_name = name;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	fprintf(fp, "<li>%s</li>", name);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid(char*name, float temp, Volume* volume1)
	{	
	char* t1 = "";
	char* t2 = "";
	Fluid result = {0};
	result.state="";
	result.type = FLUID;
	result.original_name = name;
	result.new_name = name;
	result.volume = volume1->value * volume1->mul;
	result.unit = vol_unit(volume1->unit_choice);
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	if(temp == ICE_COLD)
		{
		fprintf(fp, "<li>ice-cold %s</li>", name, temp);
		t1 = (char *)calloc(strlen(t1) + strlen(name) + 10,	sizeof(char));
		strcat(t1, "ice-cold ");
		strcat(t1, name);
		result.new_name = t1;
		}
	else if(temp == RT)
		fprintf(fp, "<li>%s stored at room temperature</li>", name);
	else
		fprintf(fp, "<li>%s stored at %g%cC</li>", name, temp, 0x00B0);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid(char*name, char* state, Volume* volume1)
	{
	char* t1 = "";
	char* t2 = "";
	Fluid result = {0};
	result.state= state;
	result.original_name = name;
	result.new_name = name;
	result.type = FLUID;
	result.volume = volume1->value * volume1->mul;
	result.unit = vol_unit(volume1->unit_choice);
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	fprintf(fp, "<li> <a name=\"%s\">%s <i><br><tab><div style=\"margin-right: 600px;\">(%s)</div></i></a></li>", name, name, state);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Fluid new_fluid (char* name, char* state, float temp, Volume* volume1)
	{
	char* t1 = "";
	char* t2 = "";
	Fluid result = {0};
	result.state= state;
	result.original_name = name;
	result.new_name = name;
	result.type = FLUID;
	result.volume = volume1->value * volume1->mul;
	result.unit = vol_unit(volume1->unit_choice);
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	if(temp == ICE_COLD)
		{
		t1 = (char *)calloc(strlen(t1) + strlen(name) + 10, sizeof(char));
		strcat(t1, "ice-cold ");
		strcat(t1, name);
		result.new_name = t1;
		fprintf(fp, "<li> <a name=\"%s\"> %s <i><br><tab><div style=\"margin-right: 600px;\">(%s)</div></i></a></li>", result.new_name, result.new_name, state);
		}
	else if(temp >= 35)
		fprintf(fp, "<li> <a name=\"%s\">%s preheated in a water bath set at <b><font color=#357EC7>%g%cC</font></b> <i>%s</i></a></li>", name, name, temp, 0x00B0, state);
	else
		fprintf(fp, "<li> <a name=\"%s\">%s <i>%s</i> </a> at <b><font color=#357EC7>%g%cC</font></b></li>", name, name, state, temp, 0x00B0);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Solid new_solid(char *name)
	{
	char* t1 = "";
	Solid result = {0};
	result.state = "";
	result.original_name = name;
	result.new_name = name;
	result.type = SOLID;
	result.volume = 1000;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	fprintf(fp, "<li>%s</li>", name);
	return result;
	}

Solid new_solid(char*name, float temp)
	{	
	char* t1 = "";
	Solid result = {0};
	result.original_name = name;
	result.new_name = name;
	result.state = "";
	result.type = SOLID;
	result.volume = 1000;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	if(temp == ICE_COLD)
		{
		fprintf(fp, "<li>ice-cold %s</li>", name, temp);
		t1 = (char *)calloc(strlen(t1) + strlen(name) + 10, sizeof(char));
		strcat(t1, "ice-cold ");
		strcat(t1, name);
		result.new_name=t1;
		}
	else
		fprintf(fp, "<li>%s at %g%cC</li>", name, temp, 0x00B0);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Solid new_solid(char*name, char* state)
	{
	char* t1 = "";
	Solid result = {0};
	result.original_name = name;
	result.new_name = name;
	result.type = SOLID;
	result.volume = 1000;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	fprintf(fp, "<li> <a name=\"%s\">%s <i><br><tab>(%s)<br></i></a></li>", name, name, state);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

Solid new_solid (char* name, char*state, float temp)
	{
	char* t1 = "";
	Solid result = {0};
	result.original_name = name;
	result.new_name = name;
	result.state = state;
	result.type = SOLID;
	result.volume = 1000;
	if(print_reagents == 1)
		{
		fprintf(fp, "<h2 style=\"margin-top:50px;\">Solutions/reagents:</h2><ul type=\"circle\">");
		print_reagents++;
		}
	if(temp == ICE_COLD)
		{
		fprintf(fp, "<li> <a name=\"%s\">ice-cold %s <i><br><tab>(%s)<br></i></a></li>", name, name, state);
		t1 = (char *)calloc(strlen(t1) + strlen(name) + 10, sizeof(char));
		strcat(t1, "ice-cold ");
		strcat(t1, name);
		result.new_name = t1;
		}
	else if(temp >= 35)
		fprintf(fp, "<li> <a name=\"%s\">%s preheated in a water bath set at <b><font color=#357EC7>%g%cC</font></b> <i>%s</i></a></li>", name, name, temp, 0x00B0, state);
	else
		fprintf(fp, "<li> <a name=\"%s\">%s <i>%s</i> </a> at <b><font color=#357EC7>%g%cC</font></b></li>", name, name, state, temp, 0x00B0);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}

void store_container_names (int i, char* name)
	{
	if(container_count[i] == 0)
		{
		containers[cont_no] = name;
		cont_no++;
		}
	}

Container new_container(enum container_type cont_id)
	{
	Container result = {0};
	result.type = CONTAINER;
	result.volume = 0;
	result.contents.new_name = "";
	char* temp;
	char* temp1 = (char *)calloc(50, sizeof(char));
	char* temp2 = (char *)calloc(50, sizeof(char));
	result.id = cont_id;
	switch(cont_id){
		case STERILE_MICROFUGE_TUBE:sprintf(temp2, " (%d)", container_count[0]+1); temp1 = strcat(temp1, "sterile 1.5-ml microcentrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Sterile 1.5-ml microcentrifuge tubes"; store_container_names(0, temp); container_count[0]++;break;
		case CENTRIFUGE_TUBE_15ML:sprintf(temp2, " (%d)", container_count[1]+1); temp1 = strcat(temp1, "sterile 15-ml centrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Sterile 15-ml centrifuge tubes";store_container_names(1, temp);container_count[1]++;break;
		case FLASK:sprintf(temp2, " (%d)", container_count[2]+1); temp1 = strcat(temp1, "flask");temp1 = strcat(temp1, temp2);result.name = temp1; temp = "Flasks of appropriate volumes";store_container_names(2, temp);container_count[2]++;break;
		case CENTRIFUGE_BOTTLE:sprintf(temp2, " (%d)", container_count[3]+1); temp1 = strcat(temp1, "centrifuge bottle");temp1 = strcat(temp1, temp2);result.name = temp1; temp = "Centrifuge bottles";store_container_names(3, temp);container_count[3]++;break;
		case GRADUATED_CYLINDER:result.name = "a graduated cylinder"; temp = "Graduated cylinders";store_container_names(4, temp);container_count[4]++; break;
		case RXN_TUBE:sprintf(temp2, " (%d)", container_count[5]+1); temp1 = strcat(temp1, "reaction tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Reaction tubes"; store_container_names(5, temp);container_count[5]++; break;
		case FRESH_COLL_TUBE:sprintf(temp2, " (%d)", container_count[6]+1); temp1 = strcat(temp1, "fresh collection tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Collection tubes";store_container_names(6, temp);container_count[6]++; break;
		case LIQUID_NITROGEN:result.name = "a container with liquid nitrogen"; temp = "Container with liquid nitrogen";store_container_names(7, temp);container_count[7]++; break;
		case PLG:result.name = "a 50- ml PLG tube"; temp = "50-ml PLG tubes";store_container_names(8, temp);container_count[8]++; break;
		case OAKRIDGE:sprintf(temp2, " (%d)", container_count[9]+1); temp1 = strcat(temp1, "Oakridge tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Oakridge tubes";store_container_names(9, temp); container_count[9]++;break;
		case QIA_CARTRIDGE:result.name= "a QIAfilter cartridge"; temp = "QIAfilter cartridge";store_container_names(10, temp);container_count[10]++;break;
		case CUVETTE_ICE:result.name= "a cuvette stored on ice"; temp = "Cuvettes stored on ice";store_container_names(11, temp);container_count[11]++;break;
		case SPEC_CUVETTE:result.name= "a spectrometry cuvette"; temp = "Spectrometry cuvettes";store_container_names(12, temp); container_count[12]++;break;
		case STOCK_PLATE_96:result.name = "a 96-well stock plate"; temp = "96-well stock plates";store_container_names(13, temp);container_count[13]++;break;
		case WELL_BLOCK_96:result.name = "a 96-well block"; temp = "96-well blocks";store_container_names(14, temp);container_count[14]++;break;
		case PCR_PLATE:result.name = "a 96-well PCR plate"; temp = "96-well PCR plates";store_container_names(15, temp); container_count[15]++;break;
		case LIQUID_BLOCK:result.name = "a 96-well liquid block"; temp = "96-well liquid blocks";store_container_names(16, temp);container_count[16]++; break;
		case CELL_CULT_CHAMBER:result.name = "a cell culture chamber"; temp = "cell culture chamber";store_container_names(17, temp);container_count[17]++; break;
		case EPPENDORF:sprintf(temp2, " (%d)", container_count[18]+1); temp1 = strcat(temp1, "Eppendorf tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Eppendorf tubes";store_container_names(18, temp); container_count[18]++; break;
		case STERILE_MICROFUGE_TUBE2ML:sprintf(temp2, " (%d)", container_count[19]+1); temp1 = strcat(temp1, "sterile 2-ml microcentrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Sterile 2-ml microcentrifuge tubes"; store_container_names(19, temp); container_count[19]++;break;
		case STERILE_PCR_TUBE:sprintf(temp2, " (%d)", container_count[20]+1); temp1 = strcat(temp1, "sterile 0.6-ml microcentrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Sterile 0.6-ml tubes"; store_container_names(20, temp);container_count[20]++;break;
		case CENTRI_TUBE_50ML:sprintf(temp2, " (%d)", container_count[21]+1); temp1 = strcat(temp1, "50-ml centrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "50-ml centrifuge tubes"; store_container_names(21, temp); container_count[21]++;break;
		case CRYO_VIAL:result.name = "a screw-topped cryo vial"; temp = "screw-topped cryo vials"; store_container_names(22, temp);container_count[22]++; break;
		case SCREW_CAP_TUBE:sprintf(temp2, " (%d)", container_count[23]+1); temp1 = strcat(temp1, "screw-cap tube");temp1 = strcat(temp1, temp2);result.name = temp1;temp = "Screw-cap tubes of appropriate volume"; store_container_names(23, temp); container_count[23]++;break;
		default:break;
		}
	result.used = 0;
	usage_list_containers[list_container_no] = result;
	result.usage_index = list_container_no;
	list_container_no++;
	return result;
	}

Container new_container(enum container_type cont_id, Fluid& fluid1)
	{
	Container result = {0};
	result.type = CONTAINER;
	result.contents.new_name = "";
	char* temp1 = (char *)calloc(50, sizeof(char));
	char* temp2 = (char *)calloc(50, sizeof(char));
	result.id = cont_id;
	switch(cont_id){
		case STERILE_MICROFUGE_TUBE:sprintf(temp2, " (%d)", container_count[0]+1);temp1 = strcat(temp1, "sterile 1.5-ml microcentrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[0]++;break;
		case CENTRIFUGE_TUBE_15ML:sprintf(temp2, " (%d)", container_count[1]+1);temp1 = strcat(temp1, "sterile 15-ml centrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[1]++;break;
		case FLASK:sprintf(temp2, " (%d)", container_count[2]+1);temp1 = strcat(temp1, "flask");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[2]++;break;
		case CENTRIFUGE_BOTTLE:sprintf(temp2, " (%d)", container_count[3]+1);temp1 = strcat(temp1, "centrifuge bottle");temp1 = strcat(temp1, temp2);result.name = temp1;break;
		case GRADUATED_CYLINDER:result.name = "a graduated cylinder";break;
		case RXN_TUBE:sprintf(temp2, " (%d)", container_count[5]+1);temp1 = strcat(temp1, "reaction tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[5]++;break;
		case FRESH_COLL_TUBE:sprintf(temp2, " (%d)", container_count[6]+1);temp1 = strcat(temp1, "fresh collection tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[6]++;break;
		case LIQUID_NITROGEN:result.name = "a container with liquid nitrogen";break;
		case PLG:result.name = "a 50- ml PLG tube"; break;
		case OAKRIDGE:sprintf(temp2, " (%d)", container_count[9]+1);temp1 = strcat(temp1, "Oakridge tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[9]++;break;
		case QIA_CARTRIDGE:result.name= "a QIAfilter cartridge";break;
		case CUVETTE_ICE:result.name= "a cuvette stored on ice";break;
		case SPEC_CUVETTE:result.name= "a spectrometry cuvette";break;
		case STOCK_PLATE_96:result.name = "a 96-well stock plate";break;
		case WELL_BLOCK_96:result.name = "a 96-well block";break;
		case PCR_PLATE:result.name = "a 96-well PCR plate";break;
		case LIQUID_BLOCK:result.name = "a 96-well liquid block";break;
		case CELL_CULT_CHAMBER:result.name = "a cell culture chamber";break;
		case EPPENDORF:sprintf(temp2, " (%d)", container_count[18]+1);temp1 = strcat(temp1, "Eppendorf tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[18]++;break;
		case STERILE_MICROFUGE_TUBE2ML:printf(temp2, " (%d)", container_count[19]+1);temp1 = strcat(temp1, "sterile 2-ml microcentrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[19]++;break;
		case STERILE_PCR_TUBE:sprintf(temp2, " (%d)", container_count[20]+1);temp1 = strcat(temp1, "sterile 0.6-ml microcentrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[20]++;break;
		case CENTRI_TUBE_50ML:sprintf(temp2, " (%d)", container_count[21]+1);temp1 = strcat(temp1, "50-ml centrifuge tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[21]++;break;
		case CRYO_VIAL:result.name = "a screw-topped cryo vial";break;
		case SCREW_CAP_TUBE:sprintf(temp2, " (%d)", container_count[23]+1);temp1 = strcat(temp1, "screw-cap tube");temp1 = strcat(temp1, temp2);result.name = temp1;container_count[23]++;break;
		default:break;
		}
	result.used = 0;
	usage_list_containers[list_container_no] = result;
	result.usage_index = list_container_no;
	list_container_no++;
	set_container(fluid1, result);
	return result;
	}

void set_container(Fluid& fluid1, Container& container1)
	{
	if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
		{
		usage_list_fluids[fluid1.usage_index].used = 1;
		}
	else 
		{
		fluid1.used = 1;
		usage_list_fluids[list_fluid_no] = fluid1;
		fluid1.usage_index = list_fluid_no;
		list_fluid_no++;
		}	
	fluid1.container = container1.id;
	container1.contents = fluid1;
	container1.volume = container1.contents.volume;
	}

void measure_fluid(Fluid& fluid1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure fluid");
		create_edge_from_fluids(&fluid1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if (fluid1.volume == 0)
			fprintf(fp, "<font color = red>Warning: You are out of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", fluid1.new_name);
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		if (first == 1)
			{
			prev_container = container1.name;
			if (container1.contents.new_name == "")
				{
				if(fluid1.state == "")
					fprintf(fp, "Measure out %s into %s.<br>", fluid1.new_name, container1.name);
				else
					fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.name);
				first = 0;
				prev_cont++;
				}
			else
				{
				if(fluid1.state == "")
					fprintf(fp, "Measure out %s into %s.<br>", fluid1.new_name, container1.contents.new_name);
				else
					fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.contents.new_name);
				first = 0;
				prev_cont++;
				}
			}
		else if (prev_cont == 1)
			{
			prev_container = container1.name;
			if(fluid1.state == "")
				fprintf(fp, "Measure out %s into %s.<br>", fluid1.new_name, container1.name);
			else
				fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.name);
			prev_cont++;
			}
		else if(prev_container == container1.name)
			{
			fprintf(fp, "Add <font color=#357EC7>%s</font> to %s.<br>", fluid1.new_name, container1.contents.new_name);
			prev_container = container1.name;
			}
		else if(fluid1.state == "")
			{
			fprintf(fp, "Measure out %s into %s.<br>", fluid1.new_name, container1.name);
			prev_container = container1.name;
			}
		else
			{
			fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.name);
			prev_container = container1.name;
			}
		fluid1.container = container1.id;
		container1.contents = fluid1;
		container1.volume = container1.volume + fluid1.volume;
	}

void measure_fluid(Container& container, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure fluid");
		create_edge_from_container_to_fluid(&container, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if (container.volume == 0)
			fprintf(fp, "<font color = red>Warning: You are out of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", container.contents.new_name);
		if (usage_list_containers[container.usage_index].name == container.name)
			usage_list_containers[container.usage_index].used = 1;
		else 
			{
			container.used = 1;
			usage_list_containers[list_container_no] = container;
			container.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		if (first == 1)
			{
			prev_container = container1.name;
			if (container1.contents.new_name == "")
				{
				if(container.contents.state == "")
					fprintf(fp, "Measure out %s into %s.<br>", container.contents.new_name, container1.name);
				else
					fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", container.contents.new_name, container.contents.new_name, container1.name);
				first = 0;
				prev_cont++;
				}
			else
				{
				if(container.contents.state == "")
					fprintf(fp, "Measure out %s into %s.<br>", container.contents.new_name, container1.contents.new_name);
				else
					fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", container.contents.new_name, container.contents.new_name, container1.contents.new_name);
				first = 0;
				prev_cont++;
				}
			}
		else if (prev_cont == 1)
			{
			prev_container = container1.name;
			if(container.contents.state == "")
				fprintf(fp, "Measure out %s into %s.<br>", container.contents.new_name, container1.name);
			else
				fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", container.contents.new_name, container.contents.new_name, container1.name);
			prev_cont++;
			}
		else if(prev_container == container1.name)
			{
			fprintf(fp, "Add <font color=#357EC7>%s</font> to %s.<br>", container.contents.new_name, container1.contents.new_name);
			prev_container = container1.name;
			}
		else if(container.contents.state == "")
			{
			fprintf(fp, "Measure out %s into %s.<br>", container.contents.new_name, container1.name);
			prev_container = container1.name;
			}
		else
			{
			fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", container.contents.new_name, container.contents.new_name, container1.name);
			prev_container = container1.name;
			}
		container.contents.container = container1.id;
		container1.contents = container.contents;
		container1.volume = container1.volume + container.volume;
	}

void measure_fluid(Fluid& fluid1, Volume* volume1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure fluid");
		create_edge_from_fluids(&fluid1, &o);
		}
		if ((fluid1.volume == 0)|| (fluid1.volume < (volume1->value*volume1->mul)))
			fprintf(fp, "<font color = red>Warning: You are out of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", fluid1.new_name);
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		fprintf(fp, "Measure out ");
		volume1->display_vol();
		if(fluid1.state == "")
			fprintf(fp, " of %s.<br>", fluid1.new_name);
		else
			fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", fluid1.new_name, fluid1.new_name);
		fluid1.volume = fluid1.volume - volume1->value * volume1->mul;
	}

void measure_fluid(Fluid &fluid1, Volume* volume1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure fluid");
		create_edge_from_fluids(&fluid1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if ((fluid1.volume == 0)|| (fluid1.volume < (volume1->value*volume1->mul)))
			fprintf(fp, "<font color = red>Warning: You are out of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", fluid1.new_name);
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		if (first == 1)
			{
			fluid1.volume = fluid1.volume - volume1->value * volume1->mul;
			prev_container = container1.name;
			fprintf(fp, "Measure out ");
			volume1->display_vol();
			if (container1.contents.new_name == "")
				{
				if(fluid1.state == "")
					fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", fluid1.new_name, container1.name);
				else
					fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.name);
				first = 0;
				prev_cont++;
				}
			else
				{
				if(fluid1.state == "")
					fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", fluid1.new_name, container1.contents.new_name);
				else
					fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.contents.new_name);
				first = 0;
				prev_cont++;
				}
			}
		else if (prev_cont == 1)
			{
			fluid1.volume = fluid1.volume - volume1->value * volume1->mul;
			prev_container = container1.name;
			fprintf(fp, "Measure out ");
			volume1->display_vol();
			if(fluid1.state == "")
				fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", fluid1.new_name, container1.name);
			else
				fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.name);
			prev_cont++;
			}
		else if (prev_container == container1.name)
			{
			fluid1.volume = fluid1.volume - volume1->value * volume1->mul;
			if(fluid1.state == "")
				{
				fprintf(fp, "Add ");
				volume1->display_vol();
				fprintf(fp, " of <font color=#357EC7>%s</font>.<br>", fluid1.new_name);
				}
			else
				{
				fprintf(fp, "Add ");
				volume1->display_vol();
				fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", fluid1.new_name, fluid1.new_name);
				}
			}
		else
			{
			fluid1.volume = fluid1.volume - volume1->value * volume1->mul;
			fprintf(fp, "Measure out ");
			volume1->display_vol();
			if(fluid1.state == "")
				{
				fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", fluid1.new_name, container1.name);
				prev_container = container1.name;
				}
			else
				{
				fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", fluid1.new_name, fluid1.new_name, container1.name);
				prev_container = container1.name;
				}
			}
		fluid1.container = container1.id;
		container1.contents = fluid1;
		container1.volume = container1.volume + volume1->value * volume1->mul;
	}

void measure_fluid(Container& con, Volume* volume1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure fluid");
		create_edge_from_container_to_fluid(&con, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if ((con.volume == 0)|| (con.volume < (volume1->value*volume1->mul)))
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", con.contents.new_name);
		if (usage_list_containers[con.usage_index].name == con.name)
			usage_list_containers[con.usage_index].used = 1;
		else 
			{
			con.used = 1;
			usage_list_containers[list_container_no] = con;
			con.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		if (first == 1)
			{
			con.contents.volume = con.contents.volume - volume1->value * volume1->mul;
			prev_container = container1.name;
			fprintf(fp, "Measure out ");
			volume1->display_vol();
			if (container1.contents.new_name == "")
				{
				if(con.contents.state == "")
					fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", con.contents.new_name, container1.name);
				else
					fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", con.contents.new_name, con.contents.new_name, container1.name);
				first = 0;
				prev_cont++;
				}
			else
				{
				if(con.contents.state == "")
					fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", con.contents.new_name, container1.contents.new_name);
				else
					fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", con.contents.new_name, con.contents.new_name, container1.contents.new_name);
				first = 0;
				prev_cont++;
				}
			}
		else if (prev_cont == 1)
			{
			con.contents.volume = con.contents.volume - volume1->value * volume1->mul;
			prev_container = container1.name;
			fprintf(fp, "Measure out ");
			volume1->display_vol();
			if(con.contents.state == "")
				fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", con.contents.new_name, container1.name);
			else
				fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", con.contents.new_name, con.contents.new_name, container1.name);
			prev_cont++;
			}
		else if (prev_container == container1.name)
			{
			con.contents.volume = con.contents.volume + volume1->value * volume1->mul;
			if(con.contents.state == "")
				{
				fprintf(fp, "Add ");
				volume1->display_vol();
				fprintf(fp, " of <font color=#357EC7>%s</font>.<br>", con.contents.new_name);
				}
			else
				{
				fprintf(fp, "Add ");
				volume1->display_vol();
				fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", con.contents.new_name, con.contents.new_name);
				}
			}
		else
			{
			con.contents.volume = con.contents.volume - volume1->value * volume1->mul;
			fprintf(fp, "Measure out ");
			volume1->display_vol();
			if(con.contents.state == "")
				{
				fprintf(fp, " of <font color=#357EC7>%s</font> into %s.<br>", con.contents.new_name, container1.name);
				prev_container = container1.name;
				}
			else
				{
				fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", con.contents.new_name, con.contents.new_name, container1.name);
				prev_container = container1.name;
				}
			}
		con.contents.container = container1.id;
		container1.contents = con.contents;
		container1.volume = container1.volume + volume1->value * volume1->mul;
	}

void set_temp(Container& container1, float temp)
	{
	// graph maintenance
		{
		Operation o = new_operation("set temp");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents at new temperature");
		create_edge(o.node, container1.node);
		}
		if (temp >= 35)
			fprintf(fp, "Pre-heat %s in a water bath set at <b><font color=#357EC7>65%cC</font></b>.<br>", container1.contents.new_name, 0x00B0);
		else
			fprintf(fp, "Set the temperature of %s to <b><font color=#357EC7>%g%cC</font></b>.<br>", container1.contents.new_name, temp, 0x00B0);
	}

void measure_prop(Container& dest, Container& source, float prop)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure prop");
		create_edge_from_container_to_fluid(&source, &o);
		create_edge_from_fluid_to_container(&o, &dest);
		}
		if ((source.contents.volume == 0)|| (source.contents.volume < (dest.volume * prop)))
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", source.contents.new_name);
		if (usage_list_containers[dest.usage_index].name == dest.name)
			usage_list_containers[dest.usage_index].used = 1;
		else 
			{
			dest.used = 1;
			usage_list_containers[list_container_no] = dest;
			dest.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[source.usage_index].name == source.name)
			usage_list_containers[source.usage_index].used = 1;
		else 
			{
			source.used = 1;
			usage_list_containers[list_container_no] = source;
			source.usage_index = list_container_no;
			list_container_no++;
			}
		if (first == 1)
			{
			prev_container = dest.name;
			if(source.contents.state == "")
				{
				if (prop > 1)
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volumes <font color=#357EC7>%s</font> into %s.<br>", prop, source.contents.new_name, dest.contents.new_name);
				else
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volume <font color=#357EC7>%s</font> into %s.<br>", prop, source.contents.new_name, dest.contents.new_name);
				}
			else
				{
				if (prop > 1)
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volumes <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", prop, source.contents.new_name, source.contents.new_name, dest.contents.new_name);
				else
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volume <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", prop, source.contents.new_name, source.contents.new_name, dest.contents.new_name);
				}
			}
		else if (prev_container == dest.name)
			{
			dest.contents.volume = dest.contents.volume +  prop * dest.contents.volume;
			if(source.contents.state != "")
				{
				if (prop > 1)
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volumes <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", prop, source.contents.new_name, source.contents.new_name);
				else 
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volume <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", prop, source.contents.new_name, source.contents.new_name);
				}
			else 
				{
				if (prop > 1)
					fprintf(fp, "Add  <b><font color=#357EC7>%g</font></b> volumes <font color=#357EC7>%s</font>.<br>", prop, source.contents.new_name);
				else
					fprintf( fp,"Add  <b><font color=#357EC7>%g</font></b> volume <font color=#357EC7>%s</font>.<br>", prop, source.contents.new_name);
				}
			}
		else
			{
			dest.contents.volume = dest.contents.volume -  prop * dest.contents.volume;
			if(source.contents.state != "")
				{
				if (prop > 1)
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volumes <a href=\"#%s\" ><font color=#357EC7>%s</font></a> to %s.<br>", prop, source.contents.new_name, source.contents.new_name, dest.name, dest.name);
				else 
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volume <a href=\"#%s\" ><font color=#357EC7>%s</font></a> to %s.<br>", prop, source.contents.new_name, source.contents.new_name, dest.name);
				}
			else 
				{
				if (prop > 1)
					fprintf(fp, "Add  <b><font color=#357EC7>%g</font></b> volumes <font color=#357EC7>%s</font> to %s.<br>", prop, source.contents.new_name, dest.name);
				else
					fprintf( fp,"Add  <b><font color=#357EC7>%g</font></b> volume <font color=#357EC7>%s</font> to %s.<br>", prop, source.contents.new_name, dest.name);
				}
			}
		prev_container = dest.name;
		dest.volume = dest.volume +  prop * dest.volume;
	}

void measure_prop(Container& dest, Fluid& source, float prop)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure prop");
		create_edge_from_fluids(&source, &o);
		create_edge_from_fluid_to_container(&o, &dest);		
		}
		if ((source.volume == 0)|| (source.volume < (dest.volume*prop)))
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", source.new_name);
		if (usage_list_fluids[source.usage_index].original_name == source.original_name)
			usage_list_fluids[source.usage_index].used = 1;
		else 
			{
			source.used = 1;
			usage_list_fluids[list_fluid_no] = source;
			source.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[dest.usage_index].name == dest.name)
			usage_list_containers[dest.usage_index].used = 1;
		else 
			{
			dest.used = 1;
			usage_list_containers[list_container_no] = dest;
			dest.usage_index = list_container_no;
			list_container_no++;
			}
		if (first == 1)
			{
			prev_container = dest.name;
			if(source.state == "")
				{
				if (prop > 1)
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volumes <font color=#357EC7>%s</font> into %s.<br>", prop, source.new_name, dest.contents.new_name);
				else
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volume <font color=#357EC7>%s</font> into %s.<br>", prop, source.new_name, dest.contents.new_name);
				}
			else
				{
				if (prop > 1)
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volumes <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", prop, source.new_name, source.new_name, dest.contents.new_name);
				else
					fprintf(fp, "Measure out <b><font color=#357EC7>%g</font></b> volume <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", prop, source.new_name, source.new_name, dest.contents.new_name);
				}
			}
		else if (prev_container == dest.name)
			{
			dest.contents.volume = dest.contents.volume +  prop * dest.contents.volume;
			if(source.state != "")
				{
				if (prop > 1)
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volumes <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", prop, source.new_name, source.new_name);
				else 
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volume <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", prop, source.new_name, source.new_name);
				}
			else 
				{
				if (prop > 1)
					fprintf(fp, "Add  <b><font color=#357EC7>%g</font></b> volumes <font color=#357EC7>%s</font>.<br>", prop, source.new_name);
				else
					fprintf(fp, "Add  <b><font color=#357EC7>%g</font></b> volume <font color=#357EC7>%s</font>.<br>", prop, source.new_name);
				}
			}
		else
			{
			dest.contents.volume = dest.contents.volume -  prop * dest.contents.volume;
			if(source.state != "")
				{
				if (prop > 1)
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volumes <a href=\"#%s\" ><font color=#357EC7>%s</font></a> to %s.<br>", prop, source.new_name, source.new_name, dest.name);
				else 
					fprintf(fp, "Add <b><font color=#357EC7>%g</font></b> volume <a href=\"#%s\" ><font color=#357EC7>%s</font></a> to %s.<br>", prop, source.new_name, source.new_name, dest.name);
				}
			else 
				{
				if (prop > 1)
					fprintf(fp, "Add  <b><font color=#357EC7>%g</font></b> volumes <font color=#357EC7>%s</font> to %s.<br>", prop, source.new_name, dest.name);
				else
					fprintf(fp, "Add  <b><font color=#357EC7>%g</font></b> volume <font color=#357EC7>%s</font> to %s.<br>", prop, source.new_name, dest.name);
				}
			}

		prev_container = dest.name;
		dest.volume = dest.volume +  prop * dest.volume;
	}

void combine_helper (int count, Container &container1, va_list ap)
	{
	int i;
	Container sample, result1;
	char* plus = "+";
	char* str3 = "";
	char* str4 = "";
	Operation o;
	// graph  maintenance
		{
		result1.node = create_node("container with mixture");
		o = new_operation("combine");
		create_edge_from_fluids(&o, &result1);
		}
		for(i=0; i<count; i++)
			{
			if (i == 0)
				{
				sample = container1;
				fprintf(fp, "Combine the %s", sample.contents.new_name);
				result1.contents.new_name = container1.contents.new_name;
				result1.volume = container1.volume;
				} 
			else {
				sample = va_arg (ap, Container);
				fprintf(fp, ",%s", sample.contents.new_name);
				str3 = (char *)calloc(strlen(str3) + strlen(sample.contents.new_name) + 2, sizeof(char));
				strcat(str3, plus);
				strcat(str3, sample.contents.new_name);
				result1.volume = result1.volume + sample.volume;
				}
			// graph maintenance
				{
				create_edge_from_container_to_fluid(&sample, &o);
				}

			}

		str4=(char *)calloc(strlen(result1.contents.new_name) + strlen(str3) + 1, sizeof(char));
		strcat(str4, result1.contents.new_name);
		strcat(str4, str3);
		result1.contents.new_name = str4;

		container1 = result1;
		fprintf(fp, "<br>");
	}

void combine (int count, Container &container1, ...) 
	{
	va_list ap;
	va_start(ap, container1);
	combine_helper(count, container1, ap);
	va_end(ap);
	}

void transfer (Container &container1, Container& container2)
	{
	// graph maintenance
		{
		Operation o = new_operation("transfer");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluid_to_container(&o, &container2);
		}
		if (container1.volume == 0)
			fprintf(fp, "<font color = red>Warning: You are out of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", container1.contents.new_name);
		if (usage_list_containers[container2.usage_index].name == container2.name)
			usage_list_containers[container2.usage_index].used = 1;
		else 
			{
			container2.used = 1;
			usage_list_containers[list_container_no] = container2;
			container2.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Transfer %s into %s.<br>" , container1.contents.new_name, container2.name);
		container2.contents = container1.contents;
		container2.contents.container = container2.id;
		container2.volume = container1.volume;
		container1.volume = 0;
	}

void centrifuge_pellet(Container& container1, Speed* speed1, float temp, Time* time1, Volume* expected_vol)
	{
	fprintf(fp, "Centrifuge ");
	check_container(container1);
	fprintf(fp, " at "); 
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp == RT)
		fprintf(fp, "at <b><font color=#357EC7>room temperature</font></b>, gently aspirate out the supernatant and discard it.<br>");
	else
		fprintf(fp, "at <b><font color=#357EC7>%g%cC</font></b>, gently aspirate out the supernatant and discard it.<br>", temp, 0x00B0);
	fprintf(fp, "<i>The expected volume of supernatant is ");
	expected_vol->display_vol();
	fprintf(fp, ".</i><br>");
	name_sample(container1, "pellet");
	container1.volume = 0;
	if(microfuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		microfuge_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("centrifuge pellet");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with pellet");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void centrifuge_pellet(Container &container1, Speed* speed1, float temp, Time* time1)
	{
	fprintf(fp, "Centrifuge ");
	check_container(container1);
	fprintf(fp, " at "); 
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp == RT)
		fprintf(fp, " at <b><font color=#357EC7>room temperature</font></b>, gently aspirate out the supernatant and discard it.<br>");
	else
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>, gently aspirate out the supernatant and discard it.<br>", temp, 0x00B0);
	name_sample(container1, "pellet");
	container1.volume = 0;
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("centrifuge pellet");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with pellet");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void centrifuge_phases_top(Container &container1, Speed* speed1, float temp, Time* time1, Volume* aspirate_vol, Container &container2)
	{
	// graph maintenance
	Operation o = new_operation("centrifuge phases -- top phase");
	create_edge_from_container_to_fluid(&container1, &o);
	create_edge_from_fluid_to_container(&o, &container2);

	if (usage_list_containers[container2.usage_index].name == container2.name)
		usage_list_containers[container2.usage_index].used = 1;
	else 
		{
		container2.used = 1;
		usage_list_containers[list_container_no] = container2;
		container2.usage_index = list_container_no;
		list_container_no++;
		}
	fprintf(fp, "Centrifuge ");
	check_container(container1);
	fprintf(fp, " at "); 
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp == RT)
		fprintf(fp, " at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b>");
	else
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>", temp, 0x00B0);
	fprintf( fp, " and aspirate out ");
	aspirate_vol->display_vol();
	fprintf(fp, " of top layer into %s.<br>", container2.name);
	name_sample(container1, "top aqueous layer");
	container2.volume = aspirate_vol->value;
	container2.contents = container1.contents;
	name_sample(container1, "bottom layer");
	discard(container1);
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	o.node->time = time1->value;
	o.node->unit = time1->unit;
	o.node->pcr_or_centrifuge = 1;
	}

void centrifuge_phases_top(Container &container1, Speed* speed1, float temp, Time* time1, Container &container2)
	{
	// graph maintenance
	Operation o = new_operation("centrifuge phases -- top phase");
	create_edge_from_container_to_fluid(&container1, &o);
	create_edge_from_fluid_to_container(&o, &container2);

	if (usage_list_containers[container2.usage_index].name == container2.name)
		usage_list_containers[container2.usage_index].used = 1;
	else 
		{
		container2.used = 1;
		usage_list_containers[list_container_no] = container2;
		container2.usage_index = list_container_no;
		list_container_no++;
		}
	fprintf(fp, "Centrifuge ");
	check_container(container1);
	fprintf(fp, " at "); 
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp == RT)
		fprintf(fp, " at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b>");
	else
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>", temp, 0x00B0);
	fprintf( fp, " and aspirate out the top layer into %s.<br>", container2.name);
	name_sample(container1, "top aqueous layer");
	container2.contents = container1.contents;
	container2.volume = 0.5 * container1.volume;
	name_sample(container1, "bottom layer");
	discard(container1);
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	o.node->time = time1->value;
	o.node->unit = time1->unit;
	o.node->pcr_or_centrifuge = 1;
	}

void centrifuge(Container &container1, Speed* speed1, float temp, Time* time1)
	{
	fprintf(fp, "Centrifuge ");
	check_container(container1);
	fprintf(fp, " at "); 
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp == RT)
		fprintf(fp, " at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b>.<br>");
	else
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("centrifuge");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents centrifuged");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void discard(Container& container1)		
	{	
	// graph maintenance
		{
		Operation o = new_operation("discard");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("contents of container discarded");
		create_edge(o.node, container1.node);
		}
		fprintf(fp, "Discard %s.<br>", container1.contents.new_name);
		container1.volume = 0;

	}

void mix (Container &container1, enum mixing type, Time* time1)    	
	{
	switch(type){
	case TAPPING: fprintf(fp, "Gently tap the mixture for ");time1->display_time();fprintf(fp, " .<br>"); break;
	case STIRRING: fprintf(fp, "Stir the mixture for ");time1->display_time();fprintf(fp, " .<br>"); break;
	case INVERTING: fprintf(fp, "Close the tube tightly and invert the tube ");time1->display_time();fprintf(fp, " times.<br>"); break;
	case VORTEXING: fprintf(fp, "Vortex the mixture for ");time1->display_time();fprintf(fp, " .<br>"); break;
	default: fprintf(fp, "Invalid entry.<br>");
		}
	// graph maintenance
		{
		Operation o = new_operation("mix");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents mixed");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void mix (Container &container1, enum mixing type, int times)    	
	{
	switch(type){
	case INVERTING: fprintf(fp, "Close the tube tightly and invert the tube <b><font color=#357EC7>%d times</font></b>.<br>", times); break;
	default: fprintf(fp, "Invalid entry.<br>");
		}
	// graph maintenance
		{
		Operation o = new_operation("mix");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents mixed");
		create_edge(o.node, container1.node);
		}
	}

void mix (Container &container1, enum mixing type, int min_times, int max_times)    	
	{
	switch(type){
	case INVERTING: fprintf(fp, "Close the tube tightly and invert the tube <b><font color=#357EC7>%d - %d times</font></b>.<br>", min_times, max_times); break;
	default: fprintf(fp, "Invalid entry.<br>");
		}
	// graph maintenance
		{
		Operation o = new_operation("mix");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents mixed");
		create_edge(o.node, container1.node);
		}
	}

void mix (Container &container1, enum mixing type)    	
	{
	switch(type){
	case TAPPING: fprintf(fp, "Gently tap the mixture for a few secs.<br>"); break;
	case STIRRING: fprintf(fp, "Stir the mixture for a few secs.<br>"); break;
	case INVERTING: fprintf(fp, "Close the tube tightly and gently mix the contents by inverting the tube.<br>"); break;
	case VORTEXING: fprintf(fp, "Vortex the mixture for a few secs.<br>"); break;
	case RESUSPENDING:fprintf(fp, "Resuspend pellet by vortexing/by shaking vigorously.<br>"); break;
	case DISSOLVING:fprintf(fp, "Dissolve the pellet in the solution.<br>");break;
	case PIPETTING:fprintf(fp, "Mix solution by pipetting up and down several times.<br>");break;
	default: fprintf(fp, "Invalid entry.<br>");
		}
	// graph maintenance
		{
		Operation o = new_operation("mix");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents mixed");
		create_edge(o.node, container1.node);
		}
	}

void mix (Container &container1, enum until event1)    	
	{
	if (event1 == PPT_STOPS_STICKING)
	fprintf(fp, "Close the tube tightly and gently mix the contents by inverting the tube until precipitate stops sticking to walls of the tube.<br>");
	if (event1 == PELLET_DISLODGES)
		fprintf(fp, "Gently mix the contents of the tube until the pellet dislodges.<br>");
	// graph maintenance
		{
		Operation o = new_operation("mix");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents mixed");
		create_edge(o.node, container1.node);
		}
	}

void tap (Container& container1, enum until event1)
	{
	mix(container1, event1);
	}

void tap(Container &container1, Time* time1)
	{
	mix(container1, TAPPING, time1);
	}

void tap(Container &container1)
	{
	mix(container1, TAPPING);
	}

void stir(Container& container1)
	{
	mix(container1, STIRRING);
	}

void stir(Container& container1, Time* time1)
	{
	mix(container1, STIRRING, time1);
	}

void invert(Container &container1, int times)
	{
	mix(container1, INVERTING, times);
	}

void invert(Container &container1, int min_times, int max_times)
	{
	mix(container1, INVERTING, min_times, max_times);
	}

void invert(Container &container1)
	{
	mix(container1, INVERTING);
	}

void invert(Container& container1, enum until event1)
	{
	mix(container1, event1);
	}

void vortex(Container &container1, Time* time1)
	{
	mix(container1, VORTEXING, time1);
	}

void vortex(Container &container1)
	{
	mix(container1, VORTEXING);
	}

void resuspend(Container &container1)
	{
	mix(container1, RESUSPENDING);
	}

void dissolve (Container &container1)
	{
	mix(container1, DISSOLVING);
	}

void pipet (Container &container1)
	{

	mix(container1, PIPETTING);
	}

void wait(Container& container1, Time* time1)
	{
	fprintf(fp, "Keep %s aside for ", container1.contents.new_name);
	time1->display_time();
	fprintf(fp, ".<br>");
	// graph maintenance
		{
		Operation o = new_operation("wait");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents after waiting");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void combine_and_mix (enum mixing type, Time* time1, int count, Container &container1, ...)
	{
	va_list ap;
	va_start(ap, container1);
	combine_helper(count, container1, ap);
	va_end(ap);
	mix(container1, type, time1);
	}

void combine_and_mix (enum mixing type, int count, Container &container1, ...)
	{
	va_list ap;
	va_start(ap, container1);
	combine_helper(count, container1, ap);
	va_end(ap);
	mix(container1, type);
	}

void inoculation(Container& medium, Fluid& sample1, Volume* volume1, float temp, Time* time1, int shaking_type)
	{
	char* unit;
	float result;
	if ((sample1.volume == 0)|| (sample1.volume < (volume1->value*volume1->mul)))
		{
		fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", sample1.new_name);

		}
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[medium.usage_index].name == medium.name)
		usage_list_containers[medium.usage_index].used = 1;
	else 
		{
		medium.used = 1;
		usage_list_containers[list_container_no] = medium;
		medium.usage_index = list_container_no;
		list_container_no++;
		}
	if (medium.volume != DEFAULT)
		{
		switch(medium.contents.unit){
			case UL: unit = "µl"; result = medium.volume; break;
			case ML: unit = "ml"; result = medium.volume/1000; break;
			case L: unit = "l"; result = medium.volume/1000000; break;
			default:break;
			}
		fprintf(fp, "Inoculate <font color=#357EC7>%g %s %s</font> with ", result, unit, medium.contents.new_name);
		}
	else
		fprintf(fp, "Inoculate <font color=#357EC7>%s</font> with ", medium.contents.new_name);
	volume1->display_vol();
	switch(shaking_type)
		{
		case 1:
			fprintf(fp, " of <font color=#357EC7>%s</font> and incubate with shaking for ", sample1.new_name);
			break;
		case 2:
			fprintf(fp, " of <font color=#357EC7>%s</font> and incubate without shaking for ", sample1.new_name);
			break;
		default:fprintf(fp, "Invalid inputs.");break;
		}
	time1->display_time();
	fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
	medium.volume = medium.volume + volume1->value * volume1->mul;
	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	name_sample(medium, "culture");
	// graph maintenance
		{
		Operation o = new_operation("inoculation");
		create_edge_from_container_to_fluid(&medium, &o);
		medium.node = create_node("container with contents after inoculation");
		create_edge(o.node, medium.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void inoculation(Container& medium, Fluid& sample1, Volume* volume1, float temp, Time* time1, float cell_density, int shaking_type)
	{
	char* unit;
	float result;
	if ((sample1.volume == 0)|| (sample1.volume < (volume1->value*volume1->mul)))
		{
		fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", sample1.new_name);

		}
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[medium.usage_index].name == medium.name)
		usage_list_containers[medium.usage_index].used = 1;
	else 
		{
		medium.used = 1;
		usage_list_containers[list_container_no] = medium;
		medium.usage_index = list_container_no;
		list_container_no++;
		}
	if (medium.volume != DEFAULT)
		{
		switch(medium.contents.unit){
			case UL: unit = "µl"; result = medium.contents.volume; break;
			case ML: unit = "ml"; result = medium.contents.volume/1000; break;
			case L: unit = "l"; result = medium.contents.volume/1000000; break;
			default:break;
			}
		fprintf(fp, "Inoculate <font color=#357EC7>%g %s %s</font> with ", result, unit, medium.contents.new_name);
		}
	else
		fprintf(fp, "Inoculate <font color=#357EC7>%s</font> with ", medium.contents.new_name);
	volume1->display_vol();
	switch(shaking_type)
		{
		case 1:
			fprintf(fp, " of <font color=#357EC7>%s</font> and incubate with shaking for ", sample1.new_name);
			break;
		case 2:
			fprintf(fp, " of <font color=#357EC7>%s</font> and incubate without shaking for ", sample1.new_name);
			break;
		default:fprintf(fp, "Invalid inputs.");break;
		}
	time1->display_time();
	fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
	fprintf(fp, "Calibrate the spectrophotometer and make sure that the cell density of the culture has reached <b><font color=#357EC7>%g X 10<sup>6</sup> cells/ml</font></b> before continuing.<br>", cell_density);
	medium.volume = medium.volume + volume1->value * volume1->mul;
	if(spectrophotometer_no == 1)
		{
		equipments[equip_no] = "Spectrophotometer";
		equip_no++;
		incubator_no++;
		}
	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	name_sample(medium, "culture");
	// graph maintenance
		{
		Operation o = new_operation("inoculation");
		create_edge_from_container_to_fluid(&medium, &o);
		medium.node = create_node("container with contents at required OD after inoculation");
		create_edge(o.node, medium.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void inoculation(Container& medium, Solid& sample1, float temp, Time* time1, int shaking_type)
	{
	char* unit;
	float result;
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[medium.usage_index].name == medium.name)
		usage_list_containers[medium.usage_index].used = 1;
	else 
		{
		medium.used = 1;
		usage_list_containers[list_container_no] = medium;
		medium.usage_index = list_container_no;
		list_container_no++;
		}
	if (medium.volume != DEFAULT)
		{
		switch(medium.contents.unit){
			case UL: unit = "µl"; result = medium.contents.volume; break;
			case ML: unit = "ml"; result = medium.contents.volume/1000; break;
			case L: unit = "l"; result = medium.contents.volume/1000000; break;
			default:break;
			}
		fprintf(fp, "Inoculate <font color=#357EC7>%g %s %s</font> with ", result, unit, medium.contents.new_name);
		}
	else
		fprintf(fp, "Inoculate <font color=#357EC7>%s</font> with ", medium.contents.new_name);
	switch(shaking_type)
		{
		case 1:
			fprintf(fp, "<font color=#357EC7>%s</font> and incubate with shaking for ", sample1.new_name);
			time1->display_time();
			fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
			break;
		case 2:
			fprintf(fp, "<font color=#357EC7>%s</font> and incubate without shaking for ", sample1.new_name);
			time1->display_time();
			fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
			break;
		default:fprintf(fp, "Invalid inputs.");break;
		}

	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	name_sample(medium, "culture");
	// graph maintenance
		{
		Operation o = new_operation("inoculation");
		create_edge_from_container_to_fluid(&medium, &o);
		medium.node = create_node("container with contents after inoculation");
		create_edge(o.node, medium.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void inoculation(Container& medium, Solid& sample1, float temp, Time* time1, float cell_density, int shaking_type)
	{
	char* unit;
	float result;
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[medium.usage_index].name == medium.name)
		usage_list_containers[medium.usage_index].used = 1;
	else 
		{
		medium.used = 1;
		usage_list_containers[list_container_no] = medium;
		medium.usage_index = list_container_no;
		list_container_no++;
		}
	if (medium.volume != DEFAULT)
		{
		switch(medium.contents.unit){
			case UL: unit = "µl"; result = medium.contents.volume; break;
			case ML: unit = "ml"; result = medium.contents.volume/1000; break;
			case L: unit = "l"; result = medium.contents.volume/1000000; break;
			default:break;
			}
		fprintf(fp, "Inoculate <font color=#357EC7>%g %s %s</font> with ", result, unit, medium.contents.new_name);
		}
	else
		fprintf(fp, "Inoculate <font color=#357EC7>%s</font> with ", medium.contents.new_name);
	switch(shaking_type)
		{
		case 1:
			fprintf(fp, "<font color=#357EC7>%s</font> and incubate with shaking for ", sample1.new_name);
			time1->display_time();
			fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
			break;
		case 2:
			fprintf(fp, "<font color=#357EC7>%s</font> and incubate without shaking for ", sample1.new_name);
			time1->display_time();
			fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
			break;
		default:fprintf(fp, "Invalid inputs.");break;
		}
	fprintf(fp, "Calibrate the spectrophotometer and make sure that the cell density of the culture has reached <b><font color=#357EC7>%g X 10<sup>6</sup> cells/ml</font></b> before continuing.<br>", cell_density);
	if(spectrophotometer_no == 1)
		{
		equipments[equip_no] = "Spectrophotometer";
		equip_no++;
		incubator_no++;
		}
	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	name_sample(medium, "culture");
	// graph maintenance
		{
		Operation o = new_operation("inoculation");
		create_edge_from_container_to_fluid(&medium, &o);
		medium.node = create_node("container with contents at required OD after inoculation");
		create_edge(o.node, medium.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void store(Container& container1, float temp)
	{
	// graph maintenance
		{
		Operation o = new_operation("store");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container stored at required temperature");
		create_edge(o.node, container1.node);
		}
		if(temp == ON_ICE)
			{
			fprintf(fp, "Store ");
			check_container(container1);
			fprintf(fp, " <b><font color=#357EC7>on ice</font></b>.<br>");
			}
		else if(temp == BOILING_WATER)
			fprintf(fp, "Immerse the tube in boiling water.");
		else 
			{
			fprintf(fp, "Store ");
			check_container(container1);
			fprintf(fp," at <b><font color=#357EC7>%g%cC</font></b>.<br>", temp, 0x00B0);
			}
	}

void store_for(Container& container1, float temp, Time* time1)
	{
	if(temp == ON_ICE)
		{
		fprintf(fp, "Store ");
		check_container(container1);
		fprintf(fp, " <b><font color=#357EC7>on ice</font></b> for ");
		}
	else if(temp == BOILING_WATER)
		{
		fprintf(fp, "Immerse ");
		check_container(container1);
		fprintf(fp, " in boiling water for ");
		}
	else if (temp == RT)
		{
		fprintf(fp, "Store ");
		check_container(container1);
		fprintf(fp, " at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b> for ");
		}
	else 
		{
		fprintf(fp, "Store ");
		check_container(container1);
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0);
		}
	time1->display_time();
	fprintf(fp, ".<br>");
	// graph maintenance
		{
		Operation o = new_operation("store for");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container stored at required temperature");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void store_for(Container& sample1, float temp, Time* time1, enum func function)
	{
	if(function == 1)
		{
		fprintf(fp, "Denature ");
		check_container(sample1);
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0);
		}
	else if (function == 2)
		{
		fprintf(fp, "Perform enzyme inactivation by storing ");
		check_container(sample1);
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0);
		}
	time1->display_time();
	fprintf(fp, ".<br>");
	// graph maintenance
		{
		Operation o = new_operation("store for");
		create_edge_from_container_to_fluid(&sample1, &o);
		sample1.node = create_node("container stored at required temperature");
		create_edge(o.node, sample1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void store_until(Container& container1, float temp, enum until type)
	{
	// graph maintenance
		{
		Operation o = new_operation("store until");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container stored at required temperature");
		create_edge(o.node, container1.node);
		}
		switch(type)
			{
			case ETHANOL_EVAP:if (temp == RT)
								  fprintf(fp, "Store %s at <b><font color=#357EC7>room temperature</font></b> until the ethanol has evaporated and no fluid is visible in the tube.<br>", container1.contents.new_name);
							  else
								  fprintf(fp, "Store %s at <b><font color=#357EC7>%g%cC</font></b> until the ethanol has evaporated and no fluid is visible in the tube.<br>", container1.contents.new_name, temp, 0x00B0);break;
			case OD:fprintf(fp, "Incubate %s at <b><font color=#357EC7>%g%cC</font></b> until the O.D.600 reaches 0.6.<br>", container1.contents.new_name, temp, 0x00B0);break;
			case THAW:fprintf(fp, "Allow %s to thaw at <b><font color=#357EC7>room temperature</font></b>.<br>", container1.contents.new_name);break;
			case COOLED:fprintf(fp, "Keep %s at <b><font color=#357EC7>room temperature</font></b> until cooled.<br>", container1.contents.new_name);break;
			case COLOUR_DEVELOPS:fprintf(fp, "Wait for the colour to develop.<br>");break;
			case THAW_ICE:fprintf(fp, "Allow %s to thaw on <b><font color=#357EC7>ice</font></b>.<br>", container1.contents.new_name);break;
			default:break;
			}
	}

void store_until(Container& container1, float temp, enum until type, Time* time1)
	{
	switch(type)
		{
		case ETHANOL_EVAP:if (temp == RT)
							  fprintf(fp, "Store %s at <b><font color=#357EC7>room temperature</font></b> until the ethanol has evaporated and no fluid is visible in the tube (~", container1.contents.new_name);
						  else 
							  fprintf(fp, "Store %s at <b><font color=#357EC7>%g%cC</font></b> until the ethanol has evaporated and no fluid is visible in the tube (~", container1.contents.new_name);break;
		case OD:fprintf(fp, "Incubate %s at <b><font color=#357EC7>%g%cC</font></b> until the O.D.600 reaches 0.6 (~", container1.contents.new_name);break;
		case THAW:fprintf(fp, "Allow %s to thaw at <b><font color=#357EC7>room temperature</font></b> (~", container1.contents.new_name);break;
		case COOLED:fprintf(fp, "Keep %s at <b><font color=#357EC7>room temperature</font></b> until cooled (~", container1.contents.new_name);break;
		case COLOUR_DEVELOPS:fprintf(fp, "Wait for the colour to develop (~", container1.contents.new_name);break;
		default:break;
		}
	time1->display_time();
	fprintf(fp, ").<br>");
	// graph maintenance
		{
		Operation o = new_operation("store until");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container stored at required temperature");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void invert_dry(Container& container1, float temp, Time* time1)
	{
	if(temp == RT)
		{
		fprintf(fp, "Stand the tube containing %s for ", container1.contents.new_name);
		time1->display_time();
		fprintf(fp, " in an inverted position on a paper towel to allow all of the fluid to drain away.<br>");
		}
	else 
		{
		fprintf(fp, "Stand the tube containing %s in an inverted position on a paper towel to allow all of the fluid to drain away. Dry at <b><font color=#357EC7>%g%cC</font></b> for ", container1.contents.new_name, temp, 0x00B0);
		time1->display_time();
		fprintf(fp, ".<br>");
		}
	container1.volume = 0;
	// graph maintenance
		{
		Operation o = new_operation("invert dry");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after drying");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void invert_dry(Container& container1, float temp)
	{
	// graph maintenance
		{
		Operation o = new_operation("invert dry");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after drying");
		create_edge(o.node, container1.node);
		}
		if(temp == RT)
			fprintf(fp, "Stand the tube containing %s in an inverted position on a tissue paper to allow all of the fluid to drain away.<br>", container1.contents.new_name);
		else 
			fprintf(fp, "Stand the tube containing %s in an inverted position on a tissue paper to allow all of the fluid to drain away.<br>", container1.contents.new_name);
		container1.volume = 0;
	}

void incubate(Container& container1, float temp, Time* time1, int rpm)
	{
	if(temp == RT)
		{
		fprintf(fp, "Incubate ");
		check_container(container1);
		fprintf(fp," at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b> for ");
		}
	else if((temp == ON_ICE)||(temp == 0))
		{
		fprintf(fp, "Incubate ");
		check_container(container1);
		fprintf(fp, " on <b><font color=#357EC7><b><font color=#357EC7>ice</font></b></font></b> for ");
		}
	else
		{
		fprintf(fp, "Incubate ");
		check_container(container1);
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0);
		}
	time1->display_time();
	fprintf(fp, " with shaking at %d rpm.<br>", rpm);
	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("incubate");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after incubation");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void incubate(Container& container1, float temp, Time* time1)
	{
	if(temp == 28)
		{
		fprintf(fp, "Incubate ");
		check_container(container1);
		fprintf(fp, " at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b> for ");
		}
	else if((temp == ON_ICE)||(temp == 0))
		{
		fprintf(fp, "Incubate ");
		check_container(container1);
		fprintf(fp, " on <b><font color=#357EC7><b><font color=#357EC7>ice</font></b></font></b> for ");
		}
	else
		{
		fprintf(fp, "Incubate ");
		check_container(container1);
		fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0);
		}
	time1->display_time();
	fprintf(fp, ".<br>");

	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("incubate");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after incubation");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void incubate_and_mix(Container& container1, float temp, Time* time1, Time* time_mix, enum mixing type)
	{
	fprintf(fp, "Incubate ");
	check_container(container1);
	switch (type){
			case STIRRING:	fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0); time1->display_time(); fprintf(fp, ", mixing gently by stirring the tube every "); time_mix->display_time(); fprintf(fp, ". <br>");break;
			case INVERTING: fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0); time1->display_time(); fprintf(fp, ", mixing gently by inverting the tube every "); time_mix->display_time(); fprintf(fp, ". <br>");break;
			case VORTEXING:	fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b> for ", temp, 0x00B0); time1->display_time(); fprintf(fp, ", mixing gently by vortexing the tube every "); time_mix->display_time(); fprintf(fp, ". <br>");break;
			default:break;
		}
	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("incubate and mix");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after incubation with mixing at periodic intervals");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void name_sample(Container& container1, char* new_name)
	{
	container1.contents.new_name = new_name;
	container1.contents.state = "";
	container1.contents.used = 1;
	usage_list_fluids[list_fluid_no] = container1.contents;
	container1.contents.usage_index = list_fluid_no;
	list_fluid_no++;
	}

void name_column(Column& column1, char* new_name)
	{
	column1.name = new_name;
	usage_list_containers[column1.usage_index].name = new_name;
	}

void nanodrop(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("nanodrop");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("nonodrop results");
		create_edge(o.node, container1.node);
		}
		fprintf(fp, "Measure the concentration of %s in your final samples with the Nanodrop.<br>", container1.contents.new_name);
	}

void time_constraint(Container& container1, Time* time1, enum time_constraint_type type)
	{
	switch (type){
		case CURRENT_FLUID:	if (time == 0)
							fprintf(fp, "<font color=red>NOTE: Use %s ", container1.contents.new_name);
							else
								{
								fprintf(fp, "<font color=red>NOTE: Use %s within ", container1.contents.new_name);
			};time1->display_time();fprintf(fp, ".</font><br>");break;
		case NEXTSTEP:	if (time == 0)
							fprintf(fp, "<font color=red>NOTE: Proceed to the next step ");
						else
							fprintf(fp, "<font color=red>NOTE: Proceed to the next step within ");
			time1->display_time();fprintf(fp, "!</font><br>");break;
		default:break;
		}
	// graph maintenance
		{
		Operation o = new_operation("time constraint");
		create_edge_from_container_to_fluid(&container1, &o);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void use_or_store(Container &container1, Time* time1, float temp_use, float temp_store)
	{
	fprintf(fp, "NOTE: If %s is/are used within ", container1.contents.new_name);
	time1->display_time();
	fprintf(fp, ", set temperature to <b><font color=#357EC7>%g%cC</font></b> and use, else store it at <b><font color=#357EC7>%g%cC</font></b>.", temp_use, 0x00B0, temp_store, 0x00B0);
	// graph maintenance
		{
		Operation o = new_operation("use or store");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container either used or stored");
		create_edge(o.node, container1.node);
		}

	}

void mixing_table(int no_rows, int no_columns, Volume* rxn_vol, Fluid array1[], char* tube[], float *vol, Container& container1)
	{
	int i,j;
	for(i=0; i<no_columns-1;i++)
		{
		if (usage_list_fluids[array1[i].usage_index].original_name == array1[i].original_name)
			usage_list_fluids[array1[i].usage_index].used = 1;
		else 
			{
			array1[i].used = 1;
			usage_list_fluids[list_fluid_no] = array1[i];
			array1[i].usage_index = list_fluid_no;
			list_fluid_no++;
			}	
		}
	if (usage_list_containers[container1.usage_index].name == container1.name)
		usage_list_containers[container1.usage_index].used = 1;
	else 
		{
		container1.used = 1;
		usage_list_containers[list_container_no] = container1;
		container1.usage_index = list_container_no;
		list_container_no++;
		}
	if(no_rows-1 == 1)
		fprintf(fp, "Use the following table as a checklist for preparing the reaction in %s:<br><br>", container1.name);
	else 
		fprintf(fp, "Use the following table as a checklist for preparing the reactions in %s:<br><br>", container1.name);
	fprintf(fp, "<table border cellpadding=5 rules=all frame=void bordercolor=#357EC7>");
	for(i=0; i<no_rows; i++)
		{
		for(j=0; j<no_columns; j++)
			{
			if((i == 0) && (j == 0))
				fprintf(fp, "<thead><tr><td>&nbsp;</td>");

			else if((i == 0) && (j != 0))
				{

				fprintf(fp, "<td><font color=#357EC7>%s</font></td>", (array1[j-1]).new_name);

				if (j == no_columns-1)
					fprintf(fp, "</tr></thead>");
				}
			else if((i != 0) && (j == 0))
				fprintf(fp, "<tbody><tr><td><font color=#357EC7>%s</font></td>", tube[i-1]);

			else 
				{
				if((i == no_rows-1 )&& (j == no_columns-1) && ((*(vol+(no_columns-1)*(i-1)+(j-1)) == XVAL)))
					{
					fprintf(fp, "<td><b><font color=#357EC7>Make up the volume to ");
					rxn_vol->display_vol();
					fprintf(fp, "</font></b></td>");
					}

				else if (*(vol+(no_columns-1)*(i-1)+(j-1)) == NA)
					fprintf(fp, "<td><b><font color=#357EC7>--</font></b></td>");	
				else if (*(vol+(no_columns-1)*(i-1)+(j-1)) == XVAL)
					fprintf(fp, "<td><b><font color=#357EC7>X</font></b></td>");
				else
					fprintf(fp, "<td><b><font color=#357EC7>%g</font></b></td>", *(vol+(no_columns-1)*(i-1)+(j-1)));
				if (j == no_columns-1)
					fprintf(fp, "</tr></body>");

				}
			}
		}
	fprintf(fp, "</table>");
	name_sample(container1, "reaction mix");
	container1.volume = 0;
	Operation o = new_operation("mixing table");
	for(i=1; i<no_rows; i++)
		{
		for(j=1; j<no_columns; j++)
			{
			if ((array1[j-1].volume == 0)|| (array1[j-1].volume < (*(vol+(no_columns-1)*(i-1)+(j-1))* rxn_vol->mul)))
				{
				fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", array1[j-1].new_name);

				}
			container1.volume = container1.volume + *(vol+(no_columns-1)*(i-1)+(j-1));
			}
		}
	for(j=1; j<no_columns; j++)
		{
		// graph maintenance
		create_edge_from_fluids(&array1[j-1], &o);
		}
	create_edge_from_fluid_to_container(&o, &container1);
	if (container1.volume > ((no_rows-1)*rxn_vol->value))
		fprintf(fp, "<font color = red>Warning: The total reaction volume is greater than the required reaction volume.</font><br>");
	else if (container1.volume < ((no_rows-1)*rxn_vol->value))
		fprintf(fp, "<font color = red>Warning: The total reaction volume is less than the required reaction volume.</font><br>");
	container1.contents.volume = container1.volume;
	}

void mixing_table(int no_rows,int no_columns, Fluid array1[], char* tube[], Volume* volumes[], Volume* rxn_vol, Container& container1)
	{
	int i,j;
	int flag = 0;
	int make_vol_up = 0;
	float sum = 0;
	for(i=0; i<no_columns-1;i++)
		{
		if (usage_list_fluids[array1[i].usage_index].original_name == array1[i].original_name)
			usage_list_fluids[array1[i].usage_index].used = 1;
		else 
			{
			array1[i].used = 1;
			usage_list_fluids[list_fluid_no] = array1[i];
			array1[i].usage_index = list_fluid_no;
			list_fluid_no++;
			}	
		}
	if (usage_list_containers[container1.usage_index].name == container1.name)
		usage_list_containers[container1.usage_index].used = 1;
	else 
		{
		container1.used = 1;
		usage_list_containers[list_container_no] = container1;
		container1.usage_index = list_container_no;
		list_container_no++;
		}
	if(no_rows-1 == 1)
		fprintf(fp, "Use the following table as a checklist for preparing the reaction in %s:<br><br>", container1.name);
	else 
		fprintf(fp, "Use the following table as a checklist for preparing the reactions in %s:<br><br>", container1.name);
	fprintf(fp, "<table border cellpadding=5 rules=all frame=void bordercolor=#357EC7>");
	for(i=0; i<no_rows; i++)
		{
		for(j=0; j<no_columns; j++)
			{
			if((i == 0) && (j == 0))
				fprintf(fp, "<thead><tr><td>&nbsp;</td>");

			else if((i == 0) && (j!=0))
				{
				fprintf(fp,"<td><font color=#357EC7>%s</font></td>", (array1[j-1]).new_name);
				if (j == no_columns-1)
					fprintf(fp,"</tr></thead>");
				}
			else if((i!=0) && (j == 0))
				fprintf(fp, "<tbody><tr><td><font color=#357EC7>%s</font></td>", tube[i-1]);

			else 
				{
				if((i == no_rows-1) && (j == no_columns-1) && (volumes[j-1]->value == XVAL))//||(*(vol+(no_columns-1)*(i-1)+(j-1))!='--')))
					{
					make_vol_up = 1;
					fprintf(fp, "<td><b><font color=#357EC7>Make up the volume to ");
					rxn_vol->display_vol();
					fprintf(fp, "</font></b></td>");
					}
				else if (volumes[i]->value == NA)
					{
					fprintf(fp, "<td><b><font color=#357EC7>--</font></b></td>");	
					volumes[i]->value = 0;
					}
				else if (volumes[i]->value == XVAL)
					{
					fprintf(fp, "<td><b><font color=#357EC7>X</font></b></td>");
					volumes[i]->value = DEFAULT;
					}
				else
					{
					fprintf(fp, "<td><b>");
					volumes[j-1]->display_vol();
					fprintf(fp, "</td>");
					}
				if (j == no_columns-1)
					fprintf(fp, "</tr></body>");

				}
			}
		}
	fprintf(fp, "</table>");
	name_sample(container1, "reaction mix");
	container1.volume = 0;
	if (make_vol_up == 1)
		{
		for (j=0; j<no_columns-2;j++)
			{
			if (volumes[j]->value == DEFAULT||volumes[j]->value < 0)
				break;
			sum = sum + volumes[j]->value;
			}
		volumes[no_columns-2]->value = rxn_vol->value - sum;
		}
	Operation o = new_operation("mixing table");
	for (i=0; i<no_columns-1;i++)
		{
		if (volumes[i]->value == DEFAULT)
			break;
		else
			{
			if ((array1[i].volume == 0)|| (array1[i].volume < (volumes[i]->value*volumes[i]->mul)))
				{
				fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", array1[i].new_name);

				}
			else if (volumes[i]->value < 0)
				{
				fprintf(fp, "<font color = red>Warning: Negative volume input.</font><br>");
				break;
				}
			else
				{
				flag++;
				container1.volume = container1.volume + volumes[i]->value;
				}
			}
		}
	for(i=0; i<no_columns-1; i++)
		{
		// graph maintenance
		create_edge_from_fluids(&array1[i], &o);
		}
	create_edge_from_fluid_to_container(&o, &container1);
	if (flag == (no_columns-1))
		{
		if (container1.volume > rxn_vol->value)
			fprintf(fp, "<font color = red>Warning: The total reaction volume is greater than the required reaction volume.</font><br>");
		else if (container1.volume < rxn_vol->value)
			fprintf(fp, "<font color = red>Warning: The total reaction volume is less than the required reaction volume.</font><br>");
		}
	container1.contents.volume = container1.volume;
	}

Plate new_plate(char *name)
	{
	char* t1 = "";
	Plate result = {0};
	result.state ="";
	result.new_name = name;
	result.original_name = name;
	result.type = SOLID;
	fprintf(fp, "<li>%s</li>", name);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}


Plate new_plate(char*name, char* state)
	{
	char* t1 = "";
	Plate result = {0};
	result.new_name = name;
	result.original_name = name;
	result.state = state;
	result.type = SOLID;
	fprintf(fp, "<li> <a name=\"%s\">%s <i><br><tab>(%s)<br></i></a></li>", name, name, state);
	result.used = 0;
	usage_list_fluids[list_fluid_no] = result;
	result.usage_index = list_fluid_no;
	list_fluid_no++;
	return result;
	}


void plate_out(Plate& plate1, Container& container1, Volume* volume1)
	{
	// graph maintenance
		{
		Operation o = new_operation("plate out");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluids(&o, &plate1);
		}
		if ((container1.volume == 0)|| (container1.volume < (volume1->value*volume1->mul)))
			{
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", container1.contents.new_name);

			}
		if (usage_list_fluids[plate1.usage_index].original_name == plate1.original_name)
			usage_list_fluids[plate1.usage_index].used = 1;
		else 
			{
			plate1.used = 1;
			usage_list_fluids[list_fluid_no] = plate1;
			plate1.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Plate out ");
		volume1->display_vol();
		fprintf(fp, " of %s onto ", container1.contents.new_name);
		if(plate1.state == "")
			fprintf(fp, "<font color=#357EC7>%s</font>.<br>", plate1.new_name);
		else
			fprintf(fp, "<a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", plate1.new_name, plate1.new_name);
		container1.volume = container1.volume - volume1->value * volume1->mul;
	}

void plate_out(Plate& plate1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("plate out");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluids(&o, &plate1);
		}
		if (usage_list_fluids[plate1.usage_index].original_name == plate1.original_name)
			usage_list_fluids[plate1.usage_index].used = 1;
		else 
			{
			plate1.used = 1;
			usage_list_fluids[list_fluid_no] = plate1;
			plate1.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		if(plate1.state == "")	
			fprintf(fp, "Plate out %s onto <font color=#357EC7>%s</font>.<br>", container1.contents.new_name, plate1.new_name);
		else
			fprintf(fp, "Plate out %s onto <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", container1.contents.new_name, plate1.new_name, plate1.new_name);
		container1.volume = 0;
	}

void name_container(Container& container1, char* name)
	{
	fprintf(fp, "Set aside a fresh %s. Call it %s. <br>", container1.name, name);
	container1.name = name;
	usage_list_containers[container1.usage_index].name = name;
	if (usage_list_containers[container1.usage_index].name == container1.name)
		usage_list_containers[container1.usage_index].used = 1;
	else 
		{
		container1.used = 1;
		usage_list_containers[list_container_no] = container1;
		container1.usage_index = list_container_no;
		list_container_no++;
		}
	}

void name_plate(Plate& plate1, char* name)
	{
	plate1.new_name = name;
	}

void thermocycler (Container& master_mix, int cycles, float temp1, Time* time1, float temp2, Time* time2, float temp3, Time* time3, enum pcr_type type)
	{
	int ttr1 = total_time_required;
	int ttr2;
	if(type == NORMAL)
		{
		fprintf(fp, "Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>%d</font></b></li>", cycles);
		fprintf(fp, "<li>Denature: <b><font color=#357EC7>%g%cC</font></b>, ", temp1, 0x00B0);
		time1->display_time();
		fprintf(fp, "</li> <li> Anneal: <b><font color=#357EC7>%g%cC</font></b>, ", temp2, 0x00B0);
		time2->display_time();
		fprintf(fp, "</li> <li>Elongate: <b><font color=#357EC7>%g%cC</font></b>, ", temp3, 0x00B0);
		time3->display_time();
		fprintf(fp, "</li></ul>");
		ttr2 = total_time_required-ttr1;
		total_time_required = total_time_required + ttr2 * cycles;
		}
	else if(type == COLONY)
		fprintf(fp, "Carry out colony PCR of %s.<br>", master_mix.contents.new_name);
	if(thermocycler_no == 1)
		{
		equipments[equip_no] = "Thermocycler";
		equip_no++;
		thermocycler_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("thermocycler");
		create_edge_from_container_to_fluid(&master_mix, &o);
		master_mix.node = create_node("container with thermocycled contents");
		create_edge(o.node, master_mix.node);
		o.node->time = (time1->value*time1->mul + time2->value*time2->mul + time3->value*time3->mul) * cycles;
		o.node->unit = "secs";
		o.node->pcr_or_centrifuge = 1;
		}
	}

void thermocycler (Container& master_mix, int cycles, float temp1, Time* time1, float temp2, Time* time2, float temp3, Time* time3, enum pcr_type type, float grad_temp)
	{
	int ttr1 = total_time_required;
	int ttr2;
	if(type == NORMAL)
		{
		fprintf(fp, "Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>%d</font></b></li>", cycles);
		fprintf(fp, "<li>Denature: <b><font color=#357EC7>%g%cC</font></b>, ", temp1, 0x00B0);
		time1->display_time();
		fprintf(fp, "</li> <li> Anneal: <b><font color=#357EC7>%g%cC</font></b>, ", temp2, 0x00B0);
		time2->display_time();
		fprintf(fp, "</li> <li>Elongate: <b><font color=#357EC7>%g%cC</font></b>, ", temp3, 0x00B0);
		time3->display_time();
		fprintf(fp, "</li></ul>");
		ttr2 = total_time_required-ttr1;
		total_time_required = total_time_required + ttr2 * cycles;
		}
	else if(type == GRADIENT)
		{
		fprintf(fp, "Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>%d</font></b></li>", cycles);
		fprintf(fp, "<li>Denature: <b><font color=#357EC7>%g%cC</font></b>, ", temp1, 0x00B0);
		time1->display_time();
		fprintf(fp, "</li> <li> Anneal: <b><font color=#357EC7>%g%cC- %g%cC (gradient PCR)</font></b>, ", temp2, 0x00B0, grad_temp, 0x00B0);
		time2->display_time();
		fprintf(fp, "</li> <li>Elongate: <b><font color=#357EC7>%g%cC</font></b>, ", temp3, 0x00B0);
		time3->display_time();
		fprintf(fp, "</li></ul>");
		ttr2 = total_time_required-ttr1;
		total_time_required = total_time_required + ttr2 * cycles;
		}
	else if(type == COLONY)
		fprintf(fp, "Carry out colony PCR of %s.<br>", master_mix.contents.new_name);

	if(thermocycler_no == 1)
		{
		equipments[equip_no] = "Thermocycler";
		equip_no++;
		thermocycler_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("thermocycler");
		create_edge_from_container_to_fluid(&master_mix, &o);
		master_mix.node = create_node("container with thermocycled contents");
		create_edge(o.node, master_mix.node);
		o.node->time = (time1->value*time1->mul + time2->value*time2->mul + time3->value*time3->mul) * cycles;
		o.node->unit = "secs";
		o.node->pcr_or_centrifuge = 1;
		}
	}

void thermocycler (Container& master_mix, int cycles, float temp1, Time* time1, float temp2, Time* time2)
	{
	int ttr1 = total_time_required;
	int ttr2;
	fprintf(fp, "Thermocycling<br><ul><li>No. of cycles: <b><font color=#357EC7>%d</font></b></li>", cycles);
	fprintf(fp, "<li>Denature: <b><font color=#357EC7>%g%cC</font></b>, ", temp1, 0x00B0);
	time1->display_time();
	fprintf(fp, "</li> <li> Anneal: <b><font color=#357EC7>%g%cC</font></b>, ", temp2, 0x00B0);
	time2->display_time();
	fprintf(fp, "</li></ul>");
	ttr2 = total_time_required-ttr1;
	total_time_required = total_time_required + ttr2 * cycles;
	if(thermocycler_no == 1)
		{
		equipments[equip_no] = "Thermocycler";
		equip_no++;
		thermocycler_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("thermocycler");
		create_edge_from_container_to_fluid(&master_mix, &o);
		master_mix.node = create_node("container with thermocycled contents");
		create_edge(o.node, master_mix.node);
		o.node->time = (time1->value*time1->mul + time2->value*time2->mul) * cycles;
		o.node->unit = "secs";
		o.node->pcr_or_centrifuge = 1;
		}
	}

void thermocycler (Container& master_mix, float temp1, Time* time1, float temp2, Time* time2)
	{
	fprintf(fp, "Set the thermocycler to run the following program:<br>");

	fprintf(fp, "<ul><li><b><font color=#357EC7>%g%cC</font></b>, ", temp1, 0x00B0);
	time1->display_time();
	fprintf(fp, "</li> <li><b><font color=#357EC7>%g%cC</font></b>, ", temp2, 0x00B0);
	time2->display_time();
	fprintf(fp, "</li></ul>");
	if(thermocycler_no == 1)
		{
		equipments[equip_no] = "Thermocycler";
		equip_no++;
		thermocycler_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("thermocycler");
		create_edge_from_container_to_fluid(&master_mix, &o);
		master_mix.node = create_node("container with thermocycled contents");
		create_edge(o.node, master_mix.node);
		o.node->time = (time1->value*time1->mul + time2->value*time2->mul);
		o.node->unit = "secs";
		o.node->pcr_or_centrifuge = 1;
		}
	}

void thermocycler (Plate& plate1, enum pcr_type type)
	{
	// graph maintenance
		{
		Operation o = new_operation("thermocycler");
		create_edge_from_fluids(&plate1, &o);
		plate1.node = create_node("container with thermocycled contents");
		create_edge(o.node, plate1.node);
		o.node->pcr_or_centrifuge = 1;
		}
		if(type == COLONY)
			fprintf(fp, "Carry out colony PCR on %s.<br>", plate1.new_name);

		if(thermocycler_no == 1)
			{
			equipments[equip_no] = "Thermocycler";
			equip_no++;
			thermocycler_no++;
			}
	}

void thermocycler(Container& container1, float temp1, Time* time1)
	{
	fprintf(fp, "Set the thermocycler to run the following program:<br>");
	fprintf(fp, "<ul><li><b><font color=#357EC7>%g%cC</font></b>, ", temp1, 0x00B0);
	time1->display_time();
	fprintf(fp, "</li></ul>");
	if(thermocycler_no == 1)
		{
		equipments[equip_no] = "Thermocycler";
		equip_no++;
		thermocycler_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("thermocycler");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with thermocycled contents");
		create_edge(o.node, container1.node);
		o.node->time = time1->value*time1->mul;
		o.node->unit = "secs";
		o.node->pcr_or_centrifuge = 1;
		}
	}

void ce_detect (Container& container1, float length, float volt_per_cm, Fluid& fluid1)
	{
	// graph maintenance
		{
		Operation o = new_operation("ce detect");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluids(&fluid1, &o);
		container1.node = create_node("results of capillary electrophoresis");
		create_edge(o.node, container1.node);
		}
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}	
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Detect/separate %s by capillary electrophoresis with the following settings - <b><font color=#357EC7>%g</font></b> cm at <b><font color=#357EC7>%g</font></b> V/cm using %s.<br>", container1.contents.new_name, length, volt_per_cm, fluid1);
		name_sample(container1, "separated flow");
	}

void ce_detect (Container& container1, float length, float volt_per_cm, Fluid& fluid1, Time* time1)
	{
	if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
		usage_list_fluids[fluid1.usage_index].used = 1;
	else 
		{
		fluid1.used = 1;
		usage_list_fluids[list_fluid_no] = fluid1;
		fluid1.usage_index = list_fluid_no;
		list_fluid_no++;
		}	
	if (usage_list_containers[container1.usage_index].name == container1.name)
		usage_list_containers[container1.usage_index].used = 1;
	else 
		{
		container1.used = 1;
		usage_list_containers[list_container_no] = container1;
		container1.usage_index = list_container_no;
		list_container_no++;
		}
	fprintf(fp, "Detect/separate %s by capillary electrophoresis with the following settings - <b><font color=#357EC7>%g</font></b> cm at <b><font color=#357EC7>%g</font></b> V/cm using %s for ", container1.contents.new_name, length, volt_per_cm, fluid1.new_name);
	time1->display_time();
	fprintf(fp, ".<br>");
	name_sample(container1, "separated flow");
	// graph maintenance
		{
		Operation o = new_operation("ce detect");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluids(&fluid1, &o);
		container1.node = create_node("results of capillary electrophoresis");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void measure_fluorescence (Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure fluorescence");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("results of fluorescence measurement");
		create_edge(o.node, container1.node);
		}
		fprintf(fp, "Measure the fluorescence of %s.<br>", container1.contents.new_name);

	}

Column new_column(char*name)
	{
	Column result = {0};
	result.name = name;
	fprintf(fp, "<li>%s</li>", name);
	result.used = 0;
	usage_list_containers[list_container_no] = result;
	result.usage_index = list_container_no;
	list_container_no++;
	return result;
	}

void centrifuge_column(Column& column1, Speed* speed1, float temp, Time* time1)
	{
	fprintf(fp, "Centrifuge "); 
	check_container(column1);
	fprintf(fp, " at ");
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp==RT);
	else
		fprintf(fp, "at <b><font color=#357EC7>%g%cC</font></b>s", temp, 0x00B0);
	fprintf(fp, ".<br>");
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	fprintf(fp, "Discard the flow-through.<br>");	
	column1.volume = 0;
	name_column(column1, "column");	
	// graph maintenance
		{
		Operation o = new_operation("centrifuge column");
		create_edge_from_container_to_fluid(&column1, &o);
		column1.node = create_node("centrifuged column");
		create_edge(o.node, column1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void centrifuge_column(Column& column1, float temp, Time* time1)
	{
	fprintf(fp, "Microfuge "); 
	check_container(column1);
	fprintf(fp, " for ");
	time1->display_time();
	if(temp==RT);
	else
		fprintf(fp, "at <b><font color=#357EC7>%g%cC</font></b>s", temp, 0x00B0);
	fprintf(fp, ".<br>");
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	fprintf(fp, "Discard the flow-through.<br>");	
	column1.volume = 0;
	name_column(column1, "column");
	// graph maintenance
		{
		Operation o = new_operation("centrifuge column");
		create_edge_from_container_to_fluid(&column1, &o);
		column1.node = create_node("centrifuged column");
		create_edge(o.node, column1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void centrifuge_flow_through(Column& column1, Speed* speed1, float temp, Time* time1, Container& container1)
	{
	fprintf(fp, "Centrifuge "); 
	check_container(column1);
	fprintf(fp, " at ");
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	if(temp==RT);
	else
		fprintf(fp, "at <b><font color=#357EC7>%g%cC</font></b>s", temp, 0x00B0);
	fprintf(fp, ".<br>");

	fprintf(fp, "Discard the column.<br>");
	if(centrifuge_no == 1)
		{
		equipments[equip_no] = "Centrifuge";
		equip_no++;
		centrifuge_no++;
		}
	container1.contents.new_name = "flow-through";
	container1.volume = column1.volume;
	column1.volume = 0;
	// graph maintenance
		{
		Operation o = new_operation("centrifuge flow through");
		create_edge_from_container_to_fluid(&column1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void centrifuge_flow_through(Column& column1, float temp, Time* time1, Container& container1)
	{
	fprintf(fp, "Centrifuge "); 
	check_container(column1);
	fprintf(fp, " at ");
	time1->display_time();
	if(temp==RT);
	else
		fprintf(fp, "at <b><font color=#357EC7>%g%cC</font></b>s", temp, 0x00B0);
	fprintf(fp, ".<br>");
	fprintf(fp, "Discard the column.<br>");
	if(microfuge_no == 1)
		{
		equipments[equip_no] = "Microfuge";
		equip_no++;
		microfuge_no++;
		}

	container1.contents.new_name = "flow-through";
	container1.volume = column1.volume;
	column1.volume = 0;
	// graph maintenance
		{
		Operation o = new_operation("centrifuge flow through");
		create_edge_from_container_to_fluid(&column1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void add_to_column(Column& column1, Fluid& fluid1, Volume* volume1)
	{
	// graph maintenance
		{
		Operation o = new_operation("add to column");
		create_edge_from_fluids(&fluid1, &o);
		create_edge_from_fluid_to_container(&o, &column1);
		}
		if ((fluid1.volume == 0)|| (fluid1.volume < (volume1->value*volume1->mul)))
			{
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", fluid1.new_name);

			}
		if (usage_list_containers[column1.usage_index].name == column1.name)
			usage_list_containers[column1.usage_index].used = 1;
		else 
			{
			column1.used = 1;
			usage_list_containers[list_container_no] = column1;
			column1.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		fprintf(fp, "Add ");
		volume1->display_vol();
		if(fluid1.state == "")
			fprintf(fp, " of <font color=#357EC7>%s</font> to %s.<br>", fluid1.new_name, column1.name);
		else 
			fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> to %s.<br>", fluid1.new_name, fluid1.new_name, column1.name);
		column1.contents = fluid1;
		column1.volume = column1.volume + volume1->value * volume1->mul;
		fluid1.volume = fluid1.volume - volume1->value * volume1->mul;
	}

void add_to_column(Column& column1, Container& container1, Volume* volume1)
	{
	// graph maintenance
		{
		Operation o = new_operation("add to column");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluid_to_container(&o, &column1);
		}
		if ((container1.volume == 0)|| (container1.volume < (volume1->value*volume1->mul)))
			{
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", container1.contents.new_name);

			}
		if (usage_list_containers[column1.usage_index].name == column1.name)
			usage_list_containers[column1.usage_index].used = 1;
		else 
			{
			column1.used = 1;
			usage_list_containers[list_container_no] = column1;
			column1.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Add ");
		volume1->display_vol();
		if(container1.contents.state == "")
			fprintf(fp, " of <font color=#357EC7>%s</font> %s.<br>", container1.contents.new_name);
		else
			fprintf(fp, " of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> %s.<br>", container1.contents.new_name, container1.contents.new_name);
		column1.contents = container1.contents;
		column1.volume = column1.volume + volume1->value * volume1->mul;
		container1.volume = container1.volume - volume1->value * volume1->mul;
	}

void add_to_column(Column& column1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("add to column");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluid_to_container(&o, &column1);
		}
		if (usage_list_containers[column1.usage_index].name == column1.name)
			usage_list_containers[column1.usage_index].used = 1;
		else 
			{
			column1.used = 1;
			usage_list_containers[list_container_no] = column1;
			column1.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Add <font color=#357EC7>%s</font> to %s.<br>", container1.contents.new_name, column1.name);
		column1.contents = container1.contents;
		column1.volume = container1.volume;
		container1.volume = 0;
	}

void transfer_column(Column& column1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("transfer column");
		create_edge_from_container_to_fluid(&column1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if (usage_list_containers[column1.usage_index].name == column1.name)
			usage_list_containers[column1.usage_index].used = 1;
		else 
			{
			column1.used = 1;
			usage_list_containers[list_container_no] = column1;
			column1.usage_index = list_container_no;
			list_container_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Transfer %s into %s.<br>", column1.name, container1.name);
	}

void vortex_column(Column& column1)
	{
	// graph maintenance
		{
		Operation o = new_operation("vortex column");
		create_edge_from_container_to_fluid(&column1, &o);
		column1.node = create_node("vortexed column");
		create_edge(o.node, column1.node);
		}
		fprintf(fp,"Vortex %s gently.<br>",column1.name);
	}

void drain(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("drain column");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("drained column");
		create_edge(o.node, container1.node);
		}
		fprintf(fp,"Drain %s.<br>", container1.name);
		container1.volume = 0;
	}

void wash_cells(Container& chamber, Fluid& sample1)
	{
	// graph maintenance
		{
		Operation o = new_operation("wash cells");
		create_edge_from_fluids(&sample1, &o);
		create_edge_from_fluid_to_container(&o, &chamber);
		}
		usage_list_fluids[sample1.usage_index].used = 1;
		usage_list_containers[chamber.usage_index].used = 1;
		fprintf(fp,"Wash cells with %s.<br>", sample1.new_name);
	}

void collect_tissue(Tissue& sample1, float wt, enum weight_unit unit, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("collect tissue");
		create_edge_from_fluids(&sample1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		int mul;
		char* wu;
		if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
			usage_list_fluids[sample1.usage_index].used = 1;
		else 
			{
			sample1.used = 1;
			usage_list_fluids[list_fluid_no] = sample1;
			sample1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		switch(unit)
			{
			case UG: wu="µg";mul = MICRO;break;
			case MG: wu="mg";mul = MICRO; break;
			case GR: wu="g";mul = MICRO;break;
			default: break;
			}
		name_sample(container1, "tissue");
		fprintf(fp,"Collect <b><font color=#357EC7>%g %s</font></b> of %s in  %s. <br>", wt, wu, sample1.new_name, container1.name);
		container1.contents = sample1;
		container1.volume = wt*mul;
	}

void collect_tissue(Tissue& sample1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("collect tissue");
		create_edge_from_fluids(&sample1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
			usage_list_fluids[sample1.usage_index].used = 1;
		else 
			{
			sample1.used = 1;
			usage_list_fluids[list_fluid_no] = sample1;
			sample1.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		fprintf(fp,"Collect %s into %s and snap-freeze in liquid nitrogen.<br>", sample1.new_name, container1.name);
		sample1.volume = DEFAULT;
	}

void homogenize_tissue(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("homogenize tissue");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("homogenized tissue");
		create_edge(o.node, container1.node);
		}
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Pre-cool the mortars by pouring liquid nitrogen into them.<br>");
		fprintf(fp, "Add %s into the mortar containing liquid nitrogen and grind with a pestle until it is a fine powder.<br>", container1.contents.new_name);
		comment("Use gloves!");
		if(mortar_no==1)
			{
			equipments[equip_no]="Micropestles/ Mortar and pestle";
			equip_no++;
			mortar_no++;
			}
		name_sample(container1, "homogenized tissue");
	}

void homogenize_tissue(Container& container1, Fluid& fluid1)
	{
	// graph maintenance
		{
		Operation o = new_operation("homogenize tissue");
		create_edge_from_fluids(&fluid1, &o);
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("homogenized tissue");
		create_edge(o.node, container1.node);
		}
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		fprintf(fp, "Homogenize %s in %s.<br>", container1.contents.new_name, fluid1.new_name);
		name_sample(container1, "homogenized tissue");
		container1.volume = fluid1.volume;
		fluid1.volume = 0;
	}

void dry_pellet(Container& container1, drying type, Time* time1)
	{
	char* additional;
	switch(type)
		{
		case IN_AIR: additional = "in air"; break;
		case IN_VACUUM: additional = "in vacuum"; break;
		default: additional = "in air"; break;
		}
	fprintf(fp,"Dry the pellet %s for ", additional);
	time1->display_time();
	fprintf(fp, ".<br>");
	container1.volume = 0;
	// graph maintenance
		{
		Operation o = new_operation("dry pellet");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with dried pellet");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void dry_pellet(Container& container1, drying type)
	{
	char* additional;
	// graph maintenance
		{
		Operation o = new_operation("dry pellet");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with dried pellet");
		create_edge(o.node, container1.node);
		}
		switch(type)
			{
			case IN_AIR: additional = "in air"; break;
			case IN_VACUUM: additional = "in vacuum"; break;
			default: additional = "in air"; break;
			}
		fprintf(fp,"Dry the pellet %s. <br>",additional);
		container1.volume = 0;
	}

void mixing_table_pcr(int no_rows, Volume* rxn_vol, Fluid array1[], char*initial_conc[], char* final_conc[], Volume* vol[], Container& container1)
	{
	int i,j;
	int flag = 0;
	int make_vol_up = 0;
	float sum = 0;
	for(i=0; i<no_rows-1;i++)
		{
		if (usage_list_fluids[array1[i].usage_index].original_name == array1[i].original_name)
			usage_list_fluids[array1[i].usage_index].used = 1;
		else 
			{
			array1[i].used = 1;
			usage_list_fluids[list_fluid_no] = array1[i];
			array1[i].usage_index = list_fluid_no;
			list_fluid_no++;
			}	
		}
	if (usage_list_containers[container1.usage_index].name == container1.name)
		usage_list_containers[container1.usage_index].used = 1;
	else 
		{
		container1.used = 1;
		usage_list_containers[list_container_no] = container1;
		container1.usage_index = list_container_no;
		list_container_no++;
		}
	fprintf(fp,"Set up a reaction in %s as follows on ice:<br><br>", container1.name);
	fprintf(fp,"<table border cellpadding=5 rules=all frame=void bordercolor=#357EC7>");
	for(i=0;i<no_rows;i++)
		{
		for(j=0;j<4;j++)
			{
			if((i==0)&&(j==0))
				{
				fprintf(fp,"<thead><tr><td>&nbsp;</td>");
				}
			else if((i==0)&&(j!=0))
				{
				if(j==1)
					{
					fprintf(fp,"<td><font color=#357EC7>Initial concentration</font></td>");
					}
				if(j==2)
					fprintf(fp,"<td><font color=#357EC7>Final concentration</font></td>");
				if (j==3)
					{
					fprintf(fp,"<td><font color=#357EC7>Final volume per ");
					rxn_vol->display_vol();
					fprintf(fp," reaction</font></td>");
					fprintf(fp,"</tr></thead>");
					}
				}
			else if((i!=0)&&(j==0))
				fprintf(fp,"<tbody><tr><td><font color=#357EC7>%s</font></td>",array1[i-1].new_name);
			else 
				{
				if(j==1)
					{
					fprintf(fp,"<td><b><font color=#357EC7>%s</font></b></td>",initial_conc[i-1]);
					}
				if(j==2)
					{
					fprintf(fp,"<td><b><font color=#357EC7>%s</font></b></td>",final_conc[i-1]);
					}
				if(j==3)
					{
					if ((i==no_rows-1)&&((vol[i-1]->value==XVAL)||(vol[i-1]->value==NA)))
						{
						make_vol_up = 1;
						fprintf(fp,"<td><b><font color=#357EC7>Make up volume to ");
						rxn_vol->display_vol();
						fprintf(fp, "</font></b></td>");
						}
					else if (vol[i-1]->value== XVAL)
						{
						fprintf(fp,"<td><b><font color=#357EC7>X</font></b></td>");
						vol[i-1]->value = DEFAULT;
						}
					else if (vol[i-1]->value== NA)
						{
						fprintf(fp,"<td><b><font color=#357EC7>--</font></b></td>");
						vol[i-1]->value = 0;
						}
					else
						{
						fprintf(fp,"<td><b>");
						vol[i-1]->display_vol();
						fprintf(fp,"</font></b></td>");
						}
					fprintf(fp,"</tr></body>");
					}


				}
			}
		}
	fprintf(fp,"</table>");
	name_sample(container1, "reaction mix");
	container1.volume = 0;
	if (make_vol_up == 1)
		{
		for (j=0; j<no_rows-2;j++)
			{
			if (vol[j]->value == DEFAULT||vol[j]->value < 0)
				break;
			sum = sum + vol[j]->value;
			}
		vol[no_rows-2]->value = rxn_vol->value - sum;
		}
	Operation o = new_operation("mixing table pcr");
	for (i=1; i<no_rows-1;i++)
		{
		if (vol[i]->value == DEFAULT)
			break;
		else
			{
			if ((array1[i].volume == 0)|| (array1[i].volume < (vol[i]->value*vol[i]->mul)))
				{
				fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", array1[i].new_name);

				}
			else if (vol[i]->value < 0)
				{
				fprintf(fp, "<font color = red>Warning: Negative volume input.</font><br>");
				break;
				}
			else
				{
				flag++;
				container1.volume = container1.volume + vol[i]->value;
				}
			}
		}
	for(i=0; i<no_rows-1; i++)
		{
		// graph maintenance
		create_edge_from_fluids(&array1[i], &o);
		}
	create_edge_from_fluid_to_container(&o, &container1);
	if (flag == (no_rows-1))
		{
		if (container1.volume > rxn_vol->value)
			fprintf(fp, "<font color = red>Warning: The total reaction volume is greater than the required reaction volume.</font><br>");
		else if (container1.volume < rxn_vol->value)
			fprintf(fp, "<font color = red>Warning: The total reaction volume is less than the required reaction volume.</font><br>");
		}
	container1.contents.volume = container1.volume;
	}

void electrophoresis(int no_samples, Container container_array[], float agar_conc, Volume* pdt_vol, Volume* loading_buff_vol)
	{
	int i;
	// graph maintenance
		{
		Operation o = new_operation("electrophoresis");
		for(i=0; i<no_samples; i++)
			create_edge_from_container_to_fluid(&container_array[i], &o);
		container_array[1].node = create_node("results of electrophoresis");
		create_edge(o.node, container_array[1].node);
		}
		fprintf(fp,"Perform %g&#37 agarose gel electrophoresis of ",agar_conc);
		if(no_samples == 2)
			fprintf(fp, "%s and %s (", container_array[0].contents.new_name, container_array[1].contents.new_name);
		else
			{
			for(i=0; i<no_samples;i++)
				{
				if(i=no_samples-1)
					fprintf(fp, "%s (", container_array[i].contents.new_name);        
				else if(i=no_samples-2)
					fprintf(fp, "%s and ", container_array[i].contents.new_name);
				else
					fprintf(fp, "%s, ", container_array[i].contents.new_name);
				container_array[i].contents.volume = container_array[i].contents.volume - pdt_vol->value * pdt_vol->mul;
				}       
			}
		pdt_vol->display_vol();
		fprintf(fp, " sample and ");
		loading_buff_vol->display_vol();
		fprintf(fp, " loading buffer), mixed with ethidium bromide and visualize with UV transilluminator to confirm the presence of required product.<br>");
		if(electrophoresis_no==1)
			{
			equipments[equip_no]="Electrophoretic unit";
			equip_no++;
			electrophoresis_no++;
			}
	}

void electrophoresis(Container& container1,float agar_conc)
	{
	// graph maintenance
		{
		Operation o = new_operation("electrophoresis");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("results of electrophoresis");
		create_edge(o.node, container1.node);
		}
		fprintf(fp,"Perform %g&#37 agarose gel electrophoresis of appropriate quantity of  %s mixed with ethidium bromide and visualize with UV transilluminator to confirm the presence of required product.<br>",agar_conc, container1.contents.new_name);
		if(electrophoresis_no==1)
			{
			equipments[equip_no]="Electrophoretic unit";
			equip_no++;
			electrophoresis_no++;
			}
	}

void electrophoresis(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("electrophoresis");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("results of electrophoresis");
		create_edge(o.node, container1.node);
		}
		fprintf(fp,"Perform agarose gel electrophoresis of appropriate quantity of  %s mixed with ethidium bromide and visualize with UV transilluminator to confirm the presence of required product.<br>", container1.contents.new_name);
		if(electrophoresis_no==1)
			{
			equipments[equip_no]="Electrophoretic unit";
			equip_no++;
			electrophoresis_no++;
			}
	}

void pcr_init_denat(Container& container1, float temp, Time* time1)
	{
	fprintf(fp, "Program a standard thermocycler to run the reaction using the following parameters:<br>");
	fprintf(fp, "Initial denaturation<br><ul><li>Denature: <b><font color=#357EC7>%g%cC</font></b>, ", temp, 0x00B0);
	time1->display_time();
	fprintf(fp, "</li></ul>");
	// graph maintenance
		{
		Operation o = new_operation("pcr init denat");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after PCR initial denaturation");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void pcr_final_ext(Container& sample1, float temp_ext, Time* time_ext, float temp_halt, Time* time_halt, float temp_hold)
	{
	fprintf(fp,"Termination<ul>");
	fprintf(fp,"<li>Elongate: <b><font color=#357EC7>%g%cC</font></b>, ", temp_ext, 0x00B0); 
	time_ext->display_time();
	fprintf(fp, "</li><li>Halt reaction: <b><font color=#357EC7>%g%cC</font></b>, ", temp_halt, 0x00B0);
	time_halt->display_time();
	fprintf(fp, "</li><li>Hold: <b><font color=#357EC7>%g%cC</font></b>, until removed from machine </li></ul>", temp_hold, 0x00B0);
	// graph maintenance
		{
		Operation o = new_operation("pcr final ext");
		create_edge_from_container_to_fluid(&sample1, &o);
		sample1.node = create_node("container after PCR final extension");
		create_edge(o.node, sample1.node);
		o.node->time = time_ext->value + time_halt->value;
		o.node->unit = time_ext->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void pcr_final_ext(Container& sample1, float temp_ext, Time* time_ext, float temp_hold)
	{
	fprintf(fp,"Termination<ul>");
	fprintf(fp,"<li>Elongate: <b><font color=#357EC7>%g%cC</font></b>, ", temp_ext, 0x00B0); 
	time_ext->display_time();
	fprintf(fp, "</li><li>Hold: <b><font color=#357EC7>%g%cC</font></b>, until removed from machine </li></ul>", temp_hold, 0x00B0);
	// graph maintenance
		{
		Operation o = new_operation("pcr final ext");
		create_edge_from_container_to_fluid(&sample1, &o);
		sample1.node = create_node("container after PCR final extension");
		create_edge(o.node, sample1.node);
		o.node->time = time_ext->value;
		o.node->unit = time_ext->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void centrifuge_phases_bottom(Container& container1, Speed* speed1, float temp, Time* time1, Volume* aspirate_vol, Volume* leave_behind)
	{
	fprintf(fp, "Centrifuge "); 
	check_container(container1);
	fprintf(fp, " at ");
	speed1->display_speed();
	fprintf(fp, " for ");
	time1->display_time();
	fprintf(fp, " and gently aspirate out ");
	aspirate_vol->display_vol();
	fprintf(fp, " of the supernatant and discard it.<br>Leave behind ");
	leave_behind->display_vol();
	fprintf(fp, ".<br>");

	name_sample(container1, "remaining portion");
	container1.volume = leave_behind->value;
	// graph maintenance
		{
		Operation o = new_operation("centrifuge phases -- bottom");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with bottom layer");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		o.node->pcr_or_centrifuge = 1;
		}
	}

void patch(Plate& plate1,Plate& colony)
	{
	// graph maintenance
		{
		Operation o = new_operation("patch");
		create_edge_from_fluids(&colony, &o);
		create_edge_from_fluids(&o, &plate1);
		}
		if (usage_list_fluids[plate1.usage_index].original_name == plate1.original_name)
			usage_list_fluids[plate1.usage_index].used = 1;
		else 
			{
			plate1.used = 1;
			usage_list_fluids[list_fluid_no] = plate1;
			plate1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_fluids[colony.usage_index].original_name == colony.original_name)
			usage_list_fluids[colony.usage_index].used = 1;
		else 
			{
			colony.used = 1;
			usage_list_fluids[list_fluid_no] = colony;
			colony.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		fprintf(fp,"Patch <font color=#357EC7>%s</font> onto %s.<br>", colony.new_name, plate1.new_name);
	}

void incubate_plate(Plate& plate1, float temp, Time* time1)
	{
	if (usage_list_fluids[plate1.usage_index].original_name == plate1.original_name)
		usage_list_fluids[plate1.usage_index].used = 1;
	else 
		{
		plate1.used = 1;
		usage_list_fluids[list_fluid_no] = plate1;
		plate1.usage_index = list_fluid_no;
		list_fluid_no++;
		}
	if(temp == RT)
		fprintf(fp, "Incubate %s at <b><font color=#357EC7><b><font color=#357EC7>room temperature</font></b></font></b> for ", plate1.new_name);
	else if((temp == ON_ICE)||(temp == 0))
		fprintf(fp, "Incubate %s on <b><font color=#357EC7><b><font color=#357EC7>ice</font></b></font></b> for ", plate1.new_name);
	else
		fprintf(fp, "Incubate %s at <b><font color=#357EC7>%g%cC</font></b> for ", plate1.new_name, temp, 0x00B0);
	time1->display_time();
	fprintf(fp, ".<br>");
	if(incubator_no == 1)
		{
		equipments[equip_no] = "Incubator";
		equip_no++;
		incubator_no++;
		}
	// graph maintenance
		{
		Operation o = new_operation("incubate plate");
		create_edge_from_fluids(&plate1, &o);
		plate1.node = create_node("incubated plate");
		create_edge(o.node, plate1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void sequencing(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("sequencing");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("results of sequencing");
		create_edge(o.node, container1.node);
		}
		fprintf(fp,"Dilute %s to <font color=#357EC7>100ng/ µl</font> and send <font color=#357EC7>1 µg (10 µL)</font> for sequencing.<br>", container1.contents.new_name);
	}

void weigh(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("weigh");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("results of weighing");
		create_edge(o.node, container1.node);
		}
		fprintf(fp, "Weigh the amount of %s present.<br>", container1.contents.new_name);
	}

void thermocycler_anneal(Container& container1, float cool_to_temp, float gradient, Time* time1)
	{
	if (cool_to_temp==RT)
		fprintf(fp,"Cool %s to room temperature at the rate of <b><font color=#357EC7>%g%cC</font></b> every ", container1.contents.new_name, gradient, 0x00B0);
	else
		fprintf(fp,"Cool %s to <b><font color=#357EC7>%g%cC</font></b> at the rate of <b><font color=#357EC7>%g%cC</font></b> every ", container1.contents.new_name, cool_to_temp, 0x00B0, gradient, 0x00B0);
	time1->display_time();
	fprintf(fp, ".<br>");
	// graph maintenance
		{
		Operation o = new_operation("thermocycler anneal");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container after products have annealed");
		create_edge(o.node, container1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

Slide new_slide(char* name)
	{
	Slide result = {0};
	result.name = name;
	result.used = 0;
	usage_list_containers[list_container_no] = result;
	result.usage_index = list_container_no;
	list_container_no++;
	return result;
	}

void immerse_slide(Slide& slide1,Fluid& sample1, Time* time1, enum func function, int no_of_times)
	{
	char* noft;
	int ttr1 = total_time_required;
	int ttr2;
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[slide1.usage_index].name == slide1.name)
		usage_list_containers[slide1.usage_index].used = 1;
	else 
		{
		slide1.used = 1;
		usage_list_containers[list_container_no] = slide1;
		slide1.usage_index = list_container_no;
		list_container_no++;
		}
	switch(no_of_times)
		{
		case 1: noft = "";break;
		case 2: noft = "twice";break;
		case 3: noft = "thrice";break;
		case 4: noft = "four times";break;
		case 5: noft = "five times";break;
		default: noft = "";break;
		}

	if (sample1.state == "")
		{
		switch(function)
			{
			case DEWAX: fprintf(fp, "Dewax %s in <b><font color=#357EC7>%s</font></b> for ",slide1.name, sample1.new_name);break;
			default:break;
			}
		}
	else{
		switch(function)
			{
			case DEWAX: fprintf(fp, "Dewax %s in <a href=\"#%s\" >%s</a> for ", slide1.name, sample1.new_name, sample1.new_name);break;
			default:break;
			}
		}
	time1->display_time();
	fprintf(fp, " %s.<br>", noft);
	ttr2 = total_time_required-ttr1;
	total_time_required = total_time_required + ttr2 * no_of_times;
	// graph maintenance
		{
		Operation o = new_operation("immerse slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide immersed");
		create_edge(o.node, slide1.node);
		o.node->time = time1->value * no_of_times;
		o.node->unit = time1->unit;
		}
	}

void immerse_slide(Slide& slide1, Fluid& sample1, Time* time1, int no_of_times)
	{
	char *noft;
	int ttr1 = total_time_required;
	int ttr2;
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[slide1.usage_index].name == slide1.name)
		usage_list_containers[slide1.usage_index].used = 1;
	else 
		{
		slide1.used = 1;
		usage_list_containers[list_container_no] = slide1;
		slide1.usage_index = list_container_no;
		list_container_no++;
		}
	switch(no_of_times)
		{
		case 1: noft = "";break;
		case 2: noft = "twice";break;
		case 3: noft = "thrice";break;
		case 4: noft = "four times";break;
		case 5: noft = "five times";break;
		default: noft = "";break;
		}

	if (sample1.state == "")
		fprintf(fp, "Immerse %s in <b><font color=#357EC7>%s</font></b> for ", slide1.name, sample1.new_name);
	else 
		fprintf(fp, "Immerse %s in <a href=\"#%s\" >%s</a> for ", slide1.name, sample1.new_name, sample1.new_name);
	time1->display_time();
	fprintf(fp, " %s.<br>", noft);
	ttr2 = total_time_required-ttr1;
	total_time_required = total_time_required + ttr2 * no_of_times;
	// graph maintenance
		{
		Operation o = new_operation("immerse slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide immersed");
		create_edge(o.node, slide1.node);
		o.node->time = time1->value * no_of_times;
		o.node->unit = time1->unit;
		}
	}

void immerse_slide(Slide& slide1,Fluid& sample1, int no_of_times)
	{
	// graph maintenance
		{
		Operation o = new_operation("immerse slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide immersed");
		create_edge(o.node, slide1.node);
		}
		char* noft;
		int ttr1 = total_time_required;
		int ttr2;
		if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
			usage_list_fluids[sample1.usage_index].used = 1;
		else 
			{
			sample1.used = 1;
			usage_list_fluids[list_fluid_no] = sample1;
			sample1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[slide1.usage_index].name == slide1.name)
			usage_list_containers[slide1.usage_index].used = 1;
		else 
			{
			slide1.used = 1;
			usage_list_containers[list_container_no] = slide1;
			slide1.usage_index = list_container_no;
			list_container_no++;
			}
		switch(no_of_times)
			{
			case 1: noft = "";break;
			case 2: noft = "twice";break;
			case 3: noft = "thrice";break;
			case 4: noft = "four times";break;
			case 5: noft = "five times";break;
			default: noft = "";break;
			}

		if ((sample1.state == "")&&(no_of_times == 1))
			fprintf(fp, "Immerse %s in %s.<br>", slide1.name, sample1.new_name);
		else if ((sample1.state == "")&&(no_of_times > 1))
			fprintf(fp, "Immerse %s in %s %s.<br>", slide1.name, sample1.new_name, noft);
		else if (no_of_times > 1)
			fprintf(fp, "Immerse %s in <a href=\"#%s\" >%s</a> %s.<br>", slide1.name, sample1.new_name, sample1.new_name, noft);
		else 
			fprintf(fp, "Immerse %s in <a href=\"#%s\" >%s</a>.<br>", slide1.name, sample1.new_name, sample1.new_name);
		ttr2 = total_time_required-ttr1;
		total_time_required = total_time_required + ttr2 * no_of_times;
	}

void remove_slide(Slide& slide1, Fluid& sample1)
	{
	// graph maintenance
		{
		Operation o = new_operation("remove slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide removed");
		create_edge(o.node, slide1.node);
		}
		fprintf(fp, "Take out %s from %s.<br>", slide1.name, sample1.new_name);
	}

void wash_tissue(Tissue& tissue1, Fluid& sample1, enum wash_type type, float temp, int shaking)
	{
	// graph maintenance
		{
		Operation o = new_operation("wash tissue");
		create_edge_from_fluids(&tissue1, &o);
		create_edge_from_fluids(&sample1, &o);
		tissue1.node = create_node("tissue washed");
		create_edge(o.node, tissue1.node);
		}
		char* shake_type;
		if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
			usage_list_fluids[sample1.usage_index].used = 1;
		else 
			{
			sample1.used = 1;
			usage_list_fluids[list_fluid_no] = sample1;
			sample1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_fluids[tissue1.usage_index].original_name == tissue1.original_name)
			usage_list_fluids[tissue1.usage_index].used = 1;
		else 
			{
			tissue1.used = 1;
			usage_list_fluids[list_fluid_no] = tissue1;
			tissue1.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		switch(shaking)
			{
			case 0: shake_type = " without shaking";break;
			case 1: shake_type = " with shaking";break;
			default: shake_type = "";break;
			}
		if(sample1.state == "")
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <font color=#357EC7>%s</font>", tissue1.new_name, sample1.new_name);break;
				case RINSING:fprintf(fp, "Rinse %s in <font color=#357EC7>%s</font>", tissue1.new_name, sample1.new_name);break;
				default:fprintf(fp, "Wash %s in <font color=#357EC7>%s</font>", tissue1.new_name, sample1.new_name);break;
				}
			}
		else
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <a href=\"#%s\" >%s</a>", tissue1.new_name, sample1.new_name, sample1.new_name);break;
				case RINSING:fprintf(fp, "Rinse %s in <a href=\"#%s\" >%s</a>", tissue1.new_name, sample1.new_name, sample1.new_name);break;
				default:fprintf(fp, "   Wash %s in <a href=\"#%s\" >%s</a>", tissue1.new_name, sample1.new_name, sample1.new_name);break;
				}
			}
		if (temp == RT)
			fprintf(fp, " at <b><font color=#357EC7>room temperature</font></b>%s.<br>", shake_type);
		else
			fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>%s.<br>", temp, 0x00B0, shake_type);
		if(shaking == 1)
			{
			if(shaker_no == 1)
				{
				equipments[equip_no]="Shaker";
				equip_no++;
				shaker_no++;
				}
			}
	}

void wash_tissue(Tissue& tissue1, Fluid& sample1, enum wash_type type, Time* time1, int no_of_times, float temp, int shaking)
	{
	char *noft, *shake_type;
	int ttr1 = total_time_required;
	int ttr2;
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_fluids[tissue1.usage_index].original_name == tissue1.original_name)
		usage_list_fluids[tissue1.usage_index].used = 1;
	else 
		{
		tissue1.used = 1;
		usage_list_fluids[list_fluid_no] = tissue1;
		tissue1.usage_index = list_fluid_no;
		list_fluid_no++;
		}
	switch(no_of_times)
		{
		case 1: noft="";break;
		case 2: noft="twice";break;
		case 3: noft="thrice";break;
		case 4: noft="four times";break;
		case 5: noft="five times";break;
		default: noft="";break;
		}

	switch(shaking)
		{
		case 0: shake_type = " without shaking";break;
		case 1: shake_type = " with shaking";break;
		default: shake_type = "";break;
		}
	if(sample1.state == "")
		{
		if (no_of_times == 1)
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <font color=#357EC7>%s</font> for ", tissue1.new_name, sample1.new_name);break;
				case RINSING:fprintf(fp, "Rinse %s in <font color=#357EC7>%s</font> for ", tissue1.new_name, sample1.new_name);break;
				default:break;
				}
			time1->display_time();
			if (temp == RT)
				fprintf(fp, " at <b><font color=#357EC7>room temperature</font></b>%s.<br>", shake_type);
			else 
				fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>%s.<br>", temp, 0x00B0, shake_type);
			}
		else
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <font color=#357EC7>%s</font>, <b><font color=#357EC7>%s</font></b> for ", tissue1.new_name, sample1.new_name, noft);break;
				case RINSING:fprintf(fp, "Rinse %s in <font color=#357EC7>%s</font>, <b><font color=#357EC7>%s</font></b> for ", tissue1.new_name, sample1.new_name, noft);break;
				default:break;
				}
			time1->display_time();
			if (temp == RT)
				fprintf(fp, " each at <b><font color=#357EC7>room temperature</font></b>%s.<br>", shake_type);
			else
				fprintf(fp, " each at <b><font color=#357EC7>%g%cC</font></b>%s.<br>", temp, 0x00B0, shake_type);
			}
		}
	else 
		{
		if (no_of_times == 1)
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <a href=\"#%s\" >%s</a> for ", tissue1.new_name, sample1.new_name, sample1.new_name);break;
				case RINSING:fprintf(fp, "Rinse %s in <a href=\"#%s\" >%s</a> for ", tissue1.new_name, sample1.new_name, sample1.new_name);break;
				default:break;
				}
			time1->display_time();
			if (temp == RT)
				fprintf(fp, " at <b><font color=#357EC7>room temperature</font></b>%s.<br>", shake_type);
			else
				fprintf(fp, " at <b><font color=#357EC7>%g%cC</font></b>%s.<br>", temp, 0x00B0, shake_type);
			}
		else
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <a href=\"#%s\" >%s</a>, <b><font color=#357EC7>%s</font></b> for ",tissue1.new_name, sample1.new_name, sample1.new_name, noft);break;
				case RINSING:fprintf(fp, "Rinse %s in <a href=\"#%s\" >%s</a>, <b><font color=#357EC7>%s</font></b> for ",tissue1.new_name, sample1.new_name, sample1.new_name, noft);break;
				default:break;
				}
			time1->display_time();
			if (temp == RT)
				fprintf(fp, " each at <b><font color=#357EC7>room temperature</font></b>%s.<br>", shake_type);
			else
				fprintf(fp, " each at <b><font color=#357EC7>%g%cC</font></b>%s.<br>", temp, 0x00B0, shake_type);
			}
		}
	if(shaking == 1)
		{
		if(shaker_no==1)
			{
			equipments[equip_no]="Shaker";
			equip_no++;
			shaker_no++;
			}
		}
	ttr2 = total_time_required-ttr1;
	total_time_required = total_time_required + ttr2 * no_of_times;
	// graph maintenance
		{
		Operation o = new_operation("wash tissue");
		create_edge_from_fluids(&tissue1, &o);
		create_edge_from_fluids(&sample1, &o);
		tissue1.node = create_node("tissue washed");
		create_edge(o.node, tissue1.node);
		o.node->time = time1->value * no_of_times;
		o.node->unit = time1->unit;
		}
	}

void immerse_slide_and_incubate(Slide& slide1, Fluid& sample1, float temp, Time* time1)
	{
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[slide1.usage_index].name == slide1.name)
		usage_list_containers[slide1.usage_index].used = 1;
	else 
		{
		slide1.used = 1;
		usage_list_containers[list_container_no] = slide1;
		slide1.usage_index = list_container_no;
		list_container_no++;
		}
	fprintf(fp, "Incubate %s for ", slide1.name);
	time1->display_time();
	if(sample1.state == "")
		{
		if(temp==RT)
			fprintf(fp, " in <b><font color=#357EC7>%s</font></b> at <b><font color=#357EC7>room temperature</font></b>.<br>", sample1.new_name);
		else
			fprintf(fp, " in <b><font color=#357EC7>%s</font></b> at <b><font color=#357EC7>%g%cC</font></b>.<br>", sample1.new_name, temp, 0x00B0);
		}
	else
		{
		if(temp==RT)
			fprintf(fp, " in <a href=\"#%s\" >%s</a> at <b><font color=#357EC7>room temperature</font></b>.<br>", sample1.new_name, sample1.new_name);
		else
			fprintf(fp, " in <a href=\"#%s\" >%s</a> at <b><font color=#357EC7>%g%cC</font></b>.<br>", sample1.new_name, sample1.new_name, temp, 0x00B0);
		}
	// graph maintenance
		{
		Operation o = new_operation("immerse slide and incubate");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide immersed and incubated");
		create_edge(o.node, slide1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void immerse_slide_and_incubate(Slide& slide1, Fluid& sample1, float temp, Time* time1, enum container_type type)
	{
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
	usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[slide1.usage_index].name == slide1.name)
		usage_list_containers[slide1.usage_index].used = 1;
	else 
		{
		slide1.used = 1;
		usage_list_containers[list_container_no] = slide1;
		slide1.usage_index = list_container_no;
		list_container_no++;
		}
	switch(type){
		case 5:
			fprintf(fp, "Incubate %s for ", slide1.name);
			time1->display_time();
			if(sample1.new_name == "")
				{
				if(type == HUMIDIFIED_CHAMBER)
					fprintf(fp, " in <b><font color=#357EC7>%s</font></b> at <b><font color=#357EC7>room temperature</font></b> in humidified chamber.<br>", sample1.new_name);
				}
			else
				{
				if(type == HUMIDIFIED_CHAMBER)
					fprintf(fp, " in <a href=\"#%s\" >%s</a> at <b><font color=#357EC7>room temperature</font></b> in humidified chamber.<br>", sample1.new_name, sample1.new_name);
				}
			break;
		default:break;
		}
	// graph maintenance
		{
		Operation o = new_operation("immerse slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide immersed and incubated");
		create_edge(o.node, slide1.node);
		o.node->time = time1->value;
		o.node->unit = time1->unit;
		}
	}

void add_to_slide(Slide& slide1, Fluid& sample1, Volume* volume1)
	{
	// graph maintenance
		{
		Operation o = new_operation("add to slide");
		create_edge_from_fluids(&sample1, &o);
		create_edge_from_fluid_to_container(&o, &slide1);
		}
		if ((sample1.volume == 0)|| (sample1.volume < (volume1->value*volume1->mul)))
			{
			fprintf(fp, "<font color = red>Warning: You are measuing out more than the available volume of %s! Please make sure you have enough before carrying on with the protocol.<br></font>", sample1.new_name);

			}
		if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
			usage_list_fluids[sample1.usage_index].used = 1;
		else 
			{
			sample1.used = 1;
			usage_list_fluids[list_fluid_no] = sample1;
			sample1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[slide1.usage_index].name == slide1.name)
			usage_list_containers[slide1.usage_index].used = 1;
		else 
			{
			slide1.used = 1;
			usage_list_containers[list_container_no] = slide1;
			slide1.usage_index = list_container_no;
			list_container_no++;
			}
		if(sample1.state == "")
			{
			fprintf(fp, "Add ");
			volume1->display_vol();
			fprintf(fp, " of <b><font color=#357EC7>%s</font></b> to each of the %s.<br>", sample1.new_name, slide1.name);
			}
		else
			{
			fprintf(fp, "Add ");
			volume1->display_vol();
			fprintf(fp, " of <a href=\"#%s\" >%s</a> to each of the %s.<br>", sample1.new_name, sample1.new_name, slide1.name);
			}
	}

void dry_slide(Slide& slide1)
	{
	// graph maintenance
		{
		Operation o = new_operation("dry slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		slide1.node = create_node("dried slide");
		create_edge(o.node, slide1.node);
		}
		fprintf(fp, "Dry %s.<br>", slide1.name);
	}

void mount_observe_slide(Slide& slide1, Fluid& fluid1, char* device)
	{
	// graph maintenance
		{
		Operation o = new_operation("mount observe slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&fluid1, &o);
		slide1.node = create_node("results of observing slide");
		create_edge(o.node, slide1.node);
		}
		if (usage_list_fluids[fluid1.usage_index].original_name == fluid1.original_name)
			usage_list_fluids[fluid1.usage_index].used = 1;
		else 
			{
			fluid1.used = 1;
			usage_list_fluids[list_fluid_no] = fluid1;
			fluid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		fprintf(fp, "Mount %s with <b><font color=#357EC7>%s</font></b>, let it solidify and view under %s.<br>", slide1.name, fluid1.new_name, device);
	}

void facs(Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("facs");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("results of facs");
		create_edge(o.node, container1.node);
		}
		fprintf(fp, "FACS: sort %s based on fluorescence.", container1.contents.new_name);
	}

void cell_culture(Container& cells,Fluid& medium, int centri_speed, float temp, float time, float percent_CO2, Fluid& for_wash_valves, Fluid& for_wash_chambers, Fluid& for_trypsinization, float for_feeding)
	{
	// graph maintenance
		{
		Operation o = new_operation("cell culture");
		create_edge_from_container_to_fluid(&cells, &o);
		cells.node = create_node("cultured cell");
		create_edge(o.node, cells.node);
		}
		fprintf(fp,"Perform cell culture with the specified parameters.");
	}

void transfection(Container& container1, Fluid& medium, Fluid& dna)
	{
	// graph maintenance
		{
		Operation o = new_operation("transfection");
		create_edge_from_container_to_fluid(&container1, &o);
		create_edge_from_fluids(&medium, &o);
		create_edge_from_fluids(&dna, &o);
		container1.node = create_node("transfected cell");
		create_edge(o.node, container1.node);
		}
		fprintf(fp,"Transfect %s with %s.", container1.contents.new_name, dna.new_name);
	}

void electroporate (Container& container1, float voltage, int no_pulses)
	{
	// graph maintenance
		{
		Operation o = new_operation("electroporate");
		create_edge_from_container_to_fluid(&container1, &o);
		container1.node = create_node("container with contents electroporated");
		create_edge(o.node, container1.node);
		}
		fprintf(fp, "Set the electroporator to deliver <b><font color=#357EC7>%g V</font></b>, and then press the PULSE button <b><font color=#357EC7>%d</font></b> times. <br>", voltage, no_pulses);
		if(electro_no==1)
			{
			equipments[equip_no]="Electroporator";
			equip_no++;
			electro_no++;
			}
	}

void store_plate(Plate& plate1, float temp)
	{
	// graph maintenance
		{
		Operation o = new_operation("store plate");
		create_edge_from_fluids(&plate1, &o);
		plate1.node = create_node("plate incubated");
		create_edge(o.node, plate1.node);
		}
		if (usage_list_fluids[plate1.usage_index].original_name == plate1.original_name)
			usage_list_fluids[plate1.usage_index].used = 1;
		else 
			{
			plate1.used = 1;
			usage_list_fluids[list_fluid_no] = plate1;
			plate1.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		fprintf(fp, "Store %s at <b><font color=#357EC7>%g%cC</font></b>.<br>", plate1.new_name, temp, 0x00B0);
	}

void wash_slide(Slide& slide1, Fluid& sample1, enum wash_type type)
	{
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
	usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[slide1.usage_index].name == slide1.name)
		usage_list_containers[slide1.usage_index].used = 1;
	else 
		{
		slide1.used = 1;
		usage_list_containers[list_container_no] = slide1;
		slide1.usage_index = list_container_no;
		list_container_no++;
		}
	if(sample1.state == "")
		{
		switch(type)
			{
			case WASHING:fprintf(fp, "Wash %s in <b><font color=#357EC7>%s</font></b>.<br>", slide1.name, sample1.new_name);break;
			case RINSING:fprintf(fp, "Rinse %s in <b><font color=#357EC7>%s</font></b>.<br>", slide1.name, sample1.new_name);break;
			default:fprintf(fp, "Wash %s in <b><font color=#357EC7>%s</font></b>.<br>", slide1.name, sample1.new_name);break;
			}
		}
	else
		{
		switch(type)
			{
			case WASHING:fprintf(fp, "Wash %s in <a href=\"#%s\" >%s</a>.<br>", slide1.name, sample1.new_name, sample1.new_name);break;
			case RINSING:fprintf(fp, "Rinse %s in <a href=\"#%s\" >%s</a>.<br>", slide1.name, sample1.new_name,  sample1.new_name);break;
			default:fprintf(fp, "   Wash %s in <a href=\"#%s\" >%s</a>.<br>", slide1.name, sample1.new_name, sample1.new_name);break;
			}
		}
	}

void wash_slide(Slide& slide1, Fluid& sample1, enum wash_type type, Time* time1, int no_of_times)
	{
	char *noft;
	int ttr1 = total_time_required;
	int ttr2;
	if (usage_list_fluids[sample1.usage_index].original_name == sample1.original_name)
		usage_list_fluids[sample1.usage_index].used = 1;
	else 
		{
		sample1.used = 1;
		usage_list_fluids[list_fluid_no] = sample1;
		sample1.usage_index = list_fluid_no;
		list_fluid_no++;
		}		
	if (usage_list_containers[slide1.usage_index].name == slide1.name)
		usage_list_containers[slide1.usage_index].used = 1;
	else 
		{
		slide1.used = 1;
		usage_list_containers[list_container_no] = slide1;
		slide1.usage_index = list_container_no;
		list_container_no++;
		}
	switch(no_of_times)
		{
		case 1: noft="";break;
		case 2: noft="twice";break;
		case 3: noft="thrice";break;
		case 4: noft="four times";break;
		case 5: noft="five times";break;
		default: noft="";break;
		}
	if(sample1.state == "")
		{
		if (no_of_times == 1)
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <font color=#357EC7>%s</font> for ", slide1.name, sample1.new_name);break;
				case RINSING:fprintf(fp, "Rinse %s in <font color=#357EC7>%s</font> for ", slide1.name, sample1.new_name);break;
				default:break;
				}
			time1->display_time();
			fprintf(fp, ".<br>");
			}
		else
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <font color=#357EC7>%s</font>, <b><font color=#357EC7>%s</font></b> for ", slide1.name, sample1.new_name, noft);break;
				case RINSING:fprintf(fp, "Rinse %s in <font color=#357EC7>%s</font>, <b><font color=#357EC7>%s</font></b> for ", slide1.name, sample1.new_name, noft);break;
				default:break;
				}
			time1->display_time();
			fprintf(fp, " each.<br>");
			}
		}
	else 
		{
		if (no_of_times == 1)
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <a href=\"#%s\" >%s</a> for ", slide1.name, sample1.new_name, sample1.new_name);break;
				case RINSING:fprintf(fp, "Rinse %s in <a href=\"#%s\" >%s</a> for ", slide1.name, sample1.new_name, sample1.new_name);break;
				default:break;
				}
			time1->display_time();
			fprintf(fp, ".<br>");
			}
		else
			{
			switch(type)
				{
				case WASHING:fprintf(fp, "Wash %s in <a href=\"#%s\" >%s</a>, <font color=#357EC7>%s</font> for ", slide1.name, sample1.new_name, sample1.new_name, noft);break;
				case RINSING:fprintf(fp, "Rinse %s in <a href=\"#%s\" >%s</a>, <font color=#357EC7>%s</font> for ", slide1.name, sample1.new_name, sample1.new_name, noft);break;
				default:break;
				}
			time1->display_time();
			fprintf(fp, " each.<br>");
			}
		}
	ttr2 = total_time_required-ttr1;
	total_time_required = total_time_required + ttr2 * no_of_times;
	// graph maintenance
		{
		Operation o = new_operation("wash slide");
		create_edge_from_container_to_fluid(&slide1, &o);
		create_edge_from_fluids(&sample1, &o);
		slide1.node = create_node("slide washed");
		create_edge(o.node, slide1.node);
		o.node->time = time1->value * no_of_times;
		o.node->unit = time1->unit;
		}
	}

void measure_solid(Solid& solid1, float weight, enum weight_unit unit, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure solid");
		create_edge_from_fluids(&solid1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		char *type;
		int mul;

		if (usage_list_fluids[solid1.usage_index].original_name == solid1.original_name)
			usage_list_fluids[solid1.usage_index].used = 1;
		else 
			{
			solid1.used = 1;
			usage_list_fluids[list_fluid_no] = solid1;
			solid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		switch(unit){
	case UG: type = "µg";mul = MICRO;break;
	case MG: type = "mg";mul = MICRO;break;
	case GR: type = "g";mul = MICRO;break;
	default : type = "mg";mul = MICRO;break;
			}
		if (prev_cont == 1)
			{
			if (solid1.state == "")
				fprintf(fp, "Measure out <b><font color=#357EC7>%g %s</font></b> of <font color=#357EC7>%s</font> into %s.<br>", weight, type, solid1.new_name, container1.name);
			else
				fprintf(fp, "Measure out <b><font color=#357EC7>%g %s</font></b> of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", weight, type, solid1.new_name, solid1.new_name, container1.name);
			prev_container = container1.name;
			prev_cont++;
			}
		else if( prev_container == container1.name)
			{
			if (solid1.state == "")
				fprintf(fp, "Add <b><font color=#357EC7> %g %s</font></b> of <font color=#357EC7>%s</font>.<br>", weight, type, solid1.new_name);
			else
				fprintf(fp, "Add <b><font color=#357EC7> %g %s</font></b> of <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", weight, type, solid1.new_name, solid1.new_name);
			}
		else if (solid1.state == "")
			{
			fprintf(fp, "Measure out <b><font color=#357EC7>%g %s</font></b> of <font color=#357EC7>%s</font> into %s.<br>", weight, type, solid1.new_name, container1.name);
			prev_container = container1.name;
			}
		else
			{
			fprintf(fp, "Measure out <b><font color=#357EC7>%g %s</font></b> of <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", weight, type, solid1.new_name, solid1.new_name, container1.name);
			prev_container = container1.name;
			}

		solid1.container = container1.id;
		container1.contents = solid1;
		container1.volume = weight * mul;
	}

void measure_solid(Solid& solid1, Container& container1)
	{
	// graph maintenance
		{
		Operation o = new_operation("measure solid");
		create_edge_from_fluids(&solid1, &o);
		create_edge_from_fluid_to_container(&o, &container1);
		}
		if (usage_list_fluids[solid1.usage_index].original_name == solid1.original_name)
			usage_list_fluids[solid1.usage_index].used = 1;
		else 
			{
			solid1.used = 1;
			usage_list_fluids[list_fluid_no] = solid1;
			solid1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_containers[container1.usage_index].name == container1.name)
			usage_list_containers[container1.usage_index].used = 1;
		else 
			{
			container1.used = 1;
			usage_list_containers[list_container_no] = container1;
			container1.usage_index = list_container_no;
			list_container_no++;
			}
		if (prev_cont == 1)
			{
			if (solid1.state == "")
				fprintf(fp, "Measure out <font color=#357EC7>%s</font> into %s.<br>", solid1.new_name, container1.name);
			else
				fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", solid1.new_name, solid1.new_name, container1.name);
			prev_container = container1.name;
			prev_cont++;
			}
		else if( prev_container == container1.name)
			{
			if (solid1.state == "")
				fprintf(fp, "Add <font color=#357EC7>%s</font>.<br>", solid1.new_name);
			else
				fprintf(fp, "Add <a href=\"#%s\" ><font color=#357EC7>%s</font></a>.<br>", solid1.new_name, solid1.new_name);
			}
		else if (solid1.state == "")
			{
			fprintf(fp, "Measure out <font color=#357EC7>%s</font> into %s.<br>", solid1.new_name, container1.name);
			prev_container = container1.name;
			}
		else
			{
			fprintf(fp, "Measure out <a href=\"#%s\" ><font color=#357EC7>%s</font></a> into %s.<br>", solid1.new_name, solid1.new_name, container1.name);
			prev_container = container1.name;
			}

		solid1.container = container1.id;
		container1.contents = solid1;
		container1.volume = solid1.volume;
		solid1.volume = 0;
	}

Symbol new_symbol(char* symbol, char* def)
	{
	Symbol s1;
	s1.value = DEFAULT;
	s1.symbol = symbol;
	s1.unit = UL;
	if(print_parameters == 1)
		{
		fprintf(fp, "</ul><h2>Parameters:</h2><ul type=\"circle\">");
		print_parameters++;
		}
	fprintf(fp, "<li>%s - %s</li>", symbol, def);
	return s1;
	}

Symbol_t new_symbol_t(char* symbol, char* def)
	{
	Symbol_t s1;
	s1.value = DEFAULT;
	s1.symbol = symbol;
	s1.unit = SECS;
	if(print_parameters == 1)
		{
		fprintf(fp, "</ul><h2>Parameters:</h2><ul type=\"circle\">");
		print_parameters++;
		}
	fprintf(fp, "<li>%s - %s</li>", symbol, def);
	return s1;
	}

void set_value(Symbol& symbol1, float value, enum vol_unit unit)
	{
	symbol1.value = value;
	symbol1.unit = unit;
	}

void set_value(Symbol_t& symbol1, float value, enum time_unit unit)
	{
	symbol1.value = value;
	symbol1.unit = unit;
	}

void assign(Symbol& s1, Symbol& s2)
	{
	fprintf(fp, "Let %s = %s.\n", s1.symbol, s2.symbol);
	s1.value = s2.value;
	}

void assign(Symbol_t& s1, Symbol_t& s2)
	{
	fprintf(fp, "Let %s = %s.\n", s1.symbol, s2.symbol);
	s1.value = s2.value;
	}

Symbol subtract(Symbolic_vol* s1, Volume* vol1)
	{
	Symbol y;
	char* temp2 = "";
	char* temp1="";
	temp1 = (char *)calloc(strlen(s1->s1.symbol)+ 10, sizeof(char));
	temp2 = (char *)calloc(strlen(s1->s1.symbol)+ 10, sizeof(char));
	strcat(temp1, s1->s1.symbol);
	strcat(temp1, "-");
	sprintf(temp2, "%g", vol1->value);
	strcat(temp1, temp2);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(s1->value != DEFAULT))
		y.value = s1->value - vol1->value;
	else 
		y.value = DEFAULT;
	return y;
	}

Symbol subtract(Volume* s1, Symbolic_vol* vol1)
	{
	Symbol y;
	char* temp2 = "";
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	temp2 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	sprintf(temp2, "%g", s1->value);
	strcat(temp1, temp2);
	strcat(temp1, "-");
	strcat(temp1, vol1->s1.symbol);
	y.symbol= temp1;
	if ((s1->value != DEFAULT)&&(vol1->value != DEFAULT))
		y.value = s1->value - vol1->value;
	else 
		y.value = DEFAULT;
	return y;
	}

Symbol subtract(Symbolic_vol* vol1, Symbolic_vol* vol2)
	{
	Symbol y;
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ strlen(vol2->s1.symbol), sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "-");
	strcat(temp1, vol2->s1.symbol);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value - vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}

Symbol add(Symbolic_vol* vol1, Volume* vol2)
	{
	Symbol y;
	char* temp2 = "";
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	temp2 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "+");
	sprintf(temp2, "%g", vol2->value);
	strcat(temp1, temp2);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value + vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}


Symbol add(Symbolic_vol* vol1, Symbolic_vol* vol2)
	{
	Symbol y;
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ strlen(vol2->s1.symbol), sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "+");
	strcat(temp1, vol2->s1.symbol);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value + vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}

Symbol multiply(Symbolic_vol* vol1, Volume* vol2)
	{
	Symbol y;
	char* temp2 = "";
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	temp2 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "*");
	sprintf(temp2, "%g", vol2->value);
	strcat(temp1, temp2);
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value * vol2->value;
	else 
		y.value = DEFAULT;
	y.symbol= temp1;
	return y;
	}

Symbol multiply(Symbolic_vol* vol1, Symbolic_vol* vol2)
	{
	Symbol y;
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ strlen(vol2->s1.symbol), sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "*");
	strcat(temp1, vol2->s1.symbol);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value * vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}
Symbol divide(Symbolic_vol* vol1, Volume* vol2)
	{
	Symbol y;
	char* temp2 = "";
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	temp2 = (char *)calloc(strlen(vol1->s1.symbol)+ 10, sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "/");
	sprintf(temp2, "%g", vol2->value);
	strcat(temp1, temp2);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value / vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}


Symbol divide(Volume*vol1, Symbolic_vol* vol2)
	{
	Symbol y;
	char* temp2 = "";
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol2->s1.symbol)+ 10, sizeof(char));
	temp2 = (char *)calloc(strlen(vol2->s1.symbol)+ 10, sizeof(char));
	sprintf(temp2, "%g", vol1->value);
	strcat(temp1, temp2);
	strcat(temp1, "/");
	strcat(temp1, vol2->s1.symbol);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value / vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}


Symbol divide(Symbolic_vol* vol1, Symbolic_vol* vol2)
	{
	Symbol y;
	char* temp1="";
	temp1 = (char *)calloc(strlen(vol1->s1.symbol)+ strlen(vol2->s1.symbol), sizeof(char));
	strcat(temp1, vol1->s1.symbol);
	strcat(temp1, "/");
	strcat(temp1, vol2->s1.symbol);
	y.symbol= temp1;
	if ((vol1->value != DEFAULT)&&(vol2->value != DEFAULT))
		y.value = vol1->value / vol2->value;
	else 
		y.value = DEFAULT;
	return y;
	}

float get_ttr()
	{
	return total_time_required;
	}

void streak_plate(Plate& plate1, Plate& colony)
	{
	// graph maintenance
		{
		Operation o = new_operation("streak");
		create_edge_from_fluids(&colony, &o);
		create_edge_from_fluids(&o, &plate1);
		}
		if (usage_list_fluids[plate1.usage_index].original_name == plate1.original_name)
			usage_list_fluids[plate1.usage_index].used = 1;
		else 
			{
			plate1.used = 1;
			usage_list_fluids[list_fluid_no] = plate1;
			plate1.usage_index = list_fluid_no;
			list_fluid_no++;
			}		
		if (usage_list_fluids[colony.usage_index].original_name == colony.original_name)
			usage_list_fluids[colony.usage_index].used = 1;
		else 
			{
			colony.used = 1;
			usage_list_fluids[list_fluid_no] = colony;
			colony.usage_index = list_fluid_no;
			list_fluid_no++;
			}
		fprintf(fp, "Use a sterile toothpick or disposable loop to transfer a small segement of %s to a streak or patch on %s. <br>", colony.new_name, plate1.new_name);
	}


