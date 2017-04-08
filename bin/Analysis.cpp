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

double registration_correlation(std::vector<MRISequence> sequences)
{
	int sequence_num = sequences.size();
	int image_num = 0;

	if (sequence_num > 0)
	{
		image_num = sequences[0].image_count();
	}
	else
	{
		return -1;
	}

	for (auto i = 1; i < image_num; i++)
	{
		std::cout << i << ": ";
		for (auto j = 0; j < sequence_num; j++)
		{
			auto M1 = sequences[j][0];
			auto M2 = sequences[j][i];
			std::cout << correlation(sequences[j][0], sequences[j][i]) << " ";
		}
		std::cout << "\n";
	}


	return 0;
}
