#pragma once
#include "MRISession.h"
#include "MRISequenceHorizontal.h"


class MRISessionHorizontal : public MRISession
{	

public:	

	/*
	Default constructor
	*/
	MRISessionHorizontal() = default;


	/*
	Constructor with folder setting
	*/
	MRISessionHorizontal(const std::string& folder) { m_folder = folder; };


	/*
	Returns i-th sequence
	*/
	std::shared_ptr<MRISequenceHorizontal> operator[](int i);


	/*
	Returns i-th sequence in const
	*/
	const std::shared_ptr<MRISequenceHorizontal> operator[](int i) const;


	void read(const MRISession& session) ;

	void read(bool new_read = false) override;

	
	/*
	Getter for names of sequence folders
	*/
	std::vector<std::string> get_folder_names() const { return m_folder_names; }

private:	


	std::vector<std::string> m_image_names;

	std::vector<std::string> m_folder_names;

};

