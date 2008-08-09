#ifndef BinTrans_h
#define BinTrans_h

#include "simparr.h"

//-------------------------------------------------------------------
class Text2BinTranslator : public SimpleString
{
public:
	int bCompareBin( Text2BinTranslator& tr2, int charmode, int binmode );
	Text2BinTranslator( char* ps );
	static int iIsBytecode( char* src, int len );
	static int iBytes2BytecodeDestLen( char* src, int srclen );
	static int iLengthOfTransToBin( char* src, int srclen );
	static int iCreateBcTranslation( char* dest, char* src, int srclen, int charmode, int binmode );
	static int iTranslateOneBytecode( char* dest, char* src, int srclen, int binmode );
	static int iFindBytePos( char* src, char c );
	static int iTranslateBytesToBC (char* pd, unsigned char* src, int srclen);
private:
	int GetTrans2Bin( SimpleArray<char>& sa, int charmode, int binmode );
};

#endif // BinTrans_h
