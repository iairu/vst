#include "public.sdk/source/main/pluginfactory.h"
#include "Processor.h"
#include "Controller.h"

#define PClass Processor
#define CClass Controller

BEGIN_FACTORY_DEF ("ZUN", "http://www.zun.com", "info@zun.com")
	DEF_CLASS2 (INLINE_UID_FROM_FUID(PClass::cid),
				PClass::cid,
				CClass::cid,
				"Zone",
				Vst::PlugType::kInstrument,
				"Synth",
				"ZUN",
				"1.0.0",
				"http://www.zun.com")
END_FACTORY
