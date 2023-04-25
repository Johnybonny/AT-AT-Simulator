#pragma once
#include <vector>
#include <string>
class Object {
public:
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<std::string> fArray;
    // constructor to load data from file
    //Object(std::string fileName);

    void load(std::string fileName);

    // settters
    void setVertices(std::vector<float>);
    void setNormals(std::vector<float>);
    void setTexCoords(std::vector<float>);

    // getters
    std::vector<float> getVertices();
    std::vector<float> getNormals();
    std::vector<float> getTexCoords();
    std::vector<std::string> getfArray();



};
