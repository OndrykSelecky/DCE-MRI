#pragma once
#include <vector>
#include <memory>

class MRISequence;

class MRISession
{

public:
	
	/*
	 default constructor
	*/
	MRISession() = default;


	/*
	Constructor with input folder
	*/
	MRISession(const std::string& folder) { m_folder = folder; };
	

	/*
	access i-th sequence via []
	*/
	std::shared_ptr<MRISequence> operator[](int i) { return m_sequences[i]; }


	/*
	acces i-th sequence in const functions
	*/
	const std::shared_ptr<MRISequence> operator[](int i) const { return m_sequences[i]; }



	/*
	Method for reading sequence list in folder. If txt file with sequence order is found, it is read. Otherwise, in each subfolder of m_folder it is searched for dicom file. 
	If valid dicom image file is found, it's order number in session is found and assigned to sequence. After all subfolders are searched, 
	sequences are sorted according to order number and order is also saved to textfile.
	*/
	virtual void read(bool new_read = false);
		

	/*
	Appends sequences at the end of vector
	*/
	void add_sequence(std::shared_ptr<MRISequence> sequence) { m_sequences.push_back(sequence); };
	


	/*
	clear sequence vector
	*/
	void clear() { m_sequences.clear(); };


	/*
	referrence to data vector
	*/
	const std::vector<std::shared_ptr<MRISequence>>& data() const { return m_sequences; }
	

	/*
	Returns number of sequences
	*/
	size_t get_sequence_count() const { return m_sequences.size(); }	
	

	/*
	Returns folder name
	*/
	std::string get_folder_name() const { return m_folder; }


	/*
	Set folder name
	*/
	void set_folder_name(const std::string& folder_name) { m_folder = folder_name; }
	

protected:
	
	/*
	Stores sequences in session in shared_ptr
	*/
	std::vector<std::shared_ptr<MRISequence>> m_sequences;


	/*
	Session folder
	*/
	std::string m_folder;



	/*
	Return id of sequence in subfolder
	*/
	int find_sequence_id(const std::string& sub_folder);

};

