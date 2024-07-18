#pragma once
#include "Shader.h"
#include "ShaderProgram.h"
#include <unordered_map>

// Shamelessly copied from https://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
class hash_tuple
{
    template<class T>
    struct component
    {
        const T& value;
        component(const T& value) : value(value) {}
        uintmax_t operator,(uintmax_t n) const
        {
            n ^= std::hash<T>()(value);
            n ^= n << (sizeof(uintmax_t) * 4 - 1);
            return n ^ std::hash<uintmax_t>()(n);
        }
    };

public:
    template<class Tuple>
    size_t operator()(const Tuple& tuple) const
    {
        return std::hash<uintmax_t>()(
            std::apply([](const auto& ... xs) { return (component(xs), ..., 0); }, tuple));
    }
};

class ShaderProgramManager
{
    unsigned int id;

public:
    ShaderProgramManager();
    ~ShaderProgramManager();

    ShaderProgram* getProgram(Shader* fragmentShader, Shader* geometryShader, Shader* vertexShader);
    ShaderProgram* getProgram(Shader* fragmentShader, Shader* vertexShader);
    std::vector<ShaderProgram*> getPrograms() const;

    unsigned int getId();
    void use();

    //std::unordered_map<std::tuple<int, int, int>, ShaderProgram*, hash_tuple> programMap;
    std::vector<std::pair<std::tuple<int, int, int>, ShaderProgram*>> programMap;
};
