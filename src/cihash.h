#pragma once
#include <QtCore/QThread>
#include <QtCore/QIODevice>
#include <QtCore/QByteArray>
#include <QtCore/QMutex>

#include <cryptopp/cryptlib.h>

class CIHash : public QThread
{
	Q_OBJECT

public:
	CIHash(QObject *parent, CryptoPP::HashTransformation *ht );
	~CIHash();

	void setInput( QIODevice* );
	QByteArray result();

	static CIHash* createMD5(); // Manual copy from source. Not in DLL.
	static CIHash* createSHA1();
	static CIHash* createSHA256();
	static CIHash* createSHA224();
	static CIHash* createSHA384();
	static CIHash* createSHA512();
	/* Not in DLL
	static CIHash* createTiger();
	static CIHash* createWhirlpool();
	static CIHash* createRIPEMD160();
	static CIHash* createRIPEMD320();
	static CIHash* createRIPEMD128();
	static CIHash* createRIPEMD256();
	static CIHash* createCRC32();
	*/

protected:
	void run();

private slots:
	bool calculate();

public slots:
	void stopProcess();

private:
	CryptoPP::HashTransformation *hash;
	QIODevice *input;
	QMutex mutex;
	bool bStop;

	QByteArray digestToBytes();

signals:
	void progressChanged( float );
	void digest( QByteArray );
};
