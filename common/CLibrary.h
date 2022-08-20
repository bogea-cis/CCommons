#ifndef CLIBRARY_H_
#define CLIBRARY_H_

class CLibrary
{

public:
	CLibrary(const char* libraryPath);
	virtual ~CLibrary();

	bool isLoaded ();
	
	void* getFunction (const char* functionName);
	
private:	
	void* m_module;
	const char* libraryPath;
};

#endif /*CLIBRARY_H_*/
