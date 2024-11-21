in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoords;
in vec4 lightVSPosition;
in vec4 fragPosLightSpace;

uniform sampler2D depthTexture;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 emissionColor;
uniform float shininess;
uniform float opacity;

uniform float ambientStrength;
uniform vec3 lightColor;

out vec4 fragColor;

#include "calcPhong.frag"

/// Calcular si un fragmento está en la sombra o no
/// Recibe como entrada las coordenadas del fragmento en el espacio de la luz
float calcShadow(vec4 fragPosLightSpace) {
	/// Convertir las coordenadas de entrada en coordenadas 3D
	vec3 projCoords = vec3(fragPosLightSpace) / fragPosLightSpace.w;
	
	/// Restar un valor de bias para evitar shadow acne
	projCoords.z -= 0.015;

	/// Las coordenadas estaban en el rango [-1, 1], las normaliza a [0, 1]
	projCoords = projCoords * 0.5 + 0.5;
	
	/// Si la coordenada z es mayor a 1 o menor a 0, está fuera del rango
	/// Entonces, se retorna un valor -2
	if(projCoords.z > 1.0 || projCoords.z < 0.0) return -2.0;
	
	/// Se guarda el depthValue (z) del fragmento en currentDepth
	float currentDepth = projCoords.z;
	
	/// Se implementa filtrado PCF para el suavizado de bordes
	/// Inicializar la variable para el filtrado PCF
	float shadow = 0.0;
	
	/// Tamaño del texel para el filtrado PCF
	float texelSize = 1.0 / textureSize(depthTexture, 0).x; 
	
	
	/// Realizar el filtrado PCF (muestras 5x5)
	for (int x = -2; x <= 2; ++x) { /// Desplazamiento en x
		for (int y = -2; y <= 2; ++y) {	/// Desplazamiento en y
			// Calcular distancia de desplazamiento
			vec2 offset = vec2(x, y) * texelSize;
			
			// Obtener la profundidad de la textura de sombra
			float depth = texture(depthTexture, projCoords.xy + offset).r;
			
			// Si la profundidad del fragmento es mayor que la profundidad almacenada, está en sombra
			if (currentDepth > depth) {
				shadow += 1.0;
			}
		}
	}
	
	/// Se promedian los 9 valores (3x3)
	shadow /= 25.0;
	
	/// Retornar el valor de sombra
	return shadow;
	
	/// Se guarda el depthValue almacenado en el depthBuffer en closestDepth
	float closestDepth = texture(depthTexture, projCoords.xy).r;
	
	/// Se compara la profundidad del fragmento con la que está guardada en el buffer
	/// Si es mayor, entonces está tapado y tiene sombra (retorna 1)
	/// Si es menor, entonces no está tapado y tiene iluminación (retorna 0)
	return currentDepth > closestDepth ? 1.0 : 0.0;
	
}

/// Calcular el color de cada fragmento
/// Depende de Phong y del valor de shadow calculado en la función anterior
vec4 calcColor(vec3 textureColor) {
	/// Calcular el color del fragmento con Phong
	vec3 phong = calcPhong(lightVSPosition, lightColor, ambientStrength,
						   ambientColor*textureColor, diffuseColor*textureColor,
						   specularColor, shininess);
	
	/// Calcular si el fragmento está sombreado o no
	float shadow = calcShadow(fragPosLightSpace);
	
	/// Chequear que el valor esté en el rango válido
	/// Sino, retornar valores especiales
	if(shadow < 0) {
		if (shadow==-1) return vec4(1.f,0.f,0.f,1.f);
		else            return vec4(0.f,1.f,0.f,1.f);
	}
	
	/// Calcular la componente ambiental
	vec3 ambientComponent = ambientColor*textureColor*ambientStrength;
	
	/// Calcular la iluminación, mezclando los valores de phong, ambiente y sombra
	vec3 lighting = mix(phong, ambientComponent, shadow);
	
	/// Retornar el valor de color
	return vec4(lighting+emissionColor,opacity);
}
