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


bool ChordProFile::load(QTextStream &cout)
{
	QString line;
	QTextStream in(this);

	/*	To be a valid ChordPro file it should be
		- less than 100k (it's a plain text file!)
		- Contain title directive in first 1k
	*/
	if (size() > (100 * 1024)) {
		return false;
	}
	// Open the file for reading
	if (!open(QIODevice::ReadOnly)) {
		cout << "Unable to open file" << endl;
	}

	// Assume file is coded in UTF-8
	in.setCodec("UTF-8");


	QString FileFirstPart = in.read(1024);

	QString dir_name;
	QString dir_value;

	for(QChar *pChar = FileFirstPart.begin(); pChar < FileFirstPart.end(); pChar++) {
		int parse_len;
		/*	Look for metadata directives:
		- {title:text}		or {t:text}		: Define text as the song title.
		- {subtitle:text}	or {st:text}	: Define text as a subtitle. More than one subtitle is possible.
		*/
		if (getDirective(pChar, dir_name, dir_value, &parse_len)) {
			if ((dir_name == "t") || (dir_name == "title")) {
				m_Title = dir_value;
			}
			if ((dir_name == "st") || (dir_name == "subtitle")) {
				m_Subtitles += dir_value;
			}

		}
	}

	if (m_Title.isNull()) {
		return false;
	}

	// Load the full file into a string
	in.seek(0);
	m_FullFile = in.readAll();

	// Close the file
	close();

	return true;
}


bool ChordProFile::getDirective(const QChar *str, QString &name, QString &value, int *parsed_len)
{
	// Example of directive: {t:Nel blu dipinto di blu}

	const QChar * const sBegin = str;
	int status = 0;
	if (*str != '{') {
		*parsed_len = 0;
		return false;
	}
	str++;
	while (!str->isNull()) {
		switch (status) {
		case 0:
			// fill name string till ':' reached
			if (*str == ':') {
				status = 1;
			} else {
				name += *str;
			}
			break;
		case 1:
			// fill value string till '}' reached
			if (*str == '}') {
				*parsed_len = str - sBegin;
				return true;
			}
			else {
				value += *str;
			}
			break;
		}
		str++;
	}

	*parsed_len = str - sBegin;
	return false;

}

void ChordProFile::print(QTextStream &cout)
{
		cout << m_FullFile << endl;
}

QString &ChordProFile::title()
{
	return m_Title;
}

