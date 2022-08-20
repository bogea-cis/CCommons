// CStrings.cpp: implementation of the CStrings class.
//
//////////////////////////////////////////////////////////////////////

#include "CStrings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrings::CStrings()
{
	resetIterator();
	m_memoryBlockLen = DEFAULT_BUFFER_MEMORY_BLOCK_SIZE;
	m_buffer = NULL;
	m_unpackedAsciiBuffer = NULL;
	m_bufferLen = 0;
	m_allocatedLen = 0;
	adjustInternalAllocation(1);
}

CStrings::~CStrings()
{
	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
	}

	if (m_unpackedAsciiBuffer)
	{
		delete[] m_unpackedAsciiBuffer;
		m_unpackedAsciiBuffer = NULL;
	}

}

unsigned char CStrings::at (int index)
{
	return m_buffer[index];
}

void CStrings::resetIterator ()
{
	m_iteratorIdx = 0;
}


void CStrings::adjustInternalAllocation (int aditionalSize)
{
	//verifica se o tamanho atual + o que vai ser 
	//adicionado eh maior que o alocado
	if (m_bufferLen + aditionalSize >= m_allocatedLen)
	{
		//precisa alocar mais
		
		//aloca aux
		unsigned char* aux = NULL;
		if (m_bufferLen > 0)
		{
			aux = new unsigned char[m_bufferLen];

			//copia conteudo para aux
			memcpy (aux, m_buffer, m_bufferLen);
		}

		//limpa buffer atual
		freeInternalAllocation ();

		//aloca mais um bloco de memoria
		m_allocatedLen = (m_memoryBlockLen * ((m_bufferLen + aditionalSize) / m_memoryBlockLen + 1));
		m_buffer = new unsigned char [m_allocatedLen + 1];
		memset (m_buffer, '\0', m_allocatedLen + 1);

		//copia de aux
		if (aux)
		{
			memcpy (m_buffer, aux, m_bufferLen);

			//destroi aux
			delete[] aux;
			aux = NULL;
		}
	}
}

void CStrings::freeInternalAllocation ()
{
	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
		m_allocatedLen = 0;
	}
}

void CStrings::clear ()
{
	m_bufferLen = 0;
	m_buffer[0] = '\0';
}

void CStrings::append (const unsigned char* buffer, int len)
{
	//verifica se precisa alocar mais memoria
	adjustInternalAllocation(len);
	
	memcpy (&m_buffer[m_bufferLen], buffer, len);
	m_bufferLen += len;
	m_buffer[m_bufferLen] = '\0';
}

void CStrings::append (const char* buffer)
{
	append( (const unsigned char*)buffer, strlen(buffer) );
}

void CStrings::append (const char cData)
{
	append( (const unsigned char*) &cData, 1 );
}

void CStrings::append (const CStrings& buffer)
{
	append(buffer.m_buffer, buffer.m_bufferLen);
}

void CStrings::append (unsigned char value)
{
	append ((unsigned char*)&value, sizeof (unsigned char));
}

int CStrings::length ()
{
	return m_bufferLen;
}

const char* CStrings::getAsciiBuffer ()
{
	return (const char*) m_buffer;
}


// int CStrings::indexOf (int startIndex, const unsigned char* buffer, int bufferLen)
// {
// 	int ret = -1;

// 	//verifica se nao ultrapassa os limites
// 	if (startIndex + bufferLen <= m_bufferLen)
// 	{
// 		for (int i = startIndex; i <= m_bufferLen - bufferLen; i++)
// 		{
// 			if (memcmp(&m_buffer[i], buffer, bufferLen) == 0)
// 			{
// 				ret = i;
// 				break;
// 			}
// 		}
// 	}

// 	return ret;
// }

// CStrings::CStrings(unsigned int memoryBlockLen)
// {
// 	resetIterator();
// 	m_memoryBlockLen = memoryBlockLen;
// 	m_buffer = NULL;
// 	m_unpackedAsciiBuffer = NULL;
// 	m_bufferLen = 0;
// 	m_allocatedLen = 0;
// 	adjustInternalAllocation(1);
// }

// CStrings::CStrings(const CStrings& buffer)
// {
// 	resetIterator();
// 	m_memoryBlockLen = buffer.m_memoryBlockLen;
// 	m_buffer = NULL;
// 	m_unpackedAsciiBuffer = NULL;
// 	m_bufferLen = 0;
// 	m_allocatedLen = 0;
// 	adjustInternalAllocation(1);

// 	setBuffer (buffer.m_buffer, buffer.m_bufferLen);
// }

// CStrings::CStrings(const char* buffer)
// {
// 	resetIterator();
// 	m_memoryBlockLen = DEFAULT_BUFFER_MEMORY_BLOCK_SIZE;
// 	m_buffer = NULL;
// 	m_unpackedAsciiBuffer = NULL;
// 	m_bufferLen = 0;
// 	m_allocatedLen = 0;
// 	adjustInternalAllocation(1);

// 	setBuffer (buffer);
// }

// CStrings::CStrings(const unsigned char* buffer, int len)
// {
// 	resetIterator();
// 	m_memoryBlockLen = DEFAULT_BUFFER_MEMORY_BLOCK_SIZE;
// 	m_buffer = NULL;
// 	m_unpackedAsciiBuffer = NULL;
// 	m_bufferLen = 0;
// 	m_allocatedLen = 0;
// 	adjustInternalAllocation(1);

// 	setBuffer (buffer, len);
// }

unsigned char CStrings::operator[] (int index)
{
	return at(index);
}

bool CStrings::operator==(const CStrings& otherBuffer)
{
	return compare(otherBuffer) == 0;
}

bool CStrings::operator!=(const CStrings& otherBuffer)
{
	return compare(otherBuffer) != 0;
}

CStrings& CStrings::operator=(const CStrings& otherBuffer)
{
	if (*this != otherBuffer)
	{
		setBuffer(otherBuffer.m_buffer, otherBuffer.m_bufferLen);
	}
	return *this;
}

CStrings& CStrings::operator=(const char* str)
{
	setBuffer(str);
	return *this;
}



// int CStrings::replace (const unsigned char* old, int len, const unsigned char* newData, int newLen)
// {
// 	int ret = 0;
	
// 	if (old == NULL || 
// 		len <= 0)
// 	{
// 		return 0;
// 	}
	
// 	if (newLen == 0 || newData == NULL)
// 	{
// 		return this->removeFromBuffer(old, len);
// 	}
	
// 	bool exit = false;
// 	int idx = 0;

// 	while (!exit)
// 	{
// 		idx = indexOf (0, old, len);

// 		if (idx >= 0)
// 		{
// 			memcpy (&m_buffer[idx], &m_buffer[idx + len], m_bufferLen - (len + idx));
// 			m_buffer[m_bufferLen - len] = 0;
// 			m_bufferLen  -= len;
			
// 			this->insert(idx, newData, newLen);
// 			ret ++;
// 		}
// 		else
// 		{
// 			exit = true;
// 		}
// 	}

// 	return ret;	
	
// }

// int CStrings::replace (const char* old, const char* newData)
// {
// 	return this->replace((const unsigned char*) old, 
// 							strlen (old), 
// 							(const unsigned char*) newData, 
// 							(newData) ? strlen(newData) : 0);
// }

void CStrings::updateBuffer (int startIndex, const unsigned char* buffer, int len)
{
	if (startIndex + len > m_bufferLen)
	{
		adjustInternalAllocation((startIndex + len) - m_bufferLen);
		m_bufferLen = startIndex + len;
	}
	memcpy (&m_buffer[startIndex], buffer, len);
}

void CStrings::setBuffer (const char* buffer)
{
	setBuffer((const unsigned char*)buffer, strlen(buffer));
}

void CStrings::setBuffer (const unsigned char* buffer, int len)
{
	clear();

	if (len > m_bufferLen)
	{
		adjustInternalAllocation(len);
	}

	memcpy (m_buffer, buffer, len);
	m_bufferLen = len;
	m_buffer[m_bufferLen] = '\0';
}

// void CStrings::setBuffer (const unsigned char fillByte, int len)
// {
// 	clear();
// 	adjustInternalAllocation(len);
// 	memset (m_buffer, fillByte, len);
// 	m_bufferLen = len;
// 	m_buffer[m_bufferLen] = '\0';
// }

void CStrings::setBuffer (CStrings& buffer)
{
	setBuffer(buffer.m_buffer, buffer.m_bufferLen);
}

// void CStrings::setBufferFormatted (const char* format, ...)
// {
// 	va_list args;
// 	va_start( args, format );
	
// 	int needed = vsnprintf(NULL, 0, format, args) + 1;
// 	char* buf = new char[needed + 1];
// 	memset (buf, 0, needed + 1);
// 	vsnprintf(buf, needed, format, args);
	
// 	setBuffer(buf);
// 	delete[] buf;

// 	va_end(args);
// }

const unsigned char* CStrings::getBuffer ()
{
	return m_buffer;
}

// const char* CStrings::getAsciiUnpackedBuffer()
// {
// 	if (m_unpackedAsciiBuffer)
// 	{
// 		delete[] m_unpackedAsciiBuffer;
// 		m_unpackedAsciiBuffer = NULL;
// 	}

// 	if (m_bufferLen > 0)
// 	{
		
// 		CStrings buf;
// 		unpack(buf);
		
// 		const char* pbuf = buf.getAsciiBuffer();
// 		if (pbuf)
// 		{
// 			int len = strlen(pbuf);
// 			m_unpackedAsciiBuffer = new char[len + 1];
			
// 			//copia o \0 tb
// 			memcpy (m_unpackedAsciiBuffer, pbuf, len + 1);
// 		}
// 	}
// 	else
// 	{
// 		return "";
// 	}
// 	return m_unpackedAsciiBuffer;
// }

// void CStrings::insert (int idx, CStrings& buffer)
// {
// 	insert (idx, buffer.m_buffer, buffer.m_bufferLen);
// }

// void CStrings::insert (int idx, const char* buffer)
// {
// 	insert (idx, (unsigned char*)buffer, strlen(buffer));
// }

// void CStrings::insert (int idx, const unsigned char* buffer, int len)
// {
// 	if (idx == 0)
// 	{
// 		CStrings b;
// 		b.append(buffer, len);
// 		b.append(*this);
// 		this->setBuffer(b);
// 	}
// 	else if (idx > 0)
// 	{
// 		CStrings b;

// 		b.append(this->m_buffer, idx);
// 		b.append(buffer, len);
// 		b.append(&this->m_buffer[idx], this->m_bufferLen - idx);
		
// 		this->setBuffer(b);
// 	}
// }

// void CStrings::insertFormatted (int idx, const char* format, ...)
// {
// 	va_list args;
// 	va_start( args, format );
	
// 	int needed = vsnprintf(NULL, 0, format, args) + 1;
// 	char* buf = new char[needed + 1];
// 	memset (buf, 0, needed + 1);
// 	vsnprintf(buf, needed, format, args);
	
// 	insert(idx, buf);
// 	delete[] buf;

// 	va_end(args);
// }

// int CStrings::indexOf(CStrings& buffer)
// {
// 	return indexOf(0, buffer.m_buffer, buffer.m_bufferLen);
// }

// int CStrings::indexOf(const char* str)
// {
// 	return indexOf(0, (const unsigned char*) str, strlen (str));
// }

// int CStrings::indexOf(char ch)
// {
// 	char str[2] = {0};
// 	str[0] = ch;
// 	return indexOf (str);
// }

// int CStrings::removeFromBuffer(CStrings& buffer)
// {
// 	return removeFromBuffer(buffer.m_buffer, buffer.m_bufferLen);
// }

// int CStrings::removeFromBuffer(const char* buffer)
// {
// 	return removeFromBuffer((const unsigned char*)buffer, strlen (buffer));
// }

// int CStrings::removeFromBuffer(char ch)
// {
// 	char str[2] = {0};
// 	str[0] = ch;
// 	return removeFromBuffer(str);
// }


// int CStrings::removeFromBuffer(const unsigned char* buffer, int bufferLen)
// {
// 	bool exit = false;
// 	int ret = 0;
// 	int idx = 0;

// 	while (!exit)
// 	{
// 		idx = indexOf (0, buffer, bufferLen);

// 		if (idx >= 0)
// 		{
// 			memcpy (&m_buffer[idx], &m_buffer[idx + bufferLen], m_bufferLen - (bufferLen + idx));
// 			m_buffer[m_bufferLen - bufferLen] = 0;
// 			m_bufferLen  -= bufferLen;
// 			ret ++;
// 		}
// 		else
// 		{
// 			exit = true;
// 		}
// 	}

// 	return ret;
// }

// bool CStrings::removeFromBuffer(int idx, int len)
// {
// 	bool ret = false;
	
// 	if (idx < 0 ||
// 		len <= 0 ||
// 		(idx + len > m_bufferLen))
// 	{
// 		return false;
// 	}
// 	else if (idx == 0 && len == m_bufferLen)
// 	{
// 		this->clear();
// 		return true;
// 	}
	
// 	memcpy (&m_buffer[idx], &m_buffer[idx + len], m_bufferLen - (idx + len));
// 	m_bufferLen = m_bufferLen - len;
// 	m_buffer[m_bufferLen] = 0;
	
// 	return ret;
// }


// void CStrings::trim (const unsigned char character)
// {
// 	this->lTrim(character);
// 	this->rTrim(character);
// }

// void CStrings::lTrim (const unsigned char character)
// {
// 	int i;

// 	if (m_buffer[0] != character)
// 	{
// 		return;
// 	}

// 	//pega inicio
// 	for (i = 0 ; (m_buffer[i] == character) && (i < m_bufferLen); i++)
// 	{}
	
// 	if (i == m_bufferLen)
// 	{
// 		//apenas com espaço
// 		this->clear ();
// 	}
// 	else if (i > 0)
// 	{
// 		CStrings aux(&m_buffer[i], m_bufferLen - i);
// 		this->setBuffer(aux);
// 	}
// }

// void CStrings::rTrim (const unsigned char character)
// {
// 	int i;

// 	if (m_buffer[m_bufferLen - 1] != character)
// 	{
// 		return;
// 	}

// 	for (i = m_bufferLen - 1 ; (m_buffer[i] == character) && (i >= 0); i--)
// 	{}
	
// 	if (i < 0)
// 	{
// 		//apenas com espaço
// 		this->clear ();
// 	}
// 	else if (i >= 0)
// 	{
// 		m_bufferLen = i + 1;
// 		m_buffer[i + 1] = 0;
// 	}
// }

// int CStrings::allocatedLen ()
// {
// 	return m_allocatedLen;
// }

int CStrings::compare (const CStrings& buffer)
{
	int max = (m_bufferLen > buffer.m_bufferLen) ? m_bufferLen : buffer.m_bufferLen;
	return memcmp(m_buffer, buffer.m_buffer, max);
}

// int CStrings::compare (const CStrings& buffer, int len)
// {
// 	return memcmp(m_buffer, buffer.m_buffer, len);
// }

// int CStrings::compare (const unsigned char* buffer, int len)
// {
// 	return memcmp(m_buffer, buffer, len);
// }

// void CStrings::append (int minLen, bool rightAlignment, char padChar, const char* value)
// {
// 	if (value == NULL)
// 	{
// 		value = "";
// 	}

// 	int vlen = strlen (value);
// 	//int size = (minLen > vlen) ? minLen : vlen;

// 	CStrings b;

// 	if (rightAlignment)
// 	{
// 		if (minLen > vlen)
// 		{
// 			b.append(padChar, minLen - vlen);
// 		}
// 		b.append(value);
// 	}
// 	else
// 	{
// 		b.append(value);
// 		if (minLen > vlen)
// 		{
// 			b.append (padChar, minLen - vlen);
// 		}
// 	}
	
// 	this->append(b);
// }

// void CStrings::append (unsigned long long value)
// {
// 	append ((unsigned char*)&value, sizeof (unsigned long long));
// }

// void CStrings::append (long long value)
// {
// 	append ((unsigned char*)&value, sizeof (long long));
// }

// void CStrings::append (unsigned long value)
// {
// 	append ((unsigned char*)&value, sizeof (unsigned long));
// }

// void CStrings::append (long value)
// {
// 	append ((unsigned char*)&value, sizeof (unsigned long));
// }

// void CStrings::append (unsigned int value)
// {
// 	append ((unsigned char*)&value, sizeof (unsigned int));
// }

// void CStrings::append (int value)
// {
// 	append ((unsigned char*)&value, sizeof (int));
// }

// void CStrings::append (unsigned short value)
// {
// 	append ((unsigned char*)&value, sizeof (unsigned short));
// }

// void CStrings::append (short value)
// {
// 	append ((unsigned char*)&value, sizeof (short));
// }

// void CStrings::append (char value)
// {
// 	append ((unsigned char*)&value, sizeof (char));
// }

// void CStrings::append (bool value)
// {
// 	append ((unsigned char*)&value, sizeof (bool));
// }

// void CStrings::append (unsigned char fillByte, int len)
// {
// 	if (len > 0)
// 	{
// 		unsigned char* buf = new unsigned char[len];
// 		memset (buf, fillByte, len);
// 		append(buf, len);
// 		delete[] buf;
// 	}
// }

// void CStrings::appendFormatted (const char* format, ...)
// {
// 	va_list args;
// 	va_start( args, format );
	
// 	int needed = vsnprintf(NULL, 0, format, args) + 1;
// 	char* buf = new char[needed + 1];
// 	memset (buf, 0, needed + 1);
// 	vsnprintf(buf, needed, format, args);
	
// 	append(buf);
// 	delete[] buf;

// 	va_end(args);
// }

// void CStrings::setInteratorIndex (int index)
// {
// 	if (index >=0 &&
// 		index < m_bufferLen)
// 	{
// 		m_iteratorIdx = index;
// 	}
// }

// int CStrings::getIteratorIndex ()
// {
// 	return m_iteratorIdx;
// }

// void CStrings::getNext (CStrings& output)
// {
// 	int len = m_bufferLen - m_iteratorIdx;
// 	subBuffer(m_iteratorIdx, len, output);
// 	m_iteratorIdx += len;
// }

// void CStrings::getNext (int len, CStrings& output)
// {
// 	subBuffer(m_iteratorIdx, len, output);
// 	m_iteratorIdx += len;
// }

// void CStrings::getNext (int len, unsigned char* output, int allocatedLen)
// {
// 	subBuffer (m_iteratorIdx, len, output, allocatedLen);
// 	m_iteratorIdx += len;
// }

// void CStrings::getNext (unsigned long long& output)
// {
// 	getNext (sizeof(unsigned long long), (unsigned char*)&output, sizeof(unsigned long long));
// }

// void CStrings::getNext (long long& output)
// {
// 	getNext (sizeof(long long), (unsigned char*)&output, sizeof(long long));
// }

// void CStrings::getNext (unsigned long& output)
// {
// 	getNext (sizeof(unsigned long), (unsigned char*)&output, sizeof(unsigned long));
// }

// void CStrings::getNext (long& output)
// {
// 	getNext (sizeof(long), (unsigned char*)&output, sizeof(long));
// }

// void CStrings::getNext (unsigned int& output)
// {
// 	getNext (sizeof(unsigned int), (unsigned char*)&output, sizeof(unsigned int));
// }

// void CStrings::getNext (int& output)
// {
// 	getNext (sizeof(int), (unsigned char*)&output, sizeof(int));
// }

// void CStrings::getNext (unsigned short& output)
// {
// 	getNext (sizeof(unsigned short), (unsigned char*)&output, sizeof(unsigned short));
// }

// void CStrings::getNext (short& output)
// {
// 	getNext (sizeof(short), (unsigned char*)&output, sizeof(short));
// }

// void CStrings::getNext (unsigned char& output)
// {
// 	getNext (sizeof(unsigned char), (unsigned char*)&output, sizeof(unsigned char));
// }

// void CStrings::getNext (char& output)
// {
// 	getNext (sizeof(char), (unsigned char*)&output, sizeof(char));
// }

// void CStrings::getNext (bool& output)
// {
// 	getNext (sizeof(bool), (unsigned char*)&output, sizeof(bool));
// }

// void CStrings::setDefaultMemoryBlockLength (int len)
// {
// 	m_memoryBlockLen = len;
	
// 	CStrings aux(len);
// 	setBuffer(aux);
// }

// int CStrings::getDefaultMemoryBlockLength ()
// {
// 	return m_memoryBlockLen;
// }


// void CStrings::subBuffer (int startIndex, CStrings& output)
// {
// 	subBuffer (startIndex, m_bufferLen - startIndex, output);
// }

// void CStrings::subBuffer (int startIndex, int len, CStrings& output)
// {
// 	output.clear();

// 	if (startIndex + len <= m_bufferLen)
// 	{
// 		output.setBuffer(&m_buffer[startIndex], len);
// 	}
// }

// void CStrings::subBuffer (int startIndex, int len, unsigned char* output, int allocatedLen)
// {
// 	if (startIndex + len <= m_bufferLen)
// 	{
// 		if (output &&
// 			allocatedLen >= len)
// 		{
// 			memcpy (output, &m_buffer[startIndex], len);
// 		}
// 	}
// }

// void CStrings::subBuffer (int startIndex, unsigned long& output)
// {
// 	subBuffer (startIndex,  sizeof (unsigned long), (unsigned char*)&output, sizeof (unsigned long));
// }

// void CStrings::subBuffer (int startIndex, long& output)
// {
// 	subBuffer (startIndex,  sizeof (long), (unsigned char*)&output, sizeof (long));
// }

// void CStrings::subBuffer (int startIndex, unsigned int& output)
// {
// 	subBuffer (startIndex,  sizeof (unsigned int), (unsigned char*)&output, sizeof (unsigned int));
// }

// void CStrings::subBuffer (int startIndex, int& output)
// {
// 	subBuffer (startIndex,  sizeof (int), (unsigned char*)&output, sizeof (int));
// }

// void CStrings::subBuffer (int startIndex, unsigned short& output)
// {
// 	subBuffer (startIndex,  sizeof (unsigned short), (unsigned char*)&output, sizeof (unsigned short));
// }

// void CStrings::subBuffer (int startIndex, short& output)
// {
// 	subBuffer (startIndex,  sizeof (short), (unsigned char*)&output, sizeof (short));
// }

// void CStrings::subBuffer (int startIndex, unsigned char& output)
// {
// 	subBuffer (startIndex, sizeof (unsigned char), (unsigned char*)&output, sizeof (unsigned char));
// }

// void CStrings::subBuffer (int startIndex, char& output)
// {
// 	subBuffer (startIndex, sizeof (char), (unsigned char*)&output, sizeof (char));
// }

// void CStrings::subBuffer (int startIndex, bool& output)
// {
// 	subBuffer (startIndex, sizeof (bool), (unsigned char*)&output, sizeof (bool));
// }

// int CStrings::bufferLen (const unsigned char* buffer, const unsigned char* terminator, int terminatorLen)
// {
// 	const unsigned char* p = buffer;

// 	if (p == NULL)
// 	{
// 		return -1;
// 	}

// 	for (; ; p++)
// 	{
// 		if (memcmp(terminator, p, terminatorLen) == 0)
// 		{
// 			break;
// 		}
// 	}

// 	return p - buffer;
// }

// void CStrings::bcdPack ()
// {
// 	CStrings packedBuffer;
// 	this->bcdPack(packedBuffer);
	
// 	this->setBuffer(packedBuffer);
// }

// void CStrings::bcdPack (CStrings& packedBuffer)
// {
// 	unsigned char packed;
// 	packedBuffer.clear();

// 	//Percorre o vetor de dados de entrada, para comprimí-los
// 	for( int i = 0; i < m_bufferLen; i += 2)
// 	{
// 		m_buffer[i] = toupper(m_buffer[i]);
// 		m_buffer[i+1] = toupper(m_buffer[i+1]);
		
// 		//Ex: 0011 1010 (A)
// 		int byteA = m_buffer[ i ];
// 		//Ex: 0011 1011 (B)
// 		int byteB= m_buffer[ i + 1 ];

// 		//Ex: 0000 1010 (A)
// 		byteA= byteA & 0x0F;

// 		//Ex: 0000 1011 (B)
// 		byteB= byteB & 0x0F;

// 		//Ex: 1010 0000 (A)
// 		byteA= byteA << 4;

// 		packed = 0;
// 		packed |= byteA;
// 		packed |= byteB;

// 		packedBuffer.append (packed);
// 	}

// }


// void CStrings::bcdUnpack ()
// {
// 	CStrings unpackedBuffer;
// 	this->bcdUnpack(unpackedBuffer);
	
// 	this->setBuffer(unpackedBuffer);
// }

// void CStrings::bcdUnpack (CStrings& unpackedBuffer)
// {
//     unsigned char al;
//     unsigned char ba;

// 	unpackedBuffer.clear();
   
//     for (int i = 0; i < m_bufferLen; i++ )
//     { 	
//         al = (m_buffer[i] & 0xf0); 
//         al = (al >> 4);
//         //if ( al <= 9)
//         al =  al + 0x30;
//         //else
//         //    al = al + 0x37;	
        
//         ba = (m_buffer[i] & 0x0f); 
//         //if ( ba <= 9)
//         ba = ba + 0x30;
//         //else
//         //    ba = ba + 0x37;
        
// 		unpackedBuffer.append((unsigned char*)&al, 1);
// 		unpackedBuffer.append((unsigned char*)&ba, 1);
//     }
// }

void CStrings::pack (CStrings& packedBuffer)
{
	packedBuffer.clear();

	int newBufSize = m_bufferLen / 2;
  unsigned char pBuf[newBufSize];

  for (int ii = 0; ii < newBufSize; ii++)
	{
		unsigned char thisChar;

		thisChar = toupper(m_buffer[(ii*2)]);
		thisChar -= '0';
		if (thisChar > 9) {
			thisChar -= 'A' - ('9'+1);
		}
		pBuf[ii] = thisChar << 4;

		thisChar = toupper(m_buffer[(ii*2 + 1)]);
		thisChar -= '0';
		if (thisChar > 9) {
			thisChar -= 'A' - ('9' + 1);
		}
		pBuf[ii] += thisChar;
	}

	packedBuffer.setBuffer(pBuf, newBufSize);
}

void CStrings::pack ()
{
	CStrings packedBuffer;
	this->pack(packedBuffer);
	
	this->setBuffer(packedBuffer);
}

void CStrings::unpack (CStrings& unpackedBuffer)
{
	unpackedBuffer.clear();
	unsigned char uBuff[m_bufferLen*2];
	int idxOut = 0;
	for (int i = 0; i < m_bufferLen; i++ )
	{
    uBuff[idxOut] = ( ( m_buffer[i] >> 4 ) & 0x0F ) + '0'; 
    if (uBuff[idxOut] > '9') {
    	uBuff[idxOut] += 'A' - ('9'+1);
    }
    idxOut++;

    uBuff[idxOut] = ( m_buffer[i] & 0x0F ) + '0'; 
    if (uBuff[idxOut] > '9') {
    	uBuff[idxOut] += 'A' - ('9'+1);
    }
    idxOut++;
	}
	unpackedBuffer.setBuffer(uBuff, m_bufferLen*2);
}

 void CStrings::unpack()
 {
 	CStrings unpackedBuffer;
 	this->unpack(unpackedBuffer);
	
 	this->setBuffer(unpackedBuffer);
 }

// void CStrings::pack (const char* ascii, unsigned char* hex)
// {
// 	CStrings b(ascii);
// 	b.pack();
// 	memcpy(hex, b.getBuffer(), b.length());
// }

// void CStrings::unpack (const unsigned char* hex, int len, char* ascii)
// {
// 	CStrings b(hex, len);
// 	memcpy(ascii, b.getAsciiUnpackedBuffer(), b.length() * 2);
// }


// void CStrings::forwardIterator (int n)
// {
// 	m_iteratorIdx += n;
// }

// void CStrings::rewindIterator (int n)
// {
// 	m_iteratorIdx -= n;
// 	if (m_iteratorIdx < 0)
// 	{
// 		m_iteratorIdx = 0;
// 	}
// }

// const unsigned char* CStrings::getBufferFromIterator ()
// {
// 	return &m_buffer[m_iteratorIdx];
// }

// const char* CStrings::getAsciiBufferFromIterator ()
// {
// 	return (const char*)getBufferFromIterator();
// }

// int CStrings::getLengthFromIterator ()
// {
// 	return m_bufferLen - m_iteratorIdx;
// }

int CStrings::str_atoi(int start, int len)
{
	char tmpBuff[len+1];
	memcpy(tmpBuff, &m_buffer[start], len);
	tmpBuff[len] = 0;
	return atoi(tmpBuff);
}
