#include "Profiler.h"
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"         // Visual Studio says it's redundant, but IT IS NOT.
#include "imgui_impl_opengl3.h"
#include <string>
#include "GLFW/glfw3.h"

void Profiler::clearFrameDataBuffer()
{
	overallUpdateTimeBuffer = 0.0;
	rebuildTreeTimeBuffer = 0.0;
	calculateTreeTimeBuffer = 0.0;
	collisionTestsTimeBuffer = 0.0;
	frustumCullingTimeBuffer = 0.0;
	renderTimeBuffer = 0.0;
	updateTimesBuffer.clear();
}

void Profiler::addFrameDataToBuffer()
{
	renderTimeBuffer = renderTime;
	overallUpdateTimeBuffer = overallUpdateTime;
	rebuildTreeTimeBuffer = rebuildTreeTime;
	calculateTreeTimeBuffer = calculateTreeTime;
	collisionTestsTimeBuffer = collisionTestsTime;
	frustumCullingTimeBuffer = frustumCullingTime;
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

void Profiler::setRebuildTreeTime(double time)
{
	rebuildTreeTime = time;
}

void Profiler::setCalculateTreeTime(double time)
{
	calculateTreeTime = time;
}

void Profiler::setCollisionTestsTime(double time)
{
	collisionTestsTime = time;
}

void Profiler::setFrustumCullingTime(double time)
{
	frustumCullingTime = time;
}

void Profiler::setRenderTime(double time)
{
	renderTime = time;
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
	if(ImGui::Begin("Profiler", &enabled, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Render Time:");
		ImGui::SameLine();
		ImGui::Text("%.8f ms", renderTimeBuffer);
		ImGui::Text("________________");
		ImGui::Text("Octree rebuild");
		ImGui::SameLine();
		ImGui::Text("%.8f ms", rebuildTreeTimeBuffer);

		ImGui::Text("Octree calculation");
		ImGui::SameLine();
		ImGui::Text("%.8f ms", calculateTreeTimeBuffer);

		ImGui::Text("Octree collisions");
		ImGui::SameLine();
		ImGui::Text("%.8f ms", collisionTestsTimeBuffer);

		ImGui::Text("Frustum Culling");
		ImGui::SameLine();
		ImGui::Text("%.8f ms", frustumCullingTimeBuffer);

		ImGui::Text("Overall Update Time");
		ImGui::SameLine();
		ImGui::Text("%.8f ms", overallUpdateTimeBuffer);
		if(ImGui::TreeNode("Update Times"))
		{
			for (auto it : updateTimesBuffer)
			{
				ImGui::Text(it.first.c_str());
				ImGui::SameLine();
				ImGui::Text("%.8f ms", it.second);
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}


