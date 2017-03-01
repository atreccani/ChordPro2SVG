/****************************************************************************
**
** ChordPro file interface.
**
****************************************************************************/

#ifndef CHORDPROFILE_H
#define CHORDPROFILE_H

typedef enum {
	PARSE_STATUS_MIN = 0,
	PARSE_STATUS_TEXT			= PARSE_STATUS_MIN,
	PARSE_STATUS_COMMENT,
	PARSE_STATUS_DIRECTIVE,
	PARSE_STATUS_SONG_LINE,

	PARSE_STATUS_MAX
} parse_status_t;


typedef enum {
	PARSED_ITEM_MIN = 0,
	PARSED_ITEM_NONE			= PARSED_ITEM_MIN,
	PARSED_ITEM_TEXT,
	PARSED_ITEM_COMMENT,
	PARSED_ITEM_DIRECTIVE,
	PARSED_ITEM_SONG_LINE,

	PARSED_ITEM_MAX
} parsed_item_t;


class ChordProFile : public QFile
{
public:
	ChordProFile(const QString &name);
	bool load(QTextStream &cout);
	QString fileNameWithExt(const QString ext) const;
	void print(QTextStream &cout);
	QString &title(void);
	
	void parserReinit(void);
	parsed_item_t parserGet(QString &arg1, QString &arg2);

private:
	QString			m_FullFile;
	QString			m_Title;
	QStringList		m_Subtitles;

	QChar			*m_ParsePos;
	parse_status_t	m_ParseStatus;
	QString			m_ParseText;
	bool			m_ParseNewLine;
};



#endif // CHORDPROFILE_H
