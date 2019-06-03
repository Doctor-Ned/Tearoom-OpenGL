#ifndef PROFILER_H
#define PROFILER_H
#include <map>

class Profiler
{
private:
	Profiler() = default;
	~Profiler() = default;

	float timeDelta = 1.0f;
	float elapsed = 0.0f;
	bool enabled = false;
	//refreshing all the time
	double overallUpdateTime = 0.0f;
	std::map<std::string, double> updateTimes;
	double rebuildTreeTime = 0.0f;
	double calculateTreeTime = 0.0f;
	double collisionTestsTime = 0.0f;
	double frustumCullingTime = 0.0f;
	double renderTime = 0.0f;
	//buffers
	double renderTimeBuffer = 0.0f;
	double rebuildTreeTimeBuffer = 0.0f;
	double calculateTreeTimeBuffer = 0.0f;
	double collisionTestsTimeBuffer = 0.0f;
	double frustumCullingTimeBuffer = 0.0f;
	double overallUpdateTimeBuffer = 0.0f;
	std::map<std::string, double> updateTimesBuffer;
	void clearFrameDataBuffer();
	void addFrameDataToBuffer();
public:
	Profiler & operator=(const Profiler&) = delete;
	static Profiler* getInstance();
	void update(float msec);
	void setEnable(bool enabled);
	bool getEnabled();
	void renderProfilerWindow();
	void clearFrameData();
	void addUpdateTime(std::string&& componentId, double time);
	void setRebuildTreeTime(double time);
	void setCalculateTreeTime(double time);
	void setCollisionTestsTime(double time);
	void setFrustumCullingTime(double time);
	void setRenderTime(double time);
};

#endif
