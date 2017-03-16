#include "Registration.h"
#include "MRISequence.h"
#include "MRISessionHorizontal.h"
#include "MRISequenceHorizontal.h"

#include "opencv2/features2d.hpp"
#include <fstream>
#include <cmath>

#include "s_hull_pro.h"

bool pointSortPredicate(const Shx& a, const Shx& b)
{
	if (a.r < b.r)
		return true;
	else if (a.r > b.r)
		return false;
	else if (a.c < b.c)
		return true;
	else
		return false;
};

bool pointComparisonPredicate(const Shx& a, const Shx& b)
{
	return a.r == b.r && a.c == b.c;
}

features detect_features(const MRISequence& sequence, double max_difference, int maxCorners, double qualityLevel, int minDistance)
{
	auto image_count = sequence.image_count();
	if (image_count < 2) throw std::invalid_argument("number of images (" + std::to_string(image_count) + ") is too small\n");

	double sequence_contrast = 255.0*sequence.get_contrast();

	features detected_features_flow;
	std::vector<std::vector<unsigned char>> detected_features_status;
	std::vector<cv::Point2f> prev_feature_points;
	

	cv::Mat prev_img;
	sequence[0].convertTo(prev_img, CV_8U, sequence_contrast);
	

	// Find features to track
	cv::goodFeaturesToTrack(prev_img, prev_feature_points, maxCorners, qualityLevel, minDistance, cv::noArray());


	int features_count = prev_feature_points.size();
	detected_features_flow.push_back(prev_feature_points);
	detected_features_status.push_back(std::vector<unsigned char>(features_count, 1));
	

	// Track features throughout the sequence
	for (std::vector<cv::Mat>::const_iterator it = sequence.data().begin() + 1; it != sequence.data().end(); ++it)
	{
		cv::Mat next_img;
		it->convertTo(next_img, CV_8U, sequence_contrast);

		std::vector< cv::Point2f > next_feature_points;
		std::vector<unsigned char> status;
		std::vector<float> error;

		cv::calcOpticalFlowPyrLK(prev_img, next_img, prev_feature_points, next_feature_points, status, error);

		detected_features_flow.push_back(next_feature_points);
		detected_features_status.push_back(status);
		
		prev_feature_points = next_feature_points;
		prev_img = next_img;
	}

	
	
	std::vector<unsigned char> valid_features(features_count, 1);

	//Find, which features are valid
	for (auto i = 1; i < image_count; i++)
	{
		for (auto j = 0; j < features_count; j++)
		{
			double dx = std::abs(detected_features_flow[i - 1][j].x - detected_features_flow[i][j].x);
			double dy = std::abs(detected_features_flow[i - 1][j].y - detected_features_flow[i][j].y);

			double difference = std::sqrt(dx*dx + dy*dy);
			
			if (difference > max_difference || detected_features_status[i][j] == 0)			
			{
				valid_features[j] = false;
			}
		}
	}


	/*std::sort(mdiff.begin(), mdiff.end());
	std::reverse(mdiff.begin(), mdiff.end());
	for (int i = 0; i < 30; i++) std::cout << mdiff[i] << " ";*/

	
	// find how many features are valid
	int good_features_count = std::count(valid_features.begin(), valid_features.end(), 1);
	std::cout << "Number of features found: " << features_count << "\nValid features: " << good_features_count << "\n";


	features final_features;

	// Discard non-valid features
	for (int i = 0; i < image_count; i++)
	{		
		std::vector<cv::Point2f> feature_vector;		
		feature_vector.reserve(good_features_count);

		for (auto j = 0; j < features_count; j++)
		{
			if (valid_features[j] == 1)
			{
				feature_vector.push_back(detected_features_flow[i][j]);				
			}
		}

		final_features.push_back(feature_vector);
	}

	return final_features;
}

void show_features(MRISequence& sequence, const features& features)
{
	if (sequence.image_count() != features.size() || sequence.image_count() < 1) throw std::out_of_range("Sequence and features must both have equal, non-zero size");

	MRISequence feature_sequence;
	feature_sequence.set_contrast(sequence.get_contrast());


	for (auto i = 0; i < sequence.image_count(); i++)
	{
		cv::Mat img;
		sequence[i].copyTo(img);		

		for (auto j = 0; j < features[i].size(); j++)
		{					
				circle(img, features[i][j], 4, cv::Scalar(255, 255, 255), -1, 8, 0);
		}
		
		feature_sequence.add_image(img);		
	}

	sequence = std::move(feature_sequence);
	sequence.show("Features Detected");
}

std::vector<Triangle> get_triangles(features& features, const MRISequence& sequence)
{

	for (auto i = 0; i < features.size(); i++)
	{
		features[i].push_back(cv::Point2f(0.0, 0.0));
		features[i].push_back(cv::Point2f(sequence[0].size().width - 1, 0.0));
		features[i].push_back(cv::Point2f(0.0, sequence[0].size().height - 1));
		features[i].push_back(cv::Point2f(sequence[0].size().width - 1, sequence[0].size().height - 1));
	}

	auto triangle_features = features[0];	


	std::vector<Shx> points;
	for (int i = 0; i < triangle_features.size(); i++)
	{
		Shx point;
		point.id = i;
		point.r = triangle_features[i].x;
		point.c = triangle_features[i].y;

		points.push_back(point);
	}
		
	std::sort(points.begin(), points.end(), pointSortPredicate);
	std::vector<Shx>::iterator newEnd = std::unique(points.begin(), points.end(), pointComparisonPredicate);
	points.resize(newEnd - points.begin());

	std::vector<Triad> triads;
	s_hull_pro(points, triads);

	std::vector<Triangle> triangles;
	triangles.reserve(triads.size());

	for (const auto& triad : triads)
	{
		Triangle triangle;
		triangle.a = triad.a;
		triangle.b = triad.b;
		triangle.c = triad.c;
		triangle.point_coordinates.push_back(triangle_features[triangle.a]);
		triangle.point_coordinates.push_back(triangle_features[triangle.b]);
		triangle.point_coordinates.push_back(triangle_features[triangle.c]);
		triangles.push_back(triangle);
	}

	return triangles;
}

void show_triangles(MRISequence& sequence, const std::vector<Triangle>& triangles, const features& features)
{
	MRISequence triangle_sequence;
	triangle_sequence.set_contrast(sequence.get_contrast());


	for (auto i = 0; i < sequence.image_count(); i++)
	{
		cv::Mat dst;
		sequence[i].copyTo(dst);

		auto triangle_features = features[i];


		for (const auto& triangle : triangles)
		{

			cv::line(dst, triangle_features[triangle.a], triangle_features[triangle.b], cv::Scalar(255, 255, 255));
			cv::line(dst, triangle_features[triangle.a], triangle_features[triangle.c], cv::Scalar(255, 255, 255));
			cv::line(dst, triangle_features[triangle.c], triangle_features[triangle.b], cv::Scalar(255, 255, 255));
			
		}

		triangle_sequence.add_image(dst);
	}

	
	sequence = std::move(triangle_sequence);
	sequence.show("Triangles");
}



MRISequence warp_sequence(const MRISequence& sequence, const features& features)
{
	auto image_count = sequence.image_count();
	if (image_count < 2) throw std::invalid_argument("number of images (" + std::to_string(image_count) + ") is too small\n");

	MRISequence transformed_sequence;
	transformed_sequence.add_image(sequence[0]);


	for (auto i = 1; i < image_count; i++)
	{		
		cv::Mat h = cv::findHomography(features[i], features[0]);

		cv::Mat dst;
		cv::warpPerspective(sequence[i], dst, h, sequence[i].size());

		transformed_sequence.add_image(dst);		
	}

	transformed_sequence.set_contrast();
	
	return transformed_sequence;
}


MRISequence warp_sequence(const MRISequence& sequence, const features& features, const std::vector<Triangle>& triangles)
{
	
	std::vector<std::vector<int>> triangle_id(sequence[0].size().width, std::vector<int>(sequence[0].size().height,0));

	//for each pixel in matrix, compute id of triangle in which it lies and store it in 2d vector triangle_id of same size as image
	for (int t_id = 0; t_id < triangles.size(); t_id++)
	{
				
				//create bounding rect around triangle
		cv::Rect bounding_rect = cv::boundingRect(triangles[t_id].point_coordinates);
		
		for (int i = bounding_rect.x; i < bounding_rect.x + bounding_rect.width; i++)
		{
			for (int j = bounding_rect.y; j < bounding_rect.y + bounding_rect.height; j++)
				if (cv::pointPolygonTest(triangles[t_id].point_coordinates, cv::Point2f(i,j), false) >= 0)
				{
					triangle_id[i][j] = t_id;
				}
		}		
	}


	std::vector<std::vector<cv::Point2f>> triangle_points(triangles.size());

	//divide each points into groups, one group is assigned with one triangle
	for (int i = 0; i < sequence[0].size().width; i++)
	{
		for (int j = 0; j < sequence[0].size().height; j++)
		{
			triangle_points[triangle_id[i][j]].push_back(cv::Point2f(i, j));			
		}
	}

	MRISequenceHorizontal transformed_sequence;
	transformed_sequence.set_contrast(sequence.get_contrast());
	cv::Mat sequence_0;
	sequence[0].copyTo(sequence_0);
	transformed_sequence.add_image(sequence_0);

	for (int sequence_id = 1; sequence_id < sequence.image_count(); sequence_id++)
	{
		//for each triangle compute 3x3 affine transformation matrix

		std::vector<cv::Mat> matrices;

		cv::Mat row = cv::Mat::zeros(1, 3, CV_64F);
		row.at<double>(0, 2) = 1;

		for (int i = 0; i < triangles.size(); i++)
		{
			std::vector<cv::Point2f> dst_points;
			dst_points.push_back(features[sequence_id][triangles[i].a]);
			dst_points.push_back(features[sequence_id][triangles[i].b]);
			dst_points.push_back(features[sequence_id][triangles[i].c]);

			cv::Mat transformation_matrix = cv::getAffineTransform(triangles[i].point_coordinates, dst_points);

			transformation_matrix.push_back(row);
			matrices.push_back(transformation_matrix);
		}



		//perform transformation
		cv::Mat map_x(sequence[0].size(), sequence[0].type());
		cv::Mat map_y(sequence[0].size(), sequence[0].type());

		for (int t_id = 0; t_id < triangle_points.size(); t_id++)
		{
			cv::Mat matrix = matrices[t_id];

			if (triangle_points[t_id].size() == 0) continue;

			std::vector<cv::Point2f> out(triangle_points[t_id].size());
			cv::perspectiveTransform(triangle_points[t_id], out, matrix);

			for (int i = 0; i < triangle_points[t_id].size(); i++)
			{
				map_x.at<float>(triangle_points[t_id][i]) = out[i].x;
				map_y.at<float>(triangle_points[t_id][i]) = out[i].y;
			}

		}

		cv::Mat dst(sequence[0].size(), sequence[0].type());

		cv::remap(sequence[sequence_id], dst, map_x, map_y, CV_INTER_LINEAR);
		
		transformed_sequence.add_image(dst);
	}
	

	return transformed_sequence;
}


void registration(const std::string& folder, int sequence_id)
{
	
	std::shared_ptr<MRISessionHorizontal> session(new MRISessionHorizontal(folder));
	session->read();	
	
	auto sequence = (*session)[54];	


	/*std::shared_ptr<MRISequence> sequence(new MRISequence());
	sequence->set_folder_name("D:/Dokumenty/Projects/QIN Breast DCE-MRI/Sequence1");*/

	try {
		sequence->read(*session);
		//sequence->read();
	}
	catch (std::invalid_argument& e)
	{
		std::cout << e.what();
		return;
	}	
	std::reverse(sequence->data().begin(), sequence->data().end());
	sequence->show("Sequence");
	

	features features = detect_features(*sequence);
	MRISequence feature_sequence(*sequence);
	show_features(feature_sequence, features);
	

	MRISequence transformed_sequence = warp_sequence(*sequence, features);
	transformed_sequence.show("Warped sequence");
	
	auto triangles = get_triangles(features, *sequence);
	MRISequence triangle_sequence(*sequence);
	show_triangles(triangle_sequence, triangles, features);


	MRISequence triangle_transformed_sequence = warp_sequence(*sequence, features, triangles);
	triangle_transformed_sequence.show("Triangulation warped sequence");

	//triangle_transformed_sequence.write("output.txt");
	//sequence->write("output_orig.txt");

	triangle_transformed_sequence.write("D:/Dokumenty/Projects/QIN Breast DCE-MRI/Sequence1", CV_16UC1);
	
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