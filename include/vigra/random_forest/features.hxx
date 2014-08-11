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
    MultiArrayShape<2>::type f_shape_;       // shape of feature array (original)
    MultiArrayShape<2>::type im_shape_;      // shape of original image (i.e. # of pixels in x,y)

    FeatureBase(MultiArrayView<2, T, C> const & original,
                MultiArrayShape<2>::type  im_shape)
        : original_(original), f_shape_(original.size(0), original.size(1)), im_shape_(im_shape)
    {}

    virtual ~FeatureBase() { }

    MultiArrayShape<2>::type shape() const
    {
        return f_shape_;
    }

    MultiArrayIndex shape(int n) const
    {
        return f_shape_[n];
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
        // could add test that i,j are "inbounds" w.r.t. im_shape_
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
        int y = (int)floor(i/FeatureBase<T,C>::im_shape_[0]);
        int xp = x + offset_x_;
        int yp = y + offset_y_;

        // deal with out of bounds indices.  just move them back to the border of the image.
        // IN FUTURE, DO SOMETHING SMARTER HERE.  E.G., REFLECT IMAGE OVER THE BORDER?
        if (xp >= FeatureBase<T,C>::im_shape_[0]) xp = FeatureBase<T,C>::im_shape_[0]-1;
        if (yp >= FeatureBase<T,C>::im_shape_[1]) yp = FeatureBase<T,C>::im_shape_[1]-1;

        // convert back to index into feature array
        int ip = yp*FeatureBase<T,C>::im_shape_[0] + xp;
        return FeatureBase<T,C>::original_(ip, j);
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
        int y = (int)floor(i/FeatureBase<T,C>::im_shape_[0]);
        int xp = x + offset_x_;
        int yp = y + offset_y_;

        // deal with out of bounds indices.  just move them back to the border of the image.
        // IN FUTURE, DO SOMETHING SMARTER HERE.  E.G., REFLECT IMAGE OVER THE BORDER?
        if (xp >= FeatureBase<T,C>::im_shape_[0]) xp = FeatureBase<T,C>::im_shape_[0]-1;
        if (yp >= FeatureBase<T,C>::im_shape_[1]) yp = FeatureBase<T,C>::im_shape_[1]-1;

        // convert back to index into feature array
        int ip = yp*FeatureBase<T,C>::im_shape_[0] + xp;

        return FeatureBase<T,C>::original_(ip, j) - FeatureBase<T,C>::original_(i, j);
    }
};


}

#endif

//template<int N, class T, class C= UnstridedArrayTag>
//class NeighborFeatures
//{
//public:
//    typedef typename MultiArrayShape<N>::type Shp;
//    typedef T value_type;
//    MultiArrayView<N, T, C> raw_data;
//    ArrayVector<Shp >
//                            feat_coos;
//    ArrayVector<int>
//                            offsets;

//    /** returns shape of feature matrix.
//     */
//    MultiArrayShape<2>::type shape() const
//    {
//        retur MultiArrayShape<2>::type(raw_data.size(),
//                                        feat_coos.size());
//    }


//    /** return shape along dimension i
//     */
//    int shape(int index) const
//    {
//        return shape()[index];
//    }

//    /**\brief value access operator
//     *
//     * returns the jth neighbor of the ith point in raw_data
//     * (scan order). If no jth neighbor exists (border)
//     * use the point itself
//     */
//    T & operator() (int i, int j)
//    {
//        int offset = i + offsets[j];
//        //check if the neighbor is in bounds.
//        offset = offset < 0 || offset >= raw_data.size() ?
//                    i : offset;
//#ifdef I_KNOW_WHAT_I_AM_DOING
//        return raw_data.data()[offset];
//#else
//        return raw_data[offset];
//#endif
//    }

//    /**\brief const value access operator
//     * \sa NeighborFeatures::operator()
//     */
//    T const & operator() (int i, int j) const
//    {
//        int offset = i + offsets[j];
//        offset = offset < 0 || offset >= raw_data.size() ?
//                    i : offset;
//#ifdef I_KNOW_WHAT_I_AM_DOING
//        return raw_data.data()[offset];
//#else
//        return raw_data[offset];
//#endif
//    }

//    /** Constructor
//     *
//     * \param in    the raw_data
//     * \param coos  a n x m matrix that contains relative
//     *              coordinates of the ith neighbor in the ith row.
//     *              m must be = N
//     */
//    NeighborFeatures(MultiArrayView<N, T, C> & in,
//                     MultiArrayView<2, int> const & coos)
//        : raw_data(in)
//    {
//        for(int ii = 0; ii < coos.shape(0); ++ii)
//        {
//            feat_coos.push_back(Shp());

//            for(int jj = 0; jj < coos.shape(1); ++jj)
//                feat_coos.back()[jj] = coos(ii, jj);
//            offsets.push_back(raw_data.coordinateToScanOrderIndex(feat_coos.back()));
//        }
//    }
//};
//#undef I_KNOW_WHAT_I_AM_DOING
