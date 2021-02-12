#ifndef CEFTMF_H
#define CEFTMF_H

/* General API Function Type */
#if defined (__CYGWIN__) || defined (linux) || defined (__EMX__)
#   define CEFTMFAPI
#else
#   define CEFTMFAPI APIENTRY
#endif

/* Define AGCEFs... */
#define AGCEFM_HARD 1
#define AGCEFM_SOFT 0


/* Common Errors Codes */

#define CEFTMF_OK							0    /**< Ok. */
#define CEFTMF_ERR							-1   /**< Erro geral. */
#define CEFTMF_ERR_NOTINITIALIZED			-2   /**< Dispositivo nao inicializado. */
#define CEFTMF_ERR_INITIALIZED				-3   /**< Dispositivo ja inicializado. */
#define CEFTMF_ERR_NORESOURCE				-4   /**< Nao ha recursos suficientes no sistema. */
#define CEFTMF_ERR_NOTREADY					-5   /**< Dispositivo nao esta pronto. */
#define CEFTMF_ERR_NOTSUPPORTED				-6   /**< Funcao nao suportada. */
#define CEFTMF_ERR_INVALIDPARAM				-7   /**< Parametro invalido. */
#define CEFTMF_ERR_DISABLED					-8   /**< Dispositivo nao habilitado. */
#define CEFTMF_ERR_TIMEOUT					-9   /**< Time Out. */
#define CEFTMF_ERR_NOPRESENCE				-10  /**< Dispositivo nao responde. */
#define CEFTMF_ERR_DEVICEDOWN				-11  /**< Dispositivo desligado. */
#define CEFTMF_ERR_READERROR				-12  /**< Erro de leitura. */
#define CEFTMF_ERR_CARTRIDGE				-13  /**< Cartucho mal posicionado ou faltando. */
#define CEFTMF_ERR_OUTOFPAPER				-14  /**< Impressora sem papel. */
#define CEFTMF_ERR_POWEROFF					-15  /**< Impressora desligada. */
#define CEFTMF_ERR_IOERROR					-16  /**< Erro de leitura/escrita. */
#define CEFTMF_ERR_JAMMED					-17  /**< Papel enroscado. */
#define CEFTMF_ERR_THERMALHEAD				-18  /**< Aquecimento da cabeca termica. */
#define CEFTMF_ERR_OFFLINE					-19  /**< Impressora em modo OFF-LINE. */
#define CEFTMF_ERR_COVEROPEN				-20  /**< Impressora com a tampa aberta. */
#define CEFTMF_ERR_BUSY						-21  /**< Erro durante a entrada de dados. */
#define CEFTMF_ERR_CRYPTO_FAIL				-22  
#define CEFTMF_ERR_INVALID_PIN				-23  
#define CEFTMF_ERR_NUMBER_OF_DIGITS			-24  
#define CEFTMF_ERR_CARTRIDGE_EMPTY			-25  /**< Cartucho vazio. */
#define CEFTMF_ERR_TAMPER					-26  /**< Deteccao de violacao no modulo criptografico do PIN.*/
#define CEFTMF_ERR_WORKING_KEY_NOT_FOUND	-27  /**< Working key inexistente.*/

typedef struct
{
	char Provider[40];				/* Nome do fornecedor */
	char DeviceName[40]; 		    /* Nome do equipamento utilizado pelo fornecedor */
	char DeviceRelease[40];			/* Versão do equipamento(firmware) em uso*/
	char DateDevRelease[9];			/* Data de versão do equipamento em uso */
	char ReleaseDLL[6];				/* Versão da DLL em uso */
	char DateDLL[9];				/* Data da versão da DLL em uso */
	char DevicePropertyNumber[30];	/* Número do patrimônio sem dígito verificador */
	char DeviceSerialNumber[30];	/* Número serial do dispositivo */  
} CEFTMF_VERSION;

#endif /* CEFTMF_H */