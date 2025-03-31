#ifndef Unpacker_h_included
#define Unpacker_h_included

typedef const unsigned short * CWSTR;

bool Unpack(LPCWSTR srcFilepath, LPCWSTR destFilepath, IDispatch* pDispatch);
bool Pack(LPCWSTR srcFilepath, LPCWSTR destFilepath, IDispatch* pDispatch);

#endif // Unpacker_h_included
