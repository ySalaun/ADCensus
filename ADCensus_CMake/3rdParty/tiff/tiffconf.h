/* Redirect to appropriate config file, depending on compiler */
#ifdef _WIN32
#include "tiffconf.vc.h"
#else
#include "tiffconf.gcc.h"
#endif
