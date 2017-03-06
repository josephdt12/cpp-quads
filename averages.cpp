#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace cv;

class Quadrant {
public:
	int top_x_, top_y_;
	cv::Mat data_;
	std::vector<double> means_;

	// Copy rectangle from region
	Quadrant(cv::Mat& region, int top_x, int top_y, int width, int height,
			int orig_x, int orig_y) : means_(3,0) {

		data_ = region(Rect(top_x, top_y, width, height));
		top_x_ = orig_x + top_x;
		top_y_ = orig_y + top_y;
	}

	// Divide Quadrant into 4 Quadrants and return
	std::vector<Quadrant> DivideRegion() {
		std::vector<Quadrant> quadrants;
		int width = data_.cols, height = data_.rows;

		quadrants.emplace_back(Quadrant(data_, 0, 0, width/2, height/2,
			top_x_, top_y_));
		quadrants.emplace_back(Quadrant(data_, width/2, 0 , width/2, height/2,
			top_x_, top_y_));
		quadrants.emplace_back(Quadrant(data_, 0, height/2, width/2, height/2,
			top_x_, top_y_));
		quadrants.emplace_back(Quadrant(data_, width/2, height/2, width/2, height/2,
			top_x_, top_y_));

		return quadrants;
	}

	// Gets the mean RGB values for this Quadrant in the original image
	std::vector<double> RegionMean(cv::Mat& original) {
		//std::vector<double> means(3);
		for (int row = 0; row < data_.rows; ++row) {
			for (int col = 0; col < data_.cols; ++col) {
				Vec3b point = original.at<Vec3b>(top_y_ + row, top_x_ + col);
				means_[0] += point[0];
				means_[1] += point[1];
				means_[2] += point[2];
			}
		}
		for (int i = 0; i < 3; ++i)
			means_[i] /= (data_.rows * data_.cols);

		return means_;
	}

	// Fill this Quadrant with the mean RGB value from original image
	void FillRegion() {
		data_ = Scalar(means_[0], means_[1], means_[2]);
	}
};

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

void FillRegion(cv::Mat& region, std::vector<double>& RGB) {
	region = Scalar(RGB[0], RGB[1], RGB[2]);
}

int main(int argc, char** argv) {
	cv::Mat image = cv::imread(argv[1]);
	cv::Mat new_image = image.clone();

	Quadrant original(new_image, 0, 0, new_image.cols, new_image.rows, 0, 0);
	std::vector<Quadrant> quadrants = original.DivideRegion();

	for (int i = 0; i < 4; ++i) {
		std::vector<double> values = quadrants[i].RegionMean(image);
		quadrants[i].FillRegion();
	}


	std::vector<Quadrant> quads2 = quadrants[3].DivideRegion();
	std::vector<double> values = quads2[1].RegionMean(image);
	quads2[1].FillRegion();
	std::vector<double> values2 = quads2[3].RegionMean(image);
	quads2[3].FillRegion();

	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Quadrants", CV_WINDOW_AUTOSIZE);
	imshow("Original", image);
	imshow("Quadrants", new_image);

	waitKey();

	return 0;
}
