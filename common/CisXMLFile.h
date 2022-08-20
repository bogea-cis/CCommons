#ifndef _CXMLFILE_H_
#define _CXMLFILE_H_

#define MAX_NAME_SIZE 255

class CXMLFile
{
public:
  CXMLFile( const char* szFileName);
  ~CXMLFile();
  
  
private:
  char* m_pcszFileName;
  char* m_pcszValue;
};

#endif  // _CXMLFILE_H_
