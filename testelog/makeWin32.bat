del obj-win32\*.o

C:\TDM-GCC-64\bin\g++ -c  common\CBasicString.cpp      	-Icommon -Iinclude -o obj-win32\CBasicString.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\CBasicStringList.cpp  	-Icommon -Iinclude -o obj-win32\CBasicStringList.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\CBuffer.cpp           	-Icommon -Iinclude -o obj-win32\CBuffer.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\CFileUtils.cpp         -Icommon -Iinclude -o obj-win32\CFileUtils.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\CIndexedStringTable.cpp         -Icommon -Iinclude -o obj-win32\CIndexedStringTable.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\clogger.cpp           	-Icommon -Iinclude -o obj-win32\clogger.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\CMutex.cpp            	-Icommon -Iinclude -o obj-win32\CMutex.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c  common\CTimeUtils.cpp        	-Icommon -Iinclude -o obj-win32\CTimeUtils.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0

C:\TDM-GCC-64\bin\g++ -c source\testelog.cpp     	    -Icommon -Iinclude -o obj-win32\testelog.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0
C:\TDM-GCC-64\bin\g++ -c source\CObjectContainer.cpp   	-Icommon -Iinclude -o obj-win32\CObjectContainer.o -D__WINDOWS__ -DLIBUSB10  -pthread -Wall -g3 -O0

C:\TDM-GCC-64\bin\g++	-Iinclude -Icommon obj-win32\CBasicString.o obj-win32\CBasicStringList.o obj-win32\CBuffer.o obj-win32\clogger.o obj-win32\CFileUtils.o obj-win32\CIndexedStringTable.o obj-win32\CMutex.o obj-win32\CTimeUtils.o obj-win32\testelog.o obj-win32\CObjectContainer.o    -lpthread   -o Release\testelog.exe -Wall -O0 -fPIC -pthread -Wl,--kill-at

pause
