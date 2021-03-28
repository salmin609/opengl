#pragma once
#include <string>
#include <vector>
#include "Affine.h"

namespace BinaryFileManager
{
	void SaveBinFile(std::string fileName, std::vector<Vertex> verticesInfo, bool consistNormal, bool consistTexture);
	void LoadDataFile(std::string fileName, std::vector<Vertex>& verticesInfo, int& faceCount);
}