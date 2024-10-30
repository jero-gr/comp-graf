#include <glm/ext.hpp>
#include "Render.hpp"
#include "Callbacks.hpp"

extern bool wireframe, play, top_view, antialiasing;

// matrices que definen la camara
glm::mat4 projection_matrix, view_matrix;

// función para renderizar cada "parte" del auto
void renderPart(const Car &car, const std::vector<Model> &v_models, const glm::mat4 &matrix, Shader &shader) {
	// select a shader
	for(const Model &model : v_models) {
		shader.use();
		
		// matrixes
		if (play) {
			/// @todo: modificar una de estas matrices para mover todo el auto (todas
			///        las partes) a la posición (y orientación) que le corresponde en la pista
			glm::mat4 model_matrix = glm::translate(glm::mat4(1.f),{car.x,0.f,car.y}) *
									 glm::rotate(glm::mat4(1.f),-car.ang,{0.f,1.f,0.f} ) *
									 matrix ;
			shader.setMatrixes(model_matrix,view_matrix,projection_matrix);
		} else {
			glm::mat4 model_matrix = glm::rotate(glm::mat4(1.f),view_angle,glm::vec3{1.f,0.f,0.f}) *
						             glm::rotate(glm::mat4(1.f),model_angle,glm::vec3{0.f,1.f,0.f}) *
			                         matrix ;
			shader.setMatrixes(model_matrix,view_matrix,projection_matrix);
		}
		
		// setup light and material
		shader.setLight(glm::vec4{20.f,40.f,20.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.35f);
		shader.setMaterial(model.material);
		
		// send geometry
		shader.setBuffers(model.buffers);
		glPolygonMode(GL_FRONT_AND_BACK,(wireframe and (not play))?GL_LINE:GL_FILL);
		model.buffers.draw();
	}
}

// función que actualiza las matrices que definen la cámara
void setViewAndProjectionMatrixes(const Car &car) {
	projection_matrix = glm::perspective( glm::radians(view_fov), float(win_width)/float(win_height), 0.1f, 100.f );
	if (play) {
		if (top_view) {
			/// @todo: modificar el look at para que en esta vista el auto siempre apunte hacia arriba
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			view_matrix = glm::lookAt( pos_auto+glm::vec3{0.f,30.f,0.f}, pos_auto, glm::vec3{cos(car.ang),0.f,sin(car.ang)} );
		} else {
			/// @todo: definir view_matrix de modo que la camara persiga al auto desde "atras"
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			view_matrix = glm::lookAt( pos_auto+glm::vec3{-cos(car.ang)*5.f,2.f,-sin(car.ang)*5.f}, pos_auto, glm::vec3{0.f,1.f,0.f} );
		}
	} else {
		view_matrix = glm::lookAt( glm::vec3{0.f,0.f,3.f}, view_target, glm::vec3{0.f,1.f,0.f} );
	}
}

// función que rendiriza todo el auto, parte por parte
void renderCar(const Car &car, const std::vector<Part> &parts, Shader &shader) {
	const Part &axis = parts[0], &body = parts[1], &wheel = parts[2],
	           &fwing = parts[3], &rwing = parts[4], &helmet = parts[antialiasing?5:6];

	/// @todo: armar la matriz de transformación de cada parte para construir el auto
	
	if (body.show or play) {
		glm::mat4 mat_body = glm::mat4(1.00f);
		mat_body = glm::scale(mat_body,{0.8f,0.8f,0.8f});
		mat_body = glm::translate(mat_body,{0.f,0.15f,0.f});
		
		renderPart(car,body.models,mat_body,shader);
	}
	
	if (wheel.show or play) {
		
		float s = 0.11;
		
		glm::mat4 mat_wheel0( s*cos(-car.rang1)*cos(-car.rang2), s*sin(-car.rang2), -s*sin(-car.rang1)*cos(-car.rang2), 0.f, // first column
							-s*cos(-car.rang1)*sin(-car.rang2), s*cos(-car.rang2), s*sin(-car.rang1)*sin(-car.rang2), 0.f, // second column
							-s*sin(-car.rang1), 0.f, -s*cos(-car.rang1), 0.f, // third column
							 0.4f, 0.12f, 0.25f, 1.f); // fourth column
		
		glm::mat4 mat_wheel1( s*cos(-car.rang1)*cos(-car.rang2), s*sin(-car.rang2), -s*sin(-car.rang1)*cos(-car.rang2), 0.f, // first column
							-s*cos(-car.rang1)*sin(-car.rang2), s*cos(-car.rang2), s*sin(-car.rang1)*sin(-car.rang2), 0.f, // second column
							s*sin(-car.rang1), 0.f, s*cos(-car.rang1), 0.f, // third column
							0.4f, 0.12f, -0.25f, 1.f); // fourth column
		
		glm::mat4 mat_wheel2(s*cos(car.rang2), -s*sin(car.rang2), 0.f, 0.f, // first column
							s*sin(car.rang2), s*cos(car.rang2), 0.f, 0.f, // second column
							0.f, 0.f, -s, 0.f, // third column
							-0.7f, 0.12f, 0.25f, 1.f); // fourth column
		
		glm::mat4 mat_wheel3(s*cos(car.rang2), -s*sin(car.rang2), 0.f, 0.f, // first column
							 s*sin(car.rang2), s*cos(car.rang2), 0.f, 0.f, // second column
							 0.f, 0.f, s, 0.f, // third column
							-0.7f, 0.12f, -0.25f, 1.f); // fourth column
		
		renderPart(car,wheel.models, mat_wheel0,shader);
		renderPart(car,wheel.models, mat_wheel1,shader);
		renderPart(car,wheel.models, mat_wheel2,shader);
		renderPart(car,wheel.models, mat_wheel3,shader);
	}
	
	if (fwing.show or play) {
		glm::mat4 mat_fwing = glm::mat4(1.00f);
		mat_fwing = glm::rotate(mat_fwing,-3.14156f/2.f,{0.f,1.f,0.f});
		mat_fwing = glm::scale(mat_fwing,{0.35f,0.35f,0.35f});
		mat_fwing = glm::translate(mat_fwing,{0.f,0.2f,-2.f});
		
		renderPart(car,fwing.models, mat_fwing,shader);
	}
	
	if (rwing.show or play) {
		glm::mat4 mat_rwing = glm::mat4(1.00f);
		mat_rwing = glm::rotate(mat_rwing,-3.14156f/2.f,{0.f,1.f,0.f});
		mat_rwing = glm::scale(mat_rwing,{0.25f,0.25f,0.25f});
		mat_rwing = glm::translate(mat_rwing,{0.f,1.f,3.3f});
		
		renderPart(car,rwing.models, mat_rwing,shader);
	}
	
	if (helmet.show or play) {
		glm::mat4 mat_helmet(	0.f, 0.f, .08f, 0.f, // first column
								0.f, .08f, 0.f, 0.f, // second column
								-.08f, 0.f, 0.f, 0.f, // third column
								0.04f, 0.2f, 0.f, 1.f); // fourth column
		
		renderPart(car,helmet.models, mat_helmet,shader);
	}
	
	if (axis.show and (not play)) renderPart(car,axis.models,glm::mat4(1.f),shader);
}

// función que renderiza la pista
void renderTrack() {
	static Model track = Model::loadSingle("models/track",Model::fDontFit);
	static Shader shader("shaders/texture");
	shader.use();
	shader.setMatrixes(glm::mat4(1.f),view_matrix,projection_matrix);
	shader.setMaterial(track.material);
	shader.setBuffers(track.buffers);
	track.texture.bind();
	static float aniso = -1.0f;
	if (aniso<0) glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	track.buffers.draw();
}

void renderShadow(const Car &car, const std::vector<Part> &parts) {
	static Shader shader_shadow("shaders/shadow");
	glEnable(GL_STENCIL_TEST); glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_EQUAL,0,~0); glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
	renderCar(car,parts,shader_shadow);
	glDisable(GL_STENCIL_TEST);
}
