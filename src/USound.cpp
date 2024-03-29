// ========================================================================= //
// Copyright (c) 2023 Agustin Jesus Durand Diaz.                             //
// This code is licensed under the Apache License 2.0.                       //
// USound.cpp                                                                //
// ========================================================================= //

#include "USound.h"

using univer::audio::USound;

USound::USound( const std::string& _name,
				const float _defaultVolumeDB,
				const float _minDistance,
				const float _maxDistance,
				const bool _is3d,
				const bool _isLooping,
				const bool _isStreaming,
				const bool _useBinaryData ) :
	name( _name ),
	defaultVolumedB( _defaultVolumeDB ),
	minDistance( _minDistance ),
	maxDistance( _maxDistance ),
	is3d( _is3d ),
	isLooping( _isLooping ),
	isStreaming( _isStreaming ),
	useBinaryData( _useBinaryData ),
	m_fmodSound( nullptr )
{}

USound::~USound()
{
	m_fmodSound = nullptr;
}
