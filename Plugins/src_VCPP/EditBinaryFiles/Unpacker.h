#ifndef Unpacker_h_included
#define Unpacker_h_included

typedef const unsigned short * CWSTR;

bool Unpack(LPCWSTR srcFilepath, LPCWSTR destFilepath);
bool Pack(LPCWSTR srcFilepath, LPCWSTR destFilepath);

#endif // Unpacker_h_included
