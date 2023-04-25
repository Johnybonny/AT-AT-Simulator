#include "Object.h"
#include <fstream>

using namespace std;

// load data from a file
void Object::load(std::string fileName) {
	string letter;
	float x, y, z;
	string x1, y1, z1;
	vector<float> vArray;
	vector<float> vnArray;
	vector<float> vtArray;


	std::ifstream file(fileName);

	while (file >> letter) {
		if (letter == "v") {
			file >> x;
			file >> y;
			file >> z;
			vArray.push_back(x);
			vArray.push_back(y);
			vArray.push_back(z);
			vArray.push_back(1.0);
		}

		if (letter == "vt") {
			file >> x;
			file >> y;
			vtArray.push_back(x);
			vtArray.push_back(y);
		}

		if (letter == "vn") {
			file >> x;
			file >> y;
			file >> z;
			vnArray.push_back(x);
			vnArray.push_back(y);
			vnArray.push_back(z);
			vnArray.push_back(0.0);
		}

		if (letter == "f") {
			file >> x1;
			file >> y1;
			file >> z1;
			this->fArray.push_back(x1);
			this->fArray.push_back(y1);
			this->fArray.push_back(z1);
		}
	}

	std::string delimiter = "/";
	int i = 0;
	while (i < this->fArray.size()) {


		std::string s = this->fArray[i];

		size_t pos = 0;
		int tmp;

		pos = s.find(delimiter);
		tmp = stoi(s.substr(0, pos)) - 1;
		s.erase(0, pos + delimiter.length());

		for (int j = 0; j < 4; j++) {
			this->vertices.push_back(vArray[tmp * 4 + j]);
		}

		pos = s.find(delimiter);
		tmp = stoi(s.substr(0, pos)) - 1;
		s.erase(0, pos + delimiter.length());

		for (int j = 0; j < 2; j++) {
			this->texCoords.push_back(vtArray[tmp * 2 + j]);
		}

		tmp = stoi(s) - 1;

		for (int j = 0; j < 4; j++) {
			this->normals.push_back(vnArray[tmp * 4 + j]);
		}
		i++;

	}
	file.close();
}

//setters
void Object::setTexCoords(std::vector<float> texCoords) {
	this->texCoords = texCoords;
}

void Object::setNormals(std::vector<float> normals) {
	this->normals = normals;
}

void Object::setVertices(std::vector<float> vertices) {
	this->vertices = vertices;
}

// getters
vector<float> Object::getNormals() {
	return normals;
}

vector<float> Object::getVertices() {
	return vertices;
}

vector<float> Object::getTexCoords() {
	return texCoords;
}

vector<string> Object::getfArray() {
	return fArray;
}

