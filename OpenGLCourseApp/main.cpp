#define STB_IMAGE_IMPLEMENTATION

#include <cstdio>
#include <cmath>
#include <stdexcept>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Texture.h"
#include "Model.h"

#include <assimp/Importer.hpp>

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
       uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0,
       uniformOmniLightPos = 0, uniformFarPlane = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture, dirtTexture, plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model laptop;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat laptopAngle = 0.0f;

static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

void calculateNormals(const unsigned int* indices, unsigned int indexCount, GLfloat* vertices,
                      unsigned int vertexCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indexCount; i += 3)
	{
		auto in0 = indices[i] * vLength;
		auto in1 = indices[i + 1] * vLength;
		auto in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1],
		             vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1],
		             vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	// Normalize normals
	for (size_t i = 0; i < vertexCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	const unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		// x     y      z        u     v		 nx    ny    nz
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	unsigned int floorIndices[] =
	{
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] =
	{
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calculateNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert",
	                                        "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom",
	                                 "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	laptopAngle += 0.1f;
	if (laptopAngle > 360.0f)
	{
		laptopAngle = deltaTime * 0.1f;
	}

	model = glm::mat4(1.0f);
	model = translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
	model = glm::rotate(model, glm::radians(laptopAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = translate(model, glm::vec3(4.0f, 0.5f, 0.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	laptop.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	auto lTransform = light->CalculateLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&lTransform);

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projection, glm::mat4 view)
{
	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glViewport(0, 0, 1366, 768);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y,
	            camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);

	auto lTransform = mainLight.CalculateLightTransform();
	shaderList[0].SetDirectionalLightTransform(&lTransform);

	mainLight.GetShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	glm::vec3 flashLightPosition = camera.getCameraPosition();
	flashLightPosition.y -= 0.3f;
	spotLights[0].SetFlash(flashLightPosition, camera.getCameraDirection());

	shaderList[0].Validate();

	RenderScene();
}

int main()
{
	mainWindow = Window(1366, 768);
	try
	{
		mainWindow.initialize();
		CreateObjects();
		CreateShaders();

		camera = Camera(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.3f);

		brickTexture = Texture("Textures/brick.png");
		brickTexture.LoadTextureA();
		dirtTexture = Texture("Textures/dirt.png");
		dirtTexture.LoadTextureA();
		plainTexture = Texture("Textures/plain.png");
		plainTexture.LoadTextureA();

		shinyMaterial = Material(4.0f, 256);
		dullMaterial = Material(0.3f, 4);

		laptop = Model();
		laptop.LoadModel("Models/Lowpoly_Notebook_2.obj");
	}
	catch (const std::runtime_error& e)
	{
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	mainLight = DirectionalLight(2048, 2048,
	                             1.0f, 1.0f, 1.0f,
								0.0f, 0.0f,
	                             0.0f, -15.0f, -10.0f);


	pointLights[0] = PointLight(1024, 1024,
	                            0.01f, 100.0f,
	                            0.0f, 0.0f, 1.0f,
	                            0.0f, 1.0f,
	                            1.0f, 2.0f, 0.0f,
	                            0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(1024, 1024,
	                            0.01f, 100.0f,
	                            0.0f, 1.0f, 0.0f,
	                            0.0f, 1.0f,
	                            -4.0f, 3.0f, 0.0f,
	                            0.3f, 0.1f, 0.1f);
	pointLightCount++;


	spotLights[0] = SpotLight(1024, 1024,
	                          0.01f, 100.0f,
	                          1.0f, 1.0f, 1.0f,
	                          0.1f, 1.0f,
	                          0.0f, 0.0f, 0.0f,
	                          0.0f, -1.0f, 0.0f,
	                          1.0f, 0.01f, 0.001f,
	                          20.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(1024, 1024,
	                          0.01f, 100.0f,
	                          1.0f, 1.0f, 1.0f,
	                          0.0f, 2.0f,
	                          0.0f, -1.5f, 0.0f,
	                          -100.0f, -1.0f, 0.0f,
	                          1.0f, 0.01f, 0.001f,
	                          20.0f);
	spotLightCount++;


	glm::mat4 projection = glm::perspective(glm::radians(60.0f),
	                                        static_cast<GLfloat>(mainWindow.getBufferWidth()) / static_cast<GLfloat>(
		                                        mainWindow.getBufferHeight()), 0.1f, 100.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (mainWindow.getKeys()[GLFW_KEY_L])
		{
			spotLights[0].Toggle();
			mainWindow.getKeys()[GLFW_KEY_L] = false;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		DirectionalShadowMapPass(&mainLight);
		// TODO: replace with only one OmniShadowPass using an array of cubemaps, one for each light
		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}
		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}
		RenderPass(projection, camera.calculateViewMatrix());


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return EXIT_SUCCESS;
}
