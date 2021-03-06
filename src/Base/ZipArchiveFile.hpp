//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ZipArchiveFile.hpp File stream from zip archive
//
#pragma once

// includes
#include <ulib/stream/IStream.hpp>
#include "ZlibDecompressor.hpp"

/// stream base class that can only be read from
class ReadOnlyNoWriteNoSeekStream: public Stream::IStream
{
public:
   virtual ~ReadOnlyNoWriteNoSeekStream() throw() {}

   /// returns if read is possible
   virtual bool CanRead() const throw() override { return true; }
   /// returns if write is possible; not supported
   virtual bool CanWrite() const throw() override { return false; }
   /// returns if seek is possible; not supported
   virtual bool CanSeek() const throw() override { return false; }

   // write support

   /// writes data; not supported
   virtual void Write(const void* /*bData*/, DWORD /*dwLength*/, DWORD& /*dwBytesWritten*/) override
   {
      throw Exception(_T("ZipArchive file can't be written to"), __FILE__, __LINE__);
   }

   /// writes byte; not supported
   virtual void WriteByte(BYTE /*b*/) override
   {
      throw Exception(_T("stream can't be written to"), __FILE__, __LINE__);
   }

   /// flushes written data; not supported
   virtual void Flush() override
   {
      throw Exception(_T("stream can't be written to"), __FILE__, __LINE__);
   }

   // seek support

   /// seeks to given position, regarding given origin; not supported
   virtual ULONGLONG Seek(LONGLONG /*llOffset*/, ESeekOrigin /*origin*/) override
   {
      throw Exception(_T("stream can't be seeked"), __FILE__, __LINE__);
   }
};

/// file from a zip archive
class ZipArchiveFile: public ReadOnlyNoWriteNoSeekStream
{
public:
   /// ctor
   ZipArchiveFile(std::shared_ptr<Stream::IStream> spArchiveFile, ULONGLONG ullOffset,
      unsigned int uiCompressedSize, unsigned int uiUncompressedSize)
      :m_spArchiveFile(spArchiveFile),
       m_uiCompressedRemaining(uiCompressedSize),
       m_uiUncompressedSize(uiUncompressedSize),
       m_ullCurrentPos(0ULL),
       m_bEndOfInputStream(false),
       m_bEndOfOutputStream(false)
   {
      spArchiveFile->Seek(static_cast<LONGLONG>(ullOffset), Stream::IStream::seekBegin);
   }

   /// dtor
   virtual ~ZipArchiveFile() throw() {}

   /// returns if seek is possible; only forward seek supported
   virtual bool CanSeek() const throw() override { return true; }

   // read support

   /// reads data from file in zip archive
   virtual bool Read(void* bBuffer, DWORD dwMaxBufferLength, DWORD& dwBytesRead) override;

   /// reads byte from file in zip archive
   virtual BYTE ReadByte() override;

   /// indicates if at end of file in zip archive
   virtual bool AtEndOfStream() const throw() override
   {
      return /*m_bEndOfInputStream &&*/ m_bEndOfOutputStream && m_vecOutBuffer.empty();
   }

   /// returns current position
   virtual ULONGLONG Position() override
   {
      return m_ullCurrentPos;
   }

   /// returns (uncompressed) file length
   virtual ULONGLONG Length() override
   {
      return m_uiUncompressedSize;
   }

   /// closes file in zip archive
   virtual void Close() override
   {
      m_bEndOfInputStream = true;
      m_bEndOfOutputStream = true;
      m_vecOutBuffer.clear();
   }

   /// seeks to given position, regarding given origin; only seek forward supported
   virtual ULONGLONG Seek(LONGLONG llOffset, ESeekOrigin origin) override;

private:
   /// fills out buffer by decoding more bytes
   void FillOutBuffer(DWORD dwMinInBufferSize);

   /// fill input buffer
   void FillInputBuffer();

private:
   /// archive file stream
   std::shared_ptr<Stream::IStream> m_spArchiveFile;

   /// remaining compressed bytes
   unsigned int m_uiCompressedRemaining;

   /// uncompressed size
   unsigned int m_uiUncompressedSize;

   /// zlib decompressor
   ZlibDecompressor m_decompressor;

   /// input buffer
   std::vector<BYTE> m_vecInBuffer;

   /// output buffer
   std::vector<BYTE> m_vecOutBuffer;

   /// current position in file in zip archive
   ULONGLONG m_ullCurrentPos;

   /// at end of output stream?
   bool m_bEndOfOutputStream;

   /// at end of input stream?
   bool m_bEndOfInputStream;
};
