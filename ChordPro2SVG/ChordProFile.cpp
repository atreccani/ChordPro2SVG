/****************************************************************************
**
** ChordPro file interface.
**
****************************************************************************/
#include <QFile>
#include <QTextStream>

#include "ChordProFile.h"

ChordProFile::ChordProFile(const QString &name)	: QFile(name)
{

}

void ChordProFile::load(QTextStream &cout)
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
		m_Lines.append(line);
	}

	// Close the file
	close();
}

bool ChordProFile::get_directive(const QString in, QString &name, QString &value)
{
	/*	Return the text delimited by curly brackets, if present */
	QStringList fields;

	int dir_begin = in.indexOf('{');
	if (dir_begin == -1) {
		return false;
	}

	int dir_end = in.indexOf('}', dir_begin);
	if (dir_end == -1) {
		return false;
	}

	QString dir_body = in.mid(dir_begin + 1, dir_end - dir_begin - 1);

	fields = dir_body.split(":", QString::SkipEmptyParts);

	switch (fields.size()) {
	case 0:
		return false;
	case 1:
		name = fields[0];
		break;
	default:
		name = fields[0];
		value = fields[1];
		break;
	}
	return true;
}

void ChordProFile::find_metadata(void)
{
	/*	Look for metadata directives:
	- {title:text}		or {t:text}		: Define text as the song title.
	- {subtitle:text}	or {st:text}	: Define text as a subtitle. More than one subtitle is possible.
	*/
	QString dir_name;
	QString dir_value;
	foreach(const QString &line, m_Lines) {
		if (get_directive(line, dir_name, dir_value)) {
			if ((dir_name == "t") || (dir_name == "title")) {
				m_Title = dir_value;
			}
			if ((dir_name == "st") || (dir_name == "subtitle")) {
				m_Subtitle = dir_value;
			}
		}
	}
}

void ChordProFile::print(QTextStream &cout)
{
	foreach(const QString &line, m_Lines) {
		cout << line << endl;
	}
}

QString &ChordProFile::title()
{
	return m_Title;
}

