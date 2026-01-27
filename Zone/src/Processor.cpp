#include "Processor.h"

FUID Processor::cid (0x12345678, 0x9ABCDEF0, 0x11111111, 0x22222222);

#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "base/source/fstreamer.h"

namespace Steinberg {
namespace Vst {

Processor::Processor ()
: phase (0.f)
{
	setControllerClass (FUID (0x12345678, 0x9ABCDEF0, 0x12345678, 0x9ABCDEF0));
}

tresult PLUGIN_API Processor::initialize (FUnknown* context)
{
	tresult result = AudioEffect::initialize (context);
	if (result == kResultOk)
	{
		addAudioInput (USTRING("Stereo In"), SpeakerArr::kStereo);
		addAudioOutput (USTRING("Stereo Out"), SpeakerArr::kStereo);
	}
	return result;
}

tresult PLUGIN_API Processor::terminate ()
{
	return AudioEffect::terminate ();
}

tresult PLUGIN_API Processor::setActive (TBool state)
{
	return AudioEffect::setActive (state);
}

tresult PLUGIN_API Processor::process (ProcessData& data)
{
	if (data.numSamples > 0)
	{
		// ZUN-style sawtooth wave
		float freq = 440.f; // A4
		float delta = freq / processSetup.sampleRate;

		for (int32 i = 0; i < data.numSamples; i++)
		{
			float sample = phase * 2.f - 1.f;
			phase += delta;
			if (phase >= 1.f)
				phase -= 1.f;
			
			if (data.outputs[0].channelBuffers32[0])
				data.outputs[0].channelBuffers32[0][i] = sample;
			if (data.outputs[0].channelBuffers32[1])
				data.outputs[0].channelBuffers32[1][i] = sample;
		}
	}
	return kResultOk;
}

} // namespace Vst
} // namespace Steinberg
