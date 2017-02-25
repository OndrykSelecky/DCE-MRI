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


void MRISession::read(bool new_read)
{
	m_sequences.clear();
	
	/*
	set up input stream for reding ordered folder names
	*/
	std::ifstream sequence_input_stream;
	sequence_input_stream.open(m_folder + "/" + "sequences.txt");

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
				m_sequences.push_back(std::make_shared<MRISequence>(sequence_name, sequence_id));
			}
		}
	}

	//else find all subfolders and get id of sequence by opening first dicom file in each folder	
	else
	{		
		std::vector<std::string> sequence_folders = get_folder_names(m_folder);
		
		for (const auto& folder : sequence_folders)
		{
			std::string folder_path = m_folder + "/" + folder;
			int id = find_sequence_id(folder_path);

			if (id >= 0)
			{				
				m_sequences.push_back(std::make_shared<MRISequence>(folder_path, id));
			}
		}

		std::sort(m_sequences.begin(), m_sequences.end(), MRISequence::compare_sequences);


		std::ofstream output_sequence_stream;
		output_sequence_stream.open(m_folder + "/" + "sequences.txt");
				
		if (!output_sequence_stream.is_open())
		{
			throw std::exception(std::string("Error: Output file " + m_folder + "/" + "sequences.txt" + " couldn't be opened").c_str());
		}

		for (const auto& sequence : m_sequences)
		{

			output_sequence_stream << sequence->get_sequence_id() << "\n" << sequence->get_folder_name() << "\n";
		}

	}

}

int MRISession::find_sequence_id(const std::string& sub_folder)
{

	/*
	Find name of first dicom file in folder
	*/
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	std::string file_name = sub_folder + "/";
	
	hFind = FindFirstFile((sub_folder+"/*.dcm").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	else
	{
		file_name += FindFileData.cFileName;
		FindClose(hFind);
	}
	//std::string file_name = sub_folder + "/000000.dcm";							
	

	/*
	Open image file with ITK
	*/
	typedef unsigned short InputPixelType;
	const unsigned int   InputDimension = 2;
	typedef itk::Image< InputPixelType, InputDimension > InputImageType;

	typedef itk::ImageFileReader< InputImageType > ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(file_name);

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
	std::string entryId = "0020|0011";
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

