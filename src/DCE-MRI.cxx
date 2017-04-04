#include "Registration.h"
#include "MRISequenceHorizontal.h"
#include "MRISessionHorizontal.h"
#include "Analysis.h"


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



int main(int argc, char** argv)
{
	
	std::string folder1 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC01/1.3.6.1.4.1.14519.5.2.1.2103.7010.333899005177230305399406859716";
	std::string folder5 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC05/";
	std::string folder6 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC06/1.3.6.1.4.1.14519.5.2.1.2103.7010.269874611034344926547684818265";
	std::string folder8 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC08/";
	std::string folder10 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC10/1.3.6.1.4.1.14519.5.2.1.2103.7010.634114621738943599785009586807";
	std::string folder12 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC12/";
	std::string folder13 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC13/";
	std::string folder14 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC14/1.3.6.1.4.1.14519.5.2.1.2103.7010.297016782834828170309889288895";
	std::string folder16 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC16/";
	std::string folder18 = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/QIN-Breast-DCE-MRI-BC16/";

	std::string folderorig = "D:/Dokumenty/Projects/QIN Breast DCE-MRI/Sequence_orig";

	auto folder = folder1;
	int sequence_id = 54;


	auto sequence = read_sequence(folderorig);
	
	auto triangle_sequence = registration(*sequence, OPTIMAL_TRIANGULATION);

	auto homography_sequence = registration(*sequence, HOMOGRAPHY);
	
	std::vector<std::shared_ptr<MRISequence>> sequences{ sequence, triangle_sequence, homography_sequence };

	registration_correlation(sequences);

	return 0;
}
