#include "MRISessionHorizontal.h"
#include "MRISequenceHorizontal.h"
#include "Util.h"



MRISequence MRISessionHorizontal::get_sequence(unsigned int sequence_number)
{
	if (sequence_number >= m_image_names[0].size() )
	{
		throw std::invalid_argument("Error: Sequence number is greater than number of sequences in session\n");
	}

	std::vector<std::string> file_names(m_image_names.size());

	for (auto i = 0; i <  m_image_names.size(); i++)
	{
		file_names[i] = m_sequence_folders[i] + "/" + m_image_names[i][sequence_number];
	}

	MRISequence sequence(m_folder, file_names);
	sequence.read();

	return sequence;
}
