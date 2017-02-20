#include <QFile>
#include <QGuiApplication> 
#include <QPainter>
#include <QPen>
#include <QSvgGenerator>
#include <QTextStream>


// 90 DPI
#define A4_INKSCAPE_WIDTH	744
#define A4_INKSCAPE_HEIGHT 1052

QTextStream cout(stdout);
QTextStream cin(stdin);

class ChordProPainter : public QPainter
{
public:
	void paint(void)
	{
		setPen(Qt::NoPen);



		setPen(QPen(Qt::GlobalColor::red, 2, Qt::DashLine));
		drawRect((QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT)));

		// QRect(int x, int y, int width, int height)
		fillRect(QRect(50, A4_INKSCAPE_HEIGHT - 250, 200, 200), Qt::GlobalColor::blue);

		QPainterPath curve;
		curve.moveTo(0, 0);

		// cubicTo(c1X, c1Y, c2X, c2Y, endPointX, endPointY)
		// Adds a cubic Bezier curve between the current position and the end point(endPointX, endPointY) 
		// with control points specified by(c1X, c1Y) and (c2X, c2Y).
		curve.cubicTo(99, 0, 50, 50, 99, 99);
		curve.cubicTo(0, 99, 50, 50, 0, 0);

		setBrush(Qt::GlobalColor::yellow);
		setPen(Qt::black);

		// Translates the coordinate system by the given offset
		translate(100, A4_INKSCAPE_HEIGHT - 200);
		drawPath(curve);

		// painter.setFont(font);

		resetTransform();

		QFont font;
		setFont(QFont("Arial", 40, QFont::Bold));
		drawText(330, 40, "Title");

		putline(40, 120, QString::fromWCharArray(L"[C]Penso che un sogno [C#dim7]così non ritorni mai [Dm]più"));

	}

	int putline(int x, int y, QString mystr)
	{
		QFont myFont("Calibri", 16, QFont::Weight::Normal);
		QFontMetrics fm(myFont, device());
		int xLyrics = x;
		int xChords = x;
		bool bChord;

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
			} else {
				if (mystr[i] == "[") {
					// start of chord sequence
					bChord = true;
					// align chord coordinate to lyrics 
					xChords = xLyrics;	// advance current position horizontally

					// special character, not to be printed
					continue;
				}
			}

			QString myChar = mystr[i];	// get character to print
			if (bChord) {
				// Print Chord
				drawText(xChords, y -20, myChar);	// output char
				xChords += fm.width(myChar, 1);		// advance current position horizontally
			} else {
				// Print Lyrics
				drawText(xLyrics, y, myChar);		// output char
				xLyrics += fm.width(myChar, 1);		// advance current position horizontally
			}

		}
		return xLyrics;
	}

};

class ChordProFile : public QFile
{
public:
	ChordProFile(const QString &name)
		: QFile(name)
	{
	}

	void load(void) 
	{
		QString line;
		QTextStream in(this);

		// Open the file for reading
		if (!open(QIODevice::ReadOnly)) {
			cout << "Unable to open file" << endl;
		}

		// Assume file is coded in UTF-8
		in.setCodec("UTF-8");

		// Load entire file into a string list
		while (!in.atEnd()) {
			QString line = in.readLine();
			lines.append(line);
		}

		// Close the file
		close();
	}

	void print(void)
	{
		foreach(const QString &line, lines) {
			cout << line << endl;
		}
	}

private:
	QStringList lines;
};


int main(int argc, char *argv[])
{
	// Needed for QFont
	QGuiApplication a(argc, argv);

	// Windows specific: use Code Page 850 (Western Europe) for console writing
	cout.setCodec("CP-850");

	ChordProFile file("../Work/Nel blu dipinto di blu (Domenico Modugno).pro");
	file.load();
	file.print();

	QString path = "../Work/ChordProTest.svg";

	QSvgGenerator generator;
	generator.setFileName(path);

	generator.setSize(QSize(A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT));
	generator.setViewBox(QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT));
	generator.setTitle("ChordPro2SVG Test");
	generator.setDescription("An SVG drawing created by ChordPro2SVG.");

	ChordProPainter painter;
	painter.begin(&generator);
	painter.paint();
	painter.end();

	// get actual size QPaintDevice 
	cout << "generator.width:  " << generator.width()  << endl;
	cout << "generator.height: " << generator.height() << endl;

	// Windows specific pause
	system("pause");

}

