#pragma once
#include <vector>

class model 
{
public:
	model(const char* filepath);
	~model();

	unsigned short getValue(int x, int y, int z);
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