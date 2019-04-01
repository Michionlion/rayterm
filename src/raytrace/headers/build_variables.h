#ifndef _RAYTERM_BUILD_VARIABLES_H_
#define _RAYTERM_BUILD_VARIABLES_H_

// PROGRAM_DIRECTORY is a directory to look in for .ptx files
// If no directory is given, the directory is 'build/ptx'
#ifndef PROGRAM_DIRECTORY
#define PROGRAM_DIRECTORY "build/ptx"
#endif

// ASSET_FOLDER is a directory to look in for .mat and .obj files
// If no directory is given, the directory is 'assets'
#ifndef ASSET_FOLDER
#define ASSET_FOLDER "assets"
#endif

// HANDLE_EXCEPTIONS is 1 or 0 -> 1 will turn on OptiX CUDA exceptions, 0 will
// turn them off
#ifndef HANDLE_EXCEPTIONS
#define HANDLE_EXCEPTIONS 0
#endif

#endif
