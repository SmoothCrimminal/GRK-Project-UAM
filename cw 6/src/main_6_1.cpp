#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include <iostream>
#include "SOIL/SOIL.h"

using namespace std;

int windowID;
int flag = 0;
GLuint programColor;
GLuint programTexture;
GLuint programSkybox;
GLuint textureCubemap;
GLuint programTreasure;

Core::RenderContext bubbleContext;

Core::Shader_Loader shaderLoader;
int points = 0;
Core::RenderContext shipContext;
Core::RenderContext bottomContext;
Core::RenderContext fishContext;
Core::RenderContext fishContext2;
Core::RenderContext sharkContext;
Core::RenderContext plantaContext;
Core::RenderContext coralContext;
Core::RenderContext sunflowerContext;
Core::RenderContext pencilContext;
Core::RenderContext treasureContext;

glm::vec3 fishPositions[500];
int fishRandom[502];
int completeRandom[602];
float fishSpreadFactor = 500.0;

float old_x, old_y = -1;
float delta_x, delta_y = 0;
glm::quat rotationCamera = glm::quat(1, 0, 0, 0);
glm::quat rotation_y = glm::normalize(glm::angleAxis(209 * 0.03f, glm::vec3(1, 0, 0)));
glm::quat rotation_x = glm::normalize(glm::angleAxis(307 * 0.03f, glm::vec3(0, 1, 0)));
float dy = 0;
float dx = 0;

glm::vec3 cameraPos = glm::vec3(0, 460, 0);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery
float cameraAngle = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(-0.1f, -0.55f, -0.69f));
glm::vec3 skyColor = glm::vec3(0.282f, 0.38f, 0.624f);
float fogDensity = 0.02;
float fogGradient = 0.99;

glm::quat rotation = glm::quat(1, 0, 0, 0);

GLuint textureReef;
GLuint textureFish;
GLuint textureFish2;
GLuint textureShark;
GLuint textureDuck;
GLuint texturePlanta;
GLuint textureBubble;
GLuint textureCoral;
GLuint textureSunflower;
GLuint texturePencil;
GLuint textureTreasure;

std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
std::vector< glm::vec3 > temp_vertices;
std::vector< glm::vec2 > temp_uvs;
std::vector< glm::vec3 > temp_normals;

unsigned int skyboxVAO, skyboxVBO;

char lineHeader[128];
float bubbleCount = 0.0f;
float skyboxSize = 998.0f;
float groundLevel = 455.0f;
float spaceLevel = 600.0f;

bool isTreasure1Free = true;
bool isTreasure2Free = true;
bool isTreasure3Free = true;

glm::vec3 treasure1Localization = glm::vec3(30, 445, 30);
glm::vec3 treasure2Localization = glm::vec3(239, 445, 34);
glm::vec3 treasure3Localization = glm::vec3(153, 445, -280);;


float skyboxVertices[] =
{
	20.0f, -20.0f,  20.0f,
	 20.0f, 20.0f,  20.0f,
	 20.0f, 20.0f, -20.0f,
	20.0f, -20.0f, -20.0f,

	-20.0f,  -20.0f,  -20.0f,
	 -20.0f,  20.0f,  -20.0f,
	 -20.0f,  20.0f, 20.0f,
	-20.0f,  -20.0f, 20.0f,

	-20.0f, 20.0f,  -20.0f,
	 20.0f, 20.0f,  -20.0f,
	 20.0f, 20.0f, 20.0f,
	-20.0f, 20.0f, 20.0f,

	-20.0f,  -20.0f,  20.0f,
	 20.0f,  -20.0f,  20.0f,
	 20.0f,  -20.0f, -20.0f,
	-20.0f,  -20.0f, -20.0f,


	20.0f, -20.0f,  20.0f,
	-20.0f, -20.0f,  20.0f,
	-20.0f, 20.0f, 20.0f,
	20.0f, 20.0f, 20.0f,

	-20.0f,  -20.0f,  -20.0f,
	 20.0f,  -20.0f,  -20.0f,
	 20.0f,  20.0f, -20.0f,
	-20.0f,  20.0f, -20.0f
};


bool insideSkybox(glm::vec3 nextPosition) {
	return nextPosition.x < skyboxSize&& nextPosition.x > -skyboxSize
		&& nextPosition.y > groundLevel && nextPosition.y < skyboxSize
		&& nextPosition.z > -skyboxSize && nextPosition.z < skyboxSize;


}

void earnTreasure(glm::vec3 localization) {
	if (localization.x < treasure1Localization.x + 10 && localization.x > treasure1Localization.x - 10
		&& localization.y < treasure1Localization.y + 10 && localization.y > treasure1Localization.y - 10
		&& localization.z < treasure1Localization.z + 10 && localization.z > treasure1Localization.z - 10
		&& isTreasure1Free)
	{
		points = points + 1;
		isTreasure1Free = false;

	}

	if (localization.x < treasure2Localization.x + 10 && localization.x > treasure2Localization.x - 10
		&& localization.y < treasure2Localization.y + 10 && localization.y > treasure2Localization.y - 10
		&& localization.z < treasure2Localization.z + 10 && localization.z > treasure2Localization.z - 10
		&& isTreasure2Free)
	{
		points = points + 1;
		isTreasure2Free = false;

	}

	if (localization.x < treasure3Localization.x + 10 && localization.x > treasure3Localization.x - 10
		&& localization.y < treasure3Localization.y + 10 && localization.y > treasure3Localization.y - 10
		&& localization.z < treasure3Localization.z + 10 && localization.z > treasure3Localization.z - 10
		&& isTreasure3Free)
	{
		points = points + 1;
		isTreasure3Free = false;
	}


}

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.8f;
	glm::vec3 desiredPosition;

	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': desiredPosition = cameraPos + cameraDir * moveSpeed; if (insideSkybox(desiredPosition)) { cameraPos = desiredPosition; } break;
	case 's': desiredPosition = cameraPos - cameraDir * moveSpeed; if (insideSkybox(desiredPosition)) { cameraPos = desiredPosition; } break;
	case 'd': desiredPosition = cameraPos + cameraSide * moveSpeed; if (insideSkybox(desiredPosition)) { cameraPos = desiredPosition; } break;
	case 'a': desiredPosition = cameraPos - cameraSide * moveSpeed; if (insideSkybox(desiredPosition)) { cameraPos = desiredPosition; } break;
	case 'p': flag = 30;
	}

	earnTreasure(desiredPosition);
	//cout << "X: " << desiredPosition.x << " Y: " << desiredPosition.y << " Z: " << desiredPosition.z << endl;
}

void mouse(int x, int y)
{
	if (old_x >= 0) {
		delta_x = x - old_x;
		delta_y = y - old_y;
	}
	old_x = x;
	old_y = y;
}

bool checkFishPos(glm::vec2 positions) {
	return positions.x < 400 && positions.y < 400;
}

bool checkPosition(float x, float z) {
	return (x > 70 || x < -70)
		&& (z > 70 || z < -70);
}


glm::mat4 createCameraMatrix()
{
	auto rot_y = glm::angleAxis(delta_y * 0.03f, glm::vec3(1, 0, 0));
	auto rot_x = glm::angleAxis(delta_x * 0.03f, glm::vec3(0, 1, 0));

	dy += delta_y;
	dx += delta_x;
	delta_x = 0;
	delta_y = 0;

	rotation_x = glm::normalize(rot_x * rotation_x);
	rotation_y = glm::normalize(rot_y * rotation_y);

	rotationCamera = glm::normalize(rotation_y * rotation_x);

	auto inverse_rot = glm::inverse(rotationCamera);

	cameraDir = inverse_rot * glm::vec3(0, 0, -1);
	glm::vec3 up = glm::vec3(0, 1, 0);
	cameraSide = inverse_rot * glm::vec3(1, 0, 0);

	glm::mat4 cameraTranslation;
	cameraTranslation[3] = glm::vec4(-cameraPos, 1.0f);

	return glm::mat4_cast(rotationCamera) * cameraTranslation;
}



void drawObjectColor(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;


	glUseProgram(program);

	glUniform1f(glGetUniformLocation(program, "fogDensity"), fogDensity);
	glUniform1f(glGetUniformLocation(program, "fogGradient"), fogGradient);


	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "skyColor"), skyColor.x, skyColor.y, skyColor.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"), 1, GL_FALSE, (float*)&perspectiveMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawContext(context);

	glUseProgram(0);
}

void drawObjectTexture(Core::RenderContext context, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform1f(glGetUniformLocation(program, "fogDensity"), fogDensity);
	glUniform1f(glGetUniformLocation(program, "fogGradient"), fogGradient);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "skyColor"), skyColor.x, skyColor.y, skyColor.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"), 1, GL_FALSE, (float*)&perspectiveMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawContext(context);

	glUseProgram(0);
}
void drawObjectTreasure(Core::RenderContext context, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTreasure;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawContext(context);

	glUseProgram(0);
}


//void drawBubbles(float time, float xPosition, float yPosition, float zPosition) {
//
//	drawObjectTexture(bubbleContext, glm::translate(glm::vec3(xPosition, yPosition + 800 * bubbleCount, zPosition)) * glm::scale(glm::vec3(1.1f) * glm::vec3(0.1f)),textureBubble);
//
//	//drawObjectTexture(bubbleContext, glm::translate(glm::vec3(xPosition + 22.5, yPosition + 2.5 + 800 * bubbleCount, zPosition - 2.5)) * glm::scale(glm::vec3(1.1f) * glm::vec3(0.1f)) *
//	//	glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, 1, 0)) *
//	//	glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) *
//	//	glm::eulerAngleY(sin(time * 5) / 4),
//	//	textureBubble);
//
//	//drawObjectTexture(bubbleContext, glm::translate(glm::vec3(xPosition - 22.5, yPosition + 2.5 + 800 * bubbleCount, zPosition)) * glm::scale(glm::vec3(1.1f) * glm::vec3(0.1f)) *
//	//	glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, 1, 0)) *
//	//	glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) *
//	//	glm::eulerAngleY(sin(time * 5) / 4),
//	//	textureBubble);
//}

void setupSkybox() {
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	textureCubemap = SOIL_load_OGL_cubemap(
		"textures/uw_ft.jpg",
		"textures/uw_bk.jpg",
		"textures/uw_up.jpg",
		"textures/uw_dn.jpg",
		"textures/uw_rt.jpg",
		"textures/uw_lf.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



}

void renderSkybox(glm::mat4 modelMatrix, GLuint textureId) {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_CLAMP);

	GLuint program = programSkybox;

	glUseProgram(program);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);

	glUniform1i(glGetUniformLocation(program, "skybox"), 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubemap);

	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_QUADS, 0, 24);
	glBindVertexArray(0);
	glRasterPos2f(0.70, 0.85);
	glColor4f(1, 1, 0, 1);


	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);

	glDepthMask(GL_TRUE);

	for (int i = 0; i < 602; i++) {

		if (i % 50 == 0 && checkPosition(completeRandom[i], completeRandom[i + 1]) && ((completeRandom[i] > 239 + 30 || completeRandom[i] < 239 - 30) && (completeRandom[i + 1] > 34 + 30 || completeRandom[i + 1] < 34 - 30)) && ((completeRandom[i] > 153 + 30 || completeRandom[i] < 153 - 30) && (completeRandom[i + 1] > -280 + 30 || completeRandom[i + 1] < -280 - 30))) {
			drawObjectTexture(coralContext, glm::translate(glm::vec3(completeRandom[i], groundLevel - 20, completeRandom[i + 1])) * glm::scale(glm::vec3(100.0f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureCoral);
		}
		else if (i % 3 == 0) {
			drawObjectTexture(plantaContext, glm::translate(glm::vec3(completeRandom[i] - 1, groundLevel - 6.5, completeRandom[i + 1])) * glm::rotate(glm::radians(0.0f), glm::vec3(-1, 0, 0)) * glm::scale(glm::vec3(0.7f) * glm::vec3(0.2f)), texturePlanta);
			drawObjectTexture(plantaContext, glm::translate(glm::vec3(completeRandom[i] + 3, groundLevel - 6.5, completeRandom[i + 1] - 2)) * glm::rotate(glm::radians(0.0f), glm::vec3(-1, 0, 0)) * glm::scale(glm::vec3(0.7f) * glm::vec3(0.2f)), texturePlanta);
			drawObjectTexture(plantaContext, glm::translate(glm::vec3(completeRandom[i] - 2, groundLevel - 6.5, completeRandom[i + 1] + 1)) * glm::rotate(glm::radians(0.0f), glm::vec3(-1, 0, 0)) * glm::scale(glm::vec3(0.7f) * glm::vec3(0.2f)), texturePlanta);
			drawObjectTexture(plantaContext, glm::translate(glm::vec3(completeRandom[i] + 2, groundLevel - 6.5, completeRandom[i + 1])) * glm::rotate(glm::radians(0.0f), glm::vec3(-1, 0, 0)) * glm::scale(glm::vec3(0.7f) * glm::vec3(0.2f)), texturePlanta);
		}
		else if (i % 2 == 0) {
			drawObjectTexture(pencilContext, glm::translate(glm::vec3(completeRandom[i] + 10, groundLevel - 5, completeRandom[i + 1] + 2)) * glm::scale(glm::vec3(6.0f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), texturePencil);
		}
		else {
			drawObjectTexture(sunflowerContext, glm::translate(glm::vec3(completeRandom[i] + 10, groundLevel - 2, completeRandom[i + 1] + 2)) * glm::scale(glm::vec3(6.5f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureSunflower);
		}
	}
}



void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glm::mat4 skyboxModel = glm::scale(glm::mat4(1.0f), glm::vec3(50, 50, 50));
	renderSkybox(skyboxModel, textureCubemap);


	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.002f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * glm::mat4_cast(glm::inverse(rotationCamera)) * shipInitialTransformation;
	drawObjectTexture(shipContext, shipModelMatrix, textureDuck);

	drawObjectTexture(bottomContext, glm::translate(glm::vec3(0, 450, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)) * glm::scale(glm::vec3(2.00f) * glm::vec3(0.2f)), textureReef);


	for (int i = 0; i < 600; i++) {
		if (i % 2 == 0) {
			drawObjectTexture(fishContext2, glm::translate(fishPositions[i]) * glm::translate(glm::vec3(0, 420, 0)) * glm::scale(glm::vec3(0.7f) * glm::vec3(0.7f)) *
				glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, 1, 0)) *
				glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) *
				glm::translate(glm::vec3(-fishRandom[i] + -fishRandom[i] / 60 * i, -fishRandom[i + 1] / 2, -fishRandom[i + 2] / 2)) *
				glm::eulerAngleY(sin(time * 10) / 4),
				textureFish2);
		}
		else {
			drawObjectTexture(fishContext2, glm::translate(fishPositions[i]) * glm::translate(glm::vec3(0, 420, 0)) * glm::scale(glm::vec3(0.7f) * glm::vec3(0.7f)) *
				glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, -1, 0)) *
				glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) *
				glm::translate(glm::vec3(-fishRandom[i] + -fishRandom[i] / 60 * i, -fishRandom[i + 1] / 2, -fishRandom[i + 2] / 2)) *
				glm::eulerAngleY(sin(time * 10) / 4) *
				glm::rotate(glm::radians(160.0f), glm::vec3(0, 1, 0)),
				textureFish2);
		}
	}

	drawObjectTexture(sharkContext, glm::translate(glm::vec3(10, 685, 400)) *
		glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, -1, 0)) *
		glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-95, -95, -100)) *
		glm::rotate(glm::radians((time / 12) * 0.f), glm::vec3(0, 1, 0)) * glm::eulerAngleY(sin(time * 2) / 4) *
		glm::scale(glm::vec3(0.4f) * glm::vec3(0.4f)) *
		glm::rotate(glm::radians(220.0f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)),
		textureShark);

	drawObjectTexture(sharkContext, glm::translate(glm::vec3(10, 590, 100)) *
		glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, -1, 0)) *
		glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-95, -95, -100)) *
		glm::rotate(glm::radians((time / 12) * 0.f), glm::vec3(0, 1, 0)) * glm::eulerAngleY(sin(time * 2) / 4) *
		glm::scale(glm::vec3(0.4f) * glm::vec3(0.4f)) *
		glm::rotate(glm::radians(225.0f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)),
		textureShark);

	drawObjectTexture(sharkContext, glm::translate(glm::vec3(40, 720, 30)) *
		glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-95, -95, -100)) *
		glm::rotate(glm::radians((time / 12) * 0.f), glm::vec3(0, 1, 0)) * glm::eulerAngleY(sin(time * 2) / 4) *
		glm::scale(glm::vec3(0.4f) * glm::vec3(0.4f)) *
		glm::rotate(glm::radians(40.0f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)),
		textureShark);

	drawObjectTexture(sharkContext, glm::translate(glm::vec3(20, 675, 360)) *
		glm::rotate(glm::radians((time / 12) * 90.f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)) * glm::translate(glm::vec3(-95, -95, -100)) *
		glm::rotate(glm::radians((time / 12) * 0.f), glm::vec3(0, 1, 0)) * glm::eulerAngleY(sin(time * 2) / 4) *
		glm::scale(glm::vec3(0.4f) * glm::vec3(0.4f)) *
		glm::rotate(glm::radians(40.0f), glm::vec3(0, 1, 0)) *
		glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)),
		textureShark);

	for (int j = 0; j < 20; j++) {
		for (int i = 400; i < 600; i = i + 6)
			drawObjectTexture(bubbleContext, glm::translate(glm::vec3(completeRandom[j] / 4 + completeRandom[i] / 200, i + 800 * bubbleCount, completeRandom[j + 1] / 4 + completeRandom[i + 1] / 200)) * glm::scale(glm::vec3(1.1f) * glm::vec3(0.1f)), textureBubble);
	}

	if (bubbleCount > 0.08) {
		bubbleCount = 0.0f;
	}
	else {
		bubbleCount = bubbleCount + 0.0004f;
	}

	//cout << isTreasure1Free << endl;

	if (isTreasure1Free) {
		if (flag > 0) {
			drawObjectTreasure(treasureContext, glm::translate(treasure1Localization) * glm::scale(glm::vec3(0.8f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureTreasure);
			flag = flag - 1;
		}
		else {
			drawObjectTexture(treasureContext, glm::translate(treasure1Localization) * glm::scale(glm::vec3(0.8f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureTreasure);


		}
	}
	if (isTreasure2Free) {
		if (flag > 0) {
			drawObjectTreasure(treasureContext, glm::translate(treasure2Localization) * glm::scale(glm::vec3(0.8f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureTreasure);
			flag = flag - 1;
		}
		else {
			drawObjectTexture(treasureContext, glm::translate(treasure2Localization) * glm::scale(glm::vec3(0.8f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureTreasure);


		}
	}
	if (isTreasure3Free) {
		if (flag > 0) {
			drawObjectTreasure(treasureContext, glm::translate(treasure3Localization) * glm::scale(glm::vec3(0.8f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureTreasure);
			flag = flag - 1;
		}
		else {
			drawObjectTexture(treasureContext, glm::translate(treasure3Localization) * glm::scale(glm::vec3(0.8f) * glm::vec3(0.2f)) * glm::rotate(glm::radians(90.0f), glm::vec3(-1, 0, 0)), textureTreasure);


		}
	}
	if (points < 2) {
		glRasterPos2f(0.70, 0.85);
		glColor4f(200, 100, 100, 1);
		std::string text = "Points: ";
		text = text + std::to_string(points);
		const unsigned char* t = reinterpret_cast<const unsigned char*>(text.c_str());
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, t);
	}
	else {
		glRasterPos2f(-0.1, 0);
		glColor4f(0.2, 0.8, 0.4, 1);
		std::string text = "You won !!!";
		const unsigned char* t = reinterpret_cast<const unsigned char*>(text.c_str());
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, t);
	}



	glutSwapBuffers();
}

void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}



void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FOG);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	programTreasure = shaderLoader.CreateProgram("shaders/shader_treasure_tex.vert", "shaders/shader_treasure_tex.frag");
	loadModelToContext("models/boat.obj", shipContext);
	loadModelToContext("models/bottom.obj", bottomContext);
	loadModelToContext("models/nemo.obj", fishContext);
	loadModelToContext("models/fish.obj", fishContext2);
	loadModelToContext("models/shark.obj", sharkContext);
	loadModelToContext("models/planta.obj", plantaContext);
	loadModelToContext("models/sphere.obj", bubbleContext);
	loadModelToContext("models/coral.obj", coralContext);
	loadModelToContext("models/sunflower.obj", sunflowerContext);
	loadModelToContext("models/pencil.obj", pencilContext);
	loadModelToContext("models/treasure.obj", treasureContext);
	setupSkybox();
	textureFish = Core::LoadTexture("textures/color.jpg");
	textureReef = Core::LoadTexture("textures/ground.jpg");
	textureFish2 = Core::LoadTexture("textures/fish.png");
	textureShark = Core::LoadTexture("textures/shark.jpg");
	textureDuck = Core::LoadTexture("textures/boat.jpg");
	texturePlanta = Core::LoadTexture("textures/planta.jpg");
	textureBubble = Core::LoadTexture("textures/bubble.png");
	textureCoral = Core::LoadTexture("textures/reef.jpg");
	textureSunflower = Core::LoadTexture("textures/sand.jpg");
	texturePencil = Core::LoadTexture("textures/sunflower.jpg");
	textureTreasure = Core::LoadTexture("textures/treasure.jpg");
	//textureFish = Core::LoadTexture("textures/xd.jpg");


	for (int i = 0; i < 500; i++) fishPositions[i] = glm::ballRand(fishSpreadFactor);
	for (int i = 0; i < 502; i++) fishRandom[i] = rand() % 200 + 40;
	for (int i = 0; i < 602; i++) completeRandom[i] = rand() % 800 - 400;
	//for (int i = 0; i < 302; i++) {
	//	drawObjectTexture(plantaContext, glm::translate(glm::vec3(completeRandom[i], 445, completeRandom[i + 1])) * glm::rotate(glm::radians(0.0f), glm::vec3(-1, 0, 0)) * glm::scale(glm::vec3(2.0f) * glm::vec3(0.2f)), texturePlanta);

	//}
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	windowID = glutCreateWindow("Underwater");
	glewInit();
	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
