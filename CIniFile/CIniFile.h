#ifndef _CINIFILE_H_
#define _CINIFILE_H_

#define MAX_NAME_SIZE 255

class CIniFile
{
public:
  CIniFile( char* szFileName);
  ~CIniFile();
  void WriteInteger( char* szSection, char* szKey, int iValue);
  void WriteFloat( char* szSection, char* szKey, float fValue);
  void WriteBoolean( char* szSection, char* szKey, bool bValue);
  void WriteString( char* szSection, char* szKey, char* szValue);

  int ReadInteger( char* szSection, char* szKey, int iDefaultValue);
  float ReadFloat( char* szSection, char* szKey, float fDefaultValue);
  bool ReadBoolean( char* szSection, char* szKey, bool bDefaultValue);
  char* ReadString( char* szSection, char* szKey, char* szDefaultValue);

private:
  char* m_szFileName;
  int GetIniKeyString( char *title, char *key, char *buf, char *filename);
  int PutIniKeyString( char *title, char *key, char *val, char *filename);
};

#endif  // _CINIFILE_H_
