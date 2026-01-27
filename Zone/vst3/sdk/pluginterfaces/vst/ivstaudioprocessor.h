//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Interfaces
// Filename    : ivstaudioprocessor.h
// Created by  : Steinberg, 04/2005
// Description : VST Audio Processor Interfaces
//
//-----------------------------------------------------------------------------
// This file is part of a Steinberg SDK. It is subject to the license terms
// in the LICENSE file found in the top-level directory of this distribution
// and at www.steinberg.net/sdklicenses. 
// No part of the SDK, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the LICENSE file.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/vst/vsttypes.h"
#include "pluginterfaces/base/funknown.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Audio processing setup.
\ingroup vst3struct
*/
struct ProcessSetup
{
	SamplePrecision processMode;		///< sample precision
	int32 sampleRate;					///< sample rate
	int32 maxSamplesPerBlock;			///< maximum number of samples per audio block
};

//------------------------------------------------------------------------
/** Audio buffer.
\ingroup vst3struct
*/
struct AudioBusBuffers
{
	int32 numChannels;					///< number of channels in this buffer
	uint64 silenceFlags;				///< each bit is a silence flag for a given channel
	union
	{
		Sample32** channelBuffers32;	///< 32-bit float buffers
		Sample64** channelBuffers64;	///< 64-bit float buffers
	};
};

//------------------------------------------------------------------------
/** Process data used in IAudioProcessor::process.
\ingroup vst3struct
*/
struct ProcessData
{
	int32 numSamples;					///< number of samples to process
	int32 numInputs;					///< number of audio input buses
	AudioBusBuffers* inputs;			///< audio input buffers
	int32 numOutputs;					///< number of audio output buses
	AudioBusBuffers* outputs;			///< audio output buffers
//	IParameterChanges* inputParameterChanges;	///< incoming parameter changes
//	IParameterChanges* outputParameterChanges;	///< outgoing parameter changes
//	IEventList* inputEvents;			///< incoming events
//	IEventList* outputEvents;			///< outgoing events
//	ProcessContext* processContext;		///< processing context
};

//------------------------------------------------------------------------
/** Audio Processor interface.
\ingroup vstIPlug
*/
class IAudioProcessor : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Set the bus arrangements. */
	virtual tresult PLUGIN_API setBusArrangements (SpeakerArrangement* inputs, int32 numIns,
													 SpeakerArrangement* outputs, int32 numOuts) = 0;

	/** Get the bus arrangement for a given direction (input/output) and index. */
	virtual tresult PLUGIN_API getBusArrangement (int32 busDirection, int32 index, SpeakerArrangement& arr) = 0;

	/** Activate or deactivate the processing. */
	virtual tresult PLUGIN_API setupProcessing (ProcessSetup& setup) = 0;

	/** The process call. */
	virtual tresult PLUGIN_API process (ProcessData& data) = 0;

	/** The number of samples to process. */
	virtual uint32 PLUGIN_API getLatencySamples () = 0;

	/** Tail time in samples. */
	virtual uint32 PLUGIN_API getTailSamples () = 0;

	/** Set the processing state. */
	virtual tresult PLUGIN_API setProcessing (TBool state) = 0;

	/** Set the sample rate. */
	virtual tresult PLUGIN_API setSampleRate (ParamValue sampleRate) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IAudioProcessor, 0x4487A24F, 0x31744573, 0x93C4362A, 0x7E12B852)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
