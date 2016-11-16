/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/**
 * @version 1.0
 */

#include "Hough.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <iostream>
#include <set>

// OpenCV
#ifdef WIN32
    #include <opencv/highgui.h>
#else
    #include <highgui.h>
#endif

/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

// STL
using namespace std;

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

// Angles
#define PI 3.141593f
#define cRadToDeg ( 180.0f / PI )
#define cDegToRad ( PI / 180.0f )

#define SQRT_2 1.414213f

#define cEPSILLON 0.001f

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Constructor
 ******************************************************************************/
Hough::Hough()
{
}

/******************************************************************************
 * Make a vote for every segment possible
 *
 * @param image image to analize
 *
 * @return the number of vote for every segment
 ******************************************************************************/
cv::Mat Hough::CreateSegmentAccumulator( const cv::Mat& image )
{
    //rho maximum size
    const int maxRho = (int)sqrt( static_cast< float >( image.rows * image.rows + image.cols * image.cols ) );

    // Hough space parameters [rho, theta]
    // - theta range: [-pi/2, pi]
    float theta = 0.0f;
    //const float deltaTheta = (((3*PI)/2) / nbTetha + 0.5 );
    const float deltaTheta = ((3.0f*PI)/2.0f) * (1.0f/(float)maxRho);
    const int nbTetha = (int)((((3*PI)/2)) / (double)deltaTheta + 0.5);
    // - rho range: [0, diagonaleImage]
    int rho = 0;
    const float deltaRho = SQRT_2;
    const int nbRho = (int)( maxRho / deltaRho + 0.5f );

    // Accumulator
    // - BEWARE : datatype is uchar so max value is 255 (check if this is valid)
    cv::Mat accumulator = cv::Mat( nbTetha, nbRho, CV_8U/*uchar type*/ );

    // Initiaize accumulator to 0
    accumulator.setTo( 0 );

    // Iterate through pixels of the input image
    // - image lines
    for ( int x = 0; x < image.rows; x++ )
    {
         // - image columns
        for ( int y = 0; y < image.cols; y++ )
        {
            // Check validity of pixel
            // - consider a binary image
            // - valid pixel usally means "is an edge/contour"
            if ( image.at< float >( x, y ) == 0.0f )
            {
                continue;
            }

            // Iterate through parameters in Hough space (i.e. [rho,theta])
            // - initialize theta (range: [-pi/2, pi])
            theta = -PI/2;
            for ( int i = 0; i < nbTetha; i++ )
            {
                // Update theta
                theta += deltaTheta;

                // Compute rho
                rho = (int)( ( ( x * cos( theta ) + y * sin( theta ) ) / (float)deltaRho ) + 0.5f );

                // Check validity of "rho" parameter
                if ( rho > 0.0f )
                {
                    if ( accumulator.at< uchar >( i, rho ) < 255 )
                    {
                        // Update accumulatore by voting
                        accumulator.at< uchar >( i/*theta*/, rho ) += 1;

                        //std::cout << "create " << theta*(180 /PI) <<" "<< rho << std::endl;
                    }
                }
            }
        }
    }

    return accumulator;
}

/******************************************************************************
 * return a matrice with all the segment that are declared valide
 *
 * @param accu accumulator contains number of vote for every segment
 * @param rows number of rows for the image
 * @param cols number of cols for the image
 * @param nbMinPoint minimum number of vote to declare a segment valide
 *
 * @return the number of vote for every segment
 ******************************************************************************/
cv::Mat Hough::getSegmentFromAccumulator( cv::Mat& accu, const int rows, const int cols , const int nbMinPoint)
{
    // Output
    // Datatype is uchar so max value is 255
    cv::Mat res = cv::Mat( rows, cols, CV_8U/*uchar type*/ );
    res.setTo( 0 );

    //rho maximum size
    const int maxRho = (int)sqrt( static_cast< float >( rows * rows + cols * cols ) );

    float theta = -PI/2;
    float rho = 0.0f;
    const float deltaRho = SQRT_2;
    const float deltaTheta = ((3.0f*PI)/2.0f) * (1.0f/(float)maxRho);

    std::pair<int, int> firstIntersect;
    std::pair<int, int> secondIntersect;
    bool findFirstIntersect;

    int temporaryPoint;

    float cosTheta;
    float sinTheta;

    // Iterate through theta
    for ( int x = 0; x < accu.rows; x++ )
    {
        theta += deltaTheta;

        cosTheta = cos( theta );
        sinTheta = sin( theta );

        // Iterate through rho
        for ( int y = 0; y < accu.cols; y++ )
        {
            // only if there is enough point for this line
            if ( accu.at< uchar >( x, y ) < nbMinPoint )
            {
                continue;
            }

            rho = y * deltaRho;
            findFirstIntersect = false;

            // Intersection with axis (x = 0)
            // i.e. y = rho / sin( theta )
            temporaryPoint = (int)( rho / (float)sinTheta );
            if ( temporaryPoint >=0 && temporaryPoint < cols )
            {
                firstIntersect = std::pair< int, int >( 0, temporaryPoint );
                findFirstIntersect = true;
            }

            // Intersection with axis (y = 0)
            // i.e. x = rho / cos( theta )
            temporaryPoint = (int)( rho / (float)cosTheta );
            if ( temporaryPoint >=0 && temporaryPoint < rows )
            {
                if ( ! findFirstIntersect )
                {
                    firstIntersect = std::pair< int, int >( temporaryPoint, 0 );
                    findFirstIntersect = true;
                }
                else
                {
                    secondIntersect = std::pair<int, int>(temporaryPoint,0);
                }
            }

            // Intersection with axis (y = nbColumns)
            // i.e. x = ( rho - y * sin( theta ) ) / cos( theta )
            temporaryPoint = (int)((float)( rho - ( cols - 1 ) * sinTheta ) / (float)cosTheta );
            if ( temporaryPoint >=0 && temporaryPoint < rows )
            {
                if ( ! findFirstIntersect )
                {
                    firstIntersect = std::pair< int, int >( temporaryPoint, cols - 1 );
                    findFirstIntersect = true;
                }
                else
                {
                    secondIntersect = std::pair< int, int >( temporaryPoint, cols - 1 );
                }
            }

            // Intersection with axis (x = nbRows)
            // i.e. y = ( rho - x * cos( theta ) ) / sin( theta )
            temporaryPoint = (int)((float)( rho - ( rows - 1 ) * cosTheta ) / (float)sinTheta );
            if ( temporaryPoint >=0 && temporaryPoint < cols )
            {
                secondIntersect = std::pair< int, int >( rows - 1, temporaryPoint );
            }

             // Use Bresenham algorithm to fill pixel between the two points
            bresenham( &res, firstIntersect.first, firstIntersect.second, secondIntersect.first, secondIntersect.second, accu.at< uchar >( x, y ) );
        }
    }

    return res;
}

/******************************************************************************
 * Cut every line out of the form
 *
 * @param image to do modification
 * @param src original image
 ******************************************************************************/
void Hough::limitSegment( cv::Mat& image, const cv::Mat& src )
{
    // Iterate through src lines
    for ( int x = 0; x < src.rows; x++ )
    {
        // Iterate through src columns
        for ( int y = 0; y < src.cols; y++ )
        {
            //if the original image contains nothing, cut the line
            if ( src.at< float >( x, y ) == 0.0f )
                image.at< uchar >( x, y ) = 0;
            else //else, stop cuting line
                break;
        }

        // do the same thing starting at the end
        for ( int y = src.cols - 1; y > 0; y-- )
        {
            //if the original image contains nothing, cut the line
            if ( src.at< float >( x, y ) == 0.0f )
                image.at< uchar >( x, y ) = 0;
            else //else, stop cuting line
                break;
        }
    }

    // Iterate through src lines
    for ( int x = 0; x < src.cols; x++ )
    {
        // Iterate through src columns
        for ( int y = 0; y < src.rows; y++ )
        {
            //if the original image contains nothing, cut the line
            if ( src.at< float >( y, x ) == 0.0f )
                image.at< uchar >( y, x ) = 0;
            else //else, stop cuting line
                break;
        }

        // do the same thing starting at the end
        for ( int y = src.rows - 1; y > 0; y-- )
        {
            //if the original image contains nothing, cut the line
            if ( src.at< float >( y, x ) == 0.0f )
                image.at< uchar >( y, x ) = 0;
            else //else, stop cuting line
                break;
        }
    }
}

/******************************************************************************
 * draw only most important lines
 *
 * @param accumulator contains number of vote for every segment
 * @param nbLines number maximum of lines to draw
 *
 * @return the minimum numbre of points
 ******************************************************************************/
int Hough::segmentThreshold(cv::Mat& accu, int nbLines )
{
    //will contains every lines sorted
    std::set<uchar> maxLines;

     std::set<uchar>::iterator it;

    // Iterate through theta
    for ( int x = 0; x < accu.rows; x++ )
    {
        // Iterate through rho
        for ( int y = 0; y < accu.cols; y++ )
        {
            if(maxLines.size() < 6 || accu.at< uchar >( x, y ) > *it){
                maxLines.insert(accu.at< uchar >( x, y ));
                if(maxLines.size() > 6){
                    it = maxLines.begin();
                    maxLines.erase(it);
                }
            }

            it = maxLines.begin();
        }

    }

    return *it;
}

/******************************************************************************
 * Generate the Hough accumulator for circle detection,
 * given a user defined radius
 *
 * @param pImage input image
 * @param pRadius circle radius
 *
 * @return the Hough accumulator for circle detection
 ******************************************************************************/
cv::Mat Hough::generateCircleAccumulator( const cv::Mat& pImage, float pRadius )
{
     //printf( "\nINSIDE generateCircleAccumulator() - FIXED radius" );

    // Hough space parameters [center, radius]
    // - radius is fixed
    const float r = pRadius;
    // - center (a,b): [...,...]
    float a = 0.0f;
    //float b = 0.0f;
    //const float deltaA = 0.0f;
    //const float deltaB = 0.0f;
    const int nbA = pImage.cols;
    const int nbB = pImage.rows;

    //printf( "\ncreate accumulator()" );

    // Accumulator
    // - 2D matrix of size (nbB,nbA) (i.e. nbB rows and nbA columns)
    // - BEWARE : datatype is uchar so max value is 255 (check if this is valid)
    //  - initiaize accumulator to 0
    cv::Mat accumulator = cv::Mat( nbB, nbA, CV_8U/*uchar type*/, cv::Scalar( 0 ) );

    // Iterate through pixels of the input image
    int A = 0;
     // - image lines
    for ( int x = 0; x < pImage.rows; x++ )
    {
         // - image columns
        for ( int y = 0; y < pImage.cols; y++ )
        {
            // Check validity of pixel
            // - consider a binary image
            // - valid pixel usally means "is an edge/contour"
            if ( pImage.at< float >( x, y ) < cEPSILLON )
            {
                continue;
            }

            // Iterate through parameters in Hough space (i.e. [a,b] and r fixed)
            for ( int i = 0; i < nbB; i++ )
            {
                // (x-a)*(x-a)+(y-b)*(y-b)=r*r
                const float tmp = r*r - (y-i)*(y-i);
                if ( tmp >= 0.0f )
                {
                    a = x - sqrtf( tmp );

                    // Check validity of "a" parameter
                    if ( a > 0.0f )
                    {
                        A = static_cast< int >( a + 0.5f );
                        assert( A < nbA );
                          if ( accumulator.at< uchar >( i/*b*/, A ) < 255 )
                        {
                            // Update accumulatore by voting
                            accumulator.at< uchar >( i/*b*/, A ) += 1;
                        }
                    }
                }
#if 0
                else
                {
                    // LOG
      //              printf( "\n(A,B) = (%f,%f)", a, b );
                }
#endif
            }
        }
    }

#if 1
    // Display accumulator
    // - origin: top left corner (as images)
    cv::Mat houghTransform = cv::Mat( nbB, nbA, CV_8U/*uchar type*/, cv::Scalar( 0 ) );
    // Iterate through "b"
    for ( int i = 0; i< accumulator.rows; i++ )
    {
        // Iterate through "a"
        for ( int j = 0; j < accumulator.cols; j++ )
        {
            cv::circle( houghTransform,
                        cv::Point( j ,i ),
                        static_cast< int >( pRadius ),
                        cv::Scalar( accumulator.at< uchar >( i, j ), accumulator.at< uchar >( i, j ), accumulator.at< uchar >( i, j ) ),
                        1, 8, 0 );
        }
    }
    cv::imshow( "Hough Transform - CIRCLE", houghTransform );
#endif

    //printf( "\nEND" );

    // Return accumulator
    return accumulator;
}

/******************************************************************************
 * Generate the Hough accumulator for circle detection
 *
 * @param pImage input image
 *
 * @return the Hough accumulator for circle detection
 ******************************************************************************/
cv::Mat Hough::generateCircleAccumulator( const cv::Mat& pImage )
{
   // printf( "\nINSIDE generateCircleAccumulator() - NON-FIXED radius" );

    // Hough space parameters [center, radius]
    // - center (a,b): [...,...]
    float a = 0.0f;
    //float b = 0.0f;
    //const float deltaA = 0.0f;
    //const float deltaB = 0.0f;
    const int nbA = pImage.cols;
    const int nbB = pImage.rows;
    // - radius r: [0,...]
    //float r = 0.0f;
    //const float deltaR = 0.0f;
    const int nbR = max( pImage.rows, pImage.cols );

    //printf( "\ncreate accumulator()" );

    // Accumulator
    // - 3D matrix of size (nbB,nbA,nbR) (i.e. nbB rows, nbA columns with nbR depth)
    // - BEWARE : datatype is uchar so max value is 255 (check if this is valid)
    //  - initiaize accumulator to 0
    int accumulatorSizes[] = { nbB, nbA, nbR };
    cv::Mat accumulator = cv::Mat( 3, accumulatorSizes, CV_8U/*uchar type*/, cv::Scalar( 0 ) );

    // Iterate through pixels of the input image
    int A = 0;
    // - image lines
    for ( int x = 0; x < pImage.rows; x++ )
    {
         // - image columns
        for ( int y = 0; y < pImage.cols; y++ )
        {
            // Check validity of pixel
            // - consider a binary image
            // - valid pixel usally means "is an edge/contour"
            if ( pImage.at< float >( x, y ) < cEPSILLON )
            {
                continue;
            }

            // Iterate through parameters in Hough space (i.e. [a,b,r])
            for ( int k = 0; k < nbR; k++ )
            {
                for ( int i = 0; i < nbB; i++ )
                {
                    // (x-a)*(x-a)+(y-b)*(y-b)=r*r
                    const float tmp = static_cast< float >( k*k - (y-i)*(y-i) );
                    if ( tmp >= 0.0f )
                    {
                        a = x - sqrtf( tmp );

                        // Check validity of "a" parameter
                        if ( a > 0.0f )
                        {
                            A = static_cast< int >( a + 0.5f );
                            assert( A < nbA );
                            if ( accumulator.at< uchar >( i/*b*/, A, k/*r*/ ) < 255 )
                            {
                                // Update accumulatore by voting
                                accumulator.at< uchar >( i/*b*/, A, k/*r*/ ) += 1;
                            }
                        }
                    }
#if 0
                    else
                    {
                        // LOG
                //        printf( "\n(A,B,R) = (%f,%f,%f)", a, b, r );
                    }
#endif
                }
            }
        }
    }

  //  printf( "\nEND" );

    // Return accumulator
    return accumulator;
}

/******************************************************************************
 * Extract circles from the Hough accumulator,
 * based on most significant values (votes)
 *
 * @param pImage input accumulator
 *
 * @return an image with extracted circles
 ******************************************************************************/
cv::Mat Hough::extractCirclesFromAccumulator( const cv::Mat& pAccumulator, float radius, unsigned int pVoteCriteria, int nbRows, int nbColumns )
{
    printf( "\nINSIDE extractCirclesFromAccumulator() - FIXED RADIUS" );

    // Image of extracted circles
    cv::Mat image = cv::Mat( nbRows, nbColumns, CV_8U/*uchar type*/, cv::Scalar( 0 ) );

    // (x-a)*(x-a)+(y-b)*(y-b)=r*r

   // printf( "\houghTransform" );

    // Display circles
    // - origin: top left corner (as images)
    cv::Mat houghTransform = cv::Mat( nbRows, nbColumns, CV_8U/*uchar type*/, cv::Scalar( 0 ) );

    // Iterate through parameters in Hough space (i.e. [a,b,r])
    // - parameter "b"
    for ( int i = 0; i < pAccumulator.rows; i++ )
    {
        // - parameter "a"
        for ( int j = 0; j < pAccumulator.cols; j++ )
        {
            // Check vote in the accumulator
            if ( pAccumulator.at< uchar >( i, j ) >= pVoteCriteria )
            {
                // Circle equation: (x-a)*(x-a)+(y-b)*(y-b)=r*r
                // - (a,b,r) are known
                // - unknown: (x,y)
                // Dessiner cercle
                cv::circle( houghTransform,
                            cv::Point( j ,i ),
                            static_cast< int >( radius + 0.5f ),
                            //cv::Scalar( pAccumulator.at< uchar >( i, j ), pAccumulator.at< uchar >( i, j ), pAccumulator.at< uchar >( i, j ) ),
                            cv::Scalar( 255, 255, 255 ),
                            1, 8, 0 );

                // Write pixels
                // - use equivalent of Bresenham to rasterize circle
                //image.at< uchar >( x, y ) = value;
            }
        }
    }

    // Display circles
    cv::imshow( "Hough - EXTRACTED CIRCLES", houghTransform );

   // printf( "\nEND" );

    // Return image
    return image;
}

/******************************************************************************
 * Extract circles from the Hough accumulator,
 * based on most significant values (votes)
 *
 * @param pImage input accumulator
 *
 * @return an image with extracted circles
 ******************************************************************************/
cv::Mat Hough::extractCirclesFromAccumulator( const cv::Mat& pAccumulator, unsigned int pVoteCriteria, int nbRows, int nbColumns )
{
  //  printf( "\nINSIDE extractCirclesFromAccumulator() NON-fixed" );
  //  printf( "\nrows: %d - columns: %d", nbRows, nbColumns );

    // Image of extracted circles
    cv::Mat image = cv::Mat( nbRows, nbColumns, CV_8U/*uchar type*/, cv::Scalar( 0 ) );

    // (x-a)*(x-a)+(y-b)*(y-b)=r*r

    // Display circles
    // - origin: top left corner (as images)
    cv::Mat houghTransform = cv::Mat( nbRows, nbColumns, CV_8U/*uchar type*/, cv::Scalar( 0 ) );

    int dim = pAccumulator.dims;
    assert( dim == 3 );
    printf( "MATRICE dims: %d", dim );
    cv::MatSize matSize = pAccumulator.size;
    printf( "MATRICE 3rd dim: %d", matSize[ 2 ] );

    // Iterate through parameters in Hough space (i.e. [a,b,r])
    // - parameter "b"
    for ( int i = 0; i < pAccumulator.rows; i++ )
    {
        // - parameter "a"
        for ( int j = 0; j < pAccumulator.cols; j++ )
        {
            int kMax = 0;
            // - parameter "r"
            for ( int k = 0; k < /*pAccumulator.depth()*/matSize[ 2 ]; k++ )
            {
                if ( pAccumulator.at< uchar >( i, j, k ) >= pVoteCriteria )
                {
                    kMax = k > kMax ? k : kMax;
                }
            }
            // Check vote in the accumulator
            if ( pAccumulator.at< uchar >( i, j, /*k*/kMax ) >= pVoteCriteria )
            {
                // Circle equation: (x-a)*(x-a)+(y-b)*(y-b)=r*r
                // - (a,b,r) are known
                // - unknown: (x,y)
                // Dessiner cercle
                cv::circle( houghTransform,
                            cv::Point( j, i ),
                            /*k*/kMax/*radius*/,
                            //cv::Scalar( pAccumulator.at< uchar >( i, j ), pAccumulator.at< uchar >( i, j ), pAccumulator.at< uchar >( i, j ) ),
                            cv::Scalar( 255, 255, 255 ),
                            1, 8, 0 );

                // Write pixels
                // - use equivalent of Bresenham to rasterize circle
                //image.at< uchar >( x, y ) = value;
            }
        }
    }

    // Display circles
    cv::imshow( "Hough - EXTRACTED CIRCLES", houghTransform );

   // printf( "\nEND" );

    // Return image
    return image;
}

/******************************************************************************
 * Bresenham
 * Draw a line between two points
 *
 * @param image matrice to draw
 * @param x1, y1 coord of the first point
 * @param x2, y2 coord of the second point
 * @param value the value for this segment, the more a line has been voted, the more the value will be
 ******************************************************************************/
void Hough::bresenham( cv::Mat* image, int x1, int y1, int x2, int y2, uchar value )
{
    assert( image != NULL );

    int delta_x( x2 - x1 );
    // if x1 == x2, then it does not matter what we set here
    signed char const ix( ( delta_x > 0 ) - ( delta_x < 0 ) );
    delta_x = std::abs( delta_x ) << 1;

    int delta_y( y2 - y1 );
    // if y1 == y2, then it does not matter what we set here
    signed char const iy( ( delta_y > 0 ) - ( delta_y < 0 ) );
    delta_y = std::abs( delta_y ) << 1;

    // Set pixel
    image->at< uchar >( x1, y1 ) = value;

    if ( delta_x >= delta_y )
    {
        // error may go below zero
        int error( delta_y - ( delta_x >> 1 ) );

        while ( x1 != x2 )
        {
            if ( ( error >= 0 ) && ( error || ( ix > 0 ) ) )
            {
                error -= delta_x;
                y1 += iy;
            }
            // else do nothing

            error += delta_y;
            x1 += ix;

            // Set pixel
            image->at< uchar >( x1, y1 ) = value;
        }
    }
    else
    {
        // error may go below zero
        int error( delta_x - ( delta_y >> 1 ) );

        while ( y1 != y2 )
        {
            if ( ( error >= 0 ) && ( error || ( iy > 0 ) ) )
            {
                error -= delta_y;
                x1 += ix;
            }
            // else do nothing

            error += delta_x;
            y1 += iy;

            // Set pixel
            image->at< uchar >( x1, y1 ) = value;
        }
    }
}
