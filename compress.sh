#!/bin/sh

# This script purpose is clean the whole project folder, and compress it
# to an archive placed in the same directory than the project is.
# Launch it with the command:
# sh compress.sh


# Safer to get this at the beginning, since we may move between directories.
PROJECT_PATH=$PWD


cleanProject()
{
	echo '\n'//////////////////////////////////////////////////
	echo // Cleaning:
	echo //////////////////////////////////////////////////'\n'
	make clean
}


# Cleaning the .o files:
cleanProject


# Compressing the whole project folder:
projectFolder=$(basename -- "$PROJECT_PATH")
echo '\n'//////////////////////////////////////////////////
echo // Compressing the folder: $projectFolder
echo //////////////////////////////////////////////////'\n'
tar -czf ../$projectFolder.tgz ../$projectFolder
echo Done.


exit
