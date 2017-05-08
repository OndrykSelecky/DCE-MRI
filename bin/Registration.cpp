#include "Registration.h"
#include "MRISequence.h"

#include "opencv2/features2d.hpp"
#include <fstream>
#include <cmath>

#include "s_hull_pro.h"



features detect_features(const MRISequence& sequence, double max_distance, int max_corners, double quality_level, int min_distance)
{
	auto image_count = sequence.image_count();
	if (image_count < 2) throw std::invalid_argument("number of images (" + std::to_string(image_count) + ") is too small\n");

	double sequence_contrast = 255.0*sequence.get_contrast();


	//Vector of features detected by calcOpticalFlowPyrLK for each image in sequence
	features detected_features_flow(image_count);

	//Status vector
	std::vector<std::vector<unsigned char>> detected_features_status(image_count);

	//Features detected in previous slice
	std::vector<cv::Point2f> prev_feature_points;
	

	//Conversion needed for flow functions
	cv::Mat prev_img;
	sequence[0].convertTo(prev_img, CV_8U, sequence_contrast);
	

	// Find features to track
	cv::goodFeaturesToTrack(prev_img, prev_feature_points, max_corners, quality_level, min_distance, cv::noArray());


	auto features_count = prev_feature_points.size();
	detected_features_flow[0] = prev_feature_points;
	detected_features_status[0] = std::vector<unsigned char>(features_count, 1);
	

	// Track features throughout the sequence
	for (int i=1;i<image_count;i++)
	{
		
		cv::Mat next_img;
		sequence[i].convertTo(next_img, CV_8U, sequence_contrast);

		std::vector< cv::Point2f > next_feature_points;
		std::vector<unsigned char> status;
		std::vector<float> error;

		cv::calcOpticalFlowPyrLK(prev_img, next_img, prev_feature_points, next_feature_points, status, error);
		
		detected_features_flow[i] = next_feature_points;
		detected_features_status[i] = status;
		
		prev_feature_points = next_feature_points;
		prev_img = next_img;
	}

	
	
	std::vector<bool> valid_features(features_count, 1);

	//Find, which features are valid (euclidian distance and status)
	for (auto i = 1; i < image_count; i++)
	{
		for (auto j = 0; j < features_count; j++)
		{
			double dx = std::abs(detected_features_flow[i - 1][j].x - detected_features_flow[i][j].x);
			double dy = std::abs(detected_features_flow[i - 1][j].y - detected_features_flow[i][j].y);

			double difference = std::sqrt(dx*dx + dy*dy);
			
			if (difference > max_distance || detected_features_status[i][j] == 0)			
			{
				valid_features[j] = false;
			}
		}
	}

		
	// find how many features are valid
	auto good_features_count = std::count(valid_features.begin(), valid_features.end(), 1);


	features final_features(image_count);

	//Accept only those features, which are valid throughout whole sequence
	for (int i = 0; i < image_count; i++)
	{		
		std::vector<cv::Point2f> feature_vector;		
		feature_vector.reserve(good_features_count);

		for (auto j = 0; j < features_count; j++)
		{
			if (valid_features[j] == true)
			{
				feature_vector.push_back(detected_features_flow[i][j]);				
			}
		}

		final_features[i] = (feature_vector);
	}

	return final_features;
}

std::vector<Triangle> triangulate(features& features, const MRISequence& sequence)
{
	
	std::vector<cv::Point2f> corner_points{
		cv::Point2f(0.0, 0.0),
		cv::Point2f((float)sequence[0].size().width - 1, 0.0),
		cv::Point2f(0.0, (float)sequence[0].size().height - 1),
		cv::Point2f((float)sequence[0].size().width - 1, (float)sequence[0].size().height - 1)
	};

	//Add corner points to features if they are not there 
	for (int i = corner_points.size() - 1; i >= 0; i--)
	{
		if (std::find(std::begin(features[0]), std::end(features[0]), corner_points[i]) != std::end(features[0]))
		{
			corner_points.erase(corner_points.begin() + i);
		}
	}

	for (auto i = 0; i < features.size(); i++)
	{
		for (auto j = 0; j <  corner_points.size(); j++)
		{
			features[i].push_back(corner_points[j]);
		}		
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
		

	//Sorting and filtering of points
	std::sort(points.begin(), points.end(), pointSortPredicate);
	std::vector<Shx>::iterator newEnd = std::unique(points.begin(), points.end(), pointComparisonPredicate);
	points.resize(newEnd - points.begin());


	//Find triangles
	std::vector<Triad> triads;
	s_hull_pro(points, triads);


	//convert into my triangle structure
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

MRISequence warp_sequence(const MRISequence& sequence, const features& features)
{
	auto image_count = sequence.image_count();
	if (image_count < 2) throw std::invalid_argument("number of images (" + std::to_string(image_count) + ") is too small\n");

	MRISequence transformed_sequence;
	transformed_sequence.add_image(sequence[0]);

	
	for (auto i = 1; i < image_count; i++)
	{		
		//find global perspective transformation
		cv::Mat homography = cv::findHomography(features[i], features[0]);

		//Warp slice
		cv::Mat dst;
		cv::warpPerspective(sequence[i], dst, homography, sequence[i].size());

		transformed_sequence.add_image(dst);		
	}

	transformed_sequence.set_contrast();
	
	return transformed_sequence;
}

MRISequence warp_sequence(const MRISequence& sequence, const features& features, const std::vector<Triangle>& triangles)
{
	
	//for each pixel in first image, compute id of triangle in which it lies and store it in 2d vector 
	//triangle_ids of same size as image

	std::vector<std::vector<int>> triangle_ids(sequence[0].size().width, std::vector<int>(sequence[0].size().height,0));
		
	for (int t_id = 0; t_id < triangles.size(); t_id++)
	{				
		//create bounding rectangle around triangle
		cv::Rect bounding_rect = cv::boundingRect(triangles[t_id].point_coordinates);
		
		//choose only points with coordinates inside bounding rectangle
		for (int i = bounding_rect.x; i < bounding_rect.x + bounding_rect.width; i++)
		{
			for (int j = bounding_rect.y; j < bounding_rect.y + bounding_rect.height; j++)

				//for each point, test if it lies inside rectangle
				if (cv::pointPolygonTest(triangles[t_id].point_coordinates, cv::Point2f((float)i, (float)j), false) >= 0)
				{
					triangle_ids[i][j] = t_id;
				}
		}		
	}


	//For each triangle create list of points inside it

	std::vector<std::vector<cv::Point2f>> triangle_points(triangles.size());

	for (int i = 0; i < sequence[0].size().width; i++)
	{
		for (int j = 0; j < sequence[0].size().height; j++)
		{
			triangle_points[triangle_ids[i][j]].push_back(cv::Point2f((float)i, (float)j));			
		}
	}


	//Transformation

	MRISequence transformed_sequence;
	transformed_sequence.set_contrast(sequence.get_contrast());
	cv::Mat sequence_0;
	sequence[0].copyTo(sequence_0);
	transformed_sequence.add_image(sequence_0);

	for (int sequence_id = 1; sequence_id < sequence.image_count(); sequence_id++)
	{
		
		/* 
		For each triangle compute 3x3 affine transformation matrix. 
		Function cv::getAffineTransform returns 2x3 affine matrix, but 3x3 matrix is needed later, so (0, 0, 1) row is added.
		*/

		std::vector<cv::Mat> matrices(triangles.size());

		cv::Mat row = cv::Mat::zeros(1, 3, CV_64F);
		row.at<double>(0, 2) = 1;

		for (int i = 0; i < triangles.size(); i++)
		{
			std::vector<cv::Point2f> dst_points;
			dst_points.push_back(features[sequence_id][triangles[i].a]);
			dst_points.push_back(features[sequence_id][triangles[i].b]);
			dst_points.push_back(features[sequence_id][triangles[i].c]);

			//get matrix
			cv::Mat transformation_matrix = cv::getAffineTransform(triangles[i].point_coordinates, dst_points);

			transformation_matrix.push_back(row);
			matrices[i]=transformation_matrix;
		}
		
		

		//Perform transformation. New x and y coordinates of each pixel are stored int map_x and map_y 

		cv::Mat map_x = cv::Mat::zeros(sequence[0].size(), sequence[0].type());
		cv::Mat map_y = cv::Mat::zeros(sequence[0].size(), sequence[0].type());

		for (int t_id = 0; t_id < triangle_points.size(); t_id++)
		{
			cv::Mat matrix = matrices[t_id];

			if (triangle_points[t_id].size() == 0) continue;

			//transform points in triangle
			std::vector<cv::Point2f> transformed_points(triangle_points[t_id].size());
			cv::perspectiveTransform(triangle_points[t_id], transformed_points, matrix);

			//update maps
			for (int i = 0; i < triangle_points[t_id].size(); i++)
			{
				map_x.at<float>(triangle_points[t_id][i]) = transformed_points[i].x;
				map_y.at<float>(triangle_points[t_id][i]) = transformed_points[i].y;
			}

		}
		
		/*
		int rows = sequence_0.rows;
		int cols = sequence_0.cols;

		cv::Mat map_x = cv::Mat::zeros(sequence[0].size(), sequence[0].type());
		cv::Mat map_y = cv::Mat::zeros(sequence[0].size(), sequence[0].type());

		int triangle_id;
		float *map_x_ptr;
		float *map_y_ptr;
		std::vector<cv::Point2f> transformed_point(1);

		for (int i = 0; i < rows; i++)
		{
			map_x_ptr = map_x.ptr<float>(i);
			map_y_ptr = map_y.ptr<float>(i);

			for (int j = 0; j < cols; j++)
			{
				triangle_id = triangle_ids[j][i];
				
				//cv::perspectiveTransform(std::vector<cv::Point2f>(1,cv::Point2f(j,i)), transformed_point, matrices[triangle_id]);
				map_x_ptr[j] = transformed_point[0].x;
				map_y_ptr[j] = transformed_point[0].y;
			}

		}

		*/

		//Final remapping
		cv::Mat dst(sequence[0].size(), sequence[0].type());

		cv::remap(sequence[sequence_id], dst, map_x, map_y, CV_INTER_LINEAR);
		
		transformed_sequence.add_image(dst);
	}
	

	return transformed_sequence;
}

void show_features(MRISequence& sequence, const features& features)
{
	if (sequence.image_count() != features.size() || sequence.image_count() < 1) throw std::out_of_range("Sequence and features must both have equal, non-zero size");

	MRISequence feature_sequence;
	feature_sequence.set_contrast(sequence.get_contrast());


	for (auto i = 0; i < sequence.image_count(); i++)
	{
		//cv::Mat img = cv::Mat::zeros(sequence[i].size(), sequence[i].type());
		cv::Mat img;
		sequence[i].copyTo(img);

		for (auto j = 0; j < features[i].size(); j++)
		{
			circle(img, features[i][j], 4, cv::Scalar(0.9, 0.9, 0.9), -1, 8, 0);
		}

		feature_sequence.add_image(img);
	}

	//we will move sequence, so window will remain active outside function
	sequence = std::move(feature_sequence);
	sequence.show("Features Detected");
	//sequence.write("D:/Dokumenty/Projects/QIN Breast DCE-MRI/registered_contrast", 2, true);
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


MRISequence registration(MRISequence& sequence, features& features, const int method, bool show)
{
	
	
	if (show) sequence.show("Sequence" + sequence.get_folder_name());
	

	MRISequence feature_sequence(sequence);
	if (show)
	{		
		show_features(feature_sequence, features);
	}
	
	MRISequence transformed_sequence;
	MRISequence triangle_sequence(sequence);
	
	if (method == HOMOGRAPHY)
	{
		transformed_sequence = warp_sequence(sequence, features);
		if (show) transformed_sequence.show("Warped sequence" + sequence.get_folder_name());
	}
	else if (method == OPTIMAL_TRIANGULATION)
	{
		auto triangles = triangulate(features, sequence);
		if (show)
		{			
			show_triangles(triangle_sequence, triangles, features);
		}

		transformed_sequence = warp_sequence(sequence, features, triangles);
		if (show) transformed_sequence.show("Triangulation warped sequence" + sequence.get_folder_name());
		
		
	}
	else
	{
		throw std::invalid_argument("Error: Method in function \"registration\" should be OPTIMAL_TRIANGULATION or HOMOGRAPHY\n");
	}
	
	
	return transformed_sequence;

}

