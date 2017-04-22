#include "Registration.h"
#include "MRISequenceHorizontal.h"
#include "MRISessionHorizontal.h"
#include "Analysis.h"
#include "windows.h"

#include <chrono>
#include <iostream>
#include <fstream>
/*
std::shared_ptr<MRISequence> read_sequence(const std::string& folder, int sequence_id)
{
	std::shared_ptr<MRISession> session(new MRISessionHorizontal(folder));
	session->read();

	std::shared_ptr<MRISequenceHorizontal> sequence = std::dynamic_pointer_cast<MRISequenceHorizontal>((*session)[sequence_id]);


	try {
		sequence->read(*(std::dynamic_pointer_cast<MRISessionHorizontal>(session)));
		//sequence->read();
	}
	catch (std::invalid_argument& e)
	{
		std::cout << e.what();
		throw std::invalid_argument("Can't read sequence");
	}

	return sequence;
}

std::shared_ptr<MRISequence> read_sequence(const std::string& folder)
{
	std::shared_ptr<MRISequence> sequence(std::make_shared<MRISequence>(folder));
	
	sequence->read();
	
	return sequence;
}
*/

void dicom_to_png(std::vector<std::string> folder, std::vector<int> sequence_id)
{
	
	for (auto i = 0; i < folder.size(); i++)
	{
		auto start = std::chrono::system_clock::now();

		MRISession s(folder[i]);
		s.read();

		auto sequence = s.get_horizontal_sequence(sequence_id[i]);
		sequence.read();

		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << i << " read:" << elapsed.count() << "ms\n";


		start = std::chrono::system_clock::now();

		std::string current_folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/orig/sequence" + std::to_string(i) + "/";
		CreateDirectory(current_folder.c_str(), NULL);
		sequence.write(current_folder, CV_16UC1);

		current_folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/contrast/sequence" + std::to_string(i) + "/";
		CreateDirectory(current_folder.c_str(), NULL);
		sequence.write(current_folder, CV_16UC1);
		sequence.write(current_folder, CV_16UC1, true);

		end = std::chrono::system_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << i << " write:" << elapsed.count() << "ms\n";
	}
}

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

	/*
	std::vector<cv::Point2f> src_points = { cv::Point2f(0,0), cv::Point2f(10,0), cv::Point2f(0,10), cv::Point2f(10,10)};
	std::vector<cv::Point2f> dst_points = { cv::Point2f(0,2), cv::Point2f(9,1), cv::Point2f(0,7), cv::Point2f(9,6) };

	cv::Mat transformation_matrix = cv::getPerspectiveTransform(src_points, dst_points);

	std::cout << transformation_matrix;

	return 0;*/


	std::vector<int> sequence_id{ 35, 60, 44, 70, 55, 70, 55, 35, 29, 58, 56, 82, 55, 79, 35, 57, 45, 69, 35, 59 };

	std::vector<MRISequence> sequences(sequence_id.size());

	//cut roi from sequence
	/*MRISequence seq("D:/Dokumenty/Projects/QIN Breast DCE-MRI/orig_contrast/sequence1/");
	seq.read();

	seq.show("dfd");
	cv::Rect myROI(176, 61, 304-176, 146-61);
	for (auto& i : seq.data())
	{
	i = i(myROI);
	}
	seq.show("dfd");
	seq.write("D:/Dokumenty/Projects/QIN Breast DCE-MRI/orig_contrast/sequence1 – kópia/", 2, true);

	return 0;*/


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


		/*auto str = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/orig_contrast/sequence" + std::to_string(i);
		CreateDirectory(str.c_str(), NULL);
		sequences[i].write(str, 2, true);

		//writing
		features features = detect_features(sequences[i]);
		auto triangle_sequence = registration(sequences[i], features, OPTIMAL_TRIANGULATION, false, true);
		*/

		/*auto str = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/registered_contrast/sequence" + std::to_string(i);
		CreateDirectory(str.c_str(), NULL);
		triangle_sequence.write(str, 2, true);*/
		/*
		str = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/orig/sequence" + std::to_string(i);
		CreateDirectory(str.c_str(), NULL);
		sequences[i].write(str);
		*/
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
		/*average_output << "Sequence " << i << "\n";
		average_output << folder[i] << "\n";
		average_output << sequence_id[i] << "\n";*/


		for (auto diff : max_diff)
		{
			for (auto q : quality)
			{
				for (auto dist : min_distance)
				{

					auto start = std::chrono::system_clock::now();

					output << "max_diff: " << diff << ", quality: " << q << ", min_dist: " << dist << "\n\n";

					features features = detect_features(sequences[i], diff, 500, q, dist);

					auto homography_sequence = registration(sequences[i], features, HOMOGRAPHY, false, false);
					auto triangle_sequence = registration(sequences[i], features, OPTIMAL_TRIANGULATION, false, false);

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
					//average_output.flush();
					//output.flush();

				}

			}

		}

	}



	/*auto triangle_sequence = registration(sequence, OPTIMAL_TRIANGULATION, false, true);
	auto homography_sequence = registration(sequence, HOMOGRAPHY);

	std::vector<MRISequence> sequences{ sequence, triangle_sequence, homography_sequence };

	registration_correlation(sequences);*/
}

int main(int argc, char** argv)
{
	test_20();

	std::cout << "koniec";

	return 0;
}
