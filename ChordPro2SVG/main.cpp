#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QGuiApplication> 
#include <QPainter>
#include <QPen>
#include <QSvgGenerator>
#include <QTextStream>

#include "../Version.h"
#include "ChordProParser.h"

// 90 DPI
#define A4_INKSCAPE_WIDTH	744
#define A4_INKSCAPE_HEIGHT 1052

QTextStream cout(stdout);
QTextStream cin(stdin);

class ChordProPainter : public QPainter
{
public:
	void paint(ChordProParser *chproFile)
	{
		parsed_item_t it;
		QString value;

		setPen(Qt::NoPen);

		setPen(QPen(Qt::GlobalColor::red, 2, Qt::DashLine));
		drawRect((QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT)));

		setPen(QPen(Qt::GlobalColor::black));
		QFont font;
		setFont(QFont("Arial", 20, QFont::Bold));
		drawText(330, 40, chproFile->title());

		chproFile->reinit();
		while ((it = chproFile->get(value)) != PARSED_ITEM_NONE) {
			cout << "Id : " << it << endl;
			cout << "Val: " << value << endl << endl;
			switch (it) {
			case PARSED_ITEM_TEXT:
				putline(40, 120, value);
				break;
			}
		}
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

bool load_file(QFile *file, ChordProParser *parser)
{
	QString line;
	QTextStream in(file);

	/*	To be a valid ChordPro file it should be
	- less than 100k (it's a plain text file!)
	- Contain title directive in first 1k
	*/
	if (file->size() > (100 * 1024)) {
		return false;
	}
	// Open the file for reading
	if (!file->open(QIODevice::ReadOnly)) {
		cout << "Unable to open file" << endl;
	}

	// Assume file is coded in UTF-8
	in.setCodec("UTF-8");

	// Load file only partially for validity test
	*parser = in.read(1024);

	parser->reinit();

	if (!parser->parseTitle()) {
		return false;
	}

	parser->m_fileinfo.setFile(file->fileName());

	// Load the full file into a string
	in.seek(0);
	*parser = in.readAll();

	// Close the file
	file->close();

	return true;
}

void fill_chordpro_file_list(QList <ChordProParser *> &list_ref)
{
	QDir dir("../Work");
	if (!dir.exists())
		qWarning("Cannot find the example directory");

	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Size | QDir::Reversed);

	QFileInfoList list = dir.entryInfoList();

	for (int i = 0; i < list.size(); ++i) {
		QString sName = list.at(i).filePath();
		QFile *file = new QFile(sName);
		ChordProParser *parser = new ChordProParser();

		if (load_file(file, parser)) {
			list_ref.append(parser);
		}
	}
}

int main(int argc, char *argv[])
{
	QList <ChordProParser *> ChordProFileList;

		// Needed for QFont
	QGuiApplication a(argc, argv);
	a.setApplicationName(VERSION_STR_NAME);
	a.setApplicationVersion(VERSION_STR_N3);

	// allow to see app version with the - v/--version option at command line
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.process(a);

	// Windows specific: use Code Page 850 (Western Europe) for console writing
	cout.setCodec("CP-850");

	// Fill a list of valid ChordPro files, each file will also be fully loaded into memory (as a QString)
	fill_chordpro_file_list(ChordProFileList);

	// Print a list of found ChordPro files
	cout << "Found " << ChordProFileList.size() << " ChordPro files:" << endl;
	for (int ii = 0; ii < ChordProFileList.size(); ii++) {
		cout << " - " << ChordProFileList[ii]->title() << endl;
	}
	cout << endl;

	ChordProParser *file = ChordProFileList[0];
	cout << file << endl;

	const QString path = file->m_fileinfo.path() + "/" + file->m_fileinfo.completeBaseName() + ".svg";

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

