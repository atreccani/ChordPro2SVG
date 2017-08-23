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
	void putlyrics(QString scan);
	void putchord(QString name);

private:
	QFont *fontLyrics;
	QFontMetrics *fmLyrics;
	int xLyrics;
	int xChords;
	int yLyrics;

};


#endif // CHORDPROPAINTER_H
