#include "Global.h"
#include <fstream>

std::string Global::BASE_PATH = "";
const std::string Global::WHITESPACE = " \n\r\t\f\v";

std::string Global::getExtension(const std::string& path) {
	auto ind = path.find_last_of('.');
	if (ind != std::string::npos) {
		std::string extension = path.substr(ind + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		return extension;
	}
	return "";
}

std::string Global::ltrim(const std::string& s) {
	size_t start = s.find_first_not_of(WHITESPACE);
	return start == std::string::npos ? "" : s.substr(start);
}

std::string Global::rtrim(const std::string& s) {
	size_t end = s.find_last_not_of(WHITESPACE);
	return end == std::string::npos ? "" : s.substr(0, end + 1);
}

std::string Global::trim(const std::string& s) {
	return rtrim(ltrim(s));
}

std::string Global::getReplace(const std::string& str, const std::string& from, const std::string& to) {
	std::string result = str;
	replace(result, from, to);
	return result;
}

std::string Global::getReplaceAll(const std::string& str, const std::string& from, const std::string& to) {
	std::string result = str;
	replaceAll(result, from, to);
	return result;
}

bool Global::replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void Global::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

glm::vec3 Global::degreesToRadians(glm::vec3 rotation) {
	return glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
}

glm::vec3 Global::radiansToDegrees(glm::vec3 rotation) {
	return glm::vec3(glm::degrees(rotation.x), glm::degrees(rotation.y), glm::degrees(rotation.z));
}

glm::vec3 Global::getScale(glm::mat4 matrix) {
	glm::vec3 result;
	glm::mat4 transpose = glm::transpose(matrix);
	glm::mat4 s = transpose * matrix;
	result.x = sqrt(s[0][0]);
	result.y = sqrt(s[1][1]);
	result.z = sqrt(s[2][2]);
	return result;
}

glm::mat4 Global::aiMatrix4x4ToGlm(const aiMatrix4x4 &from) {
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

bool Global::endsWith(std::string text, std::string end) {
	if (text.length() < end.length()) {
		return false;
	}
	return std::equal(end.rbegin(), end.rend(), text.rbegin());
}

bool Global::startsWith(std::string text, std::string start) {
	if (text.length() < start.length()) {
		return false;
	}
	return std::equal(start.begin(), start.end(), text.begin());
}

std::string Global::jsonValueToString(Json::Value value) {
	static Json::StyledWriter styledWriter;
	return styledWriter.write(value);
}

std::string Global::readFullFile(std::string path) {
	std::ifstream data(path, std::ios::binary | std::ios::ate);
	const std::streamsize fileSize = data.tellg();
	data.seekg(0, std::ios::beg);
	char* shaderText = new char[fileSize + 1];
	shaderText[fileSize] = '\0';
	std::string text = "";
	if (data.read(shaderText, fileSize)) {
		text = shaderText;
	}
	return text;
}

Json::Value Global::readJsonFile(std::string path) {
	std::string text = readFullFile(path);
	if (text.length() == 0) {
		return Json::Value();
	}
	static Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	Json::Value root;
	std::string errors;
	if (reader->parse(text.c_str(), text.c_str() + text.size(), &root, &errors)) {
		delete reader;
		return root;
	}
	delete reader;
	return Json::Value();
}

void Global::saveToFile(std::string path, Json::Value value) {
	saveToFile(path, jsonValueToString(value));
}

void Global::saveToFile(std::string path, std::string content) {
	std::remove(path.c_str());
	std::ofstream out(path);
	out << content;
	out.close();
}

glm::vec4 Global::planeEquationOfPoints(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	float a1 = p2.x - p1.x;
	float b1 = p2.y - p1.y;
	float c1 = p2.z - p1.z;
	float a2 = p3.x - p1.x;
	float b2 = p3.y - p1.y;
	float c2 = p3.z - p1.z;
	glm::vec4 result;
	result.x = b1 * c2 - b2 * c1;
	result.y = a2 * c1 - a1 * c2;
	result.z = a1 * b2 - b1 * a2;
	result.w = -result.x * p1.x - result.y * p1.y - result.z * p1.z;
	return result;
}

float Global::distanceBetweenParallelPlanes(glm::vec4 plane1, glm::vec4 plane2) {
	return fabs((plane2.z*(-plane1.w / plane1.z) + plane2.w)) / (sqrt(plane2.x * plane2.x + plane2.y * plane2.y + plane2.z*plane2.z));
}

glm::vec3* Global::createHorizontalTransformArray(const int width, const int length, const glm::vec2 min, const glm::vec2 max,
												  const float yPosition) {
	glm::vec3* result = new glm::vec3[width * length];
	const float firstX = width == 1 ? (max.x + min.x) / 2.0f : min.x;
	const float xStep = width == 1 ? 0.0f : (max.x - min.x) / static_cast<float>(width - 1);
	const float firstZ = length == 1 ? (max.y - min.y) / 2.0f : min.y;
	const float zStep = length == 1 ? 0.0f : (max.y - min.y) / static_cast<float>(length - 1);
	int counter = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < length; j++) {
			result[counter++] = glm::vec3(i * xStep + firstX, yPosition, j * zStep + firstZ);
		}
	}
	return result;
}

void Global::drawToCubemap(GLuint cubemap, glm::vec3 position, GLuint fbo,
						   const std::function<void(glm::mat4, glm::mat4)> renderCallback) {
	CHECK_GL_ERROR();
	int oldFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	CHECK_GL_ERROR();

	static const glm::mat4 p = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f);

	static const glm::vec3 targets[6] = {
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};
	static const glm::vec3 ups[6] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	};

	glViewport(0, 0, ENVMAP_SIZE.x, ENVMAP_SIZE.y);
	for (int i = 0; i < 6; i++) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		CHECK_GL_ERROR();

		glm::mat4 v = lookAt(position, position + targets[i], ups[i]);
		//v[0][2] *= -1.0f;
		//v[1][2] *= -1.0f;
		//v[2][2] *= -1.0f;
		//v[3][2] *= -1.0f;
		renderCallback(v, p);
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		CHECK_GL_ERROR();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
		CHECK_GL_ERROR();
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
	glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
	CHECK_GL_ERROR();
}

double Global::remap(const double value, const double sourceMin, const double sourceMax, double targetMin, double targetMax,
					 const bool revertTarget, const bool snapIfInvalid) {
	if (value < sourceMin || value > sourceMax) {
		if (snapIfInvalid) {
			return value < sourceMin ? targetMin : targetMax;
		}
	}
	double result = (value - sourceMin) / (sourceMax - sourceMin) * (targetMax - targetMin) + targetMin;
	if (revertTarget) {
		result = targetMax - result + targetMin;
	}

	return result;
}

float Global::remap(const float value, const float sourceMin, const float sourceMax, float targetMin, float targetMax,
					const bool revertTarget, const bool snapIfInvalid) {
	if (value < sourceMin || value > sourceMax) {
		if (snapIfInvalid) {
			return value < sourceMin ? targetMin : targetMax;
		}
	}
	float result = (value - sourceMin) / (sourceMax - sourceMin) * (targetMax - targetMin) + targetMin;
	if (revertTarget) {
		result = targetMax - result + targetMin;
	}

	return result;
}

int Global::remap(const int value, const int sourceMin, const int sourceMax, const int targetMin, const int targetMax, const bool revertTarget,
				  const bool snapIfInvalid) {
	return static_cast<int>(remap(static_cast<double>(value), static_cast<double>(sourceMin), static_cast<double>(sourceMax), static_cast<double>(targetMin),
								  static_cast<double>(targetMax),
								  revertTarget, snapIfInvalid));
}
