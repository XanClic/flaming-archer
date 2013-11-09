// The main file of the plugin. It defines a global register variable
// "global_exercise1" that is initialized when the plugin is loaded.
// The initializer of this variable calls the constructor of "exercise1"
// and thus creates a new view in the viewer.
#include <cgv/base/register.h>
#include "exercise1.h"

using namespace cgv::base;

extern cgv::base::object_registration<exercise1> global_exercise1(""); 
