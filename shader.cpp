#include "shader.h"
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
    namespace Core {
        namespace Shader {
            GLuint LoadUniform(const GLuint program, const char* uniform) {
                return glGetUniformLocation(program, uniform);
            }

			ShaderUniforms LoadUniforms(const GLuint program) {
				ShaderUniforms u;
				for (int i = 0; i < BOID_COUNT; i++) {
					u.position[i] = LoadUniform(program, ("position[" + std::to_string(i) + "]").c_str());
					u.predators[i] = LoadUniform(program, ("predators[" + std::to_string(i) + "]").c_str());
				}
				u.projection = LoadUniform(program, "projection");
				u.view = LoadUniform(program, "view");

				//float vertices[] = {
				//	 0.5f,  0.5f, 0.0f,  // top right
				//	 0.5f, -0.5f, 0.0f,  // bottom right
				//	-0.5f, -0.5f, 0.0f,  // bottom left
				//	-0.5f,  0.5f, 0.0f   // top left 
				//};
				//unsigned int indices[] = {  // note that we start from 0!
				//	0, 1, 3,  // first Triangle
				//	1, 2, 3   // second Triangle
				//};

				//glGenVertexArrays(1, &u.VAO);
				//glGenBuffers(1, &u.VBO);
				//glGenBuffers(1, &u.EBO);
				//// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
				//glBindVertexArray(u.VAO);

				//glBindBuffer(GL_ARRAY_BUFFER, u.VBO);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, u.EBO);
				//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

				//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
				//glEnableVertexAttribArray(0);

				//// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
				//glBindBuffer(GL_ARRAY_BUFFER, 0);

				//// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
				////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				//// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
				//// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
				//glBindVertexArray(0);

				float vertices[] = {
					-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
					 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
					 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
					-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

					-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
					 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
					-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
					-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

					-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
					-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
					-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
					-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

					 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
					 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
					 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
					 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

					-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
					 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
					-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
					-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

					-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
					 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
					 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
					-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
					-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
				};

				glGenVertexArrays(1, &u.VAO);
				glGenBuffers(1, &u.VBO);

				glBindVertexArray(u.VAO);

				glBindBuffer(GL_ARRAY_BUFFER, u.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				// position attribute
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				// texture coord attribute
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);

				// load and create a texture 
	// -------------------------
				//unsigned int texture1, texture2;
				//// texture 1
				//// ---------
				//glGenTextures(1, &texture1);
				//glBindTexture(GL_TEXTURE_2D, texture1);
				//// set the texture wrapping parameters
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				//// set texture filtering parameters
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//// load image, create texture and generate mipmaps
				//int width, height, nrChannels;
				//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
				//unsigned char* data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
				//if (data)
				//{
				//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				//	glGenerateMipmap(GL_TEXTURE_2D);
				//}
				//else
				//{
				//	std::cout << "Failed to load texture" << std::endl;
				//}
				//stbi_image_free(data);
				//// texture 2
				//// ---------
				//glGenTextures(1, &texture2);
				//glBindTexture(GL_TEXTURE_2D, texture2);
				//// set the texture wrapping parameters
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				//// set texture filtering parameters
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//// load image, create texture and generate mipmaps
				//data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
				//if (data)
				//{
				//	// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
				//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				//	glGenerateMipmap(GL_TEXTURE_2D);
				//}
				//else
				//{
				//	std::cout << "Failed to load texture" << std::endl;
				//}
				//stbi_image_free(data);

				return u;
			}

			void PrepareShader(const ShaderUniforms u, const GLuint program, const glm::mat4 projection, const glm::mat4 view) {
				glUseProgram(program);
				//glUniform3f(u.color, color.x, color.y, color.z);
				glUniformMatrix4fv(u.projection, 1, GL_FALSE, &projection[0][0]);
				glUniformMatrix4fv(u.view, 1, GL_FALSE, &view[0][0]);
			}

			void RenderTemp(const ShaderUniforms u, Boid v[]) {
				glBindVertexArray(u.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
				
				for (unsigned int j = 0; j < BOID_COUNT; j+= 200)
				{
					unsigned int i = 0;
					for (; i < 200 && (i + j < BOID_COUNT); i++) {

						// calculate the model matrix for each object and pass it to shader before drawing
						glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
						model = glm::translate(model, v[i + j].location);
						if (v[i + j].predator) {
							model = glm::scale(model, glm::vec3(.3f));
						}
						else {
							model = glm::scale(model, glm::vec3(0.1));
						}
						
						
						float angle = 23.678f * i + j;
						model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
						glUniformMatrix4fv(u.position[i], 1, GL_FALSE, &model[0][0]);
						glUniform1i(u.predators[i], v[i + j].predator);
						//glUniform3fv(u.position[i], 1, &v[i].location.x);
					}
					glDrawArraysInstanced(GL_TRIANGLES, 0, 36, i);
				}
			}

			GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

				// Create the shaders
				GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
				GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

				// Read the Vertex Shader code from the file
				std::string VertexShaderCode;
				std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
				if (VertexShaderStream.is_open()) {
					std::stringstream sstr;
					sstr << VertexShaderStream.rdbuf();
					VertexShaderCode = sstr.str();
					VertexShaderStream.close();
				}
				else {
					printf("Cannot open %s!\n", vertex_file_path);
					getchar();
					return 0;
				}

				// Read the Fragment Shader code from the file
				std::string FragmentShaderCode;
				std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
				if (FragmentShaderStream.is_open()) {
					std::stringstream sstr;
					sstr << FragmentShaderStream.rdbuf();
					FragmentShaderCode = sstr.str();
					FragmentShaderStream.close();
				}

				GLint Result = GL_FALSE;
				int InfoLogLength;


				// Compile Vertex Shader
				printf("Compiling shader : %s\n", vertex_file_path);
				char const* VertexSourcePointer = VertexShaderCode.c_str();
				glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
				glCompileShader(VertexShaderID);

				// Check Vertex Shader
				glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
				glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
					glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
					printf("%s\n", &VertexShaderErrorMessage[0]);
				}



				// Compile Fragment Shader
				printf("Compiling shader : %s\n", fragment_file_path);
				char const* FragmentSourcePointer = FragmentShaderCode.c_str();
				glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
				glCompileShader(FragmentShaderID);

				// Check Fragment Shader
				glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
				glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
					glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
					printf("%s\n", &FragmentShaderErrorMessage[0]);
				}



				// Link the program
				printf("Linking program\n");
				GLuint ProgramID = glCreateProgram();
				glAttachShader(ProgramID, VertexShaderID);
				glAttachShader(ProgramID, FragmentShaderID);
				glLinkProgram(ProgramID);

				// Check the program
				glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
				glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
					glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
					printf("%s\n", &ProgramErrorMessage[0]);
				}


				glDetachShader(ProgramID, VertexShaderID);
				glDetachShader(ProgramID, FragmentShaderID);

				glDeleteShader(VertexShaderID);
				glDeleteShader(FragmentShaderID);

				return ProgramID;
			}
        }
    }
}