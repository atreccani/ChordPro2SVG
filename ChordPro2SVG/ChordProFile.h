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
	bool load(QTextStream &cout);
	bool getDirective(const QChar *str, QString &name, QString &value, int *parsed_len);
	void print(QTextStream &cout);
	QString &title(void);

private:
	QString			m_FullFile;
	QString			m_Title;
	QStringList		m_Subtitles;
};



#endif // CHORDPROFILE_H
