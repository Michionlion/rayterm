#include "programs.h"
#include "build_variables.h"

#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include <string>

bool Programs::load(const char* file, const char* func, const char* id) {
    std::string path = std::string(PROGRAM_DIRECTORY) + "/" + file + ".ptx";
    try {
        programs[id] = context->createProgramFromPTXFile(path, func);
        printf("Compiled '%s' to '%s'\n", path.c_str(), id.c_str());
        programs[id]->validate();
    } catch (optix::Exception& ex) {
        printf("Error compiling '%s': %d (%s)\n", name.c_str(), ex.getErrorCode(),
            ex.getErrorString().c_str());
    }
}

optix::Program Programs::get(const char* file, const char* func) {
    std::string id                = std::string(file) + ":" + func;
    ProgramMap::const_iterator it = programs.find(id);
    if (it == programs.end()) {
        bool loaded = load(file, func);
        if (!loaded) {
            throw std::runtime_error(sprintf("Error: could not find %s", id));
        }
        it = programs.find(name);
    }
    return it->second;
}

optix::Program Programs::get(const char* id) {
    ProgramMap::const_iterator it = programs.find(id);
    if (it == programs.end()) {
        throw std::runtime_error(sprintf("Error: could not find required ptx program %s", id));
    }
    return it->second;
}
