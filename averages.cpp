#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace cv;

class Quadrant {
public:
	int top_x_, top_y_, width_, height_;
	cv::Mat data_;

	// Copy rectangle from region
	Quadrant(cv::Mat& region, int top_x, int top_y, int height, int width) {
		data_ = region(Rect(top_x, top_y, height, width));
		top_x_ = top_x;
		top_y_ = top_y;
		width_ = width;
		height = height_;
	}

	// Gets the mean RGB values for this Quadrant in the original image
	std::vector<double> RegionMean(cv::Mat& original) {
		std::cout << top_x_ << " and " << top_y_ << std::endl;
		std::vector<double> means(3);
		for (int row = 0; row < data_.rows; ++row) {
			for (int col = 0; col < data_.cols; ++col) {
				Vec3b point = original.at<Vec3b>(top_x_ + col, top_y_ + row);
				means[0] += point[0];
				means[1] += point[1];
				means[2] += point[2];
			}
		}
		for (int i = 0; i < 3; ++i)
			means[i] /= (data_.rows * data_.cols);

		return means;
	}
};

// Determine the mean RGB values for a region, based on the original pixels
std::vector<double> RegionMean(cv::Mat& region) {
	std::vector<double> means(3);
	for (int row = 0; row < region.rows; ++row) {
		for (int col = 0; col < region.cols; ++col) {
			Vec3b point = region.at<Vec3b>(row,col);
			means[0] += point[0];
			means[1] += point[1];
			means[2] += point[2];
		}
	}
	for (int i = 0; i < 3; ++i)
		means[i] /= (region.rows * region.cols);

	return means;
}

// Determine the mean square error for a region (R,G, and B combined)
double MeanSquareError(cv::Mat& region, std::vector<double>& vals) {
	double errors[3];
	for (int row = 0; row < region.rows; ++row) {
		for (int col = 0; col < region.cols; ++col) {
			cv::Vec3b point = region.at<Vec3b>(row,col);
			errors[0] += std::pow((vals[0] - point[0]),2);
			errors[1] += std::pow((vals[1] - point[1]),2);
			errors[2] += std::pow((vals[2] - point[0]),2);
		}
	}

	return (errors[0] + errors[1] + errors[2]) / 3;
}

// Divide region into 4 quadrants
std::vector<Quadrant> DivideRegion(cv::Mat& region) {
	std::vector<Quadrant> quadrants;
	int width = region.cols, height = region.rows;

	quadrants.emplace_back(Quadrant(region,0,0,width/2,height/2));
	quadrants.emplace_back(Quadrant(region,width/2,0,width/2,height/2));
	quadrants.emplace_back(Quadrant(region,0,height/2,width/2,height/2));
	quadrants.emplace_back(Quadrant(region,width/2,height/2,width/2,height/2));

	return quadrants;
	/*
	std::vector<cv::Mat> quadrants;
	int width = region.cols, height = region.rows;
	std::cout << width << " and " << height << std::endl;

	quadrants.emplace_back(region(Rect(0,0,width/2,height/2)));
	quadrants.emplace_back(region(Rect(width/2,0,width/2,height/2)));
	quadrants.emplace_back(region(Rect(0,height/2,width/2,height/2)));
	quadrants.emplace_back(region(Rect(width/2,height/2,width/2,height/2)));

	return quadrants;
	*/
}

void FillRegion(cv::Mat& region, std::vector<double>& RGB) {
	region = Scalar(RGB[0], RGB[1], RGB[2]);
}

int main(int argc, char** argv) {
	cv::Mat image = cv::imread(argv[1]);
	cv::Mat new_image = image.clone();

	std::vector<double> means = RegionMean(image);

	std::vector<Quadrant> quadrants = DivideRegion(new_image);

	for (int i = 0; i < 4; ++i) {
		std::cout << i << std::endl;
		std::vector<double> values = quadrants[i].RegionMean(image);
		//std::vector<double> values = RegionMean(quadrants[i].data_);
		FillRegion(quadrants[i].data_, values);
	}

	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Quadrants", CV_WINDOW_AUTOSIZE);
	imshow("Original", image);
	imshow("Quadrants", new_image);

	waitKey();

	return 0;
}
