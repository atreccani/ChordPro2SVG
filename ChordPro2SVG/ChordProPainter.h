/****************************************************************************
**
** ChordPro painter.
**
****************************************************************************/

#ifndef CHORDPROPAINTER_H
#define CHORDPROPAINTER_H

#include "ChordProParser.h"

class ChordProPainter : public QPainter
{
public:
	ChordProPainter();
	void paint(ChordProParser *chproFile);

private:
	void reinit(void);
	void startline(void);
	void putchar(QChar *scan);

private:
	QFont *fontLyrics;
	QFontMetrics *fmLyrics;
	int xLyrics;
	int xChords;
	int yLyrics;
	bool bChord;

};


#endif // CHORDPROPAINTER_H
