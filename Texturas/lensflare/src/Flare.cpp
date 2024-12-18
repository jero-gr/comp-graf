#include "Flare.hpp"

#include "Model.hpp"
#include "Shaders.hpp"

#include "Billboard.hpp"

extern std::vector<Shader> flare_shaders;
extern int win_width, win_height;
extern bool debug_flares;

float flares_distance = 0.27f;	// % of the distance from the sun to the center
const float flare_max_size = 0.6f;	// % of window size
const float flare_min_size = 0.015f;	// % of window size
const int flares_max_count = 9;
int flares_count = 9;

Flare::Flare(Flare &&flare) : 
	m_texture{std::move(flare.m_texture)}
{	
}

Flare::Flare(const std::string &file_name) : 
	m_texture { Texture(file_name, Texture::fClampS|Texture::fClampT) }
{
}

Flare::~Flare(){}
	
std::vector<glm::vec2> Flare::generateTextureCoordinatesForFlare(float size, const glm::vec2& center) {
	std::vector<glm::vec2> texture_coords;
	
	/// @todo: Generar las coordenadas de textura (S, T) en funcion de la 
	///	posicion y tamanio del flare en pantalla (en pixeles).
	///	Ayuda: las variables win_width y win_height contienen el ancho y 
	///	alto de la pantalla respectivamente.
	
	/// {-1.f, -1.f, 0.f}, { 1.f, -1.f, 0.f}, { 1.f, 1.f, 0.f} y {-1.f, 1.f, 0.f},
	
	/// Definir la posici�n del la esquina superior izquierda del sticker
	float x_0 = center[0]-(size/2);
	float y_0 = center[1]-(size/2);
	
	/// Usar regla de 3 para calcular las coordenadas de textura
	float s_0 = -x_0/size;
	float s_1 = (win_width-x_0)/size;
	float t_0 = -y_0/size;
	float t_1 = (win_height-y_0)/size;

	glm::vec2 st_0 = {s_0,t_0};
	texture_coords.push_back(st_0);
	
	glm::vec2 st_1 = {s_1,t_0};
	texture_coords.push_back(st_1);
	
	glm::vec2 st_2 = {s_1,t_1};
	texture_coords.push_back(st_2);
	
	glm::vec2 st_3 = {s_0,t_1};
	texture_coords.push_back(st_3);
	
	return texture_coords;
}

void Flare::render(float size, const glm::vec2& center) {
	
	auto& billboard = Billboard::getBillboard();
	if(!billboard) {
		return;
	}
	
	/// Render debug information. Red square on the desired position of the flare.
	if(debug_flares) {
		auto &debug_shader = flare_shaders[1];
		
		debug_shader.use();
		
		glEnable(GL_BLEND);
		
		debug_shader.setUniform("flarePosition", center);
		debug_shader.setUniform("screenSize", glm::vec2(win_width, win_height));
		debug_shader.setUniform("flareSize", size);
		
		debug_shader.setBuffers(billboard->buffers);
		
		// Render
		billboard->buffers.draw();
	}
	
	// Update flare texture coords
	std::vector<glm::vec2> texture_coords = generateTextureCoordinatesForFlare(size, center);
	if(texture_coords.empty()) {
		return;
	}
	
	auto &shader = flare_shaders[0];
	
	// Move texture to the model
	billboard->texture = std::move(m_texture);
	
	// Bind and update GPU data
	billboard->texture.bind();
	billboard->buffers.updateTexCoords(texture_coords, true);
	
	// Set up shader
	shader.use();

	// Send geometry
	shader.setBuffers(billboard->buffers);

	// Set Id of texture to use on shader
	shader.setUniform("flareTexture", 0);
	
	// Setup blend function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	// Render
	billboard->buffers.draw();
	
	// Move texture back to flare object
	m_texture = std::move(billboard->texture);
	
	// Restore to default
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
