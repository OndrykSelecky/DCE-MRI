#include "Registration.h"
#include "MRISequenceHorizontal.h"
#include "MRISessionHorizontal.h"
#include "Analysis.h"
#include "windows.h"
#include <chrono>

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

void dicom_to_png(std::vector<std::string> folder, std::vector<int> sequence_id)
{
	
	for (auto i = 12; i < folder.size(); i++)
	{
		auto start = std::chrono::system_clock::now();

		auto sequence = read_sequence(folder[i], sequence_id[i]);

		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << i << " read:" << elapsed.count() << "ms\n";


		start = std::chrono::system_clock::now();

		std::string current_folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/orig/sequence" + std::to_string(i) + "/";
		CreateDirectory(current_folder.c_str(), NULL);
		sequence->write(current_folder, CV_16UC1);

		current_folder = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/contrast/sequence" + std::to_string(i) + "/";
		CreateDirectory(current_folder.c_str(), NULL);
		sequence->write(current_folder, CV_16UC1);
		sequence->write(current_folder, CV_16UC1, true);

		end = std::chrono::system_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << i << " write:" << elapsed.count() << "ms\n";
	}
}

int main(int argc, char** argv)
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

	dicom_to_png(folder, sequence_id);

	
	auto sequence = read_sequence("D:/Dokumenty/Projects/QIN Breast DCE-MRI/contrast/sequence12/");
	sequence->show("sd");

	auto triangle_sequence = registration(*sequence, OPTIMAL_TRIANGULATION, false, true);

	auto homography_sequence = registration(*sequence, HOMOGRAPHY);

	CreateDirectory("D:/Dokumenty/Projects/QIN Breast DCE-MRI/Sequence3", NULL);
	sequence->write("D:/Dokumenty/Projects/QIN Breast DCE-MRI/Sequence3/", CV_16UC1);
	
	std::vector<std::shared_ptr<MRISequence>> sequences{ sequence, triangle_sequence, homography_sequence };

	registration_correlation(sequences);
	
	return 0;
}
