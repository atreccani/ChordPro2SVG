/****************************************************************************
**
** ChordPro file interface.
**
****************************************************************************/
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
	*this = other;
	return *this;
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

void ChordProParser::reinit(void)
{
	m_Pos = begin();
}

parsed_item_t ChordProParser::get(QString &arg)
{
	parsed_item_t eRet;

	if (m_Pos >= end()) {
		return PARSED_ITEM_NONE;
	}

	arg.clear();

	// detect string type depending on first character
	if (isLineBegin() && (*m_Pos == '#')) {
		// Comment started
		getComment(arg);
		return PARSED_ITEM_COMMENT;
	}
	else if (*m_Pos == '{') {
		// Directive started
		eRet = getDirective(arg);
		return eRet;
	}
	else {
		// Normal text
		getText(arg);
		return PARSED_ITEM_TEXT;
	}

	// Nothing left to be parsed
	return PARSED_ITEM_NONE;

}

bool ChordProParser::isLineBegin(void)
{
	if (m_Pos == begin()) {
		// beginning of first  line
		return true;
	}
	return (*(m_Pos - 1) == '\n');
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

		if (isLineBegin() && (*m_Pos == '#')) {
			// Comment starting
			return;
		}
		else if (*m_Pos == '{') {
			// Directive starting
			return;
		}
		arg += *m_Pos;
		m_Pos++;
	}
}
