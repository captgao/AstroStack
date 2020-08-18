#include <iostream>  
#include <opencv2\opencv.hpp>  
#include "align.h"

using namespace cv;

Mat DOG(Mat &A, double sig1, double sig2, Size Ksize)
{
	Mat AF, out, out1, out2;
	A.convertTo(AF, CV_32FC1);
	GaussianBlur(AF, out1, Ksize, sig1, 0);
	GaussianBlur(AF, out2, Ksize, sig2, 0);
	subtract(out1, out2, out);
	return out;
}


std::vector<Point2f> calcAlign(std::string path, int start, int ImageNum) {
	size_t minN;
	int min_i = 0;
	std::vector<std::vector<KeyPoint>> KeyStack;
	for (int i = 0; i < ImageNum; i++){
		Mat img = imread(path + std::to_string(start + i) + ".tif", IMREAD_GRAYSCALE);
		img = 255 - img;
		SimpleBlobDetector::Params params;
		params.minArea = 50;
		params.maxArea = 500;
		params.filterByCircularity = true;
		Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
		std::vector<KeyPoint> keypoints;
		detector->detect(img, keypoints);
		std::cout << keypoints.size() << std::endl;
		if (i == 0) {
			minN = keypoints.size();
		}
		else {
			if (keypoints.size() < minN) {
				minN = keypoints.size();
				min_i = i;
			}
		}
		KeyStack.push_back(keypoints);
	}
	int nConfidence = ImageNum / 10;
	nConfidence = nConfidence >= 3 ? nConfidence : 3;

	for (int k = 0; k < KeyStack.size(); k++) {
		for (int i = 0; i < KeyStack[k].size(); i++) {
			for (int j = i + 1; j < KeyStack[k].size(); j++) {
				if (near(KeyStack[k][i], KeyStack[k][j])) {
					goto del_i;
				}
			}
			continue;
		del_i:
			KeyStack[k].erase(KeyStack[k].begin() + i);
			i--;
		}
	}
	std::vector<KeyPoint> finalSet = KeyStack[min_i];

	std::cout << "final: " << finalSet.size() << std::endl;
	std::vector<Point2f> offset;
	float avr_x=0, avr_y=0;
	for (int i = 0; i < KeyStack.size(); i++) {
		int count=0;
		float x=0, y=0,def_x=0,def_y=0;
		for (int j = 0; j < KeyStack[i].size(); j++) {
			for (int k = 0; k < finalSet.size(); k++) {
				if (near(finalSet[k], KeyStack[i][j])) {
					x += KeyStack[i][j].pt.x;
					y += KeyStack[i][j].pt.y;
					def_x += finalSet[k].pt.x;
					def_y += finalSet[k].pt.y;
					count++;
					break;
				}
			}
		}
		std::cout << count << std::endl;
		x /= count;
		y /= count;
		def_x /= count;
		def_y /= count;
		Point2f p2f;
		p2f.x = x-def_x;
		p2f.y = y-def_y;
		avr_x += p2f.x;
		avr_y += p2f.y;
		//std::cout << p2f.x << " " << p2f.y << std::endl;
		offset.push_back(p2f);
	}
	std::cout << avr_x << " " << avr_y << std::endl;
	return offset;
}
bool near(KeyPoint p1, KeyPoint p2) {
	if (p1.pt.x - p2.pt.x <= 20 && p1.pt.x - p2.pt.x >= -20 && p1.pt.y - p2.pt.y <= 20 && p1.pt.y - p2.pt.y >= -20) {
		return true;
	}
	else {
		return false;
	}
}