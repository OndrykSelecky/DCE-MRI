#include "Registration.h"
#include "Analysis.h"
#include "windows.h"

#include <chrono>
#include <iostream>
#include <fstream>


//test code used for mass test
int test_20()
{
	std::vector<std::string> folder
	{
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC01/1.3.6.1.4.1.14519.5.2.1.2103.7010.263376750024833697192683349782/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC01/1.3.6.1.4.1.14519.5.2.1.2103.7010.263376750024833697192683349782/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC05/1.3.6.1.4.1.14519.5.2.1.2103.7010.288733150396082420009675593044/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC05/1.3.6.1.4.1.14519.5.2.1.2103.7010.288733150396082420009675593044/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC06/1.3.6.1.4.1.14519.5.2.1.2103.7010.269874611034344926547684818265/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC06/1.3.6.1.4.1.14519.5.2.1.2103.7010.269874611034344926547684818265/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC08/1.3.6.1.4.1.14519.5.2.1.2103.7010.164856426745930534819081666875/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC08/1.3.6.1.4.1.14519.5.2.1.2103.7010.164856426745930534819081666875/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC10/1.3.6.1.4.1.14519.5.2.1.2103.7010.135953723682765205394176991681/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC10/1.3.6.1.4.1.14519.5.2.1.2103.7010.135953723682765205394176991681/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC12/1.3.6.1.4.1.14519.5.2.1.2103.7010.220022316776056167578399327196/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC12/1.3.6.1.4.1.14519.5.2.1.2103.7010.220022316776056167578399327196/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC13/1.3.6.1.4.1.14519.5.2.1.2103.7010.171129929602230680730669223262/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC13/1.3.6.1.4.1.14519.5.2.1.2103.7010.171129929602230680730669223262/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC14/1.3.6.1.4.1.14519.5.2.1.2103.7010.121064295202185217340387512523/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC14/1.3.6.1.4.1.14519.5.2.1.2103.7010.121064295202185217340387512523/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC15/1.3.6.1.4.1.14519.5.2.1.2103.7010.184346130435126091729065289423/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC15/1.3.6.1.4.1.14519.5.2.1.2103.7010.184346130435126091729065289423/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC16/1.3.6.1.4.1.14519.5.2.1.2103.7010.163218365376500032683258751269/",
		"D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC16/1.3.6.1.4.1.14519.5.2.1.2103.7010.163218365376500032683258751269/"
	};


	std::vector<int> sequence_id{ 35, 60, 44, 70, 55, 70, 55, 35, 29, 58, 56, 82, 55, 79, 35, 57, 45, 69, 35, 59 };

	std::vector<MRISequence> sequences(sequence_id.size());

	
	std::vector<double> max_diff = { 2, 3, 5, 7 };
	std::vector<double> quality = { 0.0005, 0.001, 0.01, 0.05 };
	std::vector<int> min_distance = { 5, 7, 10, 13 };

	for (auto i = 0; i < folder.size(); i++)
	{
		MRISession s(folder[i]);

		auto start = std::chrono::system_clock::now();

		s.read();

		std::cout << "folder " << i << "\n";

		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << i << " write:" << elapsed.count() << "ms\n";

		sequences[i] = s.get_horizontal_sequence(sequence_id[i]);
		sequences[i].read();
				
	}


	std::ofstream output;
	output.open("feature_test.txt");
	if (!output.is_open()) return -1;
	output << "image_id sequence triangle homography\n\n\n";

	std::ofstream average_output;
	average_output.open("average.txt");
	if (!average_output.is_open()) return -1;

	for (auto i = 0; i< sequences.size(); i++)
	{
		output << "Sequence " << i << "\n";
		output << folder[i] << "\n";
		output << sequence_id[i] << "\n";


		for (auto diff : max_diff)
		{
			for (auto q : quality)
			{
				for (auto dist : min_distance)
				{

					auto start = std::chrono::system_clock::now();

					output << "max_diff: " << diff << ", quality: " << q << ", min_dist: " << dist << "\n\n";

					features features = detect_features(sequences[i], diff, 500, q, dist);

					auto homography_sequence = registration(sequences[i], features, HOMOGRAPHY,  false);
					auto triangle_sequence = registration(sequences[i], features, OPTIMAL_TRIANGULATION, false);

					auto averages = registration_correlation(std::vector<MRISequence>{sequences[i], triangle_sequence, homography_sequence}, output);

					average_output << diff << " " << q << " " << dist << "\n";
					for (const auto& a : averages)
					{
						average_output << a << " ";
					}
					average_output << "\n\n";

					auto end = std::chrono::system_clock::now();
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

					std::cout << " transform:" << elapsed.count() << "ms\n";
					

				}

			}

		}

	}

	return 0;
}

/*
example of registration horizontal sequence from session 
session_folder = folder of MRI Session
slice_number = number of slice, which is selected for horizontal sequence from each vertical sequence 
	(for example, if slice_id == 5 -> 5th image is selected from each sequence folder)
registration method = OPTIMAL TRIANGULATION or HOMOGRAPHY
*/
void example_dicom(const std::string& session_folder, int slice_id, int registration_method = OPTIMAL_TRIANGULATION)
{

	//create new session with folder
	MRISession session(session_folder);

	//if it is new read, can take minute or two
	auto sequence_num = session.read();

	//there is no sequence folder in session
	if (sequence_num <= 0)
	{
		std::cerr << "There are no sequences in session\n";
		return;
	}

	MRISequence sequence;
	try
	{
		//extract horizontal sequence with slice_id
		sequence = session.get_horizontal_sequence(slice_id);
	}
	catch (std::invalid_argument e)
	{
		std::cerr << e.what();
		return;
	}

	//read images in memory
	int image_num = sequence.read();

	if (image_num <= 0)
	{
		std::cerr << "There is no image in sequence\n";
		return;
	}

	//detect features from sequence
	features features = detect_features(sequence);

	/*
		registration of sequence
		last parameter -> show each progress step in separate window. After keyboar key is pressed, process continues to next step
	*/
	auto triangle_sequence = registration(sequence, features, registration_method, true);

	//we can export result to png
	//triangle_sequence.write("output_sequence_folder", CV_16UC1);
	
}


void example_png_sequence(const std::string& sequence_folder, int registration_method = OPTIMAL_TRIANGULATION)
{
	
	//create new sequence with folder
	MRISequence sequence(sequence_folder);

	//read images from folder
	int image_num = sequence.read();

	if (image_num <= 0)
	{
		std::cerr << "There is no image in sequence\n";
		return;
	}

	//detect features
	features features = detect_features(sequence);

	//registrate
	auto result_sequence = registration(sequence, features, registration_method, true);

	//we can export result to png
	//triangle_sequence.write("output_sequence_folder", CV_16UC1);
}

int main(int argc, char** argv)
{
	
	example_dicom("CD Content/Data/DICOM_session/1.3.6.1.4.1.14519.5.2.1.2103.7010.269874611034344926547684818265", 24);

	return 0;
}
