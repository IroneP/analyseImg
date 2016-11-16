/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/**
 * @version 1.0
 */

#ifndef HOUGH_H
#define HOUGH_H

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

 // System
#include <cstdio>
#include <cmath>

// OpenCV
#ifdef _WIN32
    #include <opencv/cv.hpp>
#else
    #include <cv.h>
#endif

// STL
#include <vector>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

 /******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

/******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/

 /**
  * @class Hough
  */

class Hough
{

    /**************************************************************************
     ***************************** PUBLIC SECTION *****************************
     **************************************************************************/

public:

    /****************************** INNER TYPES *******************************/

    /******************************* ATTRIBUTES *******************************/

    /******************************** METHODS *********************************/

    /**
     * Constructor
     */
    Hough();

    /**
     * make a vote for every segment possible
     *
     * @param image image to analize
     *
     * @return the number of vote for every segment
     */
    cv::Mat CreateSegmentAccumulator( const cv::Mat& image );

    /**
     * return a matrice with all the segment that are declared valide
     *
     * @param accu accumulator contains number of vote for every segment
     * @param rows number of rows for the image
     * @param cols number of cols for the image
     * @param nbMinPoint minimum number of vote to declare a segment valide
     *
     * @return the number of vote for every segment
     */
    cv::Mat getSegmentFromAccumulator(cv::Mat& accu, const int rows, const int cols, const int nbMinPoint );

    /**
     * Cut every line out of the form
     *
     * @param image to do modification
     * @param src original image
     */
    void limitSegment(cv::Mat& image, const cv::Mat& src );

    /**
     * draw only most important lines
     *
     * @param accumulator contains number of vote for every segment
     * @param nbLines number maximum of lines to draw
     *
     * @return the minimum numbre of points
     */
    int segmentThreshold(cv::Mat& accu, int nbLines );

    /**
     * Generate the Hough accumulator for circle detection,
     * given a user defined radius
     *
     * @param pImage input image
     * @param pRadius circle radius
     *
     * @return the Hough accumulator for circle detection
     */
    cv::Mat generateCircleAccumulator( const cv::Mat& pImage, float pRadius );

    /**
     * Generate the Hough accumulator for circle detection
     *
     * @param pImage input image
     *
     * @return the Hough accumulator for circle detection
     */
    cv::Mat generateCircleAccumulator( const cv::Mat& pImage );

    /**
     * Extract circles from the Hough accumulator,
     * based on most significant values (votes)
     *
     * @param pImage input accumulator
     *
     * @return an image with extracted circles
     */
    cv::Mat extractCirclesFromAccumulator( const cv::Mat& pAccumulator, float radius, unsigned int pVoteCriteria, int nbRows, int nbColumns );

    /**
     * Extract circles from the Hough accumulator,
     * based on most significant values (votes)
     *
     * @param pImage input accumulator
     *
     * @return an image with extracted circles
     */
    cv::Mat extractCirclesFromAccumulator( const cv::Mat& pAccumulator, unsigned int pVoteCriteria, int nbRows, int nbColumns );

    /**************************************************************************
     **************************** PROTECTED SECTION ***************************
     **************************************************************************/

protected:

    /****************************** INNER TYPES *******************************/

    /******************************* ATTRIBUTES *******************************/

    /******************************** METHODS *********************************/

    /**
     * Bresenham
     * Draw a line between two points
     *
     * @param image matrice to draw
     * @param x1, y1 coord of the first point
     * @param x2, y2 coord of the second point
     * @param value the value for this segment, the more a line has been voted, the more the value will be
     */
    void bresenham( cv::Mat* image, int x1, int y1, int x2, int y2 , uchar value );

    /**************************************************************************
     ***************************** PRIVATE SECTION ****************************
     **************************************************************************/

private:

    /****************************** INNER TYPES *******************************/

    /******************************* ATTRIBUTES *******************************/

    /******************************** METHODS *********************************/

};

/**************************************************************************
 ***************************** INLINE SECTION *****************************
 **************************************************************************/

#endif // HOUGH_H
