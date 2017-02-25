#pragma once
#include "MRISequence.h"


class MRISessionHorizontal;

class MRISequenceHorizontal : public MRISequence
{

public:

	MRISequenceHorizontal() { m_image_file_name = ""; }

	MRISequenceHorizontal(const std::string& folder, const std::string& image_file_name, int sequence_id = 1);

	MRISequenceHorizontal(const MRISequenceHorizontal& other);

	MRISequenceHorizontal(MRISequenceHorizontal&& other);

	~MRISequenceHorizontal() override {};

	//Assignment operator
	MRISequenceHorizontal& operator=(const MRISequenceHorizontal &other);

	MRISequenceHorizontal& operator=(MRISequenceHorizontal &&other);
	


	void read(const MRISessionHorizontal& session);

private:

	std::string m_image_file_name;
};