#! /bin/python
# 
# W R I T E _ C O N F I G . P Y
#
# Last Modified on Sun Jan 12 17:52:49 2025
#
# Read information about command line options from a json file
# and write config files to partially automate the command
# line option handling.
#

import shutil
import json

# Copying proto.c to config.c and proto.h to config.h
shutil.copyfile('proto.c', 'config.c')
shutil.copyfile('proto.h', 'config.h')

# Reading the config.json file
with open('config.json', 'r') as json_file:
    json_data = json.load(json_file)

# Function to write Usage routine
def obtain_usage(data):
    lines_usage = []
    shorts =[]

    lines_usage.append('void  usage( struct config *  opt, char *  exeName )  {')
    lines_usage.append('  printf( "Usage:\\n");')
    shorts.append('  printf( " %s ')
    for value in data.values():
        if value.get('type') == 'optFlg':
            shorts.append(f"[-{value.get('short', '')}]")
        elif value.get('type') == 'optInt':
            shorts.append(f"[-{value.get('short', '')} INT]")
        elif value.get('type') == 'optStr':
            shorts.append(f"[-{value.get('short', '')} TXT]")
        elif value.get('type') == 'optLng':
            shorts.append(f"[-{value.get('short', '')} INT]")
        elif value.get('type') == 'optDbl':
            shorts.append(f"[-{value.get('short', '')} DBL]")
        elif value.get('type') == 'optChr':
            shorts.append(f"[-{value.get('short', '')} CHR]")
        else:
            shorts.append(f"[-{value.get('short', '')}]")
    shorts.append(' [FILES]\\n", exeName );')
    lines_usage.append( ''.join(shorts))
    for key, value in data.items():
        short = value.get('short', 'N/A')
        lines_usage.append(f'  printf( " %s %s\\n", opt->{short}.optID, opt->{short}.helpStr ); /* {key} */')
    return lines_usage

# Function to write init configuration routine
def init_config(data):
    lines_init = []

    lines_init.append('void  initConfiguration ( struct config *  opt )  {')
    for key, value in data.items():
        opt_type = value.get('type', 'N/A')
        short = value.get('short', 'N/A')
        help_text = value.get('help', 'N/A')
        lines_init.append(f"// {key}: {opt_type}")
        lines_init.append(f"  opt->{short}.active = FALSE;")
        lines_init.append(f'  opt->{short}.optID = "-{short}";')
        lines_init.append(f'  opt->{short}.helpStr = "{help_text}";')
        if opt_type == 'optStr':
            lines_init.append(f'  opt->{short}.optionStr = "{value.get("default", "")}";')
        elif opt_type == 'optInt':
            lines_init.append(f'  opt->{short}.mostPosLimit = {value.get("most_pos_limit", "")};')
            lines_init.append(f'  opt->{short}.mostNegLimit = {value.get("most_neg_limit", "")};')
            lines_init.append(f'  opt->{short}.optionInt = {value.get("default", "")};')
            lines_init.append(f'  opt->{short}.defaultVal = {value.get("default", "")};')
        elif opt_type == 'optLng':
            lines_init.append(f'  opt->{short}.mostPosLimit = {value.get("most_pos_limit", "")};')
            lines_init.append(f'  opt->{short}.mostNegLimit = {value.get("most_neg_limit", "")};')
            lines_init.append(f'  opt->{short}.optionLng = {value.get("default", "")};')
            lines_init.append(f'  opt->{short}.defaultVal = {value.get("default", "")};')
        elif opt_type == 'optDbl':
            lines_init.append(f'  opt->{short}.mostPosLimit = {value.get("most_pos_limit", "")};')
            lines_init.append(f'  opt->{short}.mostNegLimit = {value.get("most_neg_limit", "")};')
            lines_init.append(f'  opt->{short}.optionDbl = {value.get("default", "")};')
            lines_init.append(f'  opt->{short}.defaultVal = {value.get("default", "")};')
    return lines_init

# Function to write set configuration routine
def set_config(data):
    lines_set = []
    shorts =[]

    lines_set.append('int  setConfiguration ( int  argc, char *  argv[], struct config *  opt )  {')
    lines_set.append('  int c;\n')
    lines_set.append('  opterr = 0;')
    lines_set.append('  while ((c = getopt (argc, argv, OPTIONS )) != -1 ) {')
    lines_set.append('    switch ( c ) {')
    for key, value in data.items():
        opt_type = value.get('type', 'N/A')
        short = value.get('short', 'N/A')
        long = value.get('long', 'N/A')
        if opt_type == 'optFlg':
            lines_set.append(f"      case '{short}': opt->{short}.active = TRUE; break; /* {long} */")
        if opt_type == 'optStr':
            lines_set.append(f"      case '{short}': opt->{short}.active = TRUE; opt->{short}.optionStr = optarg; break; /* {long} */")
        elif opt_type == 'optInt':
            lines_set.append(f"      case '{short}': configureIntegerOption( &opt->{short}, optarg ); break; /* {long} */")
        elif opt_type == 'optLng':
            lines_set.append(f"      case '{short}': configureLongOption( &opt->{short}, optarg ); break; /* {long} */")
        elif opt_type == 'optDbl':
            lines_set.append(f"      case '{short}': configureDoubleOption( &opt->{short}, optarg ); break; /* {long} */")
    lines_set.append("      case '?' : {")
    shorts.append('        if ( strchr( "')
    for value in data.values():
        if value.get('type') != 'optFlg':
            shorts.append(f"{value.get('short', '')}")
    shorts.append('", optopt ) != NULL ) {')
    lines_set.append( ''.join(shorts))
    lines_set.append('          fprintf (stderr, "Error: Option -%c requires an argument.\\n", optopt);')
    lines_set.append('          switch ( optopt ) {')
    for value in data.values():
        if value.get('type') != 'optFlg':
            short = value.get('short', 'N/A')
            lines_set.append(f"            case '{short}': opt->{short}.active = FALSE; break;")
    lines_set.append('          }')
    lines_set.append('        }')
    lines_set.append('        else if (isprint (optopt))')
    lines_set.append('          fprintf (stderr, "Warning: Unknown option \\"-%c\\" ? - ignoring it!\\n", optopt);')
    lines_set.append('        else')
    lines_set.append('          fprintf (stderr, "Warning: Unknown non-printable option character 0x%x ? - ignoring it!\\n", optopt);')
    lines_set.append('        break;')
    lines_set.append('      }')
    lines_set.append('    }')
    lines_set.append('  }')
    lines_set.append('  return( optind );')
    return lines_set

# Function to create an option string for the header file
def concatenate_shorts(data):
    shorts = []
    shorts.append('#define OPTIONS "')
    for value in data.values():
        if value.get('type') != 'optFlg':
            shorts.append(f"{value.get('short', '')}:")
        else:
            shorts.append(value.get('short', ''))
    return ''.join(shorts) + '"'

# Function to create a config struct for the header file
def extract_type_and_short(data):
    lines = []
    lines.append('struct config {')
    for key, value in data.items():
        short = value.get('short', 'N/A')
        type_text = value.get('type', 'N/A')
        lines.append(f"  struct {type_text} {short};  /* ({value.get('long', '')}) {value.get('help', '')} */")
    return lines


# Appending JSON information to config.c and config.h
lines2 = extract_type_and_short(json_data)
lines3 = obtain_usage(json_data)
lines4 = init_config(json_data)
lines5 = set_config(json_data)

with open('config.c', 'a') as config_c_file:
    config_c_file.write("\n// Functions for Command Line Options Configuration from JSON Data\n")
    config_c_file.write("\n".join(lines3) + "\n}\n\n")
    config_c_file.write("\n".join(lines4) + "\n}\n\n")
    config_c_file.write("\n".join(lines5) + "\n}\n")
 
with open('config.h', 'a') as config_h_file:
    config_h_file.write("\n// Command Line Options Configuration Data\n")
    config_h_file.write("\n".join(lines2) + "\n};\n")
    config_h_file.write(f"\n// getopt() option string\n{concatenate_shorts(json_data)}\n")
    config_h_file.write("\nvoid  usage ( struct config *  optStructPtr, char *  exeName );\n")
    config_h_file.write("void  initConfiguration ( struct config *  optStructPtr );\n")
    config_h_file.write("int  setConfiguration ( int  argc, char *  argv[], struct config *  optStructPtr );\n")
    config_h_file.write("\n#endif\n")
