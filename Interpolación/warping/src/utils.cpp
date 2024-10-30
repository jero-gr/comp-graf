#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "utils.hpp"
#include "Debug.hpp"

BoundingBox::BoundingBox(glm::vec3 &p1, glm::vec3 &p2) 
	: pmin({std::min(p1.x,p2.x),std::min(p1.y,p2.y),std::min(p1.z,p2.z)}),
      pmax({std::max(p1.x,p2.x),std::max(p1.y,p2.y),std::max(p1.z,p2.z)}) 
{
	
}
	
bool BoundingBox::contiene(glm::vec3 &p) const {
	return p.x>=pmin.x && p.x<=pmax.x &&
		p.y>=pmin.y && p.y<=pmax.y &&
		p.z>=pmin.z && p.z<=pmax.z;
}

Pesos calcularPesos(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 x) {
	/// @todo: implementar
	glm::vec3 a = glm::cross(x1-x0,x2-x0);
	
	glm::vec3 a0 = glm::cross(x-x2,x1-x2);
	glm::vec3 a1 = glm::cross(x-x0,x2-x0);
	glm::vec3 a2 = glm::cross(x-x1,x0-x1);
	
	float alpha0 = glm::dot(a0,a) / glm::dot(a,a);
	float alpha1 = glm::dot(a1,a) / glm::dot(a,a);
	float alpha2 = glm::dot(a2,a) / glm::dot(a,a);
	
	return {alpha0,alpha1,alpha2};
}


