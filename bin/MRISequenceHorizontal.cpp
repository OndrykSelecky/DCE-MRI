#include "MRISequenceHorizontal.h"
#include "MRISessionHorizontal.h"

MRISequenceHorizontal::MRISequenceHorizontal(const std::string& folder, const std::string& image_file_name, int sequence_id)
{
	m_image_file_name = image_file_name;
	m_sequence_id = sequence_id;
	m_folder = folder;
}

MRISequenceHorizontal::MRISequenceHorizontal(const MRISequenceHorizontal & other)
{
	*this = other;
}

MRISequenceHorizontal::MRISequenceHorizontal(MRISequenceHorizontal && other)
{
	*this = std::move(other);
}


MRISequenceHorizontal & MRISequenceHorizontal::operator=(const MRISequenceHorizontal& other)
{
	if (this != &other)
	{
		this->m_contrast = other.m_contrast;
		this->m_folder = other.m_folder;
		this->m_sequence_id = other.m_sequence_id;
		this->m_trackbar_max = other.m_trackbar_max;
		this->m_trackbar_value = other.m_trackbar_value;		
		this->m_image_file_name = other.m_image_file_name;

		std::vector<cv::Mat> images;
		for (auto img : other.m_images)
		{
			cv::Mat copy;
			img.copyTo(copy);
			images.push_back(copy);
		}
		this->m_images = images;
	}
	std::cout << "copy\n";
	return *this;
}

MRISequenceHorizontal & MRISequenceHorizontal::operator=(MRISequenceHorizontal&& other)
{
	if (this != &other)
	{
		this->m_contrast = other.m_contrast;
		this->m_folder = other.m_folder;
		this->m_sequence_id = other.m_sequence_id;
		this->m_trackbar_max = other.m_trackbar_max;
		this->m_trackbar_value = other.m_trackbar_value;
		this->m_image_file_name = other.m_image_file_name;

		this->m_images = other.m_images;
		other.m_images.clear();
	}
	std::cout << "move\n";
	return *this;
}

void MRISequenceHorizontal::read(const MRISessionHorizontal& session)
{
	auto folder_names = session.get_folder_names();

	try
	{
		for (const auto& folder : folder_names)
		{
			cv::Mat image = read_image(folder + "/" + m_image_file_name);
			m_images.push_back(image);
		}
	}
	catch (std::invalid_argument& e)
	{
		throw e;
		return;
	}
	
	set_contrast();

}

