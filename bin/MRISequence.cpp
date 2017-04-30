#include "MRISequence.h"
#include "Util.h"

#include <windows.h>
#include <string>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkOpenCVImageBridge.h"
#include "itkImage.h"

//dicom
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"
#include "gdcmGlobal.h"
#include "itkRescaleIntensityImageFilter.h"

MRISequence::MRISequence()
{
	m_sequence_id = 0;
	m_contrast = 1;
}

MRISequence::MRISequence(const std::string & folder, int sequence_id) : MRISequence()
{
	m_folder = folder;
	m_sequence_id = sequence_id;
}

MRISequence::MRISequence(const std::string & folder, std::vector<std::string> image_names, int sequence_id)
{
	m_image_names = image_names;
	m_folder = folder;
	m_sequence_id = sequence_id;
}

MRISequence::MRISequence(const MRISequence & other)
{
	*this = other;
}

MRISequence::MRISequence(MRISequence && other)
{
	*this = std::move(other);
}


MRISequence & MRISequence::operator=(const MRISequence& other)
{
	if (this != &other)
	{
		this->m_contrast = other.m_contrast;
		this->m_folder = other.m_folder;
		this->m_sequence_id = other.m_sequence_id;
		this->m_trackbar_max = other.m_trackbar_max;
		this->m_trackbar_value = other.m_trackbar_value;
		this->m_image_names = other.m_image_names;

		std::vector<cv::Mat> images;
		for (auto img : other.m_images)
		{
			cv::Mat copy;
			img.copyTo(copy);
			images.push_back(copy);
		}
		this->m_images = images;
	}
	//std::cout << "copy:\n\t" << m_folder << "\n\t" << m_sequence_id << "\n";
	return *this;
}

MRISequence & MRISequence::operator=(MRISequence&& other)
{
	if (this != &other)
	{
		this->m_contrast = other.m_contrast;
		this->m_folder = other.m_folder;
		this->m_sequence_id = other.m_sequence_id;
		this->m_trackbar_max = other.m_trackbar_max;
		this->m_trackbar_value = other.m_trackbar_value;
		this->m_image_names = other.m_image_names;
		this->m_images = other.m_images;
		other.m_images.clear();
	}
	//std::cout << "move:\n\t" << m_folder << "\n\t" << m_sequence_id << "\n";
	return *this;
}


int MRISequence::read()
{	


	if (m_image_names.size() == 0)
	{
		m_image_names = get_dicom_file_names(m_folder, false);
	}
	if (m_image_names.size() == 0)
	{
		m_image_names = get_file_names(m_folder);
	}
	for (const auto& name : m_image_names)
	{
		try
		{
			cv::Mat image = read_image(m_folder + "/" + name);
			m_images.push_back(image);
		}
		catch (std::invalid_argument e)
		{
			std::cerr << e.what();
		}
	}

	set_contrast();

	return m_images.size();

}

void MRISequence::write_txt(const std::string & file_name)
{
	std::ofstream output_stream;
	output_stream.open(file_name);
	if (!output_stream.is_open())
	{
		std::cout << "Error: file counldn't be opened\n";
		return;
	}

	output_stream << m_images[0].rows << " " << m_images[0].cols << " " << image_count() << "\n";

	for (int k = 0; k < this->image_count(); k++)
	{
		for (int i = 0; i < this->m_images[0].size().width; i++)
		{
			for (int j = 0; j < this->m_images[0].size().height; j++)
			{

				float value = m_images[k].at<float>(i, j);
				value *= (1 << 16);
				output_stream << value << " ";
				
			}
			output_stream << "\n";
		}
		output_stream << "\n";
	}
}

void MRISequence::write(const std::string & folder, int image_type, bool adjust_contrast)
{
	int depth;
	if (image_type == CV_16UC1) depth = 16;
	else if (image_type == CV_8UC1 || image_type == CV_8SC1) depth = 8;
	else return;

	depth = 1 << depth;

	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	
	int image_number = 0;
	for (auto i= 0; i< m_images.size(); i++)
	{
		cv::Mat dst(m_images[i].rows, m_images[i].cols, image_type);
		
		if (adjust_contrast)
		{
			m_images[i].convertTo(dst, image_type, depth*m_contrast);
		}
		else
		{
			m_images[i].convertTo(dst, image_type, depth);
		}

		std::stringstream image_name_stream;
		image_name_stream << std::setw(6) << std::setfill('0') << image_number++;
		std::string file_name = image_name_stream.str();
		
		std::string path = folder + "/" + file_name + ".png";
		
		cv::imwrite(path , dst, compression_params);
		
	}
	
}



void MRISequence::show(std::string window_name)
{
	if (m_images.empty()) return;

	m_trackbar_value = 0;
	m_trackbar_max = image_count() - 1;
	
	(window_name == "") ? m_window_name = m_folder : m_window_name = window_name;
	//std::cout << m_window_name;
	cv::namedWindow(m_window_name, cv::WINDOW_NORMAL);

	void *sequence_pointer = static_cast<MRISequence*>(this);
	
	cv::createTrackbar("T"+m_folder, m_window_name, &m_trackbar_value, m_trackbar_max, on_trackbar, sequence_pointer);
	on_trackbar(m_trackbar_value, sequence_pointer);
	cv::waitKey(0);
}

void MRISequence::set_contrast()
{	
	m_contrast = 0;

	for (const auto& img : m_images)
	{
		double min, max;
		cv::minMaxLoc(img, &min, &max);
		double contrast = max - min;
		
		if (contrast > m_contrast)
			m_contrast = contrast;
	}
	//std::cout << m_contrast;
	m_contrast = 1.0 / m_contrast;
	
}

void MRISequence::on_trackbar(int, void* p)
{
	MRISequence* sequence = static_cast<MRISequence*> (p);
		
	cv::Mat dst;	
	(*sequence)[sequence->m_trackbar_value].convertTo(dst, -1, sequence->m_contrast);

	cv::imshow(sequence->m_window_name, dst);
}

cv::Mat read_image(const std::string & file_name, bool print_info)
{
	cv::Mat img;
	img = cv::imread(file_name, cv::IMREAD_GRAYSCALE);

	//Image can not be read with opencv imread
	if (img.data == NULL)
	{		
		//Set read parameters

		typedef unsigned short InputPixelType;
		const unsigned int   InputDimension = 2;
		typedef itk::Image< InputPixelType, InputDimension > InputImageType;

		typedef itk::ImageFileReader< InputImageType > ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(file_name);

		typedef itk::GDCMImageIO           ImageIOType;
		ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
		reader->SetImageIO(gdcmImageIO);

		//Read Image

		try
		{
			reader->Update();
		}
		catch (itk::ExceptionObject)
		{
			std::cout << "File " + file_name + "was not recognized as image file and was skipped\n";
		}

		//Initialize Dictionary
		typedef itk::MetaDataDictionary   DictionaryType;
		const  DictionaryType & dictionary = gdcmImageIO->GetMetaDataDictionary();
		typedef itk::MetaDataObject< std::string > MetaDataStringType;

		//Print Dicom tags
		if (print_info)
		{
			DictionaryType::ConstIterator itr = dictionary.Begin();
			DictionaryType::ConstIterator end = dictionary.End();

			while (itr != end)
			{
				itk::MetaDataObjectBase::Pointer  entry = itr->second;

				MetaDataStringType::Pointer entryvalue =
					dynamic_cast<MetaDataStringType *>(entry.GetPointer());

				if (entryvalue)
				{
					std::string tagkey = itr->first;
					std::string labelId;
					bool found = itk::GDCMImageIO::GetLabelFromTag(tagkey, labelId);

					std::string tagvalue = entryvalue->GetMetaDataObjectValue();

					if (found)
					{
						std::cout << "(" << tagkey << ") " << labelId;
						std::cout << " = " << tagvalue.c_str() << std::endl;
					}

					else
					{
						std::cout << "(" << tagkey << ") " << "Unknown";
						std::cout << " = " << tagvalue.c_str() << std::endl;
					}
				}

				++itr;
			}
		}


		img = itk::OpenCVImageBridge::ITKImageToCVMat(reader->GetOutput(), false);
	}

	/*cv::namedWindow("Window");
	cv::imshow("Window", img);
	cv::waitKey(0);*/
	
	cv::Mat dst(img.size(), CV_32F);
	float depth = 1 << (img.elemSize() * 8);
		
	img.convertTo(dst, CV_32F, 1.0/depth);
	
	return dst;
}


