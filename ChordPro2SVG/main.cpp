#include <QFile>
#include <QGuiApplication> 
#include <QPainter>
#include <QPen>
#include <QSvgGenerator>
#include <QTextStream>

#include "ChordProFile.h"

// 90 DPI
#define A4_INKSCAPE_WIDTH	744
#define A4_INKSCAPE_HEIGHT 1052

QTextStream cout(stdout);
QTextStream cin(stdin);

class ChordProPainter : public QPainter
{
public:
	void paint(ChordProFile &chproFile)
	{
		setPen(Qt::NoPen);

		setPen(QPen(Qt::GlobalColor::red, 2, Qt::DashLine));
		drawRect((QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT)));

		setPen(QPen(Qt::GlobalColor::black));
		QFont font;
		setFont(QFont("Arial", 20, QFont::Bold));
		drawText(330, 40, chproFile.title());

		putline(40, 120, QString::fromWCharArray(L"[C]Penso che un sogno [C#dim7]così non ritorni mai [Dm]più"));

	}

	int putline(int x, int y, QString mystr)
	{
		QFont myFont("Calibri", 16, QFont::Weight::Normal);
		QFontMetrics fm(myFont, device());
		int xLyrics = x;
		int xChords = x;
		bool bChord = false;

		setFont(myFont);

		for (unsigned int i = 0; i < mystr.size(); i++)
		{
			if (bChord) {
				if (mystr[i] == "]") {
					// end of chord sequence
					bChord = false;
					// special character, not to be printed
					continue;
				}
			}
			else {
				if (mystr[i] == "[") {
					// start of chord sequence
					bChord = true;
					// align chord coordinate to lyrics 
					xChords = xLyrics;

					// special character, not to be printed
					continue;
				}
			}

			QString myChar = mystr[i];	// get character to print
			if (bChord) {
				// Print Chord
				drawText(xChords, y - 20, myChar);	// output char
				xChords += fm.width(myChar, 1);		// advance current position horizontally
			}
			else {
				// Print Lyrics
				drawText(xLyrics, y, myChar);		// output char
				xLyrics += fm.width(myChar, 1);		// advance current position horizontally
			}

		}
		return xLyrics;
	}

};

int main(int argc, char *argv[])
{
	// Needed for QFont
	QGuiApplication a(argc, argv);

	// Windows specific: use Code Page 850 (Western Europe) for console writing
	cout.setCodec("CP-850");

	// Data files that contain ChordPro data are usually given a file name extension ".cho".
	// Other often used extensions are ".crd", ".chordpro", and ".chopro".

	ChordProFile file("../Work/Nel blu dipinto di blu (Domenico Modugno).pro");
	file.load(cout);
	file.print(cout);
	file.find_metadata();

	QString path = "../Work/ChordProTest.svg";

	QSvgGenerator generator;
	generator.setFileName(path);

	generator.setSize(QSize(A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT));
	generator.setViewBox(QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT));
	generator.setTitle("ChordPro2SVG Test");
	generator.setDescription("An SVG drawing created by ChordPro2SVG.");

	ChordProPainter painter;
	painter.begin(&generator);
	painter.paint(file);
	painter.end();

	// get actual size QPaintDevice 
	cout << "generator.width:  " << generator.width()  << endl;
	cout << "generator.height: " << generator.height() << endl;

	// Windows specific pause
	system("pause");

}

