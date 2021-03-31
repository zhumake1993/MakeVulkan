#include "VKFrame.h"

int gFrameIndex = 0;

void UpdateFrameIndex()
{
	gFrameIndex++;
}

int GetFrameIndex()
{
	return gFrameIndex;
}
