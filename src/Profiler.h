#ifndef PROFILER_H
#define PROFILER_H
#include <map>

class Profiler
{
private:
	Profiler() = default;
	~Profiler() = default;

	double startTime = 0;
	float timeDelta = 1.0f;
	float elapsed = 0.0f;
	bool enabled = false;
	//refreshing all the time
	double overallUpdateTime = 0.0f;
	std::map<std::string, double> updateTimes;
	std::map<std::string, double> measuresMap;

	//buffers
	double overallUpdateTimeBuffer = 0.0f;
	std::map<std::string, double> measuresMapBuffer;
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
	void startCountingTime();
	void addMeasure(std::string&& measureName);
};

#endif
