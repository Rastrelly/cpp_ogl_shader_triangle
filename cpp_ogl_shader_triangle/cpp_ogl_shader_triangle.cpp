#include <iostream>
#include <ctime>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "shader.h"

//typedefs
typedef std::vector<float> flarr;
typedef std::vector<unsigned int> indarr;

//forward decalarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void prepShaders();
void drawTriangle(flarr verts, Shader * sh, float rotang);
void drawRectangle(flarr verts, indarr inds);

//global declaration of a uniform
int vertClr=0;
struct vec4clr
{
	float r, g, b, a;
} triClr;
float rotang = 45.0f;

//RAW DATA
//a triangle

// x , y, z, r, g, b
flarr vertices = {
	-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

//a rectangle
flarr vertices_rect = {
	 0.6f,  0.4f, 0.0f,  //0
	 0.6f, -0.4f, 0.0f,  //1
	-0.6f, -0.4f, 0.0f,  //2
	-0.6f,  0.4f, 0.0f   //3
};

//rectangle indices
indarr eboindices = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

//shader
Shader * shad1;
Shader * shad2;

//MAIN
int main()
{
	//allow randomizer
	srand(time(NULL));
	   
	triClr = { ((float)(rand() % 101) / 100.0f),
			   ((float)(rand() % 101) / 100.0f),
			   ((float)(rand() % 101) / 100.0f),
			   ((float)(rand() % 101) / 100.0f) };

	//select what to draw
	std::cout << "What to draw? [T/R/B]:\n";
	std::string choice = "T";
	std::cin >> choice;

	bool drawtri = false;
	bool drawrect = false;
	if (choice == "T") drawtri = true;
	else if (choice == "R") drawrect = true;
	else { drawrect = true; drawtri = true; };
	

	if (drawtri)
	{
		//Input triangle size
		std::cout << "Input triangle radius (0..1):\n";
		float r = 0;
		std::cin >> r;

		if (r < 0) r = 0.0f;
		if (r > 1) r = 1.0f;

		for (int i = 0; i < vertices.size(); i++)
			vertices[i] = vertices[i] * r;
	}
	
	//INITIALIZTION OF GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Main Window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//INITIALIZTION OF GLAD
	bool gladLoaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!gladLoaded)
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Renderer setup
	//main loop screen refresh callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//ready shaders for use
	prepShaders();

	//MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (drawtri)
		{

			//enable shader program
			shad1->use();
			
			//setup the uniform
			shad1->setVector4f("uniVertColor",
				triClr.r, triClr.g, triClr.b, triClr.a);	

			//call the draw function
			//calc triangle transform
			float ctm = (float)glfwGetTime();
			drawTriangle(vertices, shad1, ctm);

		}
		if (drawrect)
		{
			shad2->use();
			drawRectangle(vertices_rect, eboindices);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//kill GLFW
	glfwTerminate();

	return 0;
}

//screen refresh function for GLFW callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


//keypress process
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void prepShaders()
{
	std::cout << "Preparing shaders..." << std::endl;
	shad1 = new Shader("shader_vert.gls","shader_frag.gls");
	shad2 = new Shader("shader_vert2.gls", "shader_frag2.gls");
}

void drawTriangle(flarr verts, Shader * sh, float rotang)
{

	//buffers
	unsigned int VBO; //vertex buffer
	unsigned int VAO; //vertex array

	glGenBuffers(1, &VBO); //generate vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //assign data type to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*verts.size(), verts.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glm::mat4 transf = glm::mat4(1.0f);
	transf = glm::rotate(transf, rotang, glm::vec3(0.0, 0.0, 1.0));
	sh->setMatrix4f("transform", transf);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void drawRectangle(flarr verts, indarr inds)
{

	//buffers
	unsigned int VBO; //vertex buffer
	unsigned int EBO; //elemnt buffer
	unsigned int VAO; //vertex array

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO); //generate vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //assign data type to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*verts.size(), verts.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO); //generate element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //assgn data type
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*inds.size(), inds.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//different draw function!!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
}