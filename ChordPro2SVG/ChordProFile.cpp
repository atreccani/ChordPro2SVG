/****************************************************************************
**
** ChordPro file interface.
**
****************************************************************************/
#include <QFile>
#include <QFileInfo>
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

	// Load file only partially for validity test
	m_FullFile = in.read(1024);

	parserReinit();

	QString dir_name;
	QString dir_value;
	parsed_item_t it;

	while ((it = parserGet(dir_name, dir_value)) != PARSED_ITEM_NONE) {
		switch (it) {
		case PARSED_ITEM_DIRECTIVE:
			/*	Look for metadata directives:
			- {title:text}		or {t:text}		: Define text as the song title.
			- {subtitle:text}	or {st:text}	: Define text as a subtitle. More than one subtitle is possible.
			*/
			if ((dir_name == "t") || (dir_name == "title")) {
				m_Title = dir_value;
			}
			if ((dir_name == "st") || (dir_name == "subtitle")) {
				m_Subtitles += dir_value;
			}
			break;
		default:
			break;
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

QString ChordProFile::fileNameWithExt(const QString ext) const
{
	QFileInfo fileinfo(fileName());
	return (fileinfo.path() + "/" + fileinfo.completeBaseName() + "." + ext);
}

void ChordProFile::print(QTextStream &cout)
{
		cout << m_FullFile << endl;
}

QString &ChordProFile::title()
{
	return m_Title;
}

void ChordProFile::parserReinit(void)
{
	m_ParsePos		= m_FullFile.begin();
	m_ParseStatus	= PARSE_STATUS_TEXT;
	m_ParseText.clear();
	m_ParseNewLine = true;
}

parsed_item_t ChordProFile::parserGet(QString &arg1, QString &arg2)
{
	bool separator_found;

	arg1.clear();
	arg2.clear();

	m_ParseStatus = PARSE_STATUS_TEXT;

	while (m_ParsePos < m_FullFile.end()) {

		switch (m_ParseStatus) {
		case PARSE_STATUS_TEXT:
			if (m_ParseNewLine && (*m_ParsePos == '#')) {
				// Comment started
				m_ParseStatus = PARSE_STATUS_COMMENT;
			} else if (*m_ParsePos == '{') {
				// Directive started
				separator_found	= false;
				m_ParseStatus = PARSE_STATUS_DIRECTIVE;
			} else {
				m_ParseText += *m_ParsePos;
			}
			break;
		case PARSE_STATUS_COMMENT:
			if (*m_ParsePos == '\n') {
				// Comment ended
				m_ParseStatus = PARSE_STATUS_TEXT;
			}
			break;
		case PARSE_STATUS_DIRECTIVE:
			if (!separator_found) {
				// fill arg1 till ':' is reached
				if (*m_ParsePos == ':') {
					separator_found = 1;
				} else {
					arg1 += *m_ParsePos;
				}
			}
			else {
					if (*m_ParsePos == '}') {
						// Directive ended
						m_ParsePos++;
						return PARSED_ITEM_DIRECTIVE;
					} else {
						arg2 += *m_ParsePos;
					}
			}
			break;
		case PARSE_STATUS_SONG_LINE:
			break;
		}
		m_ParseNewLine = (*m_ParsePos == '\n') || (*m_ParsePos == '\r');
		m_ParsePos++;
	}
	// Nothing left to be parsed
	return PARSED_ITEM_NONE;
}
