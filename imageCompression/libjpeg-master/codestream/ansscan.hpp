// in scantypes.hpp ------------------------------------------------

enum ScanType {
  Baseline,
  Sequential,
  
  // ... 

  ACResidual,
  JPEG_LS,

  ANS // add new ScanType ANS
};

// in scan.cpp -----------------------------------------------------

/// Scan::CreateParser
// Create a suitable parser given the scan type as indicated in the
// header and the contents of the marker. The parser is kept
// here as it is local to the scan.
void Scan::CreateParser(void)
{
  ScanType type        = m_pFrame->ScanTypeOf();
  //
  assert(m_pParser == NULL);
  //
  switch(type) {
  case Baseline:
  case Sequential:
    m_pParser = new(m_pEnviron) class SequentialScan(m_pFrame,this,
						     m_ucScanStart,m_ucScanStop,
						     m_ucLowBit + m_ucHiddenBits,0);
    break;
  
  // ...

  // /add ANS case
  case ANS:
    m_pParser = new(m_pEnviron) class ANSScan(m_pFrame,this,
                 m_ucScanStart,m_ucScanStop,
                 m_ucLowBit + m_ucHiddenBits,0);
    break;
  // \add ANS case

  default:
    JPG_THROW(NOT_IMPLEMENTED,"Scan::CreateParser",
	      "sorry, the coding mode in the codestream is currently not supported");
  }
}


// in jpeg.cpp -----------------------------------------------------

/// JPEG::WriteInternal
// Write a file. Exceptions are thrown here and captured outside.
void JPEG::WriteInternal(struct JPG_TagItem *tags)
{ 
  LONG stopflags = tags->GetTagData(JPGTAG_ENCODER_STOP);

  if (m_pDecoder)
    JPG_THROW(OBJECT_EXISTS,"JPEG::WriteInternal","decoding in process, cannot start encoding");

  if (m_pImage == NULL)
    JPG_THROW(OBJECT_DOESNT_EXIST,"JPEG::WriteInternal","no image data loaded, use ProvideImage first");
  
  if (m_pEncoder == NULL) {
    m_pEncoder       = new(m_pEnviron) class Encoder(m_pEnviron);
    m_bEncoding      = true; 
    m_pFrame         = NULL;
    m_pScan          = NULL;
    m_bRow           = false;
    m_bDecoding      = false;
    m_bHeaderWritten = false;
    m_bOptimized     = false;
  }

  if (!m_bEncoding)
    return;

  if (m_pIOStream == NULL) {
    struct JPG_Hook *iohook = (struct JPG_Hook *)(tags->GetTagPtr(JPGTAG_HOOK_IOHOOK));
    if (iohook == NULL)
      JPG_THROW(OBJECT_DOESNT_EXIST,"JPEG::WriteInternal","no IOHook defined to write the data to");

    m_pIOStream = new(m_pEnviron) class IOStream(m_pEnviron,tags);
  }

  assert(m_pIOStream);
  
  if (m_bHeaderWritten == false) {
    m_pImage->WriteHeader(m_pIOStream);
    m_bHeaderWritten = true;
    if (stopflags & JPGFLAG_ENCODER_STOP_IMAGE)
      return;
  }

  assert(m_pImage);


  if (!m_bOptimized) {
    if (m_bOptimizeHuffman || (tags->GetTagData(JPGTAG_IMAGE_FRAMETYPE) & JPGFLAG_OPTIMIZE_HUFFMAN)) {
      do {
  class Frame *frame = m_pImage->StartMeasureFrame();
  do {
    class Scan *scan = frame->StartMeasureScan();
    while(scan->StartMCURow()) { 
      while(scan->WriteMCU()) {
        ;
      }
    }
    scan->Flush();
  } while(frame->NextScan());
      } while(m_pImage->NextFrame());
    }
    m_bOptimized = true;
    m_pImage->ResetToFirstFrame();
  }

  while(m_bEncoding) {
    if (m_pFrame == NULL) {
      m_pFrame = m_pImage->StartWriteFrame(m_pIOStream);
      if (stopflags & JPGFLAG_ENCODER_STOP_FRAME)
  return;
    }
    assert(m_pFrame);

    if (m_pScan == NULL) {
      m_pScan = m_pFrame->StartWriteScan(m_pIOStream);
      if (stopflags & JPGFLAG_ENCODER_STOP_SCAN)
  return;
    }
    assert(m_pScan);

    if (!m_bRow) {
      if (m_pScan->StartMCURow()) {
  m_bRow = true;
  if (stopflags & JPGFLAG_ENCODER_STOP_ROW)
    return;
      } else {
  // Scan done, flush it out.
  m_pScan->Flush();
  m_pScan = NULL;
  m_pFrame->WriteTrailer(m_pIOStream);
  if (!m_pFrame->NextScan()) {
    m_pFrame = NULL;
    if (!m_pImage->NextFrame()) {
      m_pImage->WriteTrailer(m_pIOStream);
      m_pIOStream->Flush();
      m_bEncoding = false;
      return;
    }
  }
      }
    }

    if (m_bRow) {
      while (m_pScan->WriteMCU()) {
  if (stopflags & JPGFLAG_ENCODER_STOP_MCU)
    return;
      }
      m_bRow = false;
    }
  }
}
///

看scan被谁调用了
// 需要实现方法
scan的那些方法被调用了
m_pScan->StartMCURow()  return m_pParser->StartMCURow();
m_pScan->ParseMCU() return m_pParser->ParseMCU();
scan->WriteMCU() return m_pParser->WriteMCU();
m_pScan->Flush() m_pParser->Flush(true);
成员的相关方法需要实现
class EntropyParser   *m_pParser;
通过看一个最简单的parser的实现了解如何对ANS进行封装

需要先对基类parser进行了解

/*************************************************************************
** Copyright (c) 2011-2012 Accusoft                                     **
** This program is free software, licensed under the GPLv3              **
** see README.license for details                                       **
**                  **
** For obtaining other licenses, contact the author at                  **
** thor@math.tu-berlin.de                                               **
**                                                                      **
** Written by Thomas Richter (THOR Software)                            **
** Sponsored by Accusoft, Tampa, FL and         **
** the Computing Center of the University of Stuttgart                  **
**************************************************************************

This software is a complete implementation of ITU T.81 - ISO/IEC 10918,
also known as JPEG. It implements the standard in all its variations,
including lossless coding, hierarchical coding, arithmetic coding and
DNL, restart markers and 12bpp coding.

In addition, it includes support for new proposed JPEG technologies that
are currently under discussion in the SC29/WG1 standardization group of
the ISO (also known as JPEG). These technologies include lossless coding
of JPEG backwards compatible to the DCT process, and various other
extensions.

The author is a long-term member of the JPEG committee and it is hoped that
this implementation will trigger and facilitate the future development of
the JPEG standard, both for private use, industrial applications and within
the committee itself.

  Copyright (C) 2011-2012 Accusoft, Thomas Richter <thor@math.tu-berlin.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/
/*
**
** Represents the lossless scan - lines are coded directly with predictive
** coding, though here residuals are encoded with the arithmetic encoder.
**
** $Id: aclosslessscan.hpp,v 1.19 2012-09-22 20:51:40 thor Exp $
**
*/

#ifndef CODESTREAM_ACLOSSLESSSCAN_HPP
#define CODESTREAM_ACLOSSLESSSCAN_HPP

/// Includes
#include "tools/environment.hpp"
#include "coding/qmcoder.hpp"
#include "codestream/entropyparser.hpp"
#include "codestream/predictivescan.hpp"
///

/// Forwards
class Frame;
class LineCtrl;
class ByteStream;
class LineBitmapRequester;
class LineBuffer;
class BitmaCtrl;
class Scan;
///

/// class ANSScan
// C++ fse wrappers
class ANSScan : public EntropyParser {
  
  // The real worker class.
  class QMCoder              m_Coder;
  //
  // This is actually the true MCU-parser, not the interface that reads
  // a full line.
  void ParseMCU(struct Line **prev,struct Line **top,UBYTE preshift);
  //
  // The actual MCU-writer, write a single group of pixels to the stream,
  // or measure their statistics.
  void WriteMCU(struct Line **prev,struct Line **top,UBYTE preshift);
  //
  // Flush the remaining bits out to the stream on writing.
  virtual void Flush(bool final); 
  // 
  // Restart the parser at the next restart interval
  virtual void Restart(void);
  //
  //
public:
  ACLosslessScan(class Frame *frame,class Scan *scan,UBYTE predictor,UBYTE lobit,
     bool differential = false);
  //
  virtual ~ACLosslessScan(void);
  //
  // Write the marker that indicates the frame type fitting to this scan.
  virtual void WriteFrameType(class ByteStream *io);
  //
  // Fill in the tables for decoding and decoding parameters in general.
  virtual void StartParseScan(class ByteStream *io,class BufferCtrl *ctrl);
  //
  // Write the default tables for encoding
  virtual void StartWriteScan(class ByteStream *io,class BufferCtrl *ctrl);
  //
  // Start the measurement run - not required here.
  virtual void StartMeasureScan(class BufferCtrl *ctrl);
  //
  // Start a MCU scan. Returns true if there are more rows. False otherwise.
  // Note that we emulate here that MCUs are multiples of eight lines high
  // even though from a JPEG perspective a MCU is a single pixel in the
  // lossless coding case.
  virtual bool StartMCURow(void);
  //
  // Parse a single MCU in this scan. Return true if there are more
  // MCUs in this row.
  virtual bool ParseMCU(void);
  //
  // Write a single MCU in this scan.
  virtual bool WriteMCU(void);  
};
///


///
#endif
