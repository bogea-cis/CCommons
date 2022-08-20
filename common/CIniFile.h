#ifndef _CINIFILE_H_
#define _CINIFILE_H_

#define MAX_NAME_SIZE 255

class CIniFile
{
public:
  CIniFile( const char* szFileName);
  ~CIniFile();
  void WriteInteger( const char* szSection, const char* szKey, int iValue);
  void WriteFloat( const char* szSection, const char* szKey, float fValue);
  void WriteBoolean( const char* szSection, const char* szKey, bool bValue);
  void WriteString( const char* szSection, const char* szKey, const char* szValue);

  int ReadInteger( const char* szSection, const char* szKey, int iDefaultValue);
  int ReadHexInt( const char* szSection, const char* szKey, int iDefaultValue);
  float ReadFloat( const char* szSection, const char* szKey, float fDefaultValue);
  bool ReadBoolean( const char* szSection, const char* szKey, bool bDefaultValue);
  char* ReadString( const char* szSection, const char* szKey, const char* szDefaultValue);
  char* ReadHexData(  const char* szSection, const char* szKey, const char* szDefaultValue);
  
private:
  char* m_pcszFileName;
  char* m_pcszValue;
  int GetIniKeyString( const char *title, const char *key, char *buf, const char *filename);
  int PutIniKeyString( const char *title, const char *key, const char *val, const char *filename);
  void killSpaces( char* string, int length);
};

#endif  // _CINIFILE_H_
