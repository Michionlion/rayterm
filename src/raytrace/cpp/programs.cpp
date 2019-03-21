#include "programs.h"
#include "build_variables.h"

#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include <map>
#include <string>

bool Programs::load(const char* file, const char* func, const std::string& id) {
    std::string path = std::string(PROGRAM_DIRECTORY) + "/" + file + ".ptx";
    try {
        program_map[id] = context->createProgramFromPTXFile(path, func);
        printf("Compiled '%s' to '%s'\n", path.c_str(), id.c_str());
        program_map[id]->validate();
    } catch (optix::Exception& ex) {
        printf("Error compiling '%s': %d (%s)\n", id.c_str(), ex.getErrorCode(),
            ex.getErrorString().c_str());
        return false;
    }

    return true;
}

optix::Program Programs::get(const char* file, const char* func) {
    const std::string id                                     = std::string(file) + ":" + func;
    std::map<std::string, optix::Program>::const_iterator it = program_map.find(id);
    if (it == program_map.end()) {
        bool loaded = load(file, func, id);
        if (!loaded) {
            throw std::runtime_error("Error: could not find " + id);
        }
        it = program_map.find(id);
    }
    return it->second;
}

optix::Program Programs::get(const std::string& id) {
    std::map<std::string, optix::Program>::const_iterator it = program_map.find(id);
    if (it == program_map.end()) {
        throw std::runtime_error(std::string("Error: could not find required ptx program ") + id);
    }
    return it->second;
}
