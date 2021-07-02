#ifdef LIBUSB01

#include "CUSBPrinterComm-01.h"
#include "CObjectContainer.h"
#include "CTimeUtils.h"
#include "PTRConstants.h"

#define THIS_FILE "CUSBPrinterComm.cpp"

CUSBPrinterComm::CUSBPrinterComm(void)
{
	m_dev = NULL;
	m_handle = NULL;
	m_interface = 0;
	m_epOut = 0;
	m_epIn = 0;

	m_iManufactorerIdx = 0;
	m_iProductIdx = 0;
	m_iSerialNumIdx = 0;

	m_deviceProductId = 0;
	m_deviceVendorId = 0;

	m_lastCommunicationOk = true;
}

CUSBPrinterComm::~CUSBPrinterComm(void)
{
	this->finalize();
}

int CUSBPrinterComm::initialize()
{
	usb_init();

	return UPC_SUCCESS;
}

int CUSBPrinterComm::finalize()
{
	this->close();

	return UPC_SUCCESS;
}

int CUSBPrinterComm::open(unsigned short vendorId, unsigned short productId)
{
	int ret = UPC_ERROR_OTHER;

	CTimeUtils::msleep(100);

	m_dev = this->findDevice(vendorId, productId);
	if (m_dev == NULL)
	{
		return UPC_ERROR_NO_DEVICE;
	}

	m_deviceProductId = productId;
	m_deviceVendorId = vendorId;

	m_handle = usb_open(m_dev);
	if (m_handle != NULL)
	{
		this->disableKernelDriver();

		ret = usb_claim_interface(m_handle, 0);
		if (ret == UPC_SUCCESS)
		{
			ret = this->getEndPoints(&m_epOut, &m_epIn);
			if (ret != UPC_SUCCESS)
			{
				CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "open", P_HIGH, "Libusb error [%d]", ret);
			}
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "open", P_HIGH, "usb_claim_interface error [%d] - %s", ret, usb_strerror());
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "open", P_HIGH, "usb_open error [%d] - %s", ret, usb_strerror());
	}

	return ret;
}

int CUSBPrinterComm::close()
{
	if (m_handle != NULL)
	{
		usb_release_interface(m_handle, 0);

		m_epIn = 0;
		m_epOut = 0;

		usb_close(m_handle);

		m_handle = NULL;
	}

	return UPC_SUCCESS;
}

int CUSBPrinterComm::send(unsigned char* data, int dataLen)
{
	this->reconnectionCheck();

	int ret = 0;
	if (m_handle != NULL && m_epOut != 0)
	{
		int sent = 0;

		sent = usb_bulk_write(m_handle, m_epOut, (char*)data, dataLen, DEFAULT_TIMEOUT);
		if (sent == dataLen)
		{
			return UPC_SUCCESS;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "send", P_HIGH, "usb_bulk_write error [%d] - %s", ret, usb_strerror());
			ret = sent;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "send", P_HIGH, "Error accessing usb connection, is the device really plugged?");
		ret = UPC_ERROR_ACCESS;
	}

	m_lastCommunicationOk = false;
	return ret;
}

int CUSBPrinterComm::receive(unsigned char* data, int dataLen, unsigned int timeout)
{
	this->reconnectionCheck();

	int ret = 0;
	if (m_handle != NULL && m_epIn != 0)
	{
		int readed = 0;

		readed = usb_bulk_read(m_handle, m_epIn, (char*)data, dataLen, timeout);
		if (readed > 0)
		{
			return UPC_SUCCESS;
		}
		else if(readed == 0)
		{
			return UPC_ERROR_TIMEOUT;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "receive", P_HIGH, "usb_bulk_read error [%d] - %s", readed, usb_strerror());
			ret = readed;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "receive", P_HIGH, "Error accessing usb connection, is the device really plugged?");
		ret = UPC_ERROR_ACCESS;
	}

	m_lastCommunicationOk = false;
	return ret;
}

unsigned char CUSBPrinterComm::getStatus()
{
	unsigned char status = 0xFF;

	if (m_handle != NULL)
	{
		int ret = usb_control_msg(m_handle, GET_PORT_STATUS, 1, 0, m_interface, (char*)&status, 1, DEFAULT_TIMEOUT);
		if (ret != 1)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getStatus", P_HIGH, "usb_control_msg error [%d] - %s", ret, usb_strerror());
		}
	}

	return status;
}

int CUSBPrinterComm::getInfo(unsigned char* info, unsigned short* infoLen)
{
	int ret = UPC_ERROR_OTHER;

	if (m_handle != NULL)
	{
		ret = usb_control_msg(m_handle, GET_DEVICE_ID, 0, 0, m_interface, (char*)info, *infoLen, DEFAULT_TIMEOUT);
		if (ret != *infoLen)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getInfo", P_HIGH, "usb_control_msg error [%d] - %s", ret, usb_strerror());
		}
		else
		{
			*infoLen = ret;
			ret = UPC_SUCCESS;
		}
	}

	return ret;
}

int CUSBPrinterComm::getManufactorer(unsigned char* manufactorer, unsigned short* manufactorerLen)
{
	this->reconnectionCheck();

	int ret = UPC_ERROR_OTHER;
	if (m_handle != NULL)
	{
		ret = usb_get_string_simple(m_handle, m_iManufactorerIdx, (char*)manufactorer, *manufactorerLen);
		if (ret > 0)
		{
			*manufactorerLen = ret;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getManufactorer", P_HIGH, "usb_get_string_simple error [%d] - %s", ret, usb_strerror());
			m_lastCommunicationOk = false;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getManufactorer", P_HIGH, "Error accessing usb connection, is the device really plugged?");
		ret = UPC_ERROR_ACCESS;
	}

	return ret;
}

int CUSBPrinterComm::getProduct(unsigned char* product, unsigned short* productLen)
{
	this->reconnectionCheck();

	int ret = 0;
	if (m_handle != NULL)
	{
		ret = usb_get_string_simple(m_handle, m_iProductIdx, (char*)product, *productLen);
		if (ret > 0)
		{
			*productLen = ret;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getProduct", P_HIGH, "usb_get_string_simple error [%d] - %s", ret, usb_strerror());
			m_lastCommunicationOk = false;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getProduct", P_HIGH, "Error accessing usb connection, is the device really plugged?");
		ret = UPC_ERROR_ACCESS;
	}

	return ret;
}

int CUSBPrinterComm::getSerialNumber(unsigned char* sn, unsigned short* snLen)
{
	this->reconnectionCheck();

	int ret = 0;
	if (m_handle != NULL)
	{
		ret = usb_get_string_simple(m_handle, m_iSerialNumIdx, (char*)sn, *snLen);
		if (ret > 0)
		{
			*snLen = ret;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getSerialNumber", P_HIGH, "usb_get_string_simple error [%d] - %s", ret, usb_strerror());
			m_lastCommunicationOk = false;
		}
	}
	else
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "getSerialNumber", P_HIGH, "Error accessing usb connection, is the device really plugged?");
		ret = UPC_ERROR_ACCESS;
	}

	return ret;
}

int CUSBPrinterComm::reset()
{
	int ret = UPC_ERROR_OTHER;

	if (m_handle != NULL)
	{
		ret = usb_control_msg(m_handle, SOFT_RESET, 2, 0, m_interface, NULL, 0, DEFAULT_TIMEOUT);
		if (ret != 0)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "reset", P_HIGH, "usb_get_string_simple error [%d] - %s", ret, usb_strerror());
		}

		ret = usb_reset(m_handle);
		if(ret != UPC_SUCCESS)
		{
			this->close();
			this->open(m_deviceVendorId, m_deviceProductId);
		}
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "reset", P_HIGH, "usb_reset ret [%d] - %s", ret, usb_strerror());

		if (m_handle != NULL &&
			m_epIn != 0 && m_epOut != 0)
		{
			usb_clear_halt(m_handle, m_epIn);
			usb_clear_halt(m_handle, m_epOut);
		}
	}

	return ret;
}

bool CUSBPrinterComm::statusPaperPresent()
{
	unsigned char status = this->getStatus();

	if (status != 0xFF)
	{
		return ((status & 0x20) == 0x00);
	}

	return false;
}

bool CUSBPrinterComm::statusSelected()
{
	unsigned char status = this->getStatus();

	if (status != 0xFF)
	{
		return ((status & 0x10) == 0x10);
	}

	return false;
}

bool CUSBPrinterComm::statusError()
{
	unsigned char status = this->getStatus();

	if (status != 0xFF)
	{
		return ((status & 0x08) == 0x00);
	}

	return true;
}

void CUSBPrinterComm::disableKernelDriver()
{
#ifndef WINDOWS
	char interName[64] = {0};

	int ret = usb_get_driver_np(m_handle, 0, interName, sizeof(interName));
	if (ret == 1)
	{
		usb_detach_kernel_driver_np(m_handle, 0);
	}
#endif
}

struct usb_device* CUSBPrinterComm::findDevice(unsigned short vendorId, unsigned short productId)
{
	struct usb_bus *bus = NULL;
	struct usb_device *dev = NULL;

	usb_find_busses();
	usb_find_devices();

	for (bus = usb_busses; bus; bus = bus->next)
	{
		for (dev = bus->devices; dev; dev = dev->next)
		{
			if (dev->descriptor.idVendor == vendorId &&
				dev->descriptor.idProduct == productId)
			{
				m_iManufactorerIdx = dev->descriptor.iManufacturer;
				m_iProductIdx = dev->descriptor.iProduct;
				m_iSerialNumIdx = dev->descriptor.iSerialNumber;

				return dev;
			}
		}
	}

	return NULL;
}

int CUSBPrinterComm::getEndPoints(unsigned char* epOut, unsigned char* epIn)
{
	if (m_dev != NULL)
	{
		for (int i = 0; i < m_dev->config->bNumInterfaces; i++)
		{
			for (int j = 0; j < m_dev->config->interface[i].num_altsetting; j++)
			{
				if (m_dev->config->interface[i].altsetting[j].bInterfaceClass == LIBUSB_CLASS_PRINTER)
				{
					for (int k = 0; k < m_dev->config->interface[i].altsetting[j].bNumEndpoints; k++)
					{
						unsigned char endpoint = m_dev->config->interface[i].altsetting[j].endpoint[k].bEndpointAddress;

						if ((endpoint & ENDPOINT_DIRECTION_MASK) == ENDPOINT_OUT)
						{
							*epOut = endpoint;
						}
						else
						{
							*epIn = endpoint;
						}

						if (*epOut != 0 && *epIn != 0)
						{
							m_interface = m_dev->config->interface[i].altsetting[j].bInterfaceNumber;

							usb_clear_halt(m_handle, *epIn);
							usb_clear_halt(m_handle, *epOut);

							return UPC_SUCCESS;
						}
					}
				}
			}
		}
	}

	CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "reset", P_HIGH, "Error [%d]", UPC_ERROR_NOT_FOUND);
	return UPC_ERROR_NOT_FOUND;
}

void CUSBPrinterComm::reconnectionCheck()
{
	if(m_lastCommunicationOk == false)
	{
		CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "reconnectionCheck", P_HIGH, "Connection problem, trying to reconnect");
		this->reset();
		this->close();
		CTimeUtils::msleep(250);
		if(this->open(m_deviceVendorId, m_deviceProductId) == UPC_SUCCESS)
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "reconnectionCheck", P_TRACE, "Connection restablished");
			m_lastCommunicationOk = true;
		}
		else
		{
			CObjectContainer::getTraceInstance()->logAscii(THIS_FILE, "reconnectionCheck", P_HIGH, "Error reconnecting");
		}
	}
}

#endif /*LIBUSB01*/
