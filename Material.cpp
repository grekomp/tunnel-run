#include "Material.h"

Material::Material() {
}


Material::~Material() {
}

void Material::Bind() {
	glUniform1i(DIFFUSE_SAMPLER_LOCATION, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.diffuse);

	glUniform1i(SPECULAR_SAMPLER_LOCATION, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture.specular);

	glUniform1i(NORMAL_SAMPLER_LOCATION, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture.normal);

	glUniform1i(DISPLACEMENT_SAMPLER_LOCATION, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture.displacement);


}