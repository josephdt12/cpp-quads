#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
using namespace cv;

class Quadrant {
public:
	int top_x_, top_y_;
	cv::Mat data_;
	std::vector<double> means_;
	double error_;

	// Construct a Quadrant from passed in region, and relate it to orig position
	Quadrant(cv::Mat& region, int top_x, int top_y, int width, int height,
			int orig_x, int orig_y) : means_(3,0) {

		data_ = region(Rect(top_x, top_y, width, height));
		top_x_ = orig_x + top_x;
		top_y_ = orig_y + top_y;
		error_ = 0;
	}

	// Basic copy constructor
	Quadrant(const Quadrant& quad) {
		top_x_ = quad.top_x_;
		top_y_ = quad.top_y_;
		data_ = quad.data_;
		means_ = quad.means_;
		error_ = quad.error_;
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

        /* FIXME: Adds black border around each quadrant - may be utilized
         * in future additions
        // Top, bottom row
        for (int col = 0; col < data_.cols; ++col) {
            // Top row
            data_.at<Vec3b>(0, col)[0] = 0;
            data_.at<Vec3b>(0, col)[1] = 0;
            data_.at<Vec3b>(0, col)[2] = 0;
            // Bot row
            data_.at<Vec3b>(data_.rows - 1, col)[0] = 0;
            data_.at<Vec3b>(data_.rows - 1, col)[1] = 0;
            data_.at<Vec3b>(data_.rows - 1, col)[2] = 0;
        }

        // Left, right side
        for (int row = 0; row < data_.rows; ++row) {
            // Left side
            data_.at<Vec3b>(row,0)[0] = 0;
            data_.at<Vec3b>(row,0)[1] = 0;
            data_.at<Vec3b>(row,0)[2] = 0;
            // Right side
            data_.at<Vec3b>(row,data_.cols - 1)[0] = 0;
            data_.at<Vec3b>(row,data_.cols - 1)[1] = 0;
            data_.at<Vec3b>(row,data_.cols - 1)[2] = 0;
        }
        */
	}

	// Calculates the mean squared error compared to passed in values
	double MeanSquareError(std::vector<double> vals) {
		double errors[3];
		for (int row = 0; row < data_.rows; ++row) {
			for (int col = 0; col < data_.cols; ++col) {
				cv::Vec3b point = data_.at<Vec3b>(row,col);
				errors[0] += std::pow((point[0] - vals[0]),2);
				errors[1] += std::pow((point[1] - vals[1]),2);
				errors[2] += std::pow((point[2] - vals[0]),2);
			}
		}
        
        // Add an additional weight based on area of the quadrant
		error_ = std::sqrt((errors[0] + errors[1] + errors[2]) / 3) * (data_.rows * data_.cols);
		return error_;
	}
};

bool operator<(const Quadrant& lhs, const Quadrant& rhs) {
	return lhs.error_ < rhs.error_;
}

// COMMAND LINE ARGUMENTS:
// 0: PROGRAM NAME
// 1: INPUT IMAGE
// 2: ITERATION COUNT (DEFAULT IS 2048)
// 3: OUTPUT FILE NAME (DEFAULT IS quadrants.jpg)
int main(int argc, char** argv) {
	// Read in the image
	cv::Mat image = cv::imread(argv[1]);
	cv::Mat new_image = image.clone();

	// Grab number of iterations from user, else default to 2048
	int MAX_ITERATIONS;
	if (argc < 3) {
		MAX_ITERATIONS = 2048;
	} else {
		std::istringstream ss(argv[2]);
		if (!(ss >> MAX_ITERATIONS)) {
			std::cerr << "Invalid number " << argv[2] << std::endl;
			return 1;
		}
	}

	// Priority queue for tracking the largest error quadrant
	std::priority_queue<Quadrant> min_heap;
	Quadrant original(new_image, 0, 0, new_image.cols, new_image.rows, 0, 0);
	Quadrant curr = original;

	for (int itr = 0; itr < MAX_ITERATIONS; ++itr) {
		// Get current quadrant's mean values for sub-quadrants error calculation
		std::vector<double> values = curr.RegionMean(image);

		// Divide current quadrant into 4
		std::vector<Quadrant> quadrants = curr.DivideRegion();

		// Obtain the mean of each quadrant's position in the orig. image and fill
		// Also place each quadrant into the priority_queue
		for (int i = 0; i < 4; ++i) {
			quadrants[i].MeanSquareError(values);
			min_heap.push(quadrants[i]);
			quadrants[i].RegionMean(image);
			quadrants[i].FillRegion();
		}

		curr = min_heap.top();
		min_heap.pop();
	}

	namedWindow("Original", CV_WINDOW_AUTOSIZE);
	namedWindow("Quadrants", CV_WINDOW_AUTOSIZE);
	imshow("Original", image);
	imshow("Quadrants", new_image);

	// Save new quadrants image out
	if (argc < 4)
		imwrite("quadrants.jpg", new_image);
	else
		imwrite(argv[3], new_image);

	waitKey();

	return 0;
}
