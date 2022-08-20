// CStrings.h: interface for the CStrings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBUFFER_H__59F36CBC_9E2F_4948_AD80_ECB036DC37EE__INCLUDED_)
#define AFX_CBUFFER_H__59F36CBC_9E2F_4948_AD80_ECB036DC37EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdarg.h>

#ifdef WIN32
const int DEFAULT_BUFFER_MEMORY_BLOCK_SIZE = 64;
#else
const int DEFAULT_BUFFER_MEMORY_BLOCK_SIZE = 32;
#endif

#ifndef HST_LONGLONG_FORMAT
#ifdef WIN32
#define HST_LONGLONG_FORMAT "%I64d"
#else
#define HST_LONGLONG_FORMAT "%lld"
#endif
#endif

class CStrings  
{
public:
	CStrings();
	virtual ~CStrings();

	virtual unsigned char at (int index);
	virtual void resetIterator ();
	virtual void clear ();
	virtual void append (const unsigned char* buffer, int len);
	virtual void append (const char* buffer);
	virtual void append (const char buffer);
	virtual void append (const CStrings& buffer);
	virtual int length ();
	virtual const char* getAsciiBuffer ();

	// CStrings(unsigned int memoryBlockLen);
	// CStrings(const CStrings& buffer);
	// CStrings(const char* buffer);
	// CStrings(const unsigned char* buffer, int len);

	virtual void updateBuffer (int startIndex, const unsigned char* buffer, int len);

	virtual void setBuffer (const unsigned char* buffer, int len);
	virtual void setBuffer (const char* buffer);
	// virtual void setBuffer (const unsigned char fillByte, int len);
	virtual void setBuffer (CStrings& buffer);
	// virtual void setBufferFormatted (const char* format, ...);

	virtual const unsigned char* getBuffer ();
	// virtual const char* getAsciiUnpackedBuffer();

	// virtual void insert (int idx, CStrings& buffer);
	// virtual void insert (int idx, const char* buffer);
	// virtual void insert (int idx, const unsigned char* buffer, int len);
	// virtual void insertFormatted (int idx, const char* format, ...);

	// virtual int removeFromBuffer(CStrings& buffer);
	// virtual int removeFromBuffer(const char* buffer);
	// virtual int removeFromBuffer(char ch);
	// virtual int removeFromBuffer(const unsigned char* buffer, int bufferLen);
	// virtual bool removeFromBuffer(int idx, int len);

	// virtual int allocatedLen ();

	// virtual void lTrim (const unsigned char character = ' ');
	// virtual void rTrim (const unsigned char character = ' ');
	// virtual void trim (const unsigned char character = ' ');

	virtual int compare (const CStrings& buffer);
	// virtual int compare (const CStrings& buffer, int len);
	// virtual int compare (const unsigned char* buffer, int len);

	// virtual void append (int minLen, bool rightAlignment, char padChar, const char* value);
	// virtual void append (unsigned long long value);
	// virtual void append (long long value);
	// virtual void append (unsigned long value);
	// virtual void append (long value);
	// virtual void append (unsigned int value);
	// virtual void append (int value);
	// virtual void append (unsigned short value);
	// virtual void append (short value);
	virtual void append (unsigned char value);
	// virtual void append (char value);
	// virtual void append (bool value);
	// virtual void append (unsigned char fillByte, int len);
	// virtual void appendFormatted (const char* format, ...);

	// virtual void setDefaultMemoryBlockLength (int len);
	// virtual int getDefaultMemoryBlockLength ();


	// virtual void subBuffer (int startIndex, CStrings& output);
	// virtual void subBuffer (int startIndex, int len, CStrings& output);
	// virtual void subBuffer (int startIndex, int len, unsigned char* output, int allocatedLen);

	// virtual void subBuffer (int startIndex, unsigned long& output);
	// virtual void subBuffer (int startIndex, long& output);

	// virtual void subBuffer (int startIndex, unsigned int& output);
	// virtual void subBuffer (int startIndex, int& output);

	// virtual void subBuffer (int startIndex, unsigned short& output);
	// virtual void subBuffer (int startIndex, short& output);

	// virtual void subBuffer (int startIndex, unsigned char& output);
	// virtual void subBuffer (int startIndex, char& output);

	// virtual void subBuffer (int startIndex, bool& output);


	// virtual void setInteratorIndex (int index);
	// virtual int getIteratorIndex ();

	// virtual void getNext (CStrings& output);
	// virtual void getNext (int len, CStrings& output);
	// virtual void getNext (int len, unsigned char* output, int allocatedLen);
	// virtual void getNext (unsigned long long& output);
	// virtual void getNext (long long& output);
	// virtual void getNext (unsigned long& output);
	// virtual void getNext (long& output);
	// virtual void getNext (unsigned int& output);
	// virtual void getNext (int& output);
	// virtual void getNext (unsigned short& output);
	// virtual void getNext (short& output);
	// virtual void getNext (unsigned char& output);
	// virtual void getNext (char& output);
	// virtual void getNext (bool& output);

	// virtual int indexOf (int startIndex, const unsigned char* buffer, int bufferLen);
	// virtual int indexOf(CStrings& buffer);
	// virtual int indexOf(const char* str);
	// virtual int indexOf(char ch);
	
	// virtual int replace (const unsigned char* old, int len, const unsigned char* newData, int newLen);
	// virtual int replace (const char* old, const char* newData);

	// static int bufferLen (const unsigned char* buffer, const unsigned char* terminator, int terminatorLen);

	// static void pack (const char* ascii, unsigned char* hex);
	// static void unpack (const unsigned char* hex, int len, char* ascii);

	virtual void pack ();
	virtual void pack (CStrings& packedBuffer);
	virtual void unpack ();
	virtual void unpack (CStrings& unpackedBuffer);

	// virtual void bcdPack ();
	// virtual void bcdPack (CStrings& packedBuffer);
	// virtual void bcdUnpack ();
	// virtual void bcdUnpack (CStrings& unpackedBuffer);

	// virtual void forwardIterator (int n);
	// virtual void rewindIterator (int n);

	// virtual const unsigned char* getBufferFromIterator ();
	// virtual const char* getAsciiBufferFromIterator ();
	// virtual int getLengthFromIterator ();


	virtual unsigned char operator[] (int index);
	virtual bool operator==(const CStrings& otherBuffer);
	virtual bool operator!=(const CStrings& otherBuffer);
	virtual CStrings& operator=(const CStrings& otherBuffer);
	virtual CStrings& operator=(const char* str);
	
	virtual int str_atoi(int start, int len);

private:
	void adjustInternalAllocation (int size);
	void freeInternalAllocation ();

private:
	char* m_unpackedAsciiBuffer;
	unsigned char* m_buffer;
	int m_bufferLen;
	int m_memoryBlockLen;
	int m_allocatedLen;
	int m_iteratorIdx;

};
#endif // !defined(AFX_CBUFFER_H__59F36CBC_9E2F_4948_AD80_ECB036DC37EE__INCLUDED_)
