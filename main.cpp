#include <iostream>  
#include <opencv2\opencv.hpp>  
#include "align.h"
#include "main.h"
#define SIZE_W 4288
#define SIZE_H 2848

using namespace std;

int main()

{
	auto offsets = calcAlign("E:/STACKING/M42_11_16_2012/Lights/DSC_", 4267,44);

	cv::Mat AverageLight = ImageAverage("E:/STACKING/M42_11_16_2012/Lights/DSC_", 4267, 44,1,offsets);
	cv::Mat Dark = ImageAverage("E:/STACKING/M42_11_16_2012/Darks/DSC_", 4312, 15, 0, offsets);
	
	for(int i = 0; i < AverageLight.rows;i++)
		for (int j = 0; j < AverageLight.cols; j++)
		{
			AverageLight.at<Vec3f>(i, j)[0] = AverageLight.at<Vec3f>(i, j)[0] * AverageLight.at<Vec3f>(i, j)[0];
			AverageLight.at<Vec3f>(i, j)[1] = AverageLight.at<Vec3f>(i, j)[1] * AverageLight.at<Vec3f>(i, j)[1];
			AverageLight.at<Vec3f>(i, j)[2] = AverageLight.at<Vec3f>(i, j)[2] * AverageLight.at<Vec3f>(i, j)[2];
		}
		
	cv::Mat Average_show;
	cv::normalize(AverageLight, Average_show, 1, 0, cv::NORM_MINMAX);
	cv::imwrite("e:/rawlight.tif", Average_show);
	cout << "-----------------------------------------" << endl;
	getchar();

}

cv::Mat ImageAverage(std::string path, int start, int ImageNum, int mode, vector<Point2f> offsets)
{
	cv::Mat AverageImage = cv::Mat::zeros(cv::Size(SIZE_W, SIZE_H), CV_32FC3);

	cv::Mat temp;
	for (int i = 0; i < ImageNum; i++)
	{
		temp = cv::imread(path + to_string(start + i) + ".tif", 1);
		temp.convertTo(temp, CV_32FC3);
		if (temp.data == NULL) {
			cout << "read error" << endl;
		}
		cout << "accumulate " << i << endl;
		if (mode == 1) {
			cv::Mat t_mat = cv::Mat::zeros(2, 3, CV_32FC1);

			t_mat.at<float>(0, 0) = 1;
			t_mat.at<float>(0, 2) = -offsets[i].x; //水平平移量
			t_mat.at<float>(1, 1) = 1;
			t_mat.at<float>(1, 2) = -offsets[i].y; //竖直平移量
			cv::warpAffine(temp, temp, t_mat,temp.size());
		}
		cv::accumulate(temp, AverageImage);
	}

	AverageImage /= ImageNum;
	return AverageImage;
}
