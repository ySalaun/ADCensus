/* Redirect to appropriate config file, depending on compiler */
#ifdef _WIN32
#include "tif_config.vc.h"
#else
#include "tif_config.gcc.h"
#endif
