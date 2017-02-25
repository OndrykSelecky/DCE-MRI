#include "MyOpenCVDicomReader.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkOpenCVImageBridge.h"
#include "itkImage.h"

//dicom
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"
#include "gdcmGlobal.h"
#include "itkRescaleIntensityImageFilter.h"


cv::Mat MyOpenCVDicomReader::GetImage(const std::string & file_name)
{
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
	catch (itk::ExceptionObject & e)
	{
		std::cerr << "exception in file reader " << std::endl;
		std::cerr << e << std::endl;
		throw std::invalid_argument("Error Reading File\n");
	}
	



	typedef itk::MetaDataDictionary   DictionaryType;

	const  DictionaryType & dictionary = gdcmImageIO->GetMetaDataDictionary();
	
	typedef itk::MetaDataObject< std::string > MetaDataStringType;
	
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
	
	std::string entryId = "0010|0010";
	DictionaryType::ConstIterator tagItr = dictionary.Find(entryId);
	
	if (tagItr != end)
	{
		MetaDataStringType::ConstPointer entryvalue =
			dynamic_cast<const MetaDataStringType *>(
				tagItr->second.GetPointer());
		
		if (entryvalue)
		{
			std::string tagvalue = entryvalue->GetMetaDataObjectValue();
			std::cout << "Patient's Name (" << entryId << ") ";
			std::cout << " is: " << tagvalue.c_str() << std::endl;
		}
		
	}

	
	std::string tagkey = "0008|1050";
	std::string labelId;
	if (itk::GDCMImageIO::GetLabelFromTag(tagkey, labelId))
	{
		std::string value;
		std::cout << labelId << " (" << tagkey << "): ";
		if (gdcmImageIO->GetValueFromTag(tagkey, value))
		{
			std::cout << value;
		}
		else
		{
			std::cout << "(No Value Found in File)";
		}
		std::cout << std::endl;
	}
	else
	{
		std::cerr << "Trying to access inexistant DICOM tag." << std::endl;
	}

	itk::ImageIOBase::IOPixelType pixelType
		= reader->GetImageIO()->GetPixelType();
	itk::ImageIOBase::IOComponentType componentType
		= reader->GetImageIO()->GetComponentType();
	std::cout << "PixelType: " << reader->GetImageIO()
		->GetPixelTypeAsString(pixelType) << std::endl;
	std::cout << "Component Type: " << reader->GetImageIO()
		->GetComponentTypeAsString(componentType) << std::endl;
	

	cv::Mat img = itk::OpenCVImageBridge::ITKImageToCVMat(reader->GetOutput(), false);

	return img;

}
