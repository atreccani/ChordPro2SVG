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
	QSize paint(ChordProParser *chproFile);

private:
	void reinit(void);
	void startline(void);
	void putlyrics(QString scan);
	void putchord(QString name);

private:
	QFont *fontTitle;
	QFont *fontLyrics;

	int xLyrics;
	int xChords;
	int yLyrics;

	int xMax;
	int yMax;
};


#endif // CHORDPROPAINTER_H
