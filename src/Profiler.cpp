#include "Profiler.h"
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"         // Visual Studio says it's redundant, but IT IS NOT.
#include "imgui_impl_opengl3.h"
#include <string>
#include "GLFW/glfw3.h"
#include <ctime>

void Profiler::clearFrameDataBuffer()
{
	overallUpdateTimeBuffer = 0.0;
	measuresMapBuffer.clear();
	updateTimesBuffer.clear();
}

void Profiler::addFrameDataToBuffer()
{
	overallUpdateTimeBuffer = overallUpdateTime;
	measuresMapBuffer = measuresMap;
	updateTimesBuffer = updateTimes;
}

Profiler* Profiler::getInstance()
{
	static Profiler* profiler = nullptr;
	if(profiler == nullptr)
	{
		profiler = new Profiler();
	}
	return profiler;
}

void Profiler::update(float msec)
{
	if(elapsed > timeDelta)
	{
		elapsed = 0.0f;
		clearFrameDataBuffer();
		addFrameDataToBuffer();
	} else
	{
		elapsed += msec;
	}
}

void Profiler::clearFrameData()
{
	updateTimes.clear();
	overallUpdateTime = 0.0f;
	measuresMap.clear();
}

void Profiler::addUpdateTime(std::string&& componentId, double time)
{
	if (enabled)
	{
		std::string name = std::move(componentId);
		updateTimes[name] = time;
		overallUpdateTime += time;
	}
}

void Profiler::startCountingTime()
{
	startTime = glfwGetTime();
}

void Profiler::addMeasure(std::string&& measureName)
{
	if (enabled)
	{
		measuresMap[std::move(measureName)] = (glfwGetTime() - startTime) * 1000;
		startTime = 0;
	}
}

void Profiler::setEnable(bool enabled)
{
	this->enabled = enabled;
}

bool Profiler::getEnabled()
{
	return enabled;
}

void Profiler::renderProfilerWindow()
{
	//overallUpdateTime *= 1000.0f; //in miliseconds
	if(ImGui::Begin("Profiler", &enabled, 64))
	{
		for(auto it : measuresMapBuffer)
		{
			ImGui::Text(it.first.c_str());
			ImGui::SameLine();
			ImGui::Text("%.4f ms", it.second);
		}

		ImGui::Text("Overall Update Time");
		ImGui::SameLine();
		ImGui::Text("%.4f ms", overallUpdateTimeBuffer);
		if(ImGui::TreeNode("Update Times"))
		{
			for (auto it : updateTimesBuffer)
			{
				ImGui::Text(it.first.c_str());
				ImGui::SameLine();
				ImGui::Text("%.4f ms", it.second);
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}


