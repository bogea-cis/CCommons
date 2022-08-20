#include "CLibrary.h"

#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif
#include <stdio.h>
#include <string.h>

CLibrary::CLibrary(const char* libraryPath)
{
	m_module = NULL;
    this->libraryPath = libraryPath;
#ifdef WINDOWS
	m_module = (void*)LoadLibrary(libraryPath);
#else
	m_module = dlopen(libraryPath, RTLD_LAZY);
	char* err = dlerror();
	if(err &&  strlen(err) > 1)
		printf("Erro ao carregar a biblioteca %s para o driver BBX: %s\n",libraryPath,err);
#endif
}

CLibrary::~CLibrary()
{
	if (isLoaded())
	{
#ifdef WINDOWS
		FreeLibrary((HMODULE)m_module);
#else
		dlclose (m_module);
#endif
		m_module = NULL;
	}
}

bool CLibrary::isLoaded ()
{
	return (m_module != NULL);
}

void* CLibrary::getFunction (const char* functionName)
{
	void* pFunction = NULL;

#ifdef WINDOWS
	pFunction = GetProcAddress((HMODULE)m_module, functionName);
#else
	pFunction = dlsym(m_module, functionName);
	char* err = dlerror();
	if (err != NULL)
	{
		printf("Falha ao carregar a função da biblioteca %s para o driver BBX: %s\n",this->libraryPath,err);
		if (pFunction)
		{
			pFunction = NULL;
		}
	}
#endif
	return pFunction;
}
