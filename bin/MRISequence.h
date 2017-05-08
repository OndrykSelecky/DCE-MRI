#pragma once
#include <memory>

#include "opencv2/opencv.hpp"
#include "MRISession.h"


class MRISession;

/*
Representation of sequence of images
*/
class MRISequence
{
public:

	MRISequence();

	MRISequence(const std::string& folder, int sequence_id = -1);

	MRISequence(const std::string& folder, std::vector<std::string> image_names, int sequence_id = -1);
		
	

	//Copy and move constructors
	MRISequence(const MRISequence& other);

	MRISequence(MRISequence&& other);
		
	
	//Assignment and move-assignment operators
	MRISequence& operator=(const MRISequence &other);

	MRISequence& operator=(MRISequence &&other);


	//Access to image from sequence
	cv::Mat &operator[](int i) { return m_images[i]; }

	cv::Mat const &operator[](int i) const { return m_images[i]; }


	//Read sequence
	int read();

	

	/* 
	write sequence in folder in png format. 
	image_type = CV_16UC1 or CV_8UC1.
	adjust_contrast = save with adjusted contrast or original values (original needed for analysis)
	*/
	void write(const std::string& folder, int image_type = CV_16UC1, bool adjust_contrast = false);


	//Show sequence in window window_name
	void show(std::string window_name = std::string(""));
	

	//Access to whole vector
	std::vector<cv::Mat>& data() { return m_images; }

	const std::vector<cv::Mat>& data() const { return m_images; }


	//Number of images
	size_t image_count() const { return m_images.size(); }


	//Add image
	void add_image(const cv::Mat& image) { m_images.push_back(image); }

	//clear sequence vector
	void clear() { m_images.clear(); };


	//Getters and setters
	int get_sequence_id() const { return m_sequence_id; }

	void set_sequence_id(int sequence_id) { m_sequence_id = sequence_id; }
		
	std::string get_folder_name() const { return m_folder; }

	void set_folder_name(const std::string& folder_name) { m_folder = folder_name; }

	double get_contrast() const { return m_contrast; }

	void set_contrast();

	void set_contrast(double contrast) { m_contrast = contrast; }


	//compare function for sorting
	static bool compare_sequences(std::shared_ptr<MRISequence> a, std::shared_ptr<MRISequence> b)
	{
		return a->m_sequence_id < b->m_sequence_id;
	}

protected:

	//names of image files 
	std::vector<std::string> m_image_names;


	//Vector of images
	std::vector<cv::Mat> m_images;
	

	//ID of sequence according to acquisition time
	int m_sequence_id;
	

	//Sequence folder path
	std::string m_folder;
	



	//Trackbar variables, used for visualization
	int m_trackbar_value;

	int m_trackbar_max;

	std::string m_window_name;

	//adjusted contrast for visualization
	double m_contrast;
	


	//On trackbar change
	static void on_trackbar(int, void*);
	

};



//Reads dicom image with file name file_name
cv::Mat read_image(const std::string& file_name, bool print_info = false);