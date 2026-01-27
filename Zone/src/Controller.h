#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace Steinberg {
namespace Vst {

class Controller : public EditController
{
public:
	Controller ();

	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	tresult PLUGIN_API setComponentState (IBStream* state) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*) { return (IEditController*)new Controller (); }
	static FUID cid;
};

} // namespace Vst
} // namespace Steinberg
