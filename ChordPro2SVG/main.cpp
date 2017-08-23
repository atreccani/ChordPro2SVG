#include <QCommandLineParser>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QGuiApplication> 
#include <QPainter>
#include <QSvgGenerator>
#include <QTextStream>

#include "../Version.h"
#include "ChordProPainter.h"
#include "ChordProParser.h"

// 90 DPI
#define A4_INKSCAPE_WIDTH	744
#define A4_INKSCAPE_HEIGHT 1052

QTextStream cout(stdout);
QTextStream cin(stdin);

// Smart pointer to log file
QScopedPointer<QFile>   logFile;

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

// Message handler to redirect qDebug(), qInfo(), qWarning(), qCritical() and qFatal()
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// Open stream file writes
	QTextStream out(logFile.data());
	// Write the date of recording
	out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
	// By type determine to what level belongs message
	switch (type) {
	case QtInfoMsg:     out << "INF "; break;
	case QtDebugMsg:    out << "DBG "; break;
	case QtWarningMsg:  out << "WRN "; break;
	case QtCriticalMsg: out << "CRT "; break;
	case QtFatalMsg:    out << "FTL "; break;
	}
	// Write to the output category of the message and the message itself
	out << context.category << ": " << msg << endl;
	out.flush();    // Clear the buffered data
}

// Create the svg file for the given song
void createSVG(ChordProParser *act_song)
{
	// Set the destination .svg file name changing extension to the source .pro file name
	const QString path = act_song->m_fileinfo.path() + "/" + act_song->m_fileinfo.completeBaseName() + ".svg";

	QSvgGenerator generator;
	generator.setFileName(path);

	generator.setSize(QSize(A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT));
	generator.setViewBox(QRect(0, 0, A4_INKSCAPE_WIDTH, A4_INKSCAPE_HEIGHT));
	generator.setTitle("ChordPro2SVG Test");
	generator.setDescription("An SVG drawing created by ChordPro2SVG.");

	ChordProPainter painter;
	painter.begin(&generator);
	painter.paint(act_song);
	painter.end();

	// get actual size QPaintDevice 
	cout << "generator.width:  " << generator.width() << endl;
	cout << "generator.height: " << generator.height() << endl;
}


int main(int argc, char *argv[])
{
	QList <ChordProParser *> song_list;

		// Needed for QFont
	QGuiApplication a(argc, argv);
	a.setApplicationName(VERSION_STR_NAME);
	a.setApplicationVersion(VERSION_STR_N3);

	// Set the logging file
	// check which a path to file you use 
	logFile.reset(new QFile("../Work/log.txt"));
	// Open the file logging
	logFile.data()->open(QFile::Append | QFile::Text);
	// Set handler
	qInstallMessageHandler(messageHandler);

	// allow to see app version with the - v/--version option at command line
	QCommandLineParser parser;
	parser.addVersionOption();
	parser.process(a);

	// Windows specific: use Code Page 850 (Western Europe) for console writing
	cout.setCodec("CP-850");

	// Fill a list of valid ChordPro files, each file will also be fully loaded into memory (as a QString)
	fill_chordpro_file_list(song_list);

	// Print a list of found ChordPro files
	cout << "Found " << song_list.size() << " ChordPro files:" << endl;
	for (int ii = 0; ii < song_list.size(); ii++) {
		cout << " - " << song_list[ii]->title() << endl;
	}
	cout << endl;

	ChordProParser *act_song = song_list[0];
	cout << *act_song << endl;

	// Create the svg file of the song
	createSVG(act_song);

	// Windows specific pause
	system("pause");

}

