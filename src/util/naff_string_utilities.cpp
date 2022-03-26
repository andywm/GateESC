#include "util/naff_string_utilities.h"

/// Provide sprintf with formatting option for floats, provides %f
template<>
const char* NaffSPrintfAutoFormat::ResolveOutput(float&t)
{
	return "%f";
}

/// Provide sprintf with formatting option for int, provides %d
template<>
const char* NaffSPrintfAutoFormat::ResolveOutput(int&t)
{
	return "%d";
}
