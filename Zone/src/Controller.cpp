#include "Controller.h"

FUID Controller::cid (0x87654321, 0x0FEDCBA9, 0x11111111, 0x22222222);

#include "pluginterfaces/base/ibstream.h"

namespace Steinberg {
namespace Vst {

Controller::Controller ()
{
}

tresult PLUGIN_API Controller::initialize (FUnknown* context)
{
	tresult result = EditController::initialize (context);
	if (result == kResultOk)
	{
		// Parameters
	}
	return result;
}

tresult PLUGIN_API Controller::terminate ()
{
	return EditController::terminate ();
}

tresult PLUGIN_API Controller::setComponentState (IBStream* state)
{
	return kResultOk;
}

} // namespace Vst
} // namespace Steinberg
