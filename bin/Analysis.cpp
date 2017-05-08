#include "Analysis.h"

double correlation(cv::Mat & first_image,cv::Mat & second_image)
{
	{

		cv::Scalar first_mean, first_std, second_mean, second_std;
		cv::meanStdDev(first_image, first_mean, first_std);
		cv::meanStdDev(second_image, second_mean, second_std);

		int pixel_num = first_image.rows * first_image.cols;


		double covariance = (first_image - first_mean).dot(second_image - second_mean) / pixel_num;
		double correlation = covariance / (first_std[0] * second_std[0]);

		return correlation;
	}
}

std::vector<double> registration_correlation(std::vector<MRISequence>& sequences, std::ostream& output)
{
	int sequence_num = sequences.size();
	int image_num = 0;

	if (sequence_num > 0)
	{
		image_num = sequences[0].image_count();
	}
	else
	{
		return std::vector<double>();
	}
	if (image_num <= 0) return std::vector<double>();

	std::vector<double> average(sequence_num,0.0);

	//print id of image and correlation between that slice and first slice for all sequences
	
	for (auto i = 1; i < image_num; i++)
	{
		output << i << " ";
		for (auto j = 0; j < sequence_num; j++)
		{
			auto correl = correlation(sequences[j][0], sequences[j][i]);
			output << correl << " ";
			average[j] += correl;
		}
		output << "\n";
	}
	output << "\n";
	for (auto& avg : average)
	{
		avg /= image_num;
	}

	return average;
}
