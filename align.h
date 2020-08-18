#pragma once
using namespace cv;
Mat DOG(Mat & A, double sig1, double sig2, Size Ksize);

std::vector<Point2f> calcAlign(std::string path, int start, int ImageNum);
bool near(KeyPoint p1, KeyPoint p2);
