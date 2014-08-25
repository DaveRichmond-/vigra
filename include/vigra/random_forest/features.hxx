#ifndef RN_FEATURES_HXX
#define RN_FEATURES_HXX

#include <vigra/multi_array.hxx>

namespace vigra
{

template<class T, class C = StridedArrayTag>
class FeatureBase
{
public:
    MultiArrayView<2, T, C> original_;
    MultiArrayShape<2>::type im_shape_;      // shape of original image (i.e. # of pixels in x,y)

    FeatureBase(MultiArrayView<2, T, C> const & original,
                MultiArrayShape<2>::type  im_shape)
        : original_(original), im_shape_(im_shape)
    {}

    virtual ~FeatureBase() { }

    MultiArrayShape<2>::type shape() const
    {
        MultiArrayShape<2>::type(original_.size(0), original_.size(1));
    }

    int shape(int n) const
    {
        return shape()[n];
    }

    virtual T operator() (int i, int j) const = 0;
};

template<class T, class C = StridedArrayTag>
class NormalFeatures : public FeatureBase<T,C>
{
public:
    NormalFeatures(MultiArrayView<2, T, C> const & original,
                   MultiArrayShape<2>::type  im_shape)
        : FeatureBase<T,C>(original, im_shape)
    {}

    ~NormalFeatures() { }

    T operator() (int i, int j) const
    {
        return FeatureBase<T,C>::original_(i, j);
    }

};

template<class T, class C = StridedArrayTag>
class OffsetFeatures : public FeatureBase<T,C>
{
public:
    int offset_x_, offset_y_;

    OffsetFeatures(MultiArrayView<2, T, C> const & original,
                   MultiArrayShape<2>::type im_shape,
                   int offset_x,
                   int offset_y)
        : FeatureBase<T,C>(original, im_shape)
    {
        offset_x_ = offset_x;
        offset_y_ = offset_y;
    }

    ~OffsetFeatures() { }

    T operator() (int i, int j) const
    {
        // convert index i (sample #) to x,y position in image, and then shift
        int x = i % FeatureBase<T,C>::im_shape_[0];
        int y = static_cast<int>(floor(i/FeatureBase<T,C>::im_shape_[0]));
        int xp = x + offset_x_;
        int yp = y + offset_y_;

        // deal with out of bounds indices.  just move them back to the border of the image.
        // IN FUTURE, DO SOMETHING SMARTER HERE.  E.G., REFLECT IMAGE OVER THE BORDER?
        if (xp >= FeatureBase<T,C>::im_shape_[0]) xp = FeatureBase<T,C>::im_shape_[0]-1;
        else if (xp < 0) xp = 0;

        if (yp >= FeatureBase<T,C>::im_shape_[1]) yp = FeatureBase<T,C>::im_shape_[1]-1;
        else if (yp < 0) yp = 0;

        // convert back to index into feature array
        return FeatureBase<T,C>::original_(yp * FeatureBase<T,C>::im_shape_[0] + xp, j);
    }

};

template<class T, class C = StridedArrayTag>
class DiffFeatures : public FeatureBase<T,C>
{
public:
    int offset_x_, offset_y_;

    DiffFeatures(MultiArrayView<2, T, C> const & original,
                   MultiArrayShape<2>::type im_shape,
                   int offset_x,
                   int offset_y)
        : FeatureBase<T,C>(original, im_shape)
    {
        offset_x_ = offset_x;
        offset_y_ = offset_y;
    }

    ~DiffFeatures() { }

    T operator() (int i, int j) const
    {
        // convert index i (sample #) to x,y position in image, and then shift
        int x = i % FeatureBase<T,C>::im_shape_[0];
        int y = static_cast<int>(floor(i/FeatureBase<T,C>::im_shape_[0]));
        int xp = x + offset_x_;
        int yp = y + offset_y_;

        // deal with out of bounds indices.  just move them back to the border of the image.
        // IN FUTURE, DO SOMETHING SMARTER HERE.  E.G., REFLECT IMAGE OVER THE BORDER?
        if (xp >= FeatureBase<T,C>::im_shape_[0]) xp = FeatureBase<T,C>::im_shape_[0]-1;
        else if (xp < 0) xp = 0;

        if (yp >= FeatureBase<T,C>::im_shape_[1]) yp = FeatureBase<T,C>::im_shape_[1]-1;
        else if (yp < 0) yp = 0;

        // convert back to index into feature array
        return FeatureBase<T,C>::original_(yp * FeatureBase<T,C>::im_shape_[0] + xp, j) - FeatureBase<T,C>::original_(i, j);
    }
};


}

#endif
