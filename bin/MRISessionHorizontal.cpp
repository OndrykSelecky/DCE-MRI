#include "MRISessionHorizontal.h"
#include "MRISequenceHorizontal.h"
#include "Util.h"



std::shared_ptr<MRISequenceHorizontal> MRISessionHorizontal::operator[](int i)
{
	return std::dynamic_pointer_cast<MRISequenceHorizontal>(m_sequences[i]);
}

const std::shared_ptr<MRISequenceHorizontal> MRISessionHorizontal::operator[](int i) const
{
	return std::dynamic_pointer_cast<MRISequenceHorizontal>(m_sequences[i]);
}


void MRISessionHorizontal::read(bool new_read)
{
	m_sequences.clear();

	MRISession *session = new MRISession(m_folder);
	session->read(new_read);	
	
	for (const auto& sequence : session->data())
	{
		m_folder_names.push_back(sequence->get_folder_name());
	}

	delete(session);


	std::ifstream image_input_stream;
	image_input_stream.open(m_folder + "/" + "images.txt");

	if (image_input_stream.is_open() && !new_read)
	{
		while (!image_input_stream.eof())
		{		
			std::string image_name;
			std::getline(image_input_stream, image_name);			
			if (image_name != "")
			{
				m_image_names.push_back(image_name);
			}
		}
	}
	else
	{
		m_image_names = get_file_names(m_folder_names[0], ".dcm");

		std::ofstream output_image_stream;
		output_image_stream.open(m_folder + "/" + "images.txt");

		if (!output_image_stream.is_open())
		{
			throw std::exception(std::string("Error: Output file " + m_folder + "/" + "sequences.txt" + " couldn't be opened").c_str());
		}
		for (const auto& image_name : m_image_names)
		{
			output_image_stream << image_name << "\n";
		}
	}
	
	for (int i = 0; i < m_image_names.size(); i++)
	{
		m_sequences.push_back(std::shared_ptr<MRISequence>(new MRISequenceHorizontal(m_folder, m_image_names[i], i)));
	}

}
