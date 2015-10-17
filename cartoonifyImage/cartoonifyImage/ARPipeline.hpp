#ifndef ARPIPELINE_HPP
#define ARPIPELINE_HPP

#include "PatternDetector.hpp"
#include "CameraCalibration.hpp"
#include "GeometryTypes.hpp"


class ARPipeline
{
public:
	ARPipeline(const cv::Mat& patternImage, const CameraCalibration& calibration);
	bool processFrame(const cv::Mat& inputFrame);
	bool Transformation& getPatternLocation() const ;
	PatternDetector m_patternDetector ;

private:
	CameraCalibration	m_calibration ;
	Pattern				m_pattern ;
	PatternTrackingInfo m_patternInfo ;
};


#endif