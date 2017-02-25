#include "Registration.h"
#include "MRISequence.h"
#include "MRISessionHorizontal.h"
#include "MRISequenceHorizontal.h"

#include "opencv2/features2d.hpp"
#include <fstream>
#include <cmath>

features detect_features(const MRISequence& sequence)
{
	auto image_count = sequence.image_count();
	if (image_count < 2) throw std::invalid_argument("number of images (" + std::to_string(image_count) + ") is too small\n");
	
	double contrast = 255.0*sequence.get_contrast();

	features features;
	
	std::vector<cv::Point2f> prev_features;

	cv::Mat prev_img;
	sequence[0].convertTo(prev_img, CV_8U, contrast);
	
	int maxCorners = 150;
	double qualityLevel = 0.001;
	int minDistance = 3;

	cv::goodFeaturesToTrack(prev_img, prev_features, maxCorners, qualityLevel, minDistance, cv::noArray());

	features.push_back(prev_features);

	for (std::vector<cv::Mat>::const_iterator it = sequence.data().begin() + 1; it != sequence.data().end(); ++it)
	{
		cv::Mat next_img;
		it->convertTo(next_img, CV_8U, contrast);

		std::vector< cv::Point2f > next_features;
		std::vector<unsigned char> status;
		std::vector<float> error;
		
		cv::calcOpticalFlowPyrLK(prev_img, next_img, prev_features, next_features, status, error);

		features.push_back(next_features);

		prev_features = next_features;
		prev_img = next_img;

	}
		
	return features;
}

void show_features(MRISequence& sequence, const features& features)
{
	if (sequence.image_count() != features.size() || sequence.image_count() < 1) throw std::out_of_range("Sequence and features must both have equal, non-zero size");

	MRISequence feature_sequence;
	feature_sequence.set_contrast(sequence.get_contrast());

	int max_diff = 2;
	for (auto i = 1; i < sequence.image_count(); i++)
	{
		cv::Mat prev_img, next_img;

		sequence[i - 1].copyTo(prev_img);
		sequence[i].copyTo(next_img);		

		for (auto j = 0; j < features[i].size(); j++)
		{			
			double dx = std::abs(features[i - 1][j].x - features[i][j].x);
			double dy = std::abs(features[i - 1][j].y - features[i][j].y);

			if (dx < max_diff && dy < max_diff)
			{
				circle(prev_img, features[i-1][j], 4, cv::Scalar(255, 255, 255), -1, 8, 0);
				circle(next_img, features[i][j], 4, cv::Scalar(255, 255, 255), -1, 8, 0);
			}			
		}

		feature_sequence.add_image(prev_img);
		feature_sequence.add_image(next_img);
		
	}

	sequence = std::move(feature_sequence);
	sequence.show("Featured Window");
}

MRISequence registrate(const MRISequence& sequence, const features& features)
{
	auto image_count = sequence.image_count();
	if (image_count < 2) throw std::invalid_argument("number of images (" + std::to_string(image_count) + ") is too small\n");

	MRISequence transformed_sequence;
	transformed_sequence.add_image(sequence[0]);

	int max_diff = 2;

	for (auto i = 1; i < image_count; i++)
	{
		std::vector< cv::Point2f > prev_t_corners;
		std::vector<cv::Point2f> next_t_corners;

		//max_diff = std::log2(i) + 1;

		for (int j = 0; j < features[0].size(); j++)
		{
			double dx = std::abs(features[i-1][j].x - features[i][j].x);
			double dy = std::abs(features[i-1][j].y - features[i][j].y);

			if (dx < max_diff && dy < max_diff)
			{
				prev_t_corners.push_back(features[0][j]);
				next_t_corners.push_back(features[i][j]);
			}
		}

		cv::Mat h = cv::findHomography(next_t_corners, prev_t_corners);
		cv::Mat dst;
		cv::warpPerspective(sequence[i], dst, h, sequence[i].size());

		transformed_sequence.add_image(dst);
		
	}
	transformed_sequence.set_contrast();
	
	return transformed_sequence;
}

void registration()
{
	std::string folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC14/1.3.6.1.4.1.14519.5.2.1.2103.7010.297016782834828170309889288895";
	//std::string folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC06/1.3.6.1.4.1.14519.5.2.1.2103.7010.269874611034344926547684818265";
	//std::string folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC10/1.3.6.1.4.1.14519.5.2.1.2103.7010.634114621738943599785009586807";
	

	std::shared_ptr<MRISessionHorizontal> session(new MRISessionHorizontal(folder));
	session->read();
	
	
	auto sequence = (*session)[54];		
	std::cout << sequence.use_count();

	try {
		sequence->read(*session);
	}
	catch (std::invalid_argument& e)
	{
		std::cout << e.what();
		return;
	}	
	sequence->show("W1");
	

	features features = detect_features(*sequence);
	MRISequence feature_sequence(*sequence);
	show_features(feature_sequence, features);


	MRISequence transformed_sequence = registrate(*sequence, features);
	transformed_sequence.show("W2");
	
}

/*
void motion_test()
{
	std::string folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC14/1.3.6.1.4.1.14519.5.2.1.2103.7010.297016782834828170309889288895";

	MRISession session(folder);

	session.read();


	MRISequence sequence;
	sequence.read_horizontal(session, "000055.dcm");
	

	sequence.show();


	MRISequence feature_sequence;	
	
		
	std::ofstream output_stream("pairs.txt");
	
	
	cv::Mat prev_img;
	sequence[0].convertTo(prev_img, CV_8U, 255.0);
	
	std::vector< cv::Point2f > prev_corners;
	int maxCorners = 150;
	double qualityLevel = 0.0001;

	cv::goodFeaturesToTrack(prev_img, prev_corners, maxCorners, qualityLevel, 10, cv::noArray(), 3, false, 0.04);

	for (int i = 1; i < sequence.image_count(); i++)
	{
				//, cv::Rect(180, 50, 105, 125)
		
		cv::Mat next_img;
		sequence[i].convertTo(next_img, CV_8U, 255.0);
		

		std::vector< cv::Point2f > next_corners;
		std::vector<unsigned char> status;
		std::vector<float> error;
		cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03);
		cv::Size winSize(31, 31);

		cv::calcOpticalFlowPyrLK(prev_img, next_img, prev_corners, next_corners, status, error, winSize, 3, termcrit, 0, 0.001);

		
		//std::vector<std::pair<cv::Point2f, cv::Point2f>> pairs;
		std::vector< cv::Point2f > prev_t_corners;
		std::vector<cv::Point2f> next_t_corners;

		for (int j = 0; j < prev_corners.size(); j++)
		{
			double dx = std::abs(prev_corners[j].x - next_corners[j].x);
			double dy = std::abs(prev_corners[j].y - next_corners[j].y);
			
			if ((status[j] == 1) && dx < 2 && dy < 2)
			{
				prev_t_corners.push_back(prev_corners[j]);
				next_t_corners.push_back(next_corners[j]);
			}
		}
		std::cout << prev_t_corners.size() << " ";

		output_stream << "Images: " << 2 * i << ", " << 2 * i + 1 << "\n";
			
		cv::Mat next_circled, prev_circled;
		prev_img.copyTo(prev_circled);
		next_img.copyTo(next_circled);

		for (size_t j = 0; j < prev_t_corners.size(); j++)
		{
			output_stream << "[" << prev_t_corners[j].x << ", " << prev_t_corners[j].y << "] -> [" <<
				next_t_corners[j].x << ", " << next_t_corners[j].y << "]\n";

			circle(prev_circled, prev_t_corners[j], 4, cv::Scalar(255, 255, 0), -1, 8, 0);
			circle(next_circled, next_t_corners[j], 4, cv::Scalar(255, 255, 0), -1, 8, 0);
		}
		
		output_stream << "\n\n";


		feature_sequence.add_image(prev_circled);
		feature_sequence.add_image(next_circled);
		

		//transformation
		cv::Mat h = cv::findHomography(prev_t_corners, next_t_corners);
		cv::Mat dst;
		cv::warpPerspective(next_img, dst, h, next_img.size());
		std::cout << i << std::endl;
		MRISequence transform_sequence;
		transform_sequence.add_image(prev_img);
		transform_sequence.add_image(dst);
		transform_sequence.add_image(next_img);
		//transform_sequence.show();

		prev_corners = next_corners;
		prev_img = next_img;

	}
	
	output_stream.close();

	feature_sequence.show();
	
}
*/