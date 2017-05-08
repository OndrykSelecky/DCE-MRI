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
	Method for reading list of sequences in folder. If txt file with sequence order is found, it is read. Otherwise, in each subfolder of m_folder it is searched for dicom file. 
	If valid dicom image file is found, it's order number in session is found and assigned to sequence. After all subfolders are searched, 
	sequences are sorted according to order number and order is also saved to textfile.
	new_read -> if true, ignore order in text files and read new oeder from headers
	*/
	int read(bool new_read = false);
		
		
	/*
	get veritcal sequence with by index
	*/
	MRISequence get_sequence(unsigned int sequence_number);


	/*
	get horizontal sequence by index
	*/
	MRISequence get_horizontal_sequence(unsigned int sequence_number);


	/*
	Returns number of sequences
	*/
	size_t get_sequence_count() const { return m_sequence_folders.size();  }
	

	/*
	Returns folder name
	*/
	std::string get_folder_name() const { return m_folder; }


	/*
	Set folder name
	*/
	void set_folder_name(const std::string& folder_name) { m_folder = folder_name; }
	

	/*
	Getter for names of sequence folders
	*/
	std::vector<std::string> get_sequence_folder_names() const { return m_sequence_folders; }

private:
	
	//get order of sequence folders 
	void read_sequence_folders(bool new_read);


	//get order of images in each sequence
	void read_image_names(bool new_read);

	
	/*
	Session folder
	*/
	std::string m_folder;


	/*
	Names of sequence folders, ordered by acquisition time
	*/
	std::vector<std::string> m_sequence_folders;

	
	/*
	Names of images in each folder, ordered by image id
	*/
	std::vector<std::vector<std::string>> m_image_names;

};


std::vector<std::string> get_dicom_file_names(const std::string& folder, bool new_read);


/*
Return id of sequence in subfolder
*/
int find_id(const std::string& file, const std::string& entry_id);