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
** Very low-complexity Hadamard-Transform for the residuals.
**
** $Id: hadamard.hpp,v 1.2 2012-07-29 17:00:39 thor Exp $
**
*/

#ifndef DCT_HADAMARD_HPP
#define DCT_HADAMARD_HPP

/// Includes
#include "tools/environment.hpp"
#include "dct/dct.hpp"
///

/// Forwards
class Quantization;
struct ImageBitMap;
///

/// class Hadamard
// Very low-complexity Hadamard-Transform for the residuals.
class Hadamard : public DCT {
  //
  // Bit assignment
  enum { 
    QUANTIZER_BITS    = 29  // bits for representing the quantizer
  };
  //
  // The (inverse) quantization tables, i.e. multipliers.
  LONG  m_plInvQuant[64];
  //
  // The quantizer tables.
  WORD  m_psQuant[64];
  //
  // Inverse DCT filter core.
  void InverseFilterCore(const LONG *source,LONG *d);
  //
  // Perform the forwards filtering
  void ForwardFilterCore(const LONG *d,LONG *target);
  //
  // Quantize a floating point number with a multiplier, round correctly.
  // Must remove FIX_BITS + INTER_BITS + 3
  static inline LONG Quantize(LONG n,LONG qnt)
  {
    if (n >= 0) {
      return   (n * QUAD(qnt) + (QUAD(1) << (QUANTIZER_BITS - 1))) >>
        (QUANTIZER_BITS);
    } else {
      // The -1 makes this the same rounding rule as a shift.
      return -((-n * QUAD(qnt) - 1 + (QUAD(1) << (QUANTIZER_BITS - 1))) >>
               (QUANTIZER_BITS));
    }
  }  
  //
  //
public:
  Hadamard(class Environ *env);
  //
  ~Hadamard(void);
  //
  // Use the quantization table defined here, scale them to the needs of the DCT and scale them
  // to the right size.
  virtual void DefineQuant(const UWORD *table);
  //
  // Run the DCT on a 8x8 block on the input data, giving the output table.
  virtual void TransformBlock(const LONG *source,LONG *target,LONG dcoffset);
  //
  // Run the inverse DCT on an 8x8 block reconstructing the data.
  virtual void InverseTransformBlock(LONG *target,const LONG *source,LONG dcoffset);
};
///

///
#endif
