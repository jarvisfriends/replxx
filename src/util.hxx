#ifndef REPLXX_UTIL_HXX_INCLUDED
#define REPLXX_UTIL_HXX_INCLUDED 1

#include "replxx.hxx"

namespace replxx {

inline bool isControlChar(char32_t testChar) {
	return (testChar < ' ') ||											// C0 controls
				 (testChar >= 0x7F && testChar <= 0x9F);	// DEL and C1 controls
}
int cleanupCtrl(int c);

void recomputeCharacterWidths( char32_t const* text, char* widths, int charCount );
void calculateScreenPosition( int x, int y, int screenColumns, int charCount, int& xOut, int& yOut );
int calculateColumnPosition( char32_t* buf32, int len );
char const* ansi_color( Replxx::Color );

}

#endif

