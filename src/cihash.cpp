#include "cihash.h"

#include <QtCore/QObject>
#include <QtCore/QIODevice>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QMutexLocker>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/sha.h>
//#include <cryptopp/tiger.h>
//#include <cryptopp/whrlpool.h>
//#include <cryptopp/ripemd.h>
#include <cryptopp/crc.h>
#include <cryptopp/md5.h>

#define MAX_READ_SIZE 1024

CIHash::CIHash( QObject *parent, CryptoPP::HashTransformation *ht )
	: QThread( parent ), hash( ht ), bStop( false )
{
	if( !hash )
		hash = new CryptoPP::SHA1();
}

CIHash::~CIHash()
{
	if( hash )
		delete hash;
	if( input )
		delete input;
}

QByteArray CIHash::digestToBytes()
{
	if( !hash )
		return *(new QByteArray());

	// Get the result.
	char *result = (char*)malloc( hash->DigestSize() );
	hash->Final( (CryptoPP::byte*)result );

	// Convert to hex string and return.
	QByteArray bytes( result, hash->DigestSize() );
	delete result;

	return bytes;
}

void CIHash::setInput( QIODevice *dev )
{
	mutex.lock();
	input = dev;
	mutex.unlock();
}

void CIHash::run()
{
	mutex.lock();
	bStop = false;
	calculate();
	mutex.unlock();
}

bool CIHash::calculate()
{
	if( !hash || !input )
		return false;

	// Open input device and initialize stream.
	input->open( QIODevice::ReadOnly );
	QDataStream stream( input );

	// Get (file) size of the input.
	qint64 size = input->size();
	quint64 allread = 0;

	// Create hash processor and read parameters.
	unsigned int optlength = hash->OptimalBlockSize();
	unsigned int passes = MAX_READ_SIZE / optlength;
	unsigned int nread = 0;
	unsigned int pass = 0;
	char *buf = (char*)malloc( optlength );
	// Start reading the input.
	while( !bStop && (nread = stream.readRawData( buf, optlength )) > 0 ) {
		hash->Update( (CryptoPP::byte*)buf, nread );
		allread += nread;
		pass++;
		if( pass >= passes ) {
			emit progressChanged( (float)allread / size );
			pass = 0;
		}
	}
	if( !bStop ) {
		// Be done.
		emit progressChanged( 1.0f );

		// Get result.
		emit digest( digestToBytes() );
	} else {
		emit progressChanged( 0.0f );
	}

	// Close file and release buffer.
	delete buf;
	buf = NULL;
	input->close();

	return true;
}

QByteArray CIHash::result()
{
	return digestToBytes();
}

void CIHash::stopProcess()
{
	bStop = true;
}

CIHash* CIHash::createMD5()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::Weak::MD5 );
	return h;
}

CIHash* CIHash::createSHA1()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::SHA1 );
	return h;
}

CIHash* CIHash::createSHA256()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::SHA256 );
	return h;
}

CIHash* CIHash::createSHA224()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::SHA224 );
	return h;
}

CIHash* CIHash::createSHA384()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::SHA384 );
	return h;
}

CIHash* CIHash::createSHA512()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::SHA512 );
	return h;
}

/*
CIHash* CIHash::createTiger()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::Tiger );
	return h;
}

CIHash* CIHash::createWhirlpool()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::Whirlpool );
	return h;
}

CIHash* CIHash::createRIPEMD160()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::RIPEMD160 );
	return h;
}

CIHash* CIHash::createRIPEMD320()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::RIPEMD320 );
	return h;
}

CIHash* CIHash::createRIPEMD128()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::RIPEMD128 );
	return h;
}

CIHash* CIHash::createRIPEMD256()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::RIPEMD256 );
	return h;
}

CIHash* CIHash::createCRC32()
{
	CIHash *h = new CIHash( NULL, new CryptoPP::CRC32 );
	return h;
}
*/
