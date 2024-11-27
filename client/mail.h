#ifndef INCLUDE_CURLHANDLE_H_
#define INCLUDE_CURLHANDLE_H_

#include <stdexcept>

class CurlHandle {
  public:
   static CurlHandle &instance();

   CurlHandle(CurlHandle const &) = delete;
   CurlHandle(CurlHandle &&)      = delete;

   CurlHandle &operator=(CurlHandle const &) = delete;
   CurlHandle &operator=(CurlHandle &&) = delete;

   ~CurlHandle();

  private:
   CurlHandle();
};

#endif


/*
 * @file MAILClient.h
 * @brief libcurl wrapper for email operations (POP3, IMAP and SMTP)
 * This class contains the common stuff between POP3, IMAP and SMTP
 * clients.
 * 
 * It is intended to be an abstract class but for unit tests purposes
 * I decided not to declare it so.
 * 
 * @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
 * @date 2017-01-01
 */

#ifndef INCLUDE_MAILCLIENT_H_
#define INCLUDE_MAILCLIENT_H_

#define CLIENT_USERAGENT "mailclientcpp-agent/1.0"

#include <algorithm>
#include <cstddef>         // std::size_t
#include <cstdio>          // snprintf
#include <cstdlib>
#include <cstring>         // strerror, strlen, memcpy, strcpy
#include <ctime>
#include <curl/curl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <stdarg.h>        // va_start, etc...
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <memory>          // std::unique_ptr

class CMailClient
{
public:
   // Public definitions
   typedef std::function<int(void*, double, double, double, double)> ProgressFnCallback;
   typedef std::function<void(const std::string&)>                   LogFnCallback;

   // Progress Function Data Object - parameter void* of ProgressFnCallback references it
   struct ProgressFnStruct
   {
      ProgressFnStruct() : dLastRunTime(0), pCurl(nullptr), pOwner(nullptr) {}
      double dLastRunTime;
      CURL*  pCurl;
      /* owner of the MailClient object. can be used in the body of the progress
       * function to send signals to the owner (e.g. to update a GUI's progress bar)
      */
      void*  pOwner;
   };

   enum SettingsFlag
   {
      NO_FLAGS    = 0x00,
      ENABLE_LOG  = 0x01,
      VERIFY_PEER = 0x02,
      VERIFY_HOST = 0x04,
      ALL_FLAGS   = 0xFF
   };
   //enum class SslTlsFlag : unsigned char
   // DO NOT combine them !
   enum SslTlsFlag
   {
      NO_SSLTLS  = 0x00,
      ENABLE_TLS = 0x01,
      ENABLE_SSL = 0x02
   };

   /* Please provide your logger thread-safe routine, otherwise, you can turn off 
    * error log messages printing by not using the flag ALL_FLAGS or ENABLE_LOG */
   explicit CMailClient(LogFnCallback oLogger);
   virtual ~CMailClient();

   // copy constructor and assignment operator are disabled
   CMailClient(const CMailClient& Copy) = delete;
   CMailClient& operator=(const CMailClient& Copy) = delete;

   // Setters - Getters (for unit tests)
   void SetProgressFnCallback(void* pOwner, const ProgressFnCallback& fnCallback);
   void SetProxy(const std::string& strProxy);
   inline void SetTimeout(const int& iTimeout) { m_iCurlTimeout = iTimeout; }
   inline void SetNoSignal(const bool& bNoSignal) { m_bNoSignal = bNoSignal; }
   inline auto GetProgressFnCallback() const
   {
      return m_fnProgressCallback.target<int(*)(void*,double,double,double,double)>();
   }
   inline void* GetProgressFnCallbackOwner() const { return m_ProgressStruct.pOwner; }
   inline const std::string& GetProxy() const { return m_strProxy; }
   inline const int GetTimeout() const { return m_iCurlTimeout; }
   inline const bool GetNoSignal() const { return m_bNoSignal; }
   inline const std::string& GetURL()      const { return m_strURL; }
   inline const std::string& GetUsername() const { return m_strUserName; }
   inline const std::string& GetPassword() const { return m_strPassword; }
   inline const unsigned char GetFlags() const { return m_eSettingsFlags; }
   inline const SslTlsFlag GetSslTlsFlags() const { return m_eSslTlsFlags; }

   // Session
   const bool InitSession(const std::string& strHost,
                          const std::string& strLogin,
                          const std::string& strPassword,
                          const SettingsFlag& SettingsFlags = ALL_FLAGS,
                          const SslTlsFlag& SslTlsFlags = NO_SSLTLS);
   virtual const bool CleanupSession();
   const CURL* GetCurlPointer() const { return m_pCurlSession; }

   static const std::string& GetCertificateFile() { return s_strCertificationAuthorityFile; }
   static void SetCertificateFile(const std::string& strPath) { s_strCertificationAuthorityFile = strPath; }
 
   void SetSSLCertFile(const std::string& strPath) { m_strSSLCertFile = strPath; }
   const std::string& GetSSLCertFile() const { return m_strSSLCertFile; }
   
   void SetSSLKeyFile(const std::string& strPath) { m_strSSLKeyFile = strPath; }
   const std::string& GetSSLKeyFile() const { return m_strSSLKeyFile; }

   void SetSSLKeyPassword(const std::string& strPwd) { m_strSSLKeyPwd = strPwd; }
   const std::string& GetSSLKeyPwd() const { return m_strSSLKeyPwd; }

   inline const unsigned char GetSettingsFlags() const { return m_eSettingsFlags; }

#ifdef DEBUG_CURL
   static void SetCurlTraceLogDirectory(const std::string& strPath);
#endif

protected:
   virtual const bool PrePerform() { return true; }
   /* common operations to SMTP, POP & IMAP are performed here */
   const bool Perform();
   virtual const bool PostPerform(CURLcode ePerformCode) { ePerformCode;  return true; }
   virtual inline void ParseURL(std::string& strURL) { strURL; }

   // Curl callbacks
   static size_t WriteInStringCallback(void* ptr, size_t size, size_t nmemb, void* data);
   static size_t WriteToFileCallback(void* ptr, size_t size, size_t nmemb, void* data);
   static size_t ReadLineFromFileStreamCallback(void* ptr, size_t size, size_t nmemb, void* stream);
   static size_t ReadLineFromStringStreamCallback(void* ptr, size_t size, size_t nmemb, void* userp);
   static size_t ReadFromFileCallback(void* ptr, size_t size, size_t nmemb, void* stream);

   // Helper for error log printing
   static std::string StringFormat(const std::string strFormat, ...);

#ifdef DEBUG_CURL
   static int DebugCallback(CURL* curl, curl_infotype curl_info_type, char* strace, size_t nSize, void* pFile);
   inline void StartCurlDebug();
   inline void EndCurlDebug();
#endif

   std::string          m_strUserName;
   std::string          m_strPassword;
   std::string          m_strURL;
   std::string          m_strProxy;
   
   bool                 m_bNoSignal;

   /* Can be used in derived classes to perform file I/O or
    * or input string stream operations */ 
   std::string          m_strLocalFile;
   std::fstream         m_fLocalFile;
   std::istringstream   m_ssString;

   // SSL
   static std::string   s_strCertificationAuthorityFile;
   std::string          m_strSSLCertFile;
   std::string          m_strSSLKeyFile;
   std::string          m_strSSLKeyPwd;

   mutable CURL*         m_pCurlSession;
   mutable curl_mime*    m_mime;
   struct curl_slist*    m_pRecipientslist;
   int                   m_iCurlTimeout;
   SettingsFlag          m_eSettingsFlags;
   SslTlsFlag            m_eSslTlsFlags;

   // Progress function
   ProgressFnCallback     m_fnProgressCallback;
   ProgressFnStruct       m_ProgressStruct;
   bool                   m_bProgressCallbackSet;

   // Log printer callback
   LogFnCallback          m_oLog;

#ifdef DEBUG_CURL
   static std::string s_strCurlTraceLogDirectory;
   std::ofstream       m_ofFileCurlTrace;
#endif

   CurlHandle& m_curlHandle;
};

inline CMailClient::SettingsFlag operator|(CMailClient::SettingsFlag a, CMailClient::SettingsFlag b) {
    return static_cast<CMailClient::SettingsFlag>(static_cast<int>(a) | static_cast<int>(b));
}

// Logs messages
#define LOG_ERROR_CURL_ALREADY_INIT_MSG       "[MAILClient][Error] Curl session is already initialized ! " \
                                              "Use CleanupSession() to clean the present one."
#define LOG_ERROR_EMPTY_HOST_MSG              "[MAILClient][Error] Empty hostname."
#define LOG_ERROR_CURL_NOT_INIT_MSG           "[MAILClient][Error] Curl session is not initialized ! Use InitSession() before."
#define LOG_WARNING_OBJECT_NOT_CLEANED        "[MAILClient][Warning] Object was freed before calling CMailClient::CleanupSession()." \
                                              " The API session was cleaned though."
#define LOG_ERROR_PREPERFORM_FAILED_MSG       "[MAILClient][Error] PrePerform failed !"
#define LOG_ERROR_POSTPERFORM_FAILED_MSG      "[MAILClient][Error] PostPerform failed !"
#define LOG_ERROR_CURL_PEFORM_FAILURE_FORMAT  "[MAILClient][Error] Unable to perform a request (Error=%d | %s) !"

#endif

/*
* @file SMTPClient.h
* @brief libcurl wrapper for SMTP operations
*
* @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
* @date 2017-01-02
*/

#ifndef INCLUDE_SMTPCLIENT_H_
#define INCLUDE_SMTPCLIENT_H_

class CSMTPClient : public CMailClient
{
public:
   explicit CSMTPClient(LogFnCallback oLogger);

   // copy constructor and assignment operator are disabled
   CSMTPClient(const CSMTPClient& Copy) = delete;
   CSMTPClient& operator=(const CSMTPClient& Copy) = delete;

   /* send a string as an e-mail */
   const bool SendString(const std::string& strFrom, const std::string& strTo,
                   const std::string& strCc, const std::string& strMail);
   
   /* send a text file as an e-mail */
   const bool SendMail(const std::string& strFrom, const std::string& strTo,
                             const std::string& strCc, const std::string& strSubject, const std::string& strBody, const std::string& strPath);
   
   /* verify an e-mail address */
   const bool VerifyAddress(const std::string& strAddress);

protected:
   enum MailOperation
   {
      SMTP_SEND_STRING,
      SMTP_SEND_FILE,
      SMTP_VRFY,
      SMTP_EXPN
   };

   const bool PrePerform() override;
   const bool PostPerform(CURLcode ePerformCode) override;
   inline void ParseURL(std::string& strURL) override final;

   MailOperation        m_eOperationType;

   std::string          m_strFrom;
   std::string          m_strTo;
   std::string          m_strCc;
   std::string          m_strMail;
   std::string          m_strSubject;
   std::string          m_strBody;
};

#endif

/*
* @file IMAPClient.h
* @brief libcurl wrapper for IMAP operations
*
* @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
* @date 2017-01-02
*/

#ifndef INCLUDE_IMAPCLIENT_H_
#define INCLUDE_IMAPCLIENT_H_

class CIMAPClient : public CMailClient
{
public:
   enum class MailProperty
   {
      Deleted,
      Seen,
      Answered,
      Flagged,
      Draft,
      Recent
   };
   enum class SearchOption
   {
      ANSWERED,
      DELETED,
      DRAFT,
      FLAGGED,
      NEW,
      RECENT,
      SEEN,
      CUSTOMIZED
   };


   explicit CIMAPClient(LogFnCallback oLogger);

   // copy constructor and assignment operator are disabled
   CIMAPClient(const CIMAPClient& Copy) = delete;
   CIMAPClient& operator=(const CIMAPClient& Copy) = delete;

   const bool CleanupSession() override;

   /* retrieve e-mail and save its header in strOutput */
   const bool GetHeaderString(const std::string& strMsgNumber, std::string& strOutput);

   /* retrieve e-mail and save its body in strOutput */
   const bool GetBodyString(const std::string& strMsgNumber, std::string& strOutput);

   /* perform a noop */
   const bool Noop();
 
   /* search for e-mails according to SearchOption */
   const bool Search(std::string& strRes, SearchOption eSearchOption = SearchOption::NEW);
      
protected:
   enum MailOperation
   {
      IMAP_NOOP,
      IMAP_LIST,
      IMAP_SEND_STRING,
      IMAP_SEND_FILE,
      IMAP_RETR_FILE,
      IMAP_RETR_HEADER_STRING,
      IMAP_RETR_BODY_STRING,
      IMAP_DELETE_FOLDER,
      IMAP_INFO_FOLDER,
      IMAP_LSUB,
      IMAP_COPY,
      IMAP_CREATE,
      IMAP_SEARCH,
      IMAP_STORE
   };

   const bool PrePerform() override;
   const bool PostPerform(CURLcode ePerformCode) override;
   inline void ParseURL(std::string& strURL) override final;

   MailOperation        m_eOperationType;
   MailProperty         m_eMailProperty;
   SearchOption         m_eSearchOption;

   std::string          m_strFrom;
   std::string          m_strTo;
   std::string          m_strCc;
   std::string          m_strMail;
   std::string          m_strMsgNumber;
   std::string          m_strFolderName;
   std::string*         m_pstrText;

};

#endif
