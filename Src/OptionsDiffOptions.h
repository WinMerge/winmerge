#pragma once

struct DIFFOPTIONS;
class COptionsMgr;

namespace Options { namespace DiffOptions {

void SetDefaults(COptionsMgr *pOptionsMgr);
void Load(const COptionsMgr *pOptionsMgr, DIFFOPTIONS& options);
void Save(COptionsMgr *pOptionsMgr, const DIFFOPTIONS& options);

}}
