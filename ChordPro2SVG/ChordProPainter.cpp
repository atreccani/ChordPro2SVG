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
		case PARSED_ITEM_NEWLINE:
			startline();
			break;

		case PARSED_ITEM_TEXT:
			putlyrics(value);
			break;
		case PARSED_ITEM_CHORD:
			putchord(value);
			break;
		}
	}
}

void ChordProPainter::reinit()
{
	xLyrics = 40;
	xChords = 40;
	yLyrics = 120;

	setFont(*fontLyrics);

}

void ChordProPainter::startline()
{
	xLyrics = 40;
	xChords = 40;
	yLyrics += 40;
}

void ChordProPainter::putlyrics(QString scan)
{
	// Print Lyrics
	drawText(xLyrics, yLyrics, scan);			// output string
	xLyrics += fontMetrics().width(scan);		// advance current position horizontally
}

void ChordProPainter::putchord(QString name)
{
	if (xChords < xLyrics) {
		// align chord coordinate to lyrics 
		xChords = xLyrics;
	}

	// Print Chord
	drawText(xChords, yLyrics - 20, name);		// output string
	xChords += fontMetrics().width(name);		// advance current position horizontally
}

