/****************************************************************************
**
** ChordPro parser.
**
****************************************************************************/
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "ChordProParser.h"

#define DIM(a) (sizeof(a) / sizeof(a[0]))

typedef struct {
	parsed_item_t	id;
	const char		*label_long;
	const char		*label_short;
} directive_define_t;

static const directive_define_t directive_list[] = {

	// Preamble directives
	{ PARSED_ITEM_DIRECTIVE_NEW_SONG		, "new_song"			, "ns" },

	// Metadata directives
	{ PARSED_ITEM_DIRECTIVE_TITLE			, "title"				, "t" },
	{ PARSED_ITEM_DIRECTIVE_SUBTITLE		, "subtitle"			, "st" },

	// Formatting directives
	{ PARSED_ITEM_DIRECTIVE_COMMENT			, "comment"				, "c" },
	{ PARSED_ITEM_DIRECTIVE_COMMENT_ITALIC	, "comment_italic"		, "ci" },
	{ PARSED_ITEM_DIRECTIVE_COMMENT_BOX		, "comment_box"			, "cb" },
	{ PARSED_ITEM_DIRECTIVE_CHORUS_START	, "start_of_chorus"		, "soc" },
	{ PARSED_ITEM_DIRECTIVE_CHORUS_END		, "end_of_chorus"		, "eoc" },
	{ PARSED_ITEM_DIRECTIVE_TAB_START		, "start_of_tab"		, "sot" },
	{ PARSED_ITEM_DIRECTIVE_TAB_END			, "end_of_tab"			, "eot" },
	{ PARSED_ITEM_DIRECTIVE_DEFINE			, "define"				, NULL }
};

QString &ChordProParser::operator=(const QString &other)
{
	return QString::operator=(other);
}

bool ChordProParser::parseTitle(void)
{
	QString dir_value;
	parsed_item_t it;

	while ((it = get(dir_value)) != PARSED_ITEM_NONE) {
		switch (it) {
		case PARSED_ITEM_DIRECTIVE_TITLE:
			m_Title = dir_value;
			break;
		case PARSED_ITEM_DIRECTIVE_SUBTITLE:
			m_Subtitles += dir_value;
			break;
		default:
			break;
		}
	}

	if (m_Title.isNull()) {
		return false;
	}
	return true;
}

QString &ChordProParser::title()
{
	return m_Title;
}

void ChordProParser::parseAll(void)
{
	// Create list of parsed data
	ParsedSongItem song_elem;

	reinit();
	while ((song_elem.id = get(song_elem.value)) != PARSED_ITEM_NONE) {
		m_AllItems << song_elem;
		qInfo() << "Id : " << ParserLabel(song_elem.id);
		qInfo() << "Val: " << song_elem.value << endl;
	}

}

void ChordProParser::removeMultipleSpaces(void)
{
	bool newline_found = false;

	QList<ParsedSongItem>::iterator it = m_AllItems.begin();
	while (it != m_AllItems.end()) {
		if (newline_found) {
			if (
				(it->id == PARSED_ITEM_NEWLINE) || 
				((it->id == PARSED_ITEM_TEXT) && (it->value.trimmed().isEmpty()))	/* nothing but whitespace */
			) {
				QString Log = QString("Removing Id: ") + ParserLabel(it->id) + QString(", Val: ") + it->value;
				// qInfo() << "Val: " << song_elem.value << endl;

				it = m_AllItems.erase(it);
			} else {
				newline_found = false;
				it++;
			}
		}
		else {
			if (it->id == PARSED_ITEM_NEWLINE) {
				newline_found = true;
			}
			it++;
		}
	}
	if (newline_found) {
		m_AllItems.removeLast();
	}
}

QList <ParsedSongItem> &ChordProParser::all()
{
	return m_AllItems;
}

void ChordProParser::reinit(void)
{
	m_Pos = begin();
}

parsed_item_t ChordProParser::get(QString &arg)
{
	if (m_Pos >= end()) {
		return PARSED_ITEM_NONE;
	}

	arg.clear();

	// detect string type depending on first character
	switch (item_starting()) {
	case PARSED_ITEM_NEWLINE:
		m_Pos++;
		return PARSED_ITEM_NEWLINE;

	case PARSED_ITEM_COMMENT:
		// Comment started
		getComment(arg);
		return PARSED_ITEM_COMMENT;

	case PARSED_ITEM_CHORD:
		// Chord started
		getChord(arg);
		return PARSED_ITEM_CHORD;
	
	case PARSED_ITEM_DIRECTIVE_NONE:
		// Directive started
		return getDirective(arg);
	
	default:
		// Normal text
		getText(arg);
		return PARSED_ITEM_TEXT;
	}

}

bool ChordProParser::isLineBegin(void)
{
	if (m_Pos == begin()) {
		// beginning of first  line
		return true;
	}
	return (*(m_Pos - 1) == '\n');
}

parsed_item_t ChordProParser::item_starting()
{
	if (*m_Pos == '\n')						return PARSED_ITEM_NEWLINE;
	if (isLineBegin() && (*m_Pos == '#'))	return PARSED_ITEM_COMMENT;
	if (*m_Pos == '[')						return PARSED_ITEM_CHORD;
	if (*m_Pos == '{')						return PARSED_ITEM_DIRECTIVE_NONE;

	return PARSED_ITEM_NONE;
}

parsed_item_t ChordProParser::parseDirective(QString &label)
{
	// Look for metadata directives entry
	for (int ii = 0; ii < DIM(directive_list); ii++) {
		const directive_define_t *pp = &directive_list[ii];

		// short form
		if (pp->label_short) {
			if (label == pp->label_short) {
				return pp->id;
			}
		}
		// long form
		if (pp->label_long) {
			if (label == pp->label_long) {
				return pp->id;
			}
		}
	}
	return PARSED_ITEM_DIRECTIVE_NONE;
}

void ChordProParser::getComment(QString &arg)
{
	m_Pos++;
	while (m_Pos < end()) {
		if (*m_Pos == '\n') {
			// Comment stops at the end of line
			m_Pos++;	// skip '\n'
			return;
		}
		arg += *m_Pos;
		m_Pos++;
	}
}

void ChordProParser::getChord(QString &arg)
{
	m_Pos++;	// skip '['
	while (m_Pos < end()) {
		if (*m_Pos == ']') {
			// Chord stop when ] is found 
			m_Pos++;	// skip ']'
			return;
		}
		arg += *m_Pos;
		m_Pos++;
	}
}

parsed_item_t ChordProParser::getDirective(QString &arg)
{
	QString directive_name;
	bool separator_found = false;

	m_Pos++;	// skip '{'
	while (m_Pos < end()) {

		if (*m_Pos == '}') {
			// Directive end
			m_Pos++;		// skip '}'
			return parseDirective(directive_name);
		}
		if (separator_found) {
			// fill argument
			arg += *m_Pos;
		}
		else {
			// fill directive name string till ':' is reached
			if (*m_Pos == ':') {
				separator_found = true;
			}
			else {
				directive_name += *m_Pos;
			}
		}
		m_Pos++;
	}

	return PARSED_ITEM_NONE;
}

void ChordProParser::getText(QString &arg)
{
	arg += *m_Pos;
	m_Pos++;

	while (m_Pos < end()) {

		if ( item_starting() != PARSED_ITEM_NONE)  {
			// Something different from normal text is starting
			return;
		}
		arg += *m_Pos;
		m_Pos++;
	}
}

const char *ParserLabel(parsed_item_t it)
{
	switch (it) {
	case PARSED_ITEM_NONE:						return "None";
	case PARSED_ITEM_NEWLINE:					return "Newline";
	case PARSED_ITEM_COMMENT:					return "Comment";
	case PARSED_ITEM_CHORD:						return "Chord";
	case PARSED_ITEM_TEXT:						return "Text";

	// Directives
	case PARSED_ITEM_DIRECTIVE_NONE:			return "Directive - None";
	case PARSED_ITEM_DIRECTIVE_NEW_SONG:		return "Directive - New song";
	case PARSED_ITEM_DIRECTIVE_TITLE:			return "Directive - Title";
	case PARSED_ITEM_DIRECTIVE_SUBTITLE:		return "Directive - Subtitle";
	case PARSED_ITEM_DIRECTIVE_COMMENT:			return "Directive - Comment";
	case PARSED_ITEM_DIRECTIVE_COMMENT_ITALIC:	return "Directive - Comment italic";
	case PARSED_ITEM_DIRECTIVE_COMMENT_BOX:		return "Directive - Comment box";
	case PARSED_ITEM_DIRECTIVE_CHORUS_START:	return "Directive - Chorus start";
	case PARSED_ITEM_DIRECTIVE_CHORUS_END:		return "Directive - Chorus end";
	case PARSED_ITEM_DIRECTIVE_TAB_START:		return "Directive - Tab start";
	case PARSED_ITEM_DIRECTIVE_TAB_END:			return "Directive - Tab end";
	case PARSED_ITEM_DIRECTIVE_DEFINE:			return "Directive - Define";
	}
	return "?";
}
