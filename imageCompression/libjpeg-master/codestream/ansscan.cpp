// Written by Zhenqiang Ying, for adding ANS entropy codec approach to JPEG
/*************************************************************************
** Copyright (c) 2011-2012 Accusoft                                     **
** This program is free software, licensed under the GPLv3              **
** see README.license for details                                       **
**									**
** For obtaining other licenses, contact the author at                  **
** thor@math.tu-berlin.de                                               **
**                                                                      **
** Written by Thomas Richter (THOR Software)                            **
** Sponsored by Accusoft, Tampa, FL and					**
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
** ANS entropy codec approach 
**
*/
/// ANSSequentialScan::ANSSequentialScan
/// Includes
#include "codestream/sequentialscan.hpp"
#include "codestream/tables.hpp"
#include "marker/frame.hpp"
#include "marker/component.hpp"
#include "coding/quantizedrow.hpp"
#include "codestream/rectanglerequest.hpp"
#include "dct/idct.hpp"
#include "dct/sermsdct.hpp"
#include "std/assert.hpp"
#include "interface/bitmaphook.hpp"
#include "interface/imagebitmap.hpp"
#include "colortrafo/colortrafo.hpp"
#include "tools/traits.hpp"
#include "control/blockbuffer.hpp"
#include "control/blockbitmaprequester.hpp"
#include "control/blocklineadapter.hpp"
ANSSequentialScan::ANSSequentialScan(class Frame *frame,class Scan *scan,
           UBYTE start,UBYTE stop,UBYTE lowbit,UBYTE,
           bool differential,bool residual)
  : EntropyParser(frame,scan), m_pBlockCtrl(NULL),
    m_ucScanStart(start), m_ucScanStop(stop), m_ucLowBit(lowbit),
    m_bMeasure(false), m_bDifferential(differential), m_bResidual(residual)
{
  m_ucCount = scan->ComponentsInScan();  //  m_ucCount = The number of components we have here.
  // construct ... 
  
}
///

/// ANSSequentialScan::~ANSSequentialScan
ANSSequentialScan::~ANSSequentialScan(void)
{
  // destruct ... 
}
///

// ANSSequentialScan::WriteMCU
// Write a single MCU in this scan. Return true if there are more blocks in this row.
bool ANSSequentialScan::WriteMCU(void)
{ 
  bool more = true;
  int c;

  assert(m_pBlockCtrl); 
  // 最简单直接采用ByteStream类BeginWriteMCU(m_Coder.ByteStreamOf()); // class QMCoder m_Coder; The QM coder doing the main work here.
  BeginWriteMCU(m_Stream.ByteStreamOf());
  

  for(c = 0;c < m_ucCount;c++) {
    class Component *comp    = m_pComponent[c];
    class QuantizedRow *q    = GetRow(comp->IndexOf());
    UBYTE mcux               = (m_ucCount > 1)?(comp->MCUWidthOf() ):(1);
    UBYTE mcuy               = (m_ucCount > 1)?(comp->MCUHeightOf()):(1);
    ULONG xmin               = m_ulX[c];
    ULONG xmax               = xmin + mcux;
    ULONG x,y; 
    if (xmax >= q->WidthOf()) {
      more     = false;
    }
    for(y = 0;y < mcuy;y++) {
      for(x = xmin;x < xmax;x++) {
        LONG *block,dummy[64];
        if (q && x < q->WidthOf()) {
          block  = q->BlockAt(x)->m_Data;
        } else {
          block  = dummy;
          memset(dummy ,0,sizeof(dummy) );
          block[0] = prevdc;
        }
        // 调用具体编码器进行编码
        // EncodeBlock(block,prevdc,prevdiff,l,u,kx,m_ucDCContext[c],m_ucACContext[c]); 
	    // 改为不进行编码，直接输出
        // --------------------------------------------------------------------------
      }
      if (q) q = q->NextOf();
    }
    // Done with this component, advance the block.
    m_ulX[c] = xmax;
  }

  return more;
}
/// 

/// ANSSequentialScan::ParseMCU
// Parse a single MCU in this scan. Return true if there are more blocks in this row.
bool ANSSequentialScan::ParseMCU(void)
{
  bool more = true;
  int c;

  assert(m_pBlockCtrl);

  bool valid = BeginReadMCU(m_Stream.ByteStreamOf());
  
  for(c = 0;c < m_ucCount;c++) {
    class Component *comp    = m_pComponent[c];
    class QuantizedRow *q    = GetRow(comp->IndexOf());
    LONG &prevdc             = m_lDC[c];
    UBYTE mcux               = (m_ucCount > 1)?(comp->MCUWidthOf() ):(1);
    UBYTE mcuy               = (m_ucCount > 1)?(comp->MCUHeightOf()):(1);
    ULONG xmin               = m_ulX[c];
    ULONG xmax               = xmin + mcux;
    ULONG x,y;
    if (xmax >= q->WidthOf()) {
      more     = false;
    }
    for(y = 0;y < mcuy;y++) {
      for(x = xmin;x < xmax;x++) {
        LONG *block,dummy[64];
        if (q && x < q->WidthOf()) {
          block  = q->BlockAt(x)->m_Data;
        } else {
          block  = dummy;
        }
        if (valid) {

          // 调用具体编码器进行编码
              // DecodeBlock(block,prevdc,prevdiff,l,u,kx,m_ucDCContext[c],m_ucACContext[c]);

              // --------------------------------------------------------------------------
        } else {
          for(UBYTE i = m_ucScanStart;i <= m_ucScanStop;i++) {
            block[i] = 0;
          }
        }
      }
      if (q) q = q->NextOf();
    }
    // Done with this component, advance the block.
    m_ulX[c] = xmax;
  }

  return more;
}
///

/// ANSSequentialScan::EncodeBlock
// Encode a single huffman block
void ANSSequentialScan::EncodeBlock(const LONG *block,
/*参数根据需要设置*/
           LONG &prevdc,LONG &prevdiff,
           UBYTE small,UBYTE large,UBYTE kx,UBYTE dc,UBYTE ac)
void SequentialScan::EncodeBlock(const LONG *block,
				 class HuffmanCoder *dc,class HuffmanCoder *ac,
				 LONG &prevdc,UWORD &skip)
{
// 直流系数一般较大，可以利用相邻直流系数比较接近的特点进行差值编码
// 按照首位为1进行编码（即截断二进制编码），否则输出0，解码时看是0，就说明是0，看是1，先读有几位，再读入值
// 交流系数采用zig扫描后采用游程编码，最后再进行哈夫曼编码
// 前面都是利用了数据的规律性，最后一步哈夫曼编码则是利用统计冗余，属于熵编码
  // DC coding
  if (m_ucScanStart == 0 && m_bResidual == false) {
    UBYTE symbol = 0;
    LONG diff;
    //
    // DPCM coding of the DC coefficient.
    diff   = block[0] >> m_ucLowBit; // Actually, only correct for two's complement machines...
    diff  -= prevdc;
    if (m_bDifferential) {
      prevdc = 0;
    } else {
      prevdc = block[0] >> m_ucLowBit;
    }

    if (diff) { // 编码差值
      do {
			symbol++;
			if (diff > -(1L << symbol) && diff < (1L << symbol)) {
		  		dc->Put(&m_Stream,symbol);
		  		if (diff >= 0) {
		    		m_Stream.Put(symbol,diff);
		  		} else {
			    	m_Stream.Put(symbol,diff - 1);
			  	}
		  		break;
			}
      } while(true);
    } else {
      m_Stream.Put(symbol,0); //dc->Put(&m_Stream,0);
    }
  }
  
  // AC coding
  if (m_ucScanStop) {
    UBYTE symbol,run = 0;
    int k = (m_ucScanStart)?(m_ucScanStart):((m_bResidual)?0:1);

    do {
      LONG data = block[DCT::ScanOrder[k]];
      // Implement the point transformation. This is here a division, not
      // a shift (rounding is different for negative numbers).
      data = (data >= 0)?(data >> m_ucLowBit):(-((-data) >> m_ucLowBit));
      if (data == 0) {
	run++;
      } else {
	// Are there any skipped blocks we still need to code? Since this
	// block is none of them.
	if (skip)
	  CodeBlockSkip(ac,skip);
	//
	// First ensure that the run is at most 15, the largest cathegory.
	while(run > 15) {
	  ac->Put(&m_Stream,0xf0); // r = 15 and s = 0
	  run -= 16;
	}
	symbol = 0;
	do {
	  symbol++;
	  if (data > -(1L << symbol) && data < (1L << symbol)) {
	    assert(symbol < 16);
	    // Cathegory symbol, run length run
	    ac->Put(&m_Stream,symbol | (run << 4));
	    if (data >= 0) {
	      m_Stream.Put(symbol,data);
	    } else {
	      m_Stream.Put(symbol,data - 1);
	    }
	    break;
	  }
	} while(true);
	//
	// Run is over.
	run = 0;
      }
    } while(++k <= m_ucScanStop);

    // Is there still an open run? If so, code an EOB in the regular mode.
    // If this is part of the (isolated) AC scan of the progressive JPEG,
    // check whether we could potentially accumulate this into a run of
    // zero blocks.
    if (run) {
      // Include in a block skip (or try to, rather).
      if (m_ucScanStart || m_bResidual) {
	skip++;
	if (skip == MAX_WORD) // avoid an overflow, code now
	  CodeBlockSkip(ac,skip);
      } else {
	// In sequential mode, encode as EOB.
	ac->Put(&m_Stream,0x00);
      }
    }
  }
}
///

///

/// ANSSequentialScan::DecodeBlock
// Decode a single huffman block.
void ANSSequentialScan::DecodeBlock(LONG *block,
           LONG &prevdc,LONG &prevdiff,
           UBYTE small,UBYTE large,UBYTE kx,UBYTE dc,UBYTE ac)
{
  // DC coding
  if (m_ucScanStart == 0 && m_bResidual == false) {
    LONG diff;
    struct QMContextSet::DCContextZeroSet &cz = m_Context[dc].Classify(prevdiff,small,large);
    // Check whether the difference is nonzero.
    if (m_Coder.Get(cz.S0)) {
      LONG sz;
      bool sign = m_Coder.Get(cz.SS); // sign coding, is true for negative.
      //
      //
      // Positive and negative are encoded in different contexts.
      // Decode the magnitude cathegory.
      if (m_Coder.Get((sign)?(cz.SN):(cz.SP))) {
  int  i = 0;
  LONG m = 2;
  
  while(m_Coder.Get(m_Context[dc].DCMagnitude.X[i])) {
    m <<= 1;
    i++;
    if (m == 0) 
      JPG_THROW(MALFORMED_STREAM,"ANSSequentialScan::DecodeBlock",
          "QMDecoder is out of sync");
  }
  //
  // Get the MSB to decode.
  m >>= 1;
  sz  = m;
  //
  // Refinement coding of remaining bits.
  while((m >>= 1)) {
    if (m_Coder.Get(m_Context[dc].DCMagnitude.M[i])) {
      sz |= m;
    }
  }
      } else {
  sz = 0;
      }
      //
      // Done, finally, include the sign and the offset.
      if (sign) {
  diff = -sz - 1;
      } else {
  diff = sz + 1;
      }
    } else {
      // Difference is zero.
      diff = 0;
    }

    prevdiff = diff;
    if (m_bDifferential) {
      prevdc   = diff;
    } else {
      prevdc  += diff;
    }
    block[0] = prevdc << m_ucLowBit; // point transformation
  }

  if (m_ucScanStop) {
    // AC coding. No block skipping used here.
    int k = (m_ucScanStart)?(m_ucScanStart):((m_bResidual)?0:1);
    //
    // EOB decoding.
    while(k <= m_ucScanStop && !m_Coder.Get(m_Context[ac].ACZero[k-1].SE)) {
      LONG sz;
      bool sign;
      //
      // Not yet EOB. Run coding in S0: Skip over zeros.
      while(!m_Coder.Get(m_Context[ac].ACZero[k-1].S0)) {
  k++;
  if (k > m_ucScanStop)
    JPG_THROW(MALFORMED_STREAM,"ANSSequentialScan::DecodeBlock",
        "QMDecoder is out of sync");
      }
      //
      // Now decode the sign of the coefficient.
      // This happens in the uniform context.
      sign = m_Coder.Get(m_Context[ac].Uniform);
      //
      // Decode the magnitude.
      if (m_Coder.Get(m_Context[ac].ACZero[k-1].SP)) {
  // X1 coding, identical to SN and SP.
  if (m_Coder.Get(m_Context[ac].ACZero[k-1].SP)) {
    int  i = 0;
    LONG m = 4;
    struct QMContextSet::ACContextMagnitudeSet &acm = (k > kx)?(m_Context[ac].ACMagnitudeHigh):(m_Context[ac].ACMagnitudeLow);
    
    while(m_Coder.Get(acm.X[i])) {
      m <<= 1;
      i++;
      if (m == 0)
        JPG_THROW(MALFORMED_STREAM,"ANSSequentialScan::DecodeBlock",
      "QMDecoder is out of sync");
    }
    //
    // Get the MSB to decode
    m >>= 1;
    sz  = m;
    //
    // Proceed to refinement.
    while((m >>= 1)) {
      if (m_Coder.Get(acm.M[i])) {
        sz |= m;
      }
    }
  } else {
    sz = 1;
  }
      } else {
  sz = 0;
      }
      //
      // Done. Finally, include sign and offset.
      sz++;
      if (sign) 
  sz = -sz;
      block[DCT::ScanOrder[k]] = sz << m_ucLowBit;
      //
      // Proceed to the next block.
      k++;
    }
  }
}
///

/// ANSSequentialScan::StartWriteScan
 // Write the marker to the stream.
void ANSSequentialScan::StartWriteScan(class ByteStream *io,class BufferCtrl *ctrl)
{ 
  int i;

  for(i = 0;i < m_ucCount;i++) {
    // 读参数的逆过程

    m_lDC[i]           = 0;
   
    m_ulX[i]           = 0;
  }
  restart();

  assert(!ctrl->isLineBased());
  m_pBlockCtrl = dynamic_cast<BlockBuffer *>(ctrl);
  m_pBlockCtrl->ResetToStartOfScan(m_pScan);

  m_pScan->WriteMarker(io);
  m_Stream.OpenForWrite(io);
  // 其实就是restart();m_Coder.OpenForWrite(io);
}
///

/// ANSSequentialScan::StartParseScan
// Parse the marker contents.
void ANSSequentialScan::StartParseScan(class ByteStream *io,class BufferCtrl *ctrl)
{ 
  class ACTemplate *ac,*dc;
  int i;

  for(i = 0;i < m_ucCount;i++) {
  // 调用m_pScan获取相关参数
    

    m_lDC[i]         = 0; 
    // m_lDiff[i]       = 0;
    m_ulX[i]         = 0;
  }
  
  // 
  restart();
  
  assert(!ctrl->isLineBased());
  m_pBlockCtrl = dynamic_cast<BlockBuffer *>(ctrl);
  m_pBlockCtrl->ResetToStartOfScan(m_pScan);

  m_Stream.OpenForRead(io);// m_Coder.OpenForRead(io);
}
///


/// ANSSequentialScan::StartMCURow
// Start a MCU scan. Returns true if there are more rows.
bool ANSSequentialScan::StartMCURow(void)
{
  bool more = StartRow();

  for(int i = 0;i < m_ucCount;i++) {
    m_ulX[i]   = 0;
  }

  return more;
}
///

/// ANSSequentialScan::Restart
// Restart the parser at the next restart interval
void ANSSequentialScan::Restart(void)
{
  for(int i = 0;i < m_ucCount;i++) {
    m_lDC[i]         = 0; 
    //m_lDiff[i]       = 0;
  }

  
  m_Stream.OpenForRead(m_Stream.ByteStreamOf());
}
///

/// ANSSequentialScan::Flush
// Flush the remaining bits out to the stream on writing.
void ANSSequentialScan::Flush(bool)
{
  
  
  m_Stream.Flush();//m_Coder.Flush();

  Restart();

}
///

/// ANSSequentialScan::WriteFrameType
// Write the marker that indicates the frame type fitting to this scan.
void ANSSequentialScan::WriteFrameType(class ByteStream *io)
{
  UBYTE hidden = m_pFrame->TablesOf()->HiddenDCTBitsOf();

  if (m_ucScanStart > 0 || m_ucScanStop < 63 || m_ucLowBit > hidden) {
    // is progressive. // ans do not support progressive yet.
    if (m_bDifferential){
      io->PutWord(0xffce);
    } else {
      io->PutWord(0xffca);
    }
  } else {
    if (m_bDifferential){
      io->PutWord(JPGCODE_SOF_ANS_DIF_SEQ); // AC differential sequential
    } else {
      io->PutWord(JPGCODE_SOF_ANS_SEQ); // AC sequential
    }
  }
}

///


/// 下面不需要修改-----------------------------------------------------------

/// ANSSequentialScan::GetRow
// Return the data to process for component c. Will be overridden
// for residual scan types.
class QuantizedRow *ANSSequentialScan::GetRow(UBYTE idx) const
{
  return m_pBlockCtrl->CurrentQuantizedRow(idx);
}
///

/// ANSSequentialScan::StartRow
bool ANSSequentialScan::StartRow(void) const
{
  return m_pBlockCtrl->StartMCUQuantizerRow(m_pScan);
}
///

/// 可选方法 不必须-----------------------------------------------------------
/// ANSSequentialScan::StartMeasureScan
// Measure scan statistics.
void ANSSequentialScan::StartMeasureScan(class BufferCtrl *)
{ 
  //
  // This is not required.
  JPG_THROW(NOT_IMPLEMENTED,"ANSSequentialScan::StartMeasureScan",
      "arithmetic coding is always adaptive and does not require "
      "to measure the statistics");
}
///
/// 内部方法-----------------------------------------------------------