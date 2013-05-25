#include <windows.h>
#include "UnicodeString.h"

struct DIFFOPTIONS;

namespace Options { namespace DiffOptions {

void SetDefaults();
void Load(DIFFOPTIONS& options);
void Save(const DIFFOPTIONS& options);

}}

