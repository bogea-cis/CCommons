
/**
 * @fn      Is64BitOS
 * @brief   Check if if a 64 bit system
 * @param   none
 * @return  TRUE = is a 64 bit system
 */
BOOL Is64BitOS()
{
   BOOL bIs64BitOS = FALSE;

   // We check if the OS is 64 Bit
   typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 

   LPFN_ISWOW64PROCESS
      fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
      GetModuleHandle("kernel32"),"IsWow64Process");

   if (NULL != fnIsWow64Process)
   {
      if (!fnIsWow64Process(GetCurrentProcess(),&bIs64BitOS))
      {
         //error
      }
   }
   return bIs64BitOS;
}
