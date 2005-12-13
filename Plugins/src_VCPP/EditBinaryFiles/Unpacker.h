#ifndef Unpacker_h_included
#define Unpacker_h_included

typedef const unsigned short * CWSTR;

bool Unpack(CWSTR srcFilepath, CWSTR destFilepath);
bool Pack(CWSTR srcFilepath, CWSTR destFilepath);

#endif // Unpacker_h_included
