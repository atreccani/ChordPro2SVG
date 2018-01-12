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


#define LINE_SPACING		40

#define DIM(a) (sizeof(a) / sizeof(a[0]))

ChordProPainter::ChordProPainter()
{
	fontTitle  = new QFont("Arial", 14, QFont::Bold);
	fontLyrics = new QFont("Calibri", 11, QFont::Weight::Normal);
}

QSize ChordProPainter::paint(ChordProParser *chproFile)
{
	parsed_item_t it;

	QString value;

	setFont(*fontTitle);
	setPen(QPen(Qt::GlobalColor::black));
	drawText(130, fontMetrics().height(), chproFile->title());

	reinit();

	for (const ParsedSongItem &it : chproFile->all()) {

		switch (it.id) {
		case PARSED_ITEM_NEWLINE:
			startline();
			break;

		case PARSED_ITEM_DIRECTIVE_COMMENT:
		case PARSED_ITEM_DIRECTIVE_COMMENT_ITALIC:
		case PARSED_ITEM_DIRECTIVE_COMMENT_BOX:
			putlyrics("(");
			putlyrics(it.value);
			putlyrics(")");
			break;

		case PARSED_ITEM_TEXT:
			putlyrics(it.value);
			break;

		case PARSED_ITEM_CHORD:
			putchord(it.value);
			break;
		}
	}
	yMax -= LINE_SPACING;

	setPen(QPen(Qt::GlobalColor::red, 2, Qt::DashLine));

	drawRect((QRect(0, 0, xMax, yMax)));

	return QSize(xMax, yMax);
}

void ChordProPainter::reinit()
{
	xLyrics = 0;
	xChords = 0;
	yLyrics = 0;

	xMax = 0;
	yMax = 0;

	setFont(*fontLyrics);
}

void ChordProPainter::startline()
{
	xLyrics = 0;
	xChords = 0;

	yLyrics += LINE_SPACING;

	if (yLyrics > yMax) {
		yMax = yLyrics;
	}
}

void ChordProPainter::putlyrics(QString scan)
{
	// Print Lyrics
	drawText(xLyrics, yLyrics, scan);			// output string
	xLyrics += fontMetrics().width(scan);		// advance current position horizontally

	if (xLyrics > xMax) {
		xMax = xLyrics;
	}
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

	if (xChords > xMax) {
		xMax = xChords;
	}
}

