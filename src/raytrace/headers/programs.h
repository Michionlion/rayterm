#ifndef _RAYTERM_PROGRAMS_H_
#define _RAYTERM_PROGRAMS_H_

#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include <map>
#include <string>

class Programs {
    optix::Context context;
    std::map<std::string, optix::Program> program_map;

   public:
    explicit Programs(const optix::Context& context) : context(context){};
    ~Programs() {
        try {
            for (auto it : program_map) {
                it.second->removeReference();
            }
        } catch (const optix::Exception& ex) {
            printf("~Programs Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    };

    optix::Program get(const char* file, const char* func);
    optix::Program get(const std::string& id);
    optix::Program get(const char* id) { return get(std::string(id)); }

    bool load(const char* file, const char* func, const std::string& id);
    bool load(const char* file, const char* func, const char* id) {
        return load(file, func, std::string(id));
    }
};

#endif
