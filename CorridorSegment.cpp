#include "CorridorSegment.h"



CorridorSegment::CorridorSegment()
{
}


CorridorSegment::~CorridorSegment()
{
}

void CorridorSegment::Render(glm::mat4 viewMatrix) {
	for (int i = 0; i < models.size(); i++) {
		models[i].Render(GL_TRIANGLES, viewMatrix);

		std::cout << "Kupka" << std::endl;
	}
}