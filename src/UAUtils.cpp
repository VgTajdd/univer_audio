// ========================================================================= //
// Copyright (c) 2021 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// UAUtils.cpp                                                               //
// ========================================================================= //

#include "UAUtils.h"

#include <iostream>

#include <fmod/fmod_errors.h>

namespace univer::audio
{
bool checkErrors( FMOD_RESULT result )
{
	if ( result != FMOD_OK )
	{
		std::cout << "FMOD ERROR: [" << result << "] " << FMOD_ErrorString( result ) << std::endl;
		return true;
	}
	/*std::cout << "FMOD all good" << std::endl;*/
	return false;
}
}
