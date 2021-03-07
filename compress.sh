#!/bin/sh

# This script purpose is to clean the whole project directory, and compress it
# to a .tgz archive placed in the same directory than the project is.
# Launch it with the command:
# sh compress.sh
# Extracting is done by running the command below, replacing 'filename' by the project name:
# tar -xf filename.tgz

clean()
{
	echo '\n'//////////////////////////////////////////////////
	echo // Cleaning:
	echo //////////////////////////////////////////////////'\n'
	make clean
}

# Get the current path before running other commands:
PROJECT_PATH=$PWD

# Cleaning the .o files and executables:
clean

# Compressing the whole project directory:
PROJECT_DIR=$(basename -- "$PROJECT_PATH")
echo '\n'//////////////////////////////////////////////////
echo // Compressing the directory: $PROJECT_DIR
echo //////////////////////////////////////////////////'\n'
tar -cf ../$PROJECT_DIR.tgz ../$PROJECT_DIR
echo Done.

exit
