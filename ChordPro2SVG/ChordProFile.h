/****************************************************************************
**
** ChordPro file interface.
**
****************************************************************************/

#ifndef CHORDPROFILE_H
#define CHORDPROFILE_H

class ChordProFile : public QFile
{
public:
	ChordProFile(const QString &name);
	void load(QTextStream &cout);
	bool get_directive(const QString in, QString &name, QString &value);
	void find_metadata(void);
	void print(QTextStream &cout);
	QString &title(void);

private:
	QStringList	m_Lines;
	QString		m_Title;
	QString		m_Subtitle;
};



#endif // CHORDPROFILE_H
