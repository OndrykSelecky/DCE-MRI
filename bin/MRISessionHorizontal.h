#pragma once
#include "MRISession.h"
#include "MRISequenceHorizontal.h"


class MRISessionHorizontal : public MRISession
{	

public:	

	
	MRISequence get_sequence(unsigned int sequence_number);
	

};

