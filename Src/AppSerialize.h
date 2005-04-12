#ifndef AppSerialize_h_included
#define AppSerialize_h_included

/**
 * @brief  A serialization session for user preferences
 *
 * A session is either a saving or a load session, and has a default section,
 *  for serializing a series of settings from the same section.
 */
class AppSerialize
{
public:
	typedef enum { Save, Load } Direction;

	AppSerialize(Direction direction, LPCTSTR section);

	void SerializeInt(LPCTSTR name, int & value, int defval=0);
	void SerializeInt(LPCTSTR section, LPCTSTR name, int & value, int defval=0);

	void SerializeLong(LPCTSTR name, LONG & value, LONG defval=0);
	void SerializeLong(LPCTSTR section, LPCTSTR name, LONG & value, LONG defval=0);

	void SerializeByte(LPCTSTR name, BYTE & value, BYTE defval=0);
	void SerializeByte(LPCTSTR section, LPCTSTR name, BYTE & value, BYTE defval=0);

	void SerializeTcharArray(LPCTSTR name, LPTSTR value, int valsize, LPCTSTR defval=_T(""));
	void SerializeTcharArray(LPCTSTR section, LPCTSTR name, LPTSTR value, int valsize, LPCTSTR defval=_T(""));

	void SerializeFont(LPCTSTR name, LOGFONT & value);
	void SerializeFont(LPCTSTR section, LPCTSTR name, LOGFONT & value);

private:
	Direction m_direction;
	CString m_section;
};

#endif // AppSerialize_h_included
