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

			ParticleShaderUniforms LoadParticleUniforms(const GLuint program) {
				ParticleShaderUniforms u;
				u.VP = LoadUniform(program, "VP");
				u.CameraRight_worldspace = LoadUniform(program, "CameraRight_worldspace");
				u.CameraUp_worldspace = LoadUniform(program, "CameraUp_worldspace");

				const GLfloat g_vertex_buffer_data[] = {
				 -0.5f, -0.5f, 0.0f,
				  0.5f, -0.5f, 0.0f,
				 -0.5f,  0.5f, 0.0f,
				  0.5f,  0.5f, 0.0f,
				};
				glGenVertexArrays(1, &u.VAO);
				glBindVertexArray(u.VAO);

				glGenBuffers(1, &u.billboardVertexBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardVertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

				// The VBO containing the positions and sizes of the particles
				glGenBuffers(1, &u.billboardPositionBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardPositionBuffer);
				// Initialize with empty (NULL) buffer : it will be updated later, each frame.
				glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

				// The VBO containing the colors of the particles
				glGenBuffers(1, &u.billboardColorBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardColorBuffer);
				// Initialize with empty (NULL) buffer : it will be updated later, each frame.
				glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

				return u;
			}
			ShaderUniforms LoadUniforms(const GLuint program) {
				ShaderUniforms u;
				for (int i = 0; i < BOID_COUNT; i++) {
					u.position[i] = LoadUniform(program, ("position[" + std::to_string(i) + "]").c_str());
					u.predators[i] = LoadUniform(program, ("predators[" + std::to_string(i) + "]").c_str());
				}
				u.projection = LoadUniform(program, "projection");
				u.view = LoadUniform(program, "view");

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

				return u;
			}

			void PrepareParticleShader(const ParticleShaderUniforms u, const GLuint program, const glm::mat4 vp, glm::vec3 camRight, glm::vec3 camUp) {
				glUseProgram(program);
				glUniformMatrix4fv(u.VP, 1, GL_FALSE, &vp[0][0]);
				glUniform3f(u.CameraRight_worldspace, camRight.x, camRight.y, camRight.z);
				glUniform3f(u.CameraUp_worldspace, camUp.x, camUp.y, camUp.z);
			}

			void PrepareShader(const ShaderUniforms u, const GLuint program, const glm::mat4 projection, const glm::mat4 view) {
				glUseProgram(program);
				glUniformMatrix4fv(u.projection, 1, GL_FALSE, &projection[0][0]);
				glUniformMatrix4fv(u.view, 1, GL_FALSE, &view[0][0]);
			}

			void RenderParticles(const ParticleShaderUniforms u, Particle particles[], GLfloat particlePositionData[], GLubyte particleColorData[], int count) {
				glBindVertexArray(u.VAO);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardPositionBuffer);
				glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
				glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(GLfloat) * 4, particlePositionData);

				glBindBuffer(GL_ARRAY_BUFFER, u.billboardColorBuffer);
				glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
				glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(GLubyte) * 4, particleColorData);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardVertexBuffer);
				glVertexAttribPointer(
					0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

				// 2nd attribute buffer : positions of particles' centers
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardPositionBuffer);
				glVertexAttribPointer(
					1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
					4,                                // size : x + y + z + size => 4
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);

				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, u.billboardColorBuffer);
				glVertexAttribPointer(
					2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
					4,                                // size : r + g + b + a => 4
					GL_UNSIGNED_BYTE,                 // type
					GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
					0,                                // stride
					(void*)0                          // array buffer offset
				);

				glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
				glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
				glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

				glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
			}

			void Render(const ShaderUniforms u, Boid v[]) {
				glClear(GL_DEPTH_BUFFER_BIT); 
				glBindVertexArray(u.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
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
							model = glm::scale(model, glm::vec3(0.05));
						}
						
						
						float angle = 23.678f * i + j;
						model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
						glUniformMatrix4fv(u.position[i], 1, GL_FALSE, &model[0][0]);
						glUniform1i(u.predators[i], v[i + j].predator);
						//glUniform3fv(u.position[i], 1, &v[i].location.x);
					}
					glDrawArraysInstanced(GL_TRIANGLES, 0, 36, i);
				}
				//glEnable(GL_DEPTH_TEST);
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