//// The MAIN function, from here we start our application and run our Game loop
//int main()
//{
//	// Init GLFW
//	glfwInit(); //fjc 本项目用的是glutinit
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed
//	glfwMakeContextCurrent(window);
//
//	// Set the required callback functions
//	glfwSetKeyCallback(window, key_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	// Options
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	// Initialize GLEW to setup the OpenGL Function pointers
//	glewExperimental = GL_TRUE;
//	glewInit();
//
//	// Define the viewport dimensions
//	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//
//	// Setup some OpenGL options
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
//
//	// Setup and compile our shaders
//	Shader shader("point_shadows.vs", "point_shadows.frag");
//	Shader simpleDepthShader("point_shadows_depth.vs", "point_shadows_depth.frag", "point_shadows_depth.gs");
//
//	// Set texture samples
//	shader.Use();
//	glUniform1i(glGetUniformLocation(shader.Program, "diffuseTexture"), 0);
//	glUniform1i(glGetUniformLocation(shader.Program, "depthMap"), 1);
//
//	// Light source
//	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
//
//	// Load textures
//	woodTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());
//
//	// Configure depth map FBO
//	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//	GLuint depthMapFBO;
//	glGenFramebuffers(1, &depthMapFBO);
//	// Create depth cubemap texture
//	GLuint depthCubemap;
//	glGenTextures(1, &depthCubemap);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
//	for (GLuint i = 0; i < 6; ++i)
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//	// Attach cubemap as depth map FBO's color buffer
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		std::cout << "Framebuffer not complete!" << std::endl;
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//
//	// Game loop
//	while (!glfwWindowShouldClose(window))
//	{
//		// Set frame time
//		GLfloat currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		// Check and call events
//		glfwPollEvents();
//		Do_Movement();
//
//		// Move light position over time
//		//lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;
//
//		// 0. Create depth cubemap transformation matrices
//		GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
//		GLfloat near = 1.0f;
//		GLfloat far = 25.0f;
//		glm::mat4 shadowProj = glm::perspective(90.0f, aspect, near, far);
//		std::vector<glm::mat4> shadowTransforms;
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
//
//		// 1. Render scene to depth cubemap
//		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//		glClear(GL_DEPTH_BUFFER_BIT);
//		simpleDepthShader.Use();
//		for (GLuint i = 0; i < 6; ++i)
//			glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, ("shadowTransforms[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
//		glUniform1f(glGetUniformLocation(simpleDepthShader.Program, "far_plane"), far);
//		glUniform3fv(glGetUniformLocation(simpleDepthShader.Program, "lightPos"), 1, &lightPos[0]);
//		RenderScene(simpleDepthShader);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		// 2. Render scene as normal
//		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		shader.Use();
//		glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		glm::mat4 view = camera.GetViewMatrix();
//		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
//		// Set light uniforms
//		glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
//		glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camera.Position[0]);
//		// Enable/Disable shadows by pressing 'SPACE'
//		glUniform1i(glGetUniformLocation(shader.Program, "shadows"), shadows);
//		glUniform1f(glGetUniformLocation(shader.Program, "far_plane"), far);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, woodTexture);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
//		RenderScene(shader);
//
//		// Swap the buffers
//		glfwSwapBuffers(window);
//	}
//
//	glfwTerminate();
//	return 0;
//}