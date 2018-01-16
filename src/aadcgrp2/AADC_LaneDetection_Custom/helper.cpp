#include "helper.h"

float help::xValueOfLineAt(float distance, float angle, float yValue) {
	return distance / cos(angle) + tan(angle) * yValue;
}

float help::xValueOfLineAt(cv::Vec2f& line, float yValue) {
	float distance = line[0];
	float angle = line[1];

	return xValueOfLineAt(distance, angle, yValue);
}

float help::xValueOfLineAt(cv::Vec3f& line, float yValue) {
	float distance = line[0];
	float angle = line[1];

	return xValueOfLineAt(distance, angle, yValue);
}


float help::yValueOfLineAt(float distance, float angle, float xValue) {
	return (distance / sin(angle)) - xValue / tan(angle);
}

float help::yValueOfLineAt(cv::Vec2f& line, float xValue) {
	float distance = line[0];
	float angle = line[1];

	return yValueOfLineAt(distance, angle, xValue);
}

float help::yValueOfLineAt(cv::Vec3f& line, float xValue) {
	float distance = line[0];
	float angle = line[1];

	return yValueOfLineAt(distance, angle, xValue);
}


float help::centerOfLinesAt(cv::Vec3f& first, cv::Vec3f& second, float yValue) {
	return (xValueOfLineAt(first, yValue) + xValueOfLineAt(second, yValue)) / 2;
}

float help::centerOfLinesAtBottom(cv::Vec3f& first, cv::Vec3f& second) {
	return centerOfLinesAt(first, second, screenSize.height);
}

/* ----- LINE CLASSIFICATION -----  */
bool help::lineIsHorizontal(cv::Vec3f& line) {
		float angle = rad2deg(line[1]);

		//NOTE: We're dealing with the normal vector.

		return (fabs(angle) > 75.0f);
}

bool help::lineIsVertical(cv::Vec3f& line) {
		float angle = rad2deg(line[1]);

		//NOTE: We're dealing with the normal vector.

		return fabs(angle) < 20.0f;
}

// returns if a line is a stop line
bool help::lineIsStopLine(cv::Vec3f& line) {
	return lineIsHorizontal(line);
}

// returns if a line is a left line
bool help::lineIsLeftLine(cv::Vec3f& line) {
	float dist = line[0];

	//NOTE: We're dealing with the normal vector.

	return (lineIsVertical(line) && dist < screenSize.width / 2.0f);
}

// returns if a line is a right line
bool help::lineIsRightLine(cv::Vec3f& line) {
	float dist = line[0];

	//NOTE: We're dealing with the normal vector.

	return (lineIsVertical(line) && dist > screenSize.width / 2.0f);
}
