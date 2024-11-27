#include "mail.h"

// #include <curl/curl.h>
// #include <stdexcept>

CurlHandle::CurlHandle() {
   const auto eCode = curl_global_init(CURL_GLOBAL_ALL);
   if (eCode != CURLE_OK) {
      throw std::runtime_error{"Error initializing libCURL"};
   }
}

CurlHandle::~CurlHandle() { curl_global_cleanup(); }

CurlHandle &CurlHandle::instance() {
   static CurlHandle inst{};
   return inst;
}

// Mail Client

/**
* @file MAILClient.cpp
* @brief implementation of the mail client class
* @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
*/

// Static members initialization
std::string    CMailClient::s_strCertificationAuthorityFile = "curl-ca-bundle.crt";

#ifdef DEBUG_CURL
std::string CMailClient::s_strCurlTraceLogDirectory;
#endif

/**
* @brief constructor for the mail client object
*
* @param Logger - a callabck to a logger function void(const std::string&)
*
*/
CMailClient::CMailClient(LogFnCallback Logger) :
   m_oLog(Logger),
   m_iCurlTimeout(0),
   m_eSettingsFlags(ALL_FLAGS),
   m_eSslTlsFlags(SslTlsFlag::NO_SSLTLS),
   m_pCurlSession(nullptr),   
   m_pRecipientslist(nullptr),
   m_bProgressCallbackSet(false),
   m_bNoSignal(false),
   m_curlHandle(CurlHandle::instance())
{
}

/**
* @brief destructor for the mail client object
*
*/
CMailClient::~CMailClient()
{
   if (m_pCurlSession != nullptr)
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_WARNING_OBJECT_NOT_CLEANED);
      
      CleanupSession();
   }
   if (m_mime != nullptr)
   {
      curl_mime_free(m_mime);
      m_mime = nullptr;
   }
}

/**
 * @brief Starts a new mail session, initializes the cURL API session
 *
 * If a new session was already started, the method has no effect.
 *
 * @param [in] strHost server address with or without port number
 * @param [in] strLogin username
 * @param [in] strPassword password
 * @param [in] eSettingsFlags optional use | operator to choose multiple options
 * @param [in] eSslTlsFlags optional encryption type
 *
 * @retval true   Successfully initialized the session.
 * @retval false  The session is already initialized : call CleanupSession()
 * before initializing a new one or the Curl API is not initialized.
 *
 * Example Usage:
 * @code
 *    pMailClient->InitSession("smtp.gnet.tn:25", "amine", "my_password", ENABLE_LOG);
 * @endcode
 */
const bool CMailClient::InitSession(const std::string& strHost, const std::string& strLogin,
                                    const std::string& strPassword,
                                    const SettingsFlag& eSettingsFlags /* = ALL_FLAGS */,
                                    const SslTlsFlag& eSslTlsFlags     /* = NO_SSLTLS */)
{
   if (strHost.empty())
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_ERROR_EMPTY_HOST_MSG);
      
      return false;
   }

   if (m_pCurlSession)
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_ERROR_CURL_ALREADY_INIT_MSG);
      
      return false;
   }
   m_pCurlSession = curl_easy_init();

   m_eSettingsFlags = eSettingsFlags;
   m_eSslTlsFlags = eSslTlsFlags;
   m_strURL = strHost;
   ParseURL(m_strURL);
   m_strUserName = strLogin;
   m_strPassword = strPassword;

   return (m_pCurlSession != nullptr);
}

/**
 * @brief Cleans the current mail session
 *
 * If a session was not already started, this method has no effect
 *
 * @retval true   Successfully cleaned the current session.
 * @retval false  The session is not already initialized.
 *
 * Example Usage:
 * @code
 *    objMailClient.CleanupSession();
 * @endcode
 */
const bool CMailClient::CleanupSession()
{
   if (!m_pCurlSession)
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_ERROR_CURL_NOT_INIT_MSG);
      
      return false;
   }

   #ifdef DEBUG_CURL
   if (m_ofFileCurlTrace.is_open())
   {
      m_ofFileCurlTrace.close();
   }
   #endif

   curl_easy_cleanup(m_pCurlSession);
   m_pCurlSession = nullptr;

   /* Free the list of recipients */
   if (m_pRecipientslist)
   {
      curl_slist_free_all(m_pRecipientslist);

      /* Curl won't send the QUIT command until you call cleanup, so you should
      * be able to re-use this connection for additional requests or messages
      * (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
      * curl_easy_perform() again. It may not be a good idea to keep the connection
      * open for a very long time though (more than a few minutes may result in the 
      * server timing out the connection) and you do want to clean up in the end.
      */
      m_pRecipientslist = nullptr;
   }

   return true;
}

/**
* @brief sets the progress function callback and the owner of the client
*
* @param [in] pOwner pointer to the object owning the client, nullptr otherwise
* @param [in] fnCallback callback to progress function
*
*/
void CMailClient::SetProgressFnCallback(void* pOwner, const ProgressFnCallback& fnCallback)
{
   m_ProgressStruct.pOwner = pOwner;
   m_fnProgressCallback = fnCallback;
   m_ProgressStruct.pCurl = m_pCurlSession;
   m_ProgressStruct.dLastRunTime = 0;
   m_bProgressCallbackSet = true;
}

/**
* @brief sets the HTTP Proxy address to tunnel the operation through it
*
* @param [in] strProxy URI of the HTTP Proxy
*
*/
void CMailClient::SetProxy(const std::string& strProxy)
{
   if (strProxy.empty())
      return;

   std::string strUri = strProxy;
   std::transform(strUri.begin(), strUri.end(), strUri.begin(), ::toupper);

   if (strUri.compare(0, 4, "HTTP") != 0)
      m_strProxy = "http://" + strProxy;
   else
      m_strProxy = strProxy;
};

/**
* @brief performs the request of a mail client
*
*
* @retval true   Successfully performed the request.
* @retval false  The request couldn't be performed.
*
*/
const bool CMailClient::Perform()
{
   CURLcode res = CURLE_OK;

   if (!m_pCurlSession)
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_ERROR_CURL_NOT_INIT_MSG);

      return false;
   }
   // Reset is mandatory to avoid bad surprises
   curl_easy_reset(m_pCurlSession);

   if (!PrePerform())
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_ERROR_PREPERFORM_FAILED_MSG);

      return false;
   }

   /* Set username and password */
   curl_easy_setopt(m_pCurlSession, CURLOPT_USERNAME, m_strUserName.c_str());
   curl_easy_setopt(m_pCurlSession, CURLOPT_PASSWORD, m_strPassword.c_str());


   if (m_eSslTlsFlags & ENABLE_TLS)
   {
      /* With TLS, start with a plain text connection, and upgrade
      * to Transport Layer Security (TLS) using the STARTTLS (SMTP) or the STLS
      * (POP) command. Be careful of using CURLUSESSL_TRY here, because if TLS
      * upgrade fails, the transfer will continue anyway - see the security
      * discussion in the libcurl tutorial for more details.
      *
      * If your server doesn't have a valid certificate, then you can disable
      * part of the Transport Layer Security protection by setting the
      * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
      *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      *
      * That is, in general, a bad idea. It is still better than sending your
      * authentication details in plain text though.  Instead, you should get
      * the issuer certificate (or the host certificate if the certificate is
      * self-signed) and add it to the set of certificates that are known to
      * libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See docs/SSLCERTS
      * for more information. */
      curl_easy_setopt(m_pCurlSession, CURLOPT_USE_SSL, static_cast<long>(CURLUSESSL_ALL));
   }
   if (!s_strCertificationAuthorityFile.empty())
      curl_easy_setopt(m_pCurlSession, CURLOPT_CAINFO, s_strCertificationAuthorityFile.c_str());

   if (!m_strSSLCertFile.empty())
      curl_easy_setopt(m_pCurlSession, CURLOPT_SSLCERT, m_strSSLCertFile.c_str());

   if (!m_strSSLKeyFile.empty())
      curl_easy_setopt(m_pCurlSession, CURLOPT_SSLKEY, m_strSSLKeyFile.c_str());

   if (!m_strSSLKeyPwd.empty())
      curl_easy_setopt(m_pCurlSession, CURLOPT_KEYPASSWD, m_strSSLKeyPwd.c_str());

   if (!(m_eSettingsFlags & VERIFY_PEER))
   {
      /* If you want to connect to a site who isn't using a certificate that is
      * signed by one of the certs in the CA bundle you have, you can skip the
      * verification of the server's certificate. This makes the connection
      * A LOT LESS SECURE.
      *
      * If you have a CA cert for the server stored someplace else than in the
      * default bundle, then the CURLOPT_CAPATH option might come handy for
      * you. */
      curl_easy_setopt(m_pCurlSession, CURLOPT_SSL_VERIFYPEER, 0L);
   }

   /* If the site you're connecting to uses a different host name that what
   * they have mentioned in their server certificate's commonName (or
   * subjectAltName) fields, libcurl will refuse to connect. You can skip
   * this check, but this will make the connection less secure. */
   if (!(m_eSettingsFlags & VERIFY_HOST))
      curl_easy_setopt(m_pCurlSession, CURLOPT_SSL_VERIFYHOST, 0L); // use 2L for strict name check

   if (m_bProgressCallbackSet)
   {
      // curl_easy_setopt(m_pCurlSession, CURLOPT_PROGRESSFUNCTION, *GetProgressFnCallback()); // deprecated
      curl_easy_setopt(m_pCurlSession, CURLOPT_XFERINFOFUNCTION, *GetProgressFnCallback());
      curl_easy_setopt(m_pCurlSession, CURLOPT_PROGRESSDATA, &m_ProgressStruct);
      curl_easy_setopt(m_pCurlSession, CURLOPT_NOPROGRESS, 0L);
   }

   /* some servers need this */
   curl_easy_setopt(m_pCurlSession, CURLOPT_USERAGENT, CLIENT_USERAGENT);

   if (m_iCurlTimeout > 0)
   {
      curl_easy_setopt(m_pCurlSession, CURLOPT_TIMEOUT, m_iCurlTimeout);
      // don't want to get a sig alarm on timeout
      curl_easy_setopt(m_pCurlSession, CURLOPT_NOSIGNAL, 1);
   }

   if (!m_strProxy.empty())
   {
      curl_easy_setopt(m_pCurlSession, CURLOPT_PROXY, m_strProxy.c_str());
      curl_easy_setopt(m_pCurlSession, CURLOPT_HTTPPROXYTUNNEL, 1L);
   }

   if (m_bNoSignal)
   {
      curl_easy_setopt(m_pCurlSession, CURLOPT_NOSIGNAL, 1L);
   }

#ifdef DEBUG_CURL
   StartCurlDebug();
#endif

   // Perform the requested operation
   res = curl_easy_perform(m_pCurlSession);

#ifdef DEBUG_CURL
   EndCurlDebug();
#endif

   if (!PostPerform(res))
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(LOG_ERROR_POSTPERFORM_FAILED_MSG);

      return false;
   }

   if (res != CURLE_OK)
   {
      if (m_eSettingsFlags & ENABLE_LOG)
         m_oLog(StringFormat(LOG_ERROR_CURL_PEFORM_FAILURE_FORMAT, res, curl_easy_strerror(res)));

      return false;
   }
   return true;
}

/**
* @brief returns a formatted string
*
* @param [in] strFormat string with one or many format specifiers
* @param [in] parameters to be placed in the format specifiers of strFormat
*
* @retval string formatted string
*/
std::string CMailClient::StringFormat(const std::string strFormat, ...)
{
   int n = (static_cast<int>(strFormat.size())) * 2; // Reserve two times as much as the length of the strFormat
   
   std::unique_ptr<char[]> pFormatted;

   va_list ap;

   while(true)
   {
      pFormatted.reset(new char[n]); // Wrap the plain char array into the unique_ptr
      strcpy(&pFormatted[0], strFormat.c_str());
      
      va_start(ap, strFormat);
      int iFinaln = vsnprintf(&pFormatted[0], n, strFormat.c_str(), ap);
      va_end(ap);
      
      if (iFinaln < 0 || iFinaln >= n)
      {
         n += abs(iFinaln - n + 1);
      }
      else
      {
         break;
      }
   }

   return std::string(pFormatted.get());
}

/**
* @brief stores the server response in a string
*
* @param ptr pointer of max size (size*nmemb) to read data from it
* @param size size parameter
* @param nmemb memblock parameter
* @param data pointer to user data (string)
*
* @return (size * nmemb)
*/
size_t CMailClient::WriteInStringCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
   std::string* strWriteHere = reinterpret_cast<std::string*>(data);
   if (strWriteHere != nullptr)
   {
      strWriteHere->append(reinterpret_cast<char*>(ptr), size * nmemb);
      return size * nmemb;
   }
   return 0;
}

/**
* @brief stores the server response in an already opened file stream
*
* @param buff pointer of max size (size*nmemb) to read data from it
* @param size size parameter
* @param nmemb memblock parameter
* @param userdata pointer to user data (file stream)
*
* @return (size * nmemb)
*/
size_t CMailClient::WriteToFileCallback(void* buff, size_t size, size_t nmemb, void* data)
{
   if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1) || (data == nullptr))
      return 0;

   std::fstream* pFileStream = reinterpret_cast<std::fstream*>(data);
   if (pFileStream->is_open())
   {
      pFileStream->write(reinterpret_cast<char*>(buff), size * nmemb);
   }
   else
   {
      std::cout.write(reinterpret_cast<char*>(buff), size * nmemb);
   }
   return size * nmemb;
}

/**
* @brief sends a line from an already opened file stream (text)
*
* @param ptr pointer of max size (size*nmemb) to write data to it
* @param size size parameter
* @param nmemb memblock parameter
* @param stream pointer to user data (file stream)
*
* @return (size * nmemb)
*/
size_t CMailClient::ReadLineFromFileStreamCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
   if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1) || (stream == nullptr))
      return 0;

   std::fstream* pFileStream = reinterpret_cast<std::fstream*>(stream);
   std::string strLine;

   // what if the memory pointed by ptr was less than strLine.length() ?

   if (pFileStream->is_open() && getline(*pFileStream, strLine))
   {
      strLine.append("\r\n");
      std::memcpy(ptr, strLine.c_str(), strLine.length());
      return strLine.length();
   }
   return 0;
}

/**
* @brief sends a line from an input string stream
*
* @param ptr pointer of max size (size*nmemb) to write data to it
* @param size size parameter
* @param nmemb memblock parameter
* @param userp pointer to user data (input string stream)
*
* @return (size * nmemb)
*/
size_t CMailClient::ReadLineFromStringStreamCallback(void* ptr, size_t size, size_t nmemb, void* userp)
{
   if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1) || (userp == nullptr))
      return 0;

   std::istringstream* ssText = reinterpret_cast<std::istringstream*>(userp);
   std::string strLine;

   // what if the memory pointed by ptr was less than strLine.length() ?

   if (std::getline(*ssText, strLine, '\n'))
   {
      strLine.append("\r\n");
      std::memcpy(ptr, strLine.c_str(), strLine.length());
      return strLine.length();
   }
   return 0;
}

/**
* @brief reads the content of an already opened file stream
*
* @param ptr pointer of max size (size*nmemb) to write data to it
* @param size size parameter
* @param nmemb memblock parameter
* @param stream pointer to user data (file stream)
*
* @return (size * nmemb)
*/
size_t CMailClient::ReadFromFileCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
   std::fstream* pFileStream = reinterpret_cast<std::fstream*>(stream);
   if (pFileStream->is_open())
   {
      pFileStream->read(reinterpret_cast<char*>(ptr), size * nmemb);
      return pFileStream->gcount();
   }
   return 0;
}

#ifdef DEBUG_CURL
void CMailClient::SetCurlTraceLogDirectory(const std::string& strPath)
{
   s_strCurlTraceLogDirectory = strPath;

   if (!s_strCurlTraceLogDirectory.empty()
#ifdef WINDOWS
       && s_strCurlTraceLogDirectory.at(s_strCurlTraceLogDirectory.length() - 1) != '\\')
   {
      s_strCurlTraceLogDirectory += '\\';
   }
#else
      && s_strCurlTraceLogDirectory.at(s_strCurlTraceLogDirectory.length() - 1) != '/')
   {
      s_strCurlTraceLogDirectory += '/';
   }
#endif
}

int CMailClient::DebugCallback(CURL* curl , curl_infotype curl_info_type, char* pszTrace, size_t usSize, void* pFile)
{
   std::string strText;
   std::string strTrace(pszTrace, usSize);

   switch (curl_info_type)
   {
      case CURLINFO_TEXT:
         strText = "# Information : ";
         break;
      case CURLINFO_HEADER_OUT:
         strText = "-> Sending header : ";
         break;
      case CURLINFO_DATA_OUT:
         strText = "-> Sending data : ";
         break;
      case CURLINFO_SSL_DATA_OUT:
         strText = "-> Sending SSL data : ";
         break;
      case CURLINFO_HEADER_IN:
         strText = "<- Receiving header : ";
         break;
      case CURLINFO_DATA_IN:
         strText = "<- Receiving unencrypted data : ";
         break;
      case CURLINFO_SSL_DATA_IN:
         strText = "<- Receiving SSL data : ";
         break;
      default:
         break;
   }

   std::ofstream* pofTraceFile = reinterpret_cast<std::ofstream*>(pFile);
   if (pofTraceFile == nullptr)
   {
      std::cout << "[DEBUG] cURL debug log [" << curl_info_type << "]: " << " - " << strTrace;
   }
   else
   {
      (*pofTraceFile) << strText << strTrace;
   }

   return 0;
}

void CMailClient::StartCurlDebug()
{
   if (!m_ofFileCurlTrace.is_open())
   {
      curl_easy_setopt(m_pCurlSession, CURLOPT_VERBOSE, 1L);
      curl_easy_setopt(m_pCurlSession, CURLOPT_DEBUGFUNCTION, &CMailClient::DebugCallback);

      std::string strFileCurlTraceFullName(s_strCurlTraceLogDirectory);
      if (!strFileCurlTraceFullName.empty())
      {
         char szDate[32];
         memset(szDate, 0, 32);
         time_t tNow; time(&tNow);
         // new trace file for each hour
         strftime(szDate, 32, "%Y%m%d_%H", localtime(&tNow));
         strFileCurlTraceFullName += "TraceLog_";
         strFileCurlTraceFullName += szDate;
         strFileCurlTraceFullName += ".txt";

         m_ofFileCurlTrace.open(strFileCurlTraceFullName, std::ifstream::app | std::ifstream::binary);

         if (m_ofFileCurlTrace)
            curl_easy_setopt(m_pCurlSession, CURLOPT_DEBUGDATA, &m_ofFileCurlTrace);
      }
   }
}

void CMailClient::EndCurlDebug()
{
   if (m_ofFileCurlTrace && m_ofFileCurlTrace.is_open())
   {
      m_ofFileCurlTrace << "###########################################" << std::endl;
      m_ofFileCurlTrace.close();
   }
}
#endif

// SMTP

/**
* @file SMTPClient.cpp
* @brief implementation of the SMTP client class
* @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
*/

CSMTPClient::CSMTPClient(LogFnCallback oLogger) :
   CMailClient(oLogger),
   m_eOperationType(SMTP_SEND_STRING)
{

}

const bool CSMTPClient::SendString(const std::string& strFrom, const std::string& strTo,
                             const std::string& strCc, const std::string& strMail)
{
   m_strFrom = strFrom;
   m_strTo = strTo;
   m_strCc = strCc;
   m_strMail = strMail;
   m_eOperationType = SMTP_SEND_STRING;
   
   return Perform();
}

const bool CSMTPClient::SendMail(const std::string& strFrom, const std::string& strTo,
                  const std::string& strCc, const std::string& strSubject, const std::string& strBody, const std::string& strPath)
{
   m_strFrom = strFrom;
   m_strTo = strTo;
   m_strCc = strCc;
   m_strSubject = strSubject;
   m_strBody = strBody;
   m_strLocalFile = strPath;
   m_eOperationType = SMTP_SEND_FILE;

   return Perform();
}

const bool CSMTPClient::VerifyAddress(const std::string& strAddress)
{
   m_strTo = strAddress;
   m_eOperationType = SMTP_VRFY;

   return Perform();
}

void CSMTPClient::ParseURL(std::string& strURL)
{
   /* Note the use of smtps:// rather than smtp:// to request a SSL based connection. 
    * for TLS you don't need to change the scheme to smtps. */

   std::string strTmp = strURL;
   std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::toupper);

   if (strTmp.compare(0, 8, "SMTPS://") == 0 || strTmp.compare(0, 7, "SMTP://") == 0)
   {
      if (m_eSslTlsFlags != SslTlsFlag::ENABLE_SSL)
         m_eSslTlsFlags = SslTlsFlag::ENABLE_SSL;
   }
   else if (m_eSslTlsFlags == SslTlsFlag::ENABLE_SSL)
   {
      strURL.insert(0, "smtps://");
   }
   else
      strURL.insert(0, "smtp://");
}

/**
* @brief configures the curl session according to requested
* SMTP operation.
*
*
* @retval true   Successfully configured the curl session.
* @retval false  The configuration couldn't be performed.
*
*/
const bool CSMTPClient::PrePerform()
{
   //size_t uFileSize = 0;
   //size_t uCountLF = 0;

   m_ssString.clear();

   switch (m_eOperationType)
   {
      case SMTP_SEND_STRING:
         if (!m_strFrom.empty() && !m_strTo.empty())
         {
            m_ssString.str(m_strMail);
            /* Note that this option isn't strictly required, omitting it will result
            * in libcurl sending the MAIL FROM command with empty sender data. All
            * autoresponses should have an empty reverse-path, and should be directed
            * to the address in the reverse-path which triggered them. Otherwise,
            * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
            * details.
            */
            curl_easy_setopt(m_pCurlSession, CURLOPT_MAIL_FROM, m_strFrom.c_str());

            /* Add two recipients, in this particular case they correspond to the
            * To: and Cc: addressees in the header, but they could be any kind of
            * recipient. */
            m_pRecipientslist = curl_slist_append(m_pRecipientslist, m_strTo.c_str());
            
            if (!m_strCc.empty())
               m_pRecipientslist = curl_slist_append(m_pRecipientslist, m_strCc.c_str());
            
            curl_easy_setopt(m_pCurlSession, CURLOPT_MAIL_RCPT, m_pRecipientslist);

            /* We're using a callback function to specify the payload (the headers and
            * body of the message). You could just use the CURLOPT_READDATA option to
            * specify a FILE pointer to read from. */

            // LF will be replaced by CRLF when sending the mail
            /*uCountLF = std::count_if(m_strMail.cbegin(), m_strMail.cend(),
               [](const char c) { return c == '\n'; });*/

            //curl_easy_setopt(m_pCurlSession, CURLOPT_INFILESIZE, uCountLF + m_strMail.length());
            curl_easy_setopt(m_pCurlSession, CURLOPT_READFUNCTION, ReadLineFromStringStreamCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_READDATA, &m_ssString);
            curl_easy_setopt(m_pCurlSession, CURLOPT_UPLOAD, 1L);
         }
         else
            return false;
         break;

      case SMTP_SEND_FILE:
         if (/*!m_strLocalFile.empty() && */!m_strFrom.empty() && !m_strTo.empty())
         {
            // Request file size
            /*struct stat file_info;
            if (stat(m_strLocalFile.c_str(), &file_info))
            {
               if (m_eSettingsFlags & ENABLE_LOG)
                  m_oLog(StringFormat("[SMTPClient][Error] Unable to request local file size %s : %s"
                  "- in MAILClient::Perform() in case SMTP_SEND.", m_strLocalFile.c_str(), strerror(errno)));

               return false;
            }
            curl_off_t fsize = (curl_off_t)file_info.st_size;*/


            // m_ssString.str(m_strMail);
            // curl_easy_setopt(m_pCurlSession, CURLOPT_READFUNCTION, ReadLineFromStringStreamCallback);
            // curl_easy_setopt(m_pCurlSession, CURLOPT_READDATA, &m_ssString);
            // curl_easy_setopt(m_pCurlSession, CURLOPT_UPLOAD, 1L);

            struct curl_slist *headers = nullptr;
            headers = curl_slist_append(headers, ("Subject: " + std::string(m_strSubject)).c_str());
            curl_easy_setopt(m_pCurlSession, CURLOPT_HTTPHEADER, headers);


            // LF will be replaced by CRLF when sending the mail
            /*uCountLF = std::count_if((std::istreambuf_iterator<char>(m_fLocalFile)),
                                      std::istreambuf_iterator<char>(),
                                      [&uFileSize](const char c) -> bool
                                      { ++uFileSize; return c == '\n'; });*/

            // newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
            m_mime = curl_mime_init(m_pCurlSession);
            curl_mimepart *part;

            part = curl_mime_addpart(m_mime);
            curl_mime_data(part, m_strBody.c_str(), CURL_ZERO_TERMINATED);
            curl_mime_type(part, "text/plain");

            if (!m_strLocalFile.empty())
            {
               // I want to compute the size of the file without CR bytes
               // does this work properly ?
               m_fLocalFile.open(m_strLocalFile, std::fstream::in);
               part = curl_mime_addpart(m_mime);
               curl_mime_filedata(part, m_strLocalFile.c_str()); // Đường dẫn đến file nhị phân
               curl_mime_type(part, "application/octet-stream");         // MIME type cho file nhị phân
               curl_mime_encoder(part, "base64");  
               int pos = (m_strLocalFile.find_last_of("/\\") != std::string::npos) ? m_strLocalFile.find_last_of("/\\") + 1 : 0;
               curl_mime_filename(part, m_strLocalFile.substr(pos).c_str());         // Tên file nhị phân
            }

            curl_easy_setopt(m_pCurlSession, CURLOPT_MIMEPOST, m_mime);

            // if (m_fLocalFile)
            // {
            //    m_fLocalFile.seekg(0);

            //    curl_easy_setopt(m_pCurlSession, CURLOPT_READFUNCTION, CMailClient::ReadLineFromFileStreamCallback);
            //    curl_easy_setopt(m_pCurlSession, CURLOPT_READDATA, &m_fLocalFile);
            //    curl_easy_setopt(m_pCurlSession, CURLOPT_UPLOAD, 1L);
            //    //curl_easy_setopt(m_pCurlSession, CURLOPT_INFILESIZE_LARGE, uFileSize + uCountLF);
            // }
            // else
            // {
            //    if (m_eSettingsFlags & ENABLE_LOG)
            //       m_oLog(StringFormat("[SMTPClient][Error] Unable to open local file %s in CSMTPClient::PrePerform()"
            //                           "in case SMTP_SEND_FILE.", m_strLocalFile.c_str()));

            //    return false;
            // }
            curl_easy_setopt(m_pCurlSession, CURLOPT_MAIL_FROM, m_strFrom.c_str());
            m_pRecipientslist = curl_slist_append(m_pRecipientslist, m_strTo.c_str());

            if (!m_strCc.empty())
               m_pRecipientslist = curl_slist_append(m_pRecipientslist, m_strCc.c_str());
            
            curl_easy_setopt(m_pCurlSession, CURLOPT_MAIL_RCPT, m_pRecipientslist);
         }
         else
            return false;

         break;

      case SMTP_VRFY:
         /* Note that the CURLOPT_MAIL_RCPT takes a list, not a char array  */
         if (!m_strTo.empty())
         {
            if (m_strTo.at(0) != '<')
               m_strTo.insert(0, "<");
            
            if (m_strTo.at(m_strTo.length() - 1) != '>')
               m_strTo += '>';

            m_pRecipientslist = curl_slist_append(m_pRecipientslist, m_strTo.c_str());
            curl_easy_setopt(m_pCurlSession, CURLOPT_MAIL_RCPT, m_pRecipientslist);
         }
         else
            return false;

         break;

      case SMTP_EXPN:
         /* Note that the CURLOPT_MAIL_RCPT takes a list, not a char array. e.g. "Friends" */
         m_pRecipientslist = curl_slist_append(m_pRecipientslist, m_strMail.c_str());
         curl_easy_setopt(m_pCurlSession, CURLOPT_MAIL_RCPT, m_pRecipientslist);

         /* Set the EXPN command */
         curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, "EXPN");
         break;

      default:
         if (m_eSettingsFlags & ENABLE_LOG)
            m_oLog("[SMTPClient][Error] Unknown operation.");

         break;
   }

   curl_easy_setopt(m_pCurlSession, CURLOPT_URL, m_strURL.c_str());

   return true;
}

/**
* @brief performs operations that need to be done after performing
* an SMTP request.
*
*
* @retval true   Successfully performed post request operations.
* @retval false  The post request operations couldn't be performed.
*
*/
const bool CSMTPClient::PostPerform(CURLcode ePerformCode)
{
   ePerformCode;

   if (m_eOperationType == SMTP_SEND_FILE)
      if (m_fLocalFile.is_open())
         m_fLocalFile.close();

   return true;
}

// IMAP

/**
* @file IMAPClient.cpp
* @brief implementation of the IMAP client class
* @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
*/

CIMAPClient::CIMAPClient(LogFnCallback oLogger) :
   CMailClient(oLogger),
   m_pstrText(nullptr),
   m_eOperationType(IMAP_NOOP),
   m_eMailProperty(MailProperty::Flagged),
   m_eSearchOption(SearchOption::FLAGGED)
{

}

const bool CIMAPClient::CleanupSession()
{
   m_pstrText = nullptr;
   return CMailClient::CleanupSession();
}

const bool CIMAPClient::GetHeaderString(const std::string& strMsgNumber, std::string& strOutput)
{
   m_strMsgNumber = strMsgNumber;
   m_pstrText = &strOutput;
   m_eOperationType = IMAP_RETR_HEADER_STRING;

   return Perform();
}

const bool CIMAPClient::GetBodyString(const std::string& strMsgNumber, std::string& strOutput)
{
   m_strMsgNumber = strMsgNumber;
   m_pstrText = &strOutput;
   m_eOperationType = IMAP_RETR_BODY_STRING;

   return Perform();
}

const bool CIMAPClient::Noop()
{
   m_eOperationType = IMAP_NOOP;

   return Perform();
}

const bool CIMAPClient::Search(std::string& strRes, SearchOption eSearchOption)
{
   m_pstrText = &strRes;
   m_eSearchOption = eSearchOption;
   m_eOperationType = IMAP_SEARCH;

   return Perform();
}

void CIMAPClient::ParseURL(std::string& strURL)
{
   std::string strTmp = strURL;
   std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::toupper);

   if (strTmp.compare(0, 8, "IMAPS://") == 0 || strTmp.compare(0, 7, "IMAP://") == 0)
   {
      if (m_eSslTlsFlags != SslTlsFlag::ENABLE_SSL)
         m_eSslTlsFlags = SslTlsFlag::ENABLE_SSL;
   }
   else if (m_eSslTlsFlags == SslTlsFlag::ENABLE_SSL)
   {
      strURL.insert(0, "imaps://");
   }
   else
      strURL.insert(0, "imap://");

   /* append a '/' to the end of the URL so that we can easily add the message number
   * at the end of it */
   if (strURL.at(strURL.length() - 1) != '/')
      strURL += '/';

}

/**
* @brief configures the curl session according to requested
* IMAp operation.
*
*
* @retval true   Successfully configured the curl session.
* @retval false  The configuration couldn't be performed.
*
*/
const bool CIMAPClient::PrePerform()
{
   std::string strRequestURL(m_strURL);
   std::string strCmd; // for SEARCH or STORE
   //curl_off_t fsize;
   //size_t uFileSize = 0;
   //size_t uCountLF = 0;
   m_ssString.clear();

   switch (m_eOperationType)
   {
      case IMAP_SEND_STRING:
         /* This will create a new message 100. Note that you should perform an
         * EXAMINE command to obtain the UID of the next message to create and a
         * SELECT to ensure you are creating the message in the OUTBOX. */
         strRequestURL += m_strMsgNumber;
         m_ssString.str(m_strMail);
         
         /* LF will be replaced by CRLF when sending the mail.
         * This must be taken in consideration in the total size of the payload */
         /*uCountLF = std::count_if(m_strMail.cbegin(), m_strMail.cend(),
                                       [](const char c) { return c == '\n'; });*/

         //curl_easy_setopt(m_pCurlSession, CURLOPT_INFILESIZE, uCountLF + m_strMail.length());
         curl_easy_setopt(m_pCurlSession, CURLOPT_READFUNCTION, ReadLineFromStringStreamCallback);
         curl_easy_setopt(m_pCurlSession, CURLOPT_READDATA, &m_ssString);
         curl_easy_setopt(m_pCurlSession, CURLOPT_UPLOAD, 1L);
         break;

      case IMAP_SEND_FILE:
         if (!m_strLocalFile.empty())
         {
            // Request file size
            /*struct stat file_info;
            if (stat(m_strLocalFile.c_str(), &file_info))
            {
               if (m_eSettingsFlags & ENABLE_LOG)
                  m_oLog(StringFormat("[IMAPClient][Error] Unable to request local file size "
                  "%s : %s - in CIMAPClient::PrePerform() in case IMAP_SEND_FILE.", m_strLocalFile.c_str(),
                     strerror(errno)));

               return false;
            }
            fsize = (curl_off_t)file_info.st_size;*/

            m_fLocalFile.open(m_strLocalFile, std::fstream::in);
            
            // LF will be replaced by CRLF when sending the mail
            /*uCountLF = std::count_if((std::istreambuf_iterator<char>(m_fLocalFile)),
                                      std::istreambuf_iterator<char>(),
                                      [&uFileSize](const char c) -> bool
                                      { ++uFileSize; return c == '\n'; });*/

            if (m_fLocalFile)
            {
               m_fLocalFile.seekg(0);

               curl_easy_setopt(m_pCurlSession, CURLOPT_READFUNCTION, CMailClient::ReadLineFromFileStreamCallback);
               curl_easy_setopt(m_pCurlSession, CURLOPT_READDATA, &m_fLocalFile);
               curl_easy_setopt(m_pCurlSession, CURLOPT_UPLOAD, 1L);
               //curl_easy_setopt(m_pCurlSession, CURLOPT_INFILESIZE_LARGE, uFileSize + uCountLF);
            }
            else
            {
               if (m_eSettingsFlags & ENABLE_LOG)
                  m_oLog(StringFormat("[IMAPClient][Error] Unable to open local file %s in CIMAPClient::PrePerform()"
                                      "in case IMAP_SEND_FILE.", m_strLocalFile.c_str()));

               return false;
            }
         }
         else
            return false;

         break;

      case IMAP_NOOP:
         /* Set the NOOP command */
         curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, "NOOP");

         /* Do not perform a transfer as NOOP returns no data */
         curl_easy_setopt(m_pCurlSession, CURLOPT_NOBODY, 1L);
         break;

      case IMAP_LIST:
         if (m_pstrText != nullptr)
         {
            if (!m_strFolderName.empty())
               strRequestURL += m_strFolderName;

            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteInStringCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, m_pstrText);
         }
         else
            return false;

         break;

      case IMAP_DELETE_FOLDER:
         /* You can specify the message either in the URL or DELE command */
         if (!m_strFolderName.empty())
         {
            /* Set the DELE command */
            curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, ("DELETE " + m_strFolderName).c_str());
         }
         else
            return false;

         break;

      case IMAP_RETR_HEADER_STRING:
         if (!m_strMsgNumber.empty())
            strRequestURL += "INBOX;MAILINDEX=" + m_strMsgNumber + "/;SECTION=HEADER.FIELDS%20(SUBJECT%20FROM)";
         else
            return false;

         /* This will retrieve message 'm_strMsgNumber' from the user's mailbox */
         if (m_pstrText != nullptr)
         {
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteInStringCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, m_pstrText);
         }
         else
            return false;
         break;

      case IMAP_RETR_BODY_STRING:
         if (!m_strMsgNumber.empty())
            strRequestURL += "INBOX;MAILINDEX=" + m_strMsgNumber + "/;SECTION=TEXT";
         else
            return false;

         /* This will retrieve message 'm_strMsgNumber' from the user's mailbox */
         if (m_pstrText != nullptr)
         {
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteInStringCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, m_pstrText);
         }
         else
            return false;
         break;

      case IMAP_RETR_FILE:
         if (!m_strMsgNumber.empty())
            strRequestURL += "INBOX/;MAILINDEX=" + m_strMsgNumber;
         else
            return false;

         m_fLocalFile.open(m_strLocalFile, std::fstream::out | std::fstream::binary | std::fstream::trunc);
         if (m_fLocalFile)
         {
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteToFileCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, &m_fLocalFile);
         }
         else
         {
            if (m_eSettingsFlags & ENABLE_LOG)
               m_oLog(StringFormat("[IMAPClient][Error] Unable to open local file %s in CIMAPClient::PrePerform()"
                  " in case IMAP_RETR_FILE.", m_strLocalFile.c_str()));

            return false;
         }
         break;

      case IMAP_INFO_FOLDER:
         if (m_pstrText != nullptr)
         {
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteInStringCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, m_pstrText);
         }
         else
            return false;

         /* Set the EXAMINE command specifing the mailbox folder */
         curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, ("EXAMINE " + m_strFolderName).c_str());

         break;

      case IMAP_LSUB:
         if (m_pstrText != nullptr)
         {
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteInStringCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, m_pstrText);
         }
         else
            return false;

         /* Set the LSUB command. Note the syntax is very similar to that of a LIST
         command. */
         curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, "LSUB \"\" *");

         break;

      case IMAP_COPY:
         if (!m_strMsgNumber.empty() && !m_strFolderName.empty())
         {
            strRequestURL += "INBOX";
            /* Set the COPY command specifing the message ID and destination folder */
            curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST,
               ("COPY "+ m_strMsgNumber + " " + m_strFolderName).c_str());

            /* Note that to perform a move operation you will need to perform the copy,
            * then mark the original mail as Deleted and EXPUNGE or CLOSE. Please see
            * imap-store.c for more information on deleting messages. */
         }
         else
            return false;

         break;

      case IMAP_CREATE:
         if (!m_strFolderName.empty())
         {
            /* Set the CREATE command specifing the new folder name */
            curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, ("CREATE " + m_strFolderName).c_str());
         }
         else
            return false;

         break;

      case IMAP_SEARCH:
         if (m_pstrText != nullptr)
         {
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEFUNCTION, &CMailClient::WriteInStringCallback);
            curl_easy_setopt(m_pCurlSession, CURLOPT_WRITEDATA, m_pstrText);
         }
         else
            return false;

         strRequestURL += "INBOX";

         if (m_eSearchOption == SearchOption::ANSWERED)
            strCmd = "ANSWERED";
         else if (m_eSearchOption == SearchOption::DELETED)
            strCmd = "DELETED";
         else if (m_eSearchOption == SearchOption::DRAFT)
            strCmd = "DRAFT";
         else if (m_eSearchOption == SearchOption::FLAGGED)
            strCmd = "FLAGGED";
         else if (m_eSearchOption == SearchOption::NEW)
            strCmd = "NEW";
         else if (m_eSearchOption == SearchOption::RECENT)
            strCmd = "RECENT";
         else if (m_eSearchOption == SearchOption::SEEN)
            strCmd = "SEEN";
         else if (m_eSearchOption == SearchOption::CUSTOMIZED)
            strCmd = "UNSEEN SUBJECT PROJECT_MMT";
         else
         {
            return false;
         }

         /* Set the SEARCH command specifing what we want to search for. Note that
         * this can contain a message sequence set and a number of search criteria
         * keywords including flags such as ANSWERED, DELETED, DRAFT, FLAGGED, NEW,
         * RECENT and SEEN. For more information about the search criteria please
         * see RFC-3501 section 6.4.4.   */
         curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, ("SEARCH " + strCmd).c_str());

         break;

      case IMAP_STORE:
         if (!m_strMsgNumber.empty())
         {
            if (m_eMailProperty == MailProperty::Deleted)
               strCmd = "Deleted";
            else if (m_eMailProperty == MailProperty::Seen)
               strCmd = "Seen";
            else if (m_eMailProperty == MailProperty::Answered)
               strCmd = "Answered";
            else if (m_eMailProperty == MailProperty::Flagged)
               strCmd = "Flagged";
            else if (m_eMailProperty == MailProperty::Draft)
               strCmd = "Draft";
            else if (m_eMailProperty == MailProperty::Recent)
               strCmd = "Recent";
            else
            {
               return false;
            }

            strRequestURL += "INBOX";

            /* Set the STORE command with the Deleted flag for message m_strMsgNumber */
            curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST,
               ("STORE " + m_strMsgNumber + " +Flags \\" + strCmd).c_str());
         }
         else
            return false;

         break;

      default:
         if (m_eSettingsFlags & ENABLE_LOG)
            m_oLog("[IMAPClient][Error] Unknown operation.");

         break;
   }

   curl_easy_setopt(m_pCurlSession, CURLOPT_URL, strRequestURL.c_str());

   return true;
}

/**
* @brief performs operations that need to be done after performing
* an IMAP request.
*
*
* @retval true   Successfully performed post request operations.
* @retval false  The post request operations couldn't be performed.
*
*/
const bool CIMAPClient::PostPerform(CURLcode ePerformCode)
{
   if (m_eOperationType == IMAP_SEND_FILE || m_eOperationType == IMAP_RETR_FILE)
      if (m_fLocalFile.is_open())
         m_fLocalFile.close();

   if (m_eOperationType == IMAP_STORE)
   {
      if (ePerformCode != CURLE_OK)
      {
         /* Set the EXPUNGE command, although you can use the CLOSE command if you
         * don't want to know the result of the STORE */
         curl_easy_setopt(m_pCurlSession, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

         /* Perform the second custom request */
         ePerformCode = curl_easy_perform(m_pCurlSession);

         /* Check for errors */
         if (ePerformCode != CURLE_OK)
            if (m_eSettingsFlags & ENABLE_LOG)
               m_oLog(StringFormat(LOG_ERROR_CURL_PEFORM_FAILURE_FORMAT, ePerformCode, curl_easy_strerror(ePerformCode)));
      }
   }

   return true;
}
