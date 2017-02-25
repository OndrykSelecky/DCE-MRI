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

		std::vector<cv::Mat> images;
		for (auto img : other.m_images)
		{
			cv::Mat copy;
			img.copyTo(copy);
			images.push_back(copy);
		}
		this->m_images = images;
	}
	
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

		this->m_images = other.m_images;
		other.m_images.clear();
	}
	
	return *this;
}


void MRISequence::read()
{	
	std::vector<std::string> names = get_file_names(m_folder);
	
	for (const auto& n : names)
	{		
		try 
		{
			cv::Mat image = read_image(m_folder + "/" + n);
			m_images.push_back(image);		
		}
		catch (std::invalid_argument e)
		{
			std::cerr << e.what();
		}
	}

	set_contrast();

}

/*void MRISequence::read_horizontal(MRISession& session, const std::string & file_name) 
{	
	try
	{
		for (const auto& s : session.data())
		{			
			cv::Mat image = read_image(s.m_folder + "/" + file_name);
			m_images.push_back(image);
		}
	}
	catch (std::invalid_argument& e)
	{
		throw e;
		return;
	}

	m_folder = session.get_folder_name() + " - " + file_name;	

	set_contrast();
}*/

void MRISequence::show(std::string window_name)
{
	if (m_images.empty()) return;

	m_trackbar_value = 0;
	m_trackbar_max = image_count() - 1;
	
	(window_name == "") ? m_window_name = m_folder : m_window_name = window_name;
	std::cout << m_window_name;
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
		throw std::invalid_argument("Error Reading File "+file_name+"\n");
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


	cv::Mat img = itk::OpenCVImageBridge::ITKImageToCVMat(reader->GetOutput(), false);

	cv::Mat dst(img.size(), CV_32F);
	float depth = 1 << (img.elemSize() * 8);
		
	img.convertTo(dst, CV_32F, 1.0/depth);
	
	return dst;
}


