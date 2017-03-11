/****************************************************************************
**
** ChordPro parser.
**
****************************************************************************/

#ifndef CHORDPROPARSER_H
#define CHORDPROPARSER_H

typedef enum {
	PARSED_ITEM_MIN = 0,
	PARSED_ITEM_NONE			= PARSED_ITEM_MIN,
	PARSED_ITEM_TEXT,
	PARSED_ITEM_COMMENT,

	// Directives
	PARSED_ITEM_DIRECTIVE_NONE,
	PARSED_ITEM_DIRECTIVE_NEW_SONG,
	PARSED_ITEM_DIRECTIVE_TITLE,
	PARSED_ITEM_DIRECTIVE_SUBTITLE,
	PARSED_ITEM_DIRECTIVE_COMMENT,
	PARSED_ITEM_DIRECTIVE_COMMENT_ITALIC,
	PARSED_ITEM_DIRECTIVE_COMMENT_BOX,
	PARSED_ITEM_DIRECTIVE_CHORUS_START,
	PARSED_ITEM_DIRECTIVE_CHORUS_END,
	PARSED_ITEM_DIRECTIVE_TAB_START,
	PARSED_ITEM_DIRECTIVE_TAB_END,
	PARSED_ITEM_DIRECTIVE_DEFINE,

	PARSED_ITEM_MAX
} parsed_item_t;

class ChordProParser : public QString
{
public:
	QString &operator=(const QString &other);
	bool parseTitle(void);
	QString &title();
	void reinit(void);
	parsed_item_t get(QString &arg);

private:
	bool isLineBegin(void);
	parsed_item_t parseDirective(QString &label);
	void getComment(QString &arg);
	parsed_item_t getDirective(QString &arg);
	void getText(QString &arg);

public:
	QFileInfo		m_fileinfo;

private:
	QChar			*m_Pos;
	QString			m_Title;
	QStringList		m_Subtitles;
};

extern const char *ParserLabel(parsed_item_t it);

#endif // CHORDPROPARSER_H
