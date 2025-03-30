#include <map>
#include <string>s

class Model3d;
class Material;

std::map<std::string, Material*> readMaterialFile(Model3d* model, const std::string& matfilestr);
void readFromFile(Model3d* model, const std::string& file);

#pragma once
