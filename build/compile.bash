#!/bin/bash
################################################################################
# Build script for NiShiKi
#
# This script will
#   * automatically search all C++ script under the source directory,
#   * compile them if the source code is newer than the existing obj file,
#   * link all object files and necessary libraries.
################################################################################

################################################################################
# Parse command line arguments
################################################################################

# Define default values.
compiler="gcc"
gcov="false"

# Parse command line arguments.
while getopts c:g:l:o:s:- OPT; do

    # Ignore "-" optoin.
    if [[ "$OPT" = - ]]; then
        continue
    fi

    # Split $OPTARG by "=" and store them to $opt and $optarg respectively.
    opt="-${OPT}${OPTARG%%=*}"
    optarg="${OPTARG##*=}"

    # Process options.
    case "-${opt}" in
        -c|--compiler) compiler=${optarg};;
        -g|--gcov    ) gcov=${optarg};;
        -l|--link    ) link=${optarg};;
        -o|--obj     ) obj=${optarg};;
        -s|--src     ) src=${optarg};;
          *          ) echo "Unknow option ${opt}=${optarg}";;
    esac

done

################################################################################
# Compute compile command
################################################################################

echo -e ""
echo -e "\033[95m//////////////////////////////////////////////////\033[0m"
echo -e "\033[95m// Determine compile options\033[0m"
echo -e "\033[95m//////////////////////////////////////////////////\033[0m"

# Determine compiler.
if [ ${compiler} = "clang" ]; then
    CC="clang++ -stdlib=libc++"
else
    CC="g++"
fi

# Add extra options.
CC="${CC} -std=c++23 -Wall -Wextra -I${src} -I./external -I."

# Add cflags for libraries.
CC="${CC} `pkg-config --cflags ${link}`"

# Add optimization.
if [ ${gcov} = "true" ]; then
    CC="${CC} -O0 -fprofile-arcs -ftest-coverage"
else
    CC="${CC} -O3 -mtune=native -march=native -flto=auto"
fi

# Print compile command.
echo "CC = ${CC}"

################################################################################
# Compile all C++ files to object files
################################################################################

echo -e ""
echo -e "\033[95m//////////////////////////////////////////////////\033[0m"
echo -e "\033[95m// Compile all C++ files to object files\033[0m"
echo -e "\033[95m//////////////////////////////////////////////////\033[0m"

# Make output directory.
mkdir -p ${obj}

# This flag will be set to true if at least one file is compiled.
is_updated="false"

# Process all C++ files.
for path_cxx in `ls ${src}/*.cxx`; do

    # Get basename of the target file.
    name=`basename ${path_cxx} .cxx`

    # Get the output file path.
    path_o="${obj}/${name}.o"

    # Compile the C++ file if the C++ file is newer than the compiled cache.
    if [ ${path_cxx} -nt ${path_o} ]; then

        # Compute build command to run.
        cmd="${CC} -c -o ${path_o} ${path_cxx}"

        # Print build message.
        echo -e "\033[94m${path_o}\033[0m <- \033[92m${path_cxx}\033[0m"

        # Run the build command.
        ${cmd}

        # Exit if error occured.
        if [ ${?} -ne "0" ]; then
            exit 0
        fi

        # Update the flag.
        is_updated="true"

    else

        # Print build message.
        echo -e "\033[94m${path_o}\033[0m is already up-to-date"

    fi

done

################################################################################
# Link all object files and necessary libraries
################################################################################

echo -e ""
echo -e "\033[95m//////////////////////////////////////////////////\033[0m"
echo -e "\033[95m// Link all object files and necessary libraries\033[0m"
echo -e "\033[95m//////////////////////////////////////////////////\033[0m"

# Compute the output path.
target="./nishiki"

# Link object files and libraries.
if [ ! -f ${target} ] || [ ${is_updated} = "true" ];
then

    # Print build message.
    echo -e "\033[94m${target}\033[0m <- \033[92m${obj}/*.o\033[0m"

    # Compute link options.
    link_options=`pkg-config --libs ${link}`
    link_options="${link_options} -lstdc++ -lm"

    # Link all compiled objects.
    ${CC} -o ${target} ${obj}/*.o ./external/*.o ${link_options}

    # Exit if error occured.
    if [ ${?} -ne "0" ]; then
        exit 0
    fi

else

    # Print build message.
    echo -e "\033[94m${target}\033[0m is already up-to-date"

    # Exit if not updated.
    exit 0

fi

# Print empty line for readability.
echo -e ""
echo -e "\033[93mNiShiKi has been built successfully!!\033[0m"
echo -e ""

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
