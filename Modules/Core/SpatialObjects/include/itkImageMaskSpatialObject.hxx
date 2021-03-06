/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkImageMaskSpatialObject_hxx
#define itkImageMaskSpatialObject_hxx

#include "itkMath.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{
/** Constructor */
template< unsigned int TDimension, typename TPixel >
ImageMaskSpatialObject< TDimension, TPixel >
::ImageMaskSpatialObject()
{
  this->SetTypeName("ImageMaskSpatialObject");
}

/** Test whether a point is inside or outside the object
 *  For computational speed purposes, it is faster if the method does not
 *  check the name of the class and the current depth */
template< unsigned int TDimension, typename TPixel >
bool
ImageMaskSpatialObject< TDimension, TPixel >
::IsInsideInObjectSpace(const PointType & point) const
{
  typename Superclass::InterpolatorType::ContinuousIndexType index;
  if( this->GetImage()->TransformPhysicalPointToContinuousIndex( point,
      index ) )
    {
    using InterpolatorOutputType = typename InterpolatorType::OutputType;
    bool insideMask = (
      Math::NotExactlyEquals(
        DefaultConvertPixelTraits<InterpolatorOutputType>::GetScalarValue(
          this->GetInterpolator()->EvaluateAtContinuousIndex(index)),
        NumericTraits<PixelType>::ZeroValue() ) );
    if( insideMask )
      {
      return true;
      }
    }

  return false;
}

template< unsigned int TDimension, typename TPixel >
void
ImageMaskSpatialObject< TDimension, TPixel >
::ComputeMyBoundingBox()
{
  using IteratorType = ImageRegionConstIteratorWithIndex< ImageType >;
  IteratorType it( this->GetImage(),
    this->GetImage()->GetLargestPossibleRegion() );
  IteratorType prevIt( this->GetImage(),
    this->GetImage()->GetLargestPossibleRegion() );
  it.GoToBegin();
  prevIt = it;

  bool first = true;
  PixelType outsideValue = NumericTraits< PixelType >::ZeroValue();
  PixelType value = outsideValue;
  PixelType prevValue = outsideValue;
  IndexType tmpIndex;
  PointType tmpPoint;
  int count = 0;
  int rowSize
    = this->GetImage()->GetLargestPossibleRegion().GetSize()[0];
  while ( !it.IsAtEnd() )
    {
    value = it.Get();
    if ( value != prevValue || ( count == rowSize-1 && value != outsideValue ) )
      {
      prevValue = value;
      if( value == outsideValue )
        {
        tmpIndex = prevIt.GetIndex();
        }
      else
        {
        tmpIndex = it.GetIndex();
        }
      this->GetImage()->TransformIndexToPhysicalPoint( tmpIndex, tmpPoint );
      if( first )
        {
        first = false;
        this->GetModifiableMyBoundingBoxInObjectSpace()->SetMinimum( tmpPoint );
        this->GetModifiableMyBoundingBoxInObjectSpace()->SetMaximum( tmpPoint );
        }
      else
        {
        this->GetModifiableMyBoundingBoxInObjectSpace()->ConsiderPoint( tmpPoint );
        }
      }
    prevIt = it;
    ++it;
    ++count;
    if( count == rowSize )
      {
      count = 0;
      prevValue = outsideValue;
      }
    }

  if( first )
    {
    tmpPoint.Fill(
      NumericTraits< typename BoundingBoxType::PointType::ValueType >::ZeroValue() );

    this->GetModifiableMyBoundingBoxInObjectSpace()->SetMinimum( tmpPoint );
    this->GetModifiableMyBoundingBoxInObjectSpace()->SetMaximum( tmpPoint );

    // NOT AN EXCEPTION!!!, used to return false, but never checked
    // itkExceptionMacro(<< "ImageMask bounding box computation failed.")
    }
}

/** InternalClone */
template< unsigned int TDimension, typename TPixel >
typename LightObject::Pointer
ImageMaskSpatialObject< TDimension, TPixel >
::InternalClone() const
{
  // Default implementation just copies the parameters from
  // this to new transform.
  typename LightObject::Pointer loPtr = Superclass::InternalClone();

  typename Self::Pointer rval =
    dynamic_cast<Self *>(loPtr.GetPointer());
  if(rval.IsNull())
    {
    itkExceptionMacro(<< "downcast to type "
                      << this->GetNameOfClass()
                      << " failed.");
    }

  return loPtr;
}

/** Print the object */
template< unsigned int TDimension, typename TPixel >
void
ImageMaskSpatialObject< TDimension, TPixel >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


template< unsigned int TDimension, typename TPixel >
typename ImageMaskSpatialObject< TDimension, TPixel >::RegionType
ImageMaskSpatialObject< TDimension, TPixel >
::ComputeMyBoundingBoxInIndexSpace() const
{
  const ImagePointer imagePointer = this->GetImage();

  if (imagePointer == nullptr)
  {
    return {};
  }

  const ImageType& image = *imagePointer;

  const auto HasForegroundPixels = [&image](const RegionType& region)
  {
    for (ImageRegionConstIterator<ImageType> it{ &image, region }; !it.IsAtEnd(); ++it)
    {
      constexpr auto zeroValue = NumericTraits<PixelType>::ZeroValue();

      if (it.Get() != zeroValue)
      {
        return true;
      }
    }
    return false;
  };

  const auto CreateRegion = [](const IndexType& minIndex, const IndexType& maxIndex)
  {
    SizeType regionSize;

    for (unsigned dim = 0; dim < SizeType::Dimension; ++dim)
    {
      regionSize[dim] = static_cast<SizeValueType>(maxIndex[dim] + 1 - minIndex[dim]);
    }
    return RegionType{ minIndex, regionSize };
  };

  const RegionType requestedRegion = image.GetRequestedRegion();

  if (requestedRegion.GetNumberOfPixels() == 0)
  {
    return {};
  }

  const SizeType imageSize = requestedRegion.GetSize();

  IndexType minIndex = requestedRegion.GetIndex();
  IndexType maxIndex = minIndex + imageSize;

  for (auto& maxIndexValue : maxIndex)
  {
    --maxIndexValue;
  }

  // Iterate from high to low (for significant performance reasons).
  for (int dim = TDimension - 1; dim >= 0; --dim)
  {
    auto subregion = CreateRegion(minIndex, maxIndex);
    subregion.SetSize(dim, 1);
    const auto initialMaxIndexValue = maxIndex[dim];

    // Estimate minIndex[dim]
    while (!HasForegroundPixels(subregion))
    {
      const auto indexValue = subregion.GetIndex(dim) + 1;

      if (indexValue > initialMaxIndexValue)
      {
        // The requested image region has only zero-valued pixels.
        return {};
      }
      subregion.SetIndex(dim, indexValue);
    }
    minIndex[dim] = subregion.GetIndex(dim);

    // Estimate maxIndex[dim]
    subregion.SetIndex(dim, initialMaxIndexValue);
    while (!HasForegroundPixels(subregion))
    {
      subregion.SetIndex(dim, subregion.GetIndex(dim) - 1);
    }
    maxIndex[dim] = subregion.GetIndex(dim);
  }
  return CreateRegion(minIndex, maxIndex);
}


#if ! defined ( ITK_LEGACY_REMOVE )
template< unsigned int TDimension, typename TPixel >
typename ImageMaskSpatialObject< TDimension, TPixel >::RegionType
ImageMaskSpatialObject< TDimension, TPixel >
::GetAxisAlignedBoundingBoxRegion() const
{
  return ComputeMyBoundingBoxInIndexSpace();
}
#endif //ITK_LEGACY_REMOVE
} // end namespace itk

#endif //__ImageMaskSpatialObject_hxx
