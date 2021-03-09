#pragma once
#include <vector>

class Model 
{
public:
	Model(const char* filepath);
	~Model();

	unsigned short getValue(unsigned int x, unsigned int y, unsigned int z);
	std::vector<unsigned short> getDimensions();

private:
	bool load(const char* filepath);

	// Model dimensions
	unsigned int m_height;
	unsigned int m_width;
	unsigned int m_depth;

	// Model data
	unsigned short* m_Data;
};