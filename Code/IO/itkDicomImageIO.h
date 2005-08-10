/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkDicomImageIO.h
  Language:  C++
  Date:      $Date$
  Version:   $1.0$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDicomImageIO_h
#define __itkDicomImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkGDCMImageIO.h"

namespace itk
{

/** \brief Read DicomImage file format.
 *
 * NOTE: This reader has been replaced with GDCMDicomImageIO
 * \ingroup IOFilters
 *
 */
class ITK_EXPORT DicomImageIO : public GDCMImageIO
{
public:
  /** Standard class typedefs. */
  typedef DicomImageIO Self;
  typedef GDCMImageIO Superclass;
  typedef SmartPointer<Self>  Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DicomImageIO, Superclass);

protected:
  DicomImageIO()
    {
    itkWarningMacro (<< "DicomImageIO is now implemented as a subclass of GDCMImageIO. Please replace your DicomImageIO references with GDCMImageIO.");
    };
};

} // end namespace itk

#endif // __itkDicomImageIO_h
