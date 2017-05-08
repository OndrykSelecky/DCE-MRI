#include <fstream>
#include <string>

#include "MRISession.h"
#include "Util.h"
#include "MRISequence.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkOpenCVImageBridge.h"
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"
#include "gdcmGlobal.h"
#include "itkRescaleIntensityImageFilter.h"


int MRISession::read(bool new_read)
{
	read_sequence_folders(new_read);

	read_image_names(new_read);

	return m_sequence_folders.size();
}

MRISequence MRISession::get_sequence(unsigned int sequence_number)
{

	if (m_sequence_folders.size() == 0)
	{
		throw std::invalid_argument("Error: Session is empty\n");

	}

	if (sequence_number >= m_sequence_folders.size())
	{
		throw std::invalid_argument("Error: Sequence number is greater than number of sequences in session\n");
	}

	MRISequence sequence(m_folder + "/" + m_sequence_folders[sequence_number], m_image_names[sequence_number]);
	
	return sequence;
}

MRISequence MRISession::get_horizontal_sequence(unsigned int sequence_number)
{
	if (m_sequence_folders.size() == 0)
	{
		throw std::invalid_argument("Error: Session is empty\n");

	}

	if (sequence_number >= m_image_names[0].size())
	{
		throw std::invalid_argument("Error: Sequence number in subscript is greater than number of sequences in session\n");
	}

	std::vector<std::string> file_names(m_image_names.size());

	for (auto i = 0; i < m_image_names.size(); i++)
	{
		file_names[i] = m_sequence_folders[i] + "/" + m_image_names[i][sequence_number];
	}

	MRISequence sequence(m_folder, file_names);
	

	return sequence;
}

void MRISession::read_sequence_folders(bool new_read)
{
	
	/*
	set up input stream for reading ordered folder names
	*/
	std::ifstream sequence_input_stream;
	sequence_input_stream.open(m_folder + "/" + SEQUENCE_ORDER_FILE);

	/*
	if file was found, read folder names
	*/
	if (sequence_input_stream.is_open() && !new_read)
	{
		while (!sequence_input_stream.eof())
		{
			int sequence_id;
			sequence_input_stream >> sequence_id;

			std::string sequence_name;
			std::getline(sequence_input_stream, sequence_name);
			std::getline(sequence_input_stream, sequence_name);
			if (sequence_name != "")
			{
				m_sequence_folders.push_back(sequence_name);
			}
		}
	}

	//else find all subfolders and get id of sequence by opening first dicom file in each folder	
	else
	{
		std::vector<std::string> subfolders = get_folder_names(m_folder);
		std::vector<std::pair<int, std::string>> sequence_folders;

		for (const auto& folder : subfolders)
		{
			std::string folder_path = m_folder + "/" + folder;
			std::string file_name = get_first_file_name(folder_path, ".dcm");

			if (file_name == "")
			{
				continue;
			}
			
			int id = find_id(folder_path + "/" + file_name, SEQUENCE_ENTRY_ID);
			
			if (id >= 0)
			{
				sequence_folders.push_back(std::make_pair(id, folder));
			}
		}

		std::sort(sequence_folders.begin(), sequence_folders.end());


		for (const auto& folder : sequence_folders)
		{
			m_sequence_folders.push_back(folder.second);
		}

		//Write in file for quicker read
		std::ofstream output_sequence_stream;
		output_sequence_stream.open(m_folder + "/" + SEQUENCE_ORDER_FILE);

		if (!output_sequence_stream.is_open())
		{
			std::cout << "Warning: Output file " + m_folder + "/" + SEQUENCE_ORDER_FILE + " couldn't be opened. Sequence folder names were not written to text file\n";
			return;
		}

		for (const auto& folder : sequence_folders)
		{

			output_sequence_stream << folder.first << "\n" << folder.second << "\n";
		}

	}
}

void MRISession::read_image_names(bool new_read)
{
	m_image_names.reserve(m_sequence_folders.size());

	for (const auto& folder : m_sequence_folders)
	{
		m_image_names.push_back(get_dicom_file_names(m_folder + "/" + folder, new_read));
	}

}

int find_id(const std::string& file, const std::string& entry_id)
{


	/*
	Open image file with ITK
	*/
	typedef unsigned short InputPixelType;
	const unsigned int   InputDimension = 2;
	typedef itk::Image< InputPixelType, InputDimension > InputImageType;

	typedef itk::ImageFileReader< InputImageType > ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(file);

	typedef itk::GDCMImageIO           ImageIOType;
	ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
	reader->SetImageIO(gdcmImageIO);

	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject)
	{
		return -1;
	}


	/*
	Set dictionary attributes
	*/
	typedef itk::MetaDataDictionary   DictionaryType;
	const  DictionaryType & dictionary = gdcmImageIO->GetMetaDataDictionary();
	typedef itk::MetaDataObject< std::string > MetaDataStringType;


	/*
	Set entry id
	*/
	std::string entryId = entry_id;
	DictionaryType::ConstIterator tagItr = dictionary.Find(entryId);


	/*
	Search for entry
	*/
	if (tagItr != dictionary.End())
	{
		MetaDataStringType::ConstPointer entryvalue =
			dynamic_cast<const MetaDataStringType *>(
				tagItr->second.GetPointer());

		if (entryvalue)
		{
			try {

				int id = std::stoi(entryvalue->GetMetaDataObjectValue());
				return id;
			}
			catch (std::invalid_argument())
			{
				return -1;
			}
		}

	}

	return -1;

}


std::vector<std::string> get_dicom_file_names(const std::string & folder, bool new_read)
{
	std::vector<std::string> image_names;

	std::ifstream image_input_stream;
	image_input_stream.open(folder + "/" + IMAGE_ORDER_FILE);


	if (image_input_stream.is_open() && !new_read)
	{
		while (!image_input_stream.eof())
		{
			std::string image_name;
			std::getline(image_input_stream, image_name);
			if (image_name != "")
			{
				image_names.push_back(image_name);
			}
		}
	}
	else
	{
		auto file_names = get_file_names(folder, ".dcm");


		if (file_names.size() == 0) return image_names;

		std::vector<std::pair<int, std::string>> name_id_pairs;

		for (const auto& image_name : file_names)
		{
			std::string image_path = folder + "/" + image_name;
			

			int id = find_id(image_path, IMAGE_ENTRY_ID);

			if (id >= 0)
			{
				name_id_pairs.push_back(std::make_pair(id, image_name));
			}
		}

		std::sort(name_id_pairs.begin(), name_id_pairs.end());


		for (const auto& image : name_id_pairs)
		{
			image_names.push_back(image.second);
		}

		
			std::ofstream output_image_stream;
			output_image_stream.open(folder + "/" + IMAGE_ORDER_FILE);

			if (!output_image_stream.is_open())
			{
				std::cout << "Error: Output file " + folder + "/" + IMAGE_ORDER_FILE + " couldn't be opened for writing";
			}
			else for (const auto& image_name : image_names)
			{
				output_image_stream << image_name << "\n";
			}
		
	}


	return image_names;

}
