#include "Material.h"

Material::Material() {
}


Material::~Material() {
}

// Sends textures and uniforms to shader
void Material::Bind() {
	if (!ignoreTextures) {
		glUniform1i(IGNORE_TEXTURES_LOCATION, false);

		// Textures
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
	else {
		glUniform1i(IGNORE_TEXTURES_LOCATION, true);
	}

	// Material parameters
	glUniform3fv(MATERIAL_AMBIENT_LOCATION, 1, glm::value_ptr(ambient));
	glUniform3fv(MATERIAL_DIFFUSE_LOCATION, 1, glm::value_ptr(diffuse));
	glUniform3fv(MATERIAL_SPECULAR_LOCATION, 1, glm::value_ptr(specular));
	glUniform1f(MATERIAL_SHININESS_LOCATION, shininess);
	glUniform1f(PARALLAX_STRENGTH_LOCATION, parallaxStrength);
}