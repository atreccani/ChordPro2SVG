/****************************************************************************
**
** ChordPro painter.
**
****************************************************************************/
#include <QFileInfo>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QTextStream>

#include "ChordProPainter.h"
#include "ChordProParser.h"

#define DIM(a) (sizeof(a) / sizeof(a[0]))

ChordProPainter::ChordProPainter()
{
	fontLyrics = new QFont("Calibri", 16, QFont::Weight::Normal);
	fmLyrics = new QFontMetrics(*fontLyrics); // , device());
}

void ChordProPainter::paint(ChordProParser *chproFile)
{
	parsed_item_t it;

	QString value;

	setPen(Qt::NoPen);

	setPen(QPen(Qt::GlobalColor::red, 2, Qt::DashLine));

#define A4_INKSCAPE_WIDTH	744
#define A4_INKSCAPE_HEIGHT 1052
	drawRect((QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT)));

	setPen(QPen(Qt::GlobalColor::black));
	QFont font;
	setFont(QFont("Arial", 20, QFont::Bold));
	drawText(330, 40, chproFile->title());

	chproFile->reinit();
	reinit();
	while ((it = chproFile->get(value)) != PARSED_ITEM_NONE) {
		qInfo() << "Id : " << ParserLabel(it);
		qInfo() << "Val: " << value << endl;
		switch (it) {
		case PARSED_ITEM_TEXT:
			for (QChar *cc = value.begin(); cc < value.end(); cc++) {
				if (*cc == '\n') {
					startline();
				}
				else {
					putchar(cc);
				}
			}
			break;
		}
	}
}

void ChordProPainter::reinit()
{
	xLyrics = 40;
	xChords = 40;
	yLyrics = 120;
	bChord = false;

	setFont(*fontLyrics);

}

void ChordProPainter::startline()
{
	xLyrics = 40;
	xChords = 40;
	yLyrics += 40;
	bChord = false;
}

void ChordProPainter::putchar(QChar *scan)
{
	// String made of a single char needed for fontMetrics().width passing
	const QString Ch(*scan);

	if (bChord) {
		if(Ch == ']') {
			// end of chord sequence
			bChord = false;
			// special character, not to be printed
			return;
		}
	}
	else {
		if (Ch == '[') {
			// start of chord sequence
			bChord = true;
			if(xChords < xLyrics) {
				// align chord coordinate to lyrics 
				xChords = xLyrics;
			}
			// special character, not to be printed
			return;
		}
	}

	if (bChord) {
		// Print Chord
		drawText(xChords, yLyrics - 20, Ch);	// output char
		xChords += fontMetrics().width(Ch, 1);		// advance current position horizontally
	}
	else {
		// Print Lyrics
		drawText(xLyrics, yLyrics, Ch);		// output char
		xLyrics += fontMetrics().width(Ch, 1);		// advance current position horizontally
	}
}


