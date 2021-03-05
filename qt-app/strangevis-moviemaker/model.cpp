#include "model.h"
#include <stdio.h>

model::model(const char* filepath)
{
	load(filepath);
}

model::~model()
{}

bool model::load(const char* filepath)
{
	FILE* fp = fopen(filepath, "rb");

	unsigned int uWidth, uHeight, uDepth;
	fread((void*)&uWidth, 1, sizeof(unsigned short), fp);
	fread((void*)&uHeight, 1, sizeof(unsigned short), fp);
	fread((void*)&uDepth, 1, sizeof(unsigned short), fp);
	unsigned short* pData = new unsigned short[uWidth * uHeight * uDepth];
	fread((void*)pData, uWidth * uHeight * uDepth, sizeof(unsigned short), fp);
	fclose(fp);

	if (uWidth == 0 || uHeight == 0 || uDepth == 0) // Impossible dimensions, model not loaded properly.
	{
		return false;
	}

	m_width = uWidth;
	m_height = uHeight;
	m_depth = uDepth;
	m_Data = pData;
}

std::vector<unsigned short> model::getDimensions() 
{
	std::vector<unsigned short> dims;
	dims.push_back(m_width);
	dims.push_back(m_height);
	dims.push_back(m_depth);
	return dims;
}

unsigned short model::getValue(int x, int y, int z)
{
	unsigned short uValue = m_Data[x + y * m_width + z * m_width * m_height];
	return uValue;
}