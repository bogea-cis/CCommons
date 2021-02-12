#ifdef LIBUSB10

#ifndef CUSBPRINTERCOMM_H_
#define CUSBPRINTERCOMM_H_

#include "libusb.h"

class CUSBPrinterComm
{
public:
	CUSBPrinterComm(void);
	virtual ~CUSBPrinterComm(void);

	int initialize();
	int finalize();

	int open(unsigned short vendorId, unsigned short productId);
	int close();

	int send(unsigned char* data, int dataLen);
	int receive(unsigned char* data, int dataLen, unsigned int timeout);

	unsigned char getStatus();
	int getInfo(unsigned char* info, unsigned short* infoLen);

	int getManufactorer(unsigned char* manufactorer, unsigned short* manufactorerLen);
	int getProduct(unsigned char* product, unsigned short* productLen);
	int getSerialNumber(unsigned char* sn, unsigned short* snLen);

	int reset();

	bool statusPaperPresent();
	bool statusSelected();
	bool statusError();

private:
	void disableKernelDriver();
	libusb_device* findDevice(unsigned short vendorId, unsigned short productId);
	int getEndPoints(unsigned char* epOut, unsigned char* epIn);

	void reconnectionCheck();

private:
	bool m_initialized;
	libusb_device** m_devsList;
	libusb_device* m_dev;
	libusb_device_handle* m_handle;
	unsigned short m_interface;
	unsigned char m_epOut;
	unsigned char m_epIn;

	unsigned char m_iManufactorerIdx;
	unsigned char m_iProductIdx;
	unsigned char m_iSerialNumIdx;

	unsigned short m_deviceProductId;
	unsigned short m_deviceVendorId;

	bool m_lastCommunicationOk;
};

#endif /*CUSBPRINTERCOMM_H_*/

#endif /*LIBUSB10*/