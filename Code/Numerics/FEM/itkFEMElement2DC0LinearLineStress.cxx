/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFEMElement2DC0LinearLineStress.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// disable debug warnings in MS compiler
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "itkFEMElement2DC0LinearLineStress.h"

namespace itk {
namespace fem {




Element2DC0LinearLineStress
::Element2DC0LinearLineStress() : Superclass()
{
}

Element2DC0LinearLineStress
::Element2DC0LinearLineStress(
      NodeIDType n1_,
      NodeIDType n2_,
      Material::ConstPointer m_) : Superclass()
{
  // Set the geometrical points
  this->SetNode( 0, n1_ );
  this->SetNode( 1, n2_ );

  /*
   * Initialize the pointer to material object and check that
   * we were given the pointer to the right class.
   * If the material class was incorrect an exception is thrown.
   */
  if( (m_mat=dynamic_cast<const MaterialLinearElasticity*>(&*m_)) == 0 )
  {
    throw FEMExceptionWrongClass(__FILE__,__LINE__,"Element2DC0LinearLineStress::Element2DC0LinearLineStress()");
  }
}




FEM_CLASS_REGISTER(Element2DC0LinearLineStress)




}} // end namespace itk::fem
