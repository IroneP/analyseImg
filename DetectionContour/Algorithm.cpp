/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */
 
/**
 * @version 1.0
 */

#include "Algorithm.h"

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/
 
// STL
#include <cassert>
#include <algorithm>
#include <iostream>

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
 
/**
 * percentage of show pixels
 */
//float algorithm::percent = 80.0f;

/**
 * percentage of the high Threshold
 */
float algorithm::_highThresholdPercent = 60.0f;

/**
 * percentage of the low Threshold
 */
float algorithm::_lowThresholdPercent = 92.0f;

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

#define PI 3.14159265
 
/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

/******************************************************************************
 * Constructor
 ******************************************************************************/
algorithm::algorithm()
{
}

/******************************************************************************
 * Image filtering - Convolution
 * - use a kernel to filter an image (in place)
 *
 * @param src input image that will be filtered by a kernel
 * @param filter kernel (Mean, Prewitt, Sobel, etc...)
 *
 * @return output filtered data
 ******************************************************************************/
cv::Mat algorithm::filter( const cv::Mat& src, const cv::Mat& filter )
{
	// Output
    cv::Mat res = cv::Mat( src.rows, src.cols, src.type() );
    res.setTo( 0 );

	// Iterate through image columns
    float value = 0.0f;
    for ( int x = 1; x < src.rows - 1; x++ ) {
		// Iterate through image lines
        for ( int y = 1; y < src.cols - 1; y++ ) {

			// Iterate through filter kernel columns
            value = 0.0;
            for ( int i = -1; i < filter.rows - 1; i++ ) {
				// Iterate through filter kernel lines
                for ( int j = -1; j < filter.cols - 1; j++ ) {

                    value += src.at< float >( x + i, y + j ) * filter.at< float >( i + 1, j + 1 );
                }
            }
			
			// Write output
            res.at< float >( x, y ) = value;
			
			// Clamp data
            /*if ( res.at< float >( x, y ) < 0.0f ){
                res.at< float >( x, y ) = 0.0f;
            }
            if ( res.at< float >( x, y ) > 255.0f ){
                res.at< float >( x, y ) = 255.0f ;
            }*/
        }
    }

    return res;
}

/******************************************************************************
 * Normalize a matrice (i.e. an image)
 *
 * @param input matrice
 *
 * @return the normalized matrice
 ******************************************************************************/
void algorithm::normalize( cv::Mat& src )
{
	// Initialize min and max values
    float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();

	// Find min and max values
	// Iterate through image columns
    for ( int x = 0; x < src.rows; x++ ) {
		// Iterate through image lines
        for ( int y = 0; y < src.cols; y++ ) {
            if ( src.at< float >( x, y ) > max )
                max = src.at< float >( x, y );
            if ( src.at< float >( x, y ) < min )
                min = src.at< float >( x, y );
        }
    }

	// Rescale data between 0 and 255
	// Iterate through image columns
    for ( int x = 0; x < src.rows; x++ ) {
		// Iterate through image lines
        for ( int y = 0; y < src.cols; y++ ) {
            src.at< float >( x, y ) = ( src.at< float >( x, y ) - min ) / ( max - min ) * 255.0f;
        }
    }
}

/******************************************************************************
 * Normalize a matrice (i.e. an image)
 *
 * @param input matrice
 *
 * @return the normalized matrice
 ******************************************************************************/
cv::Mat algorithm::normalizeData( const cv::Mat& src )
{
    // Ouput matrice
    cv::Mat res = cv::Mat(src.rows, src.cols, src.type() );

    // Initialize min and max values
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    // Find min and max values
    // Iterate through image columns
    for ( int x = 0; x < src.rows; x++ ) {
        // Iterate through image lines
        for ( int y = 0; y < src.cols; y++ ) {
            if ( src.at< float >( x, y ) > max )
                max = src.at< float >( x, y );
            if ( src.at< float >( x, y ) < min )
                min = src.at< float >( x, y );
        }
    }

    // Rescale data between 0 and 255
    // Iterate through image columns
    for ( int x = 0; x < src.rows; x++ ) {
        // Iterate through image lines
        for ( int y = 0; y < src.cols; y++ ) {
            res.at< float >( x, y ) = ( src.at< float >( x, y ) - min ) / ( max - min ) * 255.0f;
        }
    }

    return res;
}

/******************************************************************************
 * Set every pixels of a matrice to 0.0 or 255.0
 *
 * @param src input matrice
 * @param limit limit for the validity of the pixel
 *
 * @return the binary matrice
 ******************************************************************************/
cv::Mat algorithm::toBinary( const cv::Mat& src )
{
    // Ouput matrice
    cv::Mat res = cv::Mat(src.rows, src.cols, 0 );

    // Iterate through image columns
    for ( int x = 0; x < src.rows; x++ ) {
        // Iterate through image lines
        for ( int y = 0; y < src.cols; y++ ) {
            if ( src.at< uchar >( x, y ) > 0 )
                res.at< uchar >( x, y ) = 255;
            else
                res.at< uchar >( x, y ) = 0;
        }
    }

    return res;
}

/******************************************************************************
 * Generate the module with norm "L infinite" given a set of components
 * - i.e. for each pixel, find the max of all components
 *
 * @param modules list of module components
 * @param NbDirection nulber of directions
 *
 * @return the module
 ******************************************************************************/
cv::Mat algorithm::moduleLinf( cv::Mat* modules, int NbDirection )
{
	// Ouput matrice
    cv::Mat res = cv::Mat( modules[0].rows, modules[0].cols, modules[0].type() );
    float max;

	// Iterate through columns
    for ( int x = 0; x < modules[0].rows; x++ ) {
		// Iterate through lines
        for ( int y = 0; y < modules[0].cols; y++ ) {

			// For each pixel, find the max of all components (i.e. norm "L infinite")
            max = std::max( std::abs( modules[0].at< float >( x, y ) ), std::abs( modules[1].at< float >( x, y ) ) );
			// Handle multidirectional filtering case
            if ( NbDirection == 4 ){
                max = std::max( max, std::abs( modules[2].at< float >( x, y ) ) );
                max = std::max( max, std::abs( modules[3].at< float >( x, y ) ) );
            }
			
			// Clamp data
            /*if ( max < 0.0f ){
                max = 0.0f;
            }
            if( max > 255.0f ){
                max = 255.0f;
            }*/
			
			// Write result
            res.at< float >( x, y ) = max;
        }
    }

	// Normalize result
    normalize( res );
	
    return res;
}

/******************************************************************************
 * Generate the module with norm "L1" given a set of components
 * - i.e. for each pixel, compute the sum of absolute of components
 *
 * NOTE : for multidirectional filtering, first find the 2 max components, then compute classical L1 norm
 *
 * @param modules list of module components
 * @param NbDirection nulber of directions
 *
 * @return the module
 ******************************************************************************/
cv::Mat algorithm::moduleL1( cv::Mat* modules, int NbDirection )
{
	// Ouput matrice
    cv::Mat res = cv::Mat( modules[0].rows, modules[0].cols, modules[0].type() );
    
	float max1;
	float max2;
	// Iterate through columns
	for ( int x = 0; x < modules[0].rows; x++ ) {
        // Iterate through lines
		for ( int y = 0; y < modules[0].cols; y++ ) {

			// For each pixel, find the sum of absolute values of components (i.e. norm "L1")
            if ( std::abs( modules[0].at< float >( x, y ) ) > std::abs( modules[1].at< float >( x , y ) ) ) {
                max1 = std::abs( modules[0].at< float >( x, y ) );
                max2 = std::abs( modules[1].at< float >( x, y ) );
            } else {
                max2 = std::abs( modules[0].at< float >( x, y ) );
                max1 = std::abs( modules[1].at< float >( x, y ) );
            }

			// Handle multidirectional filtering case
            if ( NbDirection == 4 ) {
                if ( std::abs( modules[2].at< float >( x , y ) ) > max1 ) {
                    max2 = max1;
                    max1 = std::abs( modules[2].at< float >( x, y ) );
                } else if ( std::abs( modules[2].at< float >( x, y ) ) > max2 ) {
                    max2 = std::abs( modules[2].at< float >( x , y ) );
                }
                if ( std::abs( modules[3].at< float >( x , y ) ) > max1 ) {
                    max2 = max1;
                    max1 = std::abs( modules[3].at< float >( x, y ) );
                } else if( std::abs( modules[3].at< float >( x , y ) ) > max2 ) {
                    max2 = std::abs( modules[3].at< float >( x , y ) );
                }
            }

			// L1 nom
            res.at< float >( x, y ) = max1 + max2;
            
			// Clamp data
			/*if ( res.at< float >( x, y ) < 0.0f ){
                res.at< float >( x, y ) = 0.0f;
            }
            if ( res.at< float >( x , y ) > 255.0f ){
                res.at< float >( x, y ) = 255.0f;
            }*/
        }
    }

	// Normalize result
    normalize( res );
	
    return res;
}

/******************************************************************************
 * Generate the slope given a module
 *
 * @param resulting slope image
 * @param number of directions
 * @param module input module
 *
 * @return the slope
 ******************************************************************************/
cv::Mat algorithm::pente( cv::Mat* pentes, int NbDirection, const cv::Mat& module )
{
    // Ouput matrice
    cv::Mat res = cv::Mat( module.rows, module.cols, module.type() );
    // Initialize data with 0 values
	res.setTo( 0.0f );

    float teta;
	// Iterate through lines
    for (int x = 0; x < module.rows; x++) {
		// Iterate through columns
        for (int y = 0; y < module.cols; y++) {

            teta = atan2( pentes[1].at< float >( x, y ), pentes[0].at< float >( x , y ) );
			// Handle multidirectional filtering case
            if ( NbDirection == 4 ) {
                std::max( (float)atan2( pentes[3].at< float >( x, y ), pentes[2].at< float >( x, y ) ), teta );
            }

            if ( module.at< float >( x , y ) == 0.0 )
                res.at< float >( x , y ) = 0.0;
            else
                res.at< float >( x , y ) = teta;
        }
    }

    return res;
}

/******************************************************************************
 * Generate the slope given a module
 *
 * @param slope input slope
 * @param module input module
 *
 * @return the matrice color
 ******************************************************************************/
cv::Mat algorithm::colorMat(const cv::Mat& slope, const cv::Mat& module )
{
    // Ouput matrice
    cv::Mat matColor = cv::Mat( module.rows, module.cols, CV_32FC3 );

    // Iterate through lines
    for (int x = 0; x < module.rows; x++) {
        // Iterate through columns
        for (int y = 0; y < module.cols; y++) {

            //put color into matcolor
            if ( module.at< float >( x , y ) == 0.0 ){
                matColor.at< cv::Vec3f >(x,y) = cv::Vec3f( 0.0f, 0.0f, 0.0f );
            }else {
                // Map gradient direction to one of four colors
                //std::cout << max << std::endl;
                if ( slope.at< float >( x, y ) >= -PI && slope.at< float >( x, y ) < -PI / 2.0f ) {
                    matColor.at< cv::Vec3f >( x, y )[1] = module.at< float >( x, y );
                } else if( slope.at< float >( x, y ) >= -PI / 2.0f && slope.at< float >( x, y ) < 0.0f ) {
                    matColor.at< cv::Vec3f >( x, y )[0] = module.at< float >( x, y );
                    matColor.at< cv::Vec3f >( x, y )[2] = module.at< float >( x, y );
                } else if ( slope.at< float >( x, y ) >= 0.0f && slope.at< float >( x, y ) < PI / 2.0f ) {
                    matColor.at< cv::Vec3f >( x, y )[0] = module.at< float >( x, y );
                } else {
                    matColor.at< cv::Vec3f >( x, y )[2] = module.at< float >( x, y );
                }

            }
        }
    }

    return matColor;
}

/******************************************************************************
 * Apply a threshold given an input dataset with a "local" criteria
 * NOTE : based on an histogram of input data
 * - users have to specify a size to the window of locality
 *
 * @param src input data
 * @param window size of the window
 *
 * @return the Matrix with applied threshold
 ******************************************************************************/
cv::Mat algorithm::localThreshold( cv::Mat& src, int window )
{
    //a copy of input matrix
    cv::Mat matCopy = cv::Mat( src.rows, src.cols, src.type() );
    matCopy.setTo( 0.0f );

    //count number of neighbor
    float localSum;

    // Iterate through lines except border
    for ( int x = window; x < src.rows-window; x++ )
    {
        // Iterate through columns except border
        for ( int y = window; y < src.cols-window; y++ )
        {
            localSum = 0.0;

            // Iterate through lines neighborhood
            for ( int i = -window; i <= window; i++ )
            {
                // Iterate through columns neighborhood
                for ( int j = -window; j <= window; j++ )
                {
                   //sum of all pixels in the locality
                   localSum += src.at< float >( x + i, y + j );
                }
            }

            // Threshold data
            if ( src.at< float >( x, y ) < ( localSum / ( ( window * 2 + 1 ) * ( window * 2 + 1 ) ) ) )
            {
                matCopy.at< float >( x, y ) = 0.0f;
            }
            else
            {
                matCopy.at< float >( x, y ) = src.at< float >( x, y );
            }
        }
    }

    return matCopy;
}

/******************************************************************************
 * Apply a threshold given an input dataset with a "local" criteria
 * NOTE : based on an histogram of input data
 * - users have to specify a size to the window of locality
 *
 * @param src input data
 * @param window size of the window
 *
 * @return the Matrix with applied threshold
 ******************************************************************************/
cv::Mat algorithm::localThresholdEnhanced( cv::Mat& src, int window )
{
    // Copy of input matrix
    cv::Mat matCopy = src.clone();

    //count number of neighbor
    float localMean;

    // Mean filter coefficient
    const float meanFilterCoeff = 1.0f / ( ( window * 2 + 1 ) * ( window * 2 + 1 ) );

    // Iterate through image columns
    for ( int x = 1; x < src.rows - 1; x++ )
    {
        // Iterate through image lines
        for ( int y = 1; y < src.cols - 1; y++ )
        {
            localMean = 0.0;

            // Iterate through filter kernel columns
            for ( int i = -window; i <= window; i++ )
            {
                // Iterate through filter kernel lines
                for ( int j = -window; j <= window; j++ )
                {             
                    if ( ( ( x + i ) >= 1 ) &&
                         ( ( x + i ) <= ( src.rows - 1 ) ) &&
                         ( ( y + j ) >= 1 ) &&
                         ( ( y + j ) <= ( src.cols - 1 ) ) )
                    {
                         //sum of all pixels in the locality
                         localMean += src.at< float >( x + i, y + j );
                    }
                }
            }
            localMean *= meanFilterCoeff;

            // Threshold data
            if ( src.at< float >( x, y ) < localMean )
            {
                matCopy.at< float >( x, y ) = 0.0f;
            }
        }
    }

    return matCopy;
}

/******************************************************************************
 * Determine a threshold given an input dataset with a "global" criteria
 * NOTE : based on an histogram of input data
 * - users have to specify a percentage of valid pixels
 *
 * @param input data
 *
 * @return the computed threshold 
 ******************************************************************************/
int algorithm::globalThreshold( const cv::Mat& src, float percentFilter )
{
	// Histogram (initialized to 0)
    int histogram[ 256 ] = { 0 };
    
    unsigned long sumValue = 0;

	// Generate histogram
	// Iterate through lines
    for ( int x = 0; x < src.rows; x++ ) {
		// Iterate through columns
		for (int y = 0; y < src.cols; y++) {
            // Fill histogram
			histogram[ (int)src.at< float >( x, y ) ] += 1;
            // Update
			sumValue += (int)src.at< float >( x, y );
        }
    }

    // LOG - DEBUG
    cout << "Histogram" << endl;
    cout << "- sum: " << sumValue << endl;
//    for ( int i = 0; i < 256; i++ )
//    {
//        cout << histogram[ i ] << " ";
//    }
//    cout << endl;

	// Find threshold based on a user defined percentage of valid pixels
    unsigned long target = (unsigned long)sumValue * ( ( 100.0f - percentFilter ) / 100.0f );
    cout << "- target: " << target << endl;
    sumValue = 0;
    int i = 1;
    while ( sumValue < target && i < 255 ) {
        sumValue += histogram[ i ] * i;
        // Update
		i++;
    }

	// Return computed threshold
    return i;
}

/******************************************************************************
 * Apply a threshold given an input dataset
 *
 * @param input data to threshold
 * @param threshold
 ******************************************************************************/
void algorithm::applyThreshold( cv::Mat* src, int seuil )
{
	// Iterate through lines
    for (int x = 0; x < src->rows; x++) {
        // Iterate through columns
		for (int y = 0; y < src->cols; y++) {
            // Threshold data
			if ( src->at< float >( x, y ) < seuil ) {
                src->at< float >( x, y ) = 0.0f;
				// LOG
                //std::cout << src.at<float>(x,y) << "  " << res.at<uchar>(x,y) << std::endl;
            }
        }
    }
}

/******************************************************************************
 * Apply a threshold on a given input dataset
 *
 * @param pData input data
 * @param threshold
 *
 * @return the thresholded data
 ******************************************************************************/
cv::Mat algorithm::applyThreshold( const cv::Mat& pData, int pThreshold )
{
    // Ouput matrice
    cv::Mat res = pData.clone();

    // Iterate through lines
    for ( int x = 0; x < pData.rows; x++ )
    {
        // Iterate through columns
        for ( int y = 0; y < pData.cols; y++ )
        {
            // Threshold data
            if ( pData.at< float >( x, y ) < static_cast< float >( pThreshold ) )
            {
                res.at< float >( x, y ) = 0.0f;
            }
        }
    }

    return res;
}

/******************************************************************************
 *  Hysteresis Threshold filtering : Return a Matrice made with a high filter and a low filter
 *
 * @param src The matrice on wich we apply the Threshold
 *
 * @return res The matrice filtered
 ******************************************************************************/
cv::Mat algorithm::hysteresis( const cv::Mat& src, int& pHysteresisHighThreshold, int& pHysteresisLowThreshold )
{
    // Ouput matrice
    cv::Mat res = cv::Mat( src.rows, src.cols, src.type() );
    res.setTo(0.0f);

    // Get the two Threshold
    int highThreshold = globalThreshold( src, _highThresholdPercent );
    int lowThreshold = globalThreshold( src, _lowThresholdPercent );
    assert( highThreshold > lowThreshold );

    // Write output info
    pHysteresisHighThreshold = highThreshold;
    pHysteresisLowThreshold = lowThreshold;

    // Apply filter
    cv::Mat highBinaryMat = src.clone();
    applyThreshold(&highBinaryMat, highThreshold);
    cv::Mat lowBinaryMat = src.clone();
    applyThreshold(&lowBinaryMat, lowThreshold);
    // LOG
    cout << "- low threshold value: " << lowThreshold << endl;
    cout << "- high threshold value: " << highThreshold << endl;

    // Iterate through columns
    for ( int x = 0; x < src.rows; x++ ) {
        // Iterate through lines
        for ( int y = 0; y < src.cols; y++ ) {

            // For each pixel, test if highBinaryMat as a value
            if(highBinaryMat.at< float >( x, y ) != 0.0){
                res.at< float >( x, y ) = highBinaryMat.at< float >( x, y );
            //else, test if lowBinaryMat as a connexity to highBinaryMat
            }else if(lowBinaryMat.at< float >( x, y ) != 0.0){
                if(highBinaryMat.at< float >( x-1, y ) != 0.0){
                    res.at< float >( x, y ) = lowBinaryMat.at< float >( x, y );
                }else if(highBinaryMat.at< float >( x, y-1 ) != 0.0){
                    res.at< float >( x, y ) = lowBinaryMat.at< float >( x, y );
                }else if(highBinaryMat.at< float >( x+1, y ) != 0.0){
                    res.at< float >( x, y ) = lowBinaryMat.at< float >( x, y );
                }else if(highBinaryMat.at< float >( x, y+1 ) != 0.0){
                    res.at< float >( x, y ) = lowBinaryMat.at< float >( x, y );
                }
            }
        }
    }

    //destroy other matrice
    highBinaryMat.~Mat();
    lowBinaryMat.~Mat();

    return res;
}

/******************************************************************************
 * Suppression of pixels whitout others in n-ring neighborhood
 *
 * @param src The matrice on wich we apply the reduction
 ******************************************************************************/
void algorithm::supprIsoletedPoints( cv::Mat& src, int n )
{

    //count number of neighbor
    int count;

    // Iterate through lines except border
    for (int x = n; x < src.rows-n; x++) {
        // Iterate through columns except border
        for ( int y = n; y < src.cols-n; y++ ) {
            count = 0;

            // Iterate through lines neighborhood
            for (int i = -n; i <= n; i++) {
                // Iterate through columns neighborhood
                for (int j = -n; j <= n; j++) {

                    if(src.at< float >( x+i, y+j ) != 0.0)
                        count += 1;

                }
            }
            if(count <= n)
                src.at< float >( x, y ) = 0.0;
        }
    }
}

/******************************************************************************
 * extract all the local extremum
 *
 * @param slope matrice of the slope value
 * @param module input module
 ******************************************************************************/
cv::Mat algorithm::localExtremum( const cv::Mat& slope, const cv::Mat& module )
{
    // Ouput matrice
    cv::Mat res = module.clone();

    int direction;
    float point1, point2;

    // Iterate through lines except border
    for (int x = 1; x < slope.rows-1; x++) {
        // Iterate through columns except border
        for ( int y = 1; y < slope.cols-1; y++ ) {
            direction = (int)(std::abs(slope.at< float >( x, y ))/(PI/4));
            assert(direction<5);

            //get the two points in the same direction
            if(direction == 0 || direction == 4){
                point1 = module.at< float >( x+1, y );
                point2 = module.at< float >( x-1, y );
            }else if(direction == 1){
                point1 = module.at< float >( x+1, y+1 );
                point2 = module.at< float >( x-1, y-1 );
            }else if(direction == 2){
                point1 = module.at< float >( x, y+1 );
                point2 = module.at< float >( x, y-1 );
            }else{
                point1 = module.at< float >( x-1, y+1 );
                point2 = module.at< float >( x+1, y-1 );
            }

            //affect the value if point is superior at the two other points
            if(res.at< float >( x, y ) >= point1 && res.at< float >( x, y ) >= point2){
                //res.at< float >( x, y ) = module.at< float >( x, y );
            }else{
                res.at< float >( x, y ) = 0.0;
            }
         }
    }

    return res;
}

/******************************************************************************
 * Detect all edges in the Matrix
 *
 * @param src input matrix
 *
 * @result list of edges
 ******************************************************************************/
std::vector<algorithm::Edge> algorithm::freemanEncoding( cv::Mat& src )
{
    //matrix of pixels already encontered
    cv::Mat dejaVue = cv::Mat( src.rows, src.cols, CV_8U );
    dejaVue.setTo(0);

     //list of edges
    std::vector<algorithm::Edge> listEdges;

    Edge edg;
    // Iterate through lines
    for (int x = 1; x < src.rows-1; x++) {
        // Iterate through columns
        for ( int y = 1; y < src.cols-1; y++ ) {
            //std::cout << x << " " << y << std::endl;
            if(dejaVue.at< uchar >( x, y )==0 && src.at< float >( x, y ) > 20.0/*input data is eiher 0 or 255, so it's just a test*/){
                dejaVue.at< uchar >( x, y ) = 1;
                edg = *(new Edge());
                edg.s_x = x;
                edg.s_y = y;
                freemanEdges( src, dejaVue, edg );
                listEdges.push_back(edg);
            }
        }
    }

    return listEdges;
}

 /******************************************************************************
  * Follow an edge to an end
  *
  * @param src input matrix
  * @param dejaVue matrix of pixels already seen
  * @param edg Edge to follow
  ******************************************************************************/
void algorithm::freemanEdges(cv::Mat& src, cv::Mat& dejaVue, Edge& edg)
{
    //Freeman directions encoding
    static const int freemanDirections[ 8 ][ 2 ] = { {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1} };

    //current position
    int new_x = edg.s_x, new_y = edg.s_y;
    int x = new_x, y = new_y;

    //counter of step
    int count = 0;

    //direction to follow
    int dir = 1;

    while(true){

        new_x = x + freemanDirections[dir][0];
        new_y = y + freemanDirections[dir][1];

        if(dejaVue.at< uchar >( new_x, new_y ) ==0 && src.at< float >( new_x, new_y ) > 20.0/*input data is eiher 0 or 255, so it's just a test*/ ){
            edg._directions.push_back(dir);
            x = new_x;
            y = new_y;
            dejaVue.at< uchar >( x, y ) = 1;
            count = 0;
            dir = (dir+2)%8;
        }else{
            dir = (dir+7)%8;
            count += 1;
        }

        //return to start point
        if(edg.s_x == new_x && edg.s_y==new_y)
            break;

        //already see all possibility
        if(count == 5)
            break;
    }

    edg.e_x = x;
    edg.e_y = y;

}

/******************************************************************************
 * Connect every edge close to each other
 *
 * @param listEdges list of all edges already exist
 * @param src binary matrice of edges
 * @param slope matrice of gradient directions
 * @param nbIterations number of iterations for the algorithm
 ******************************************************************************/
void algorithm::edgesClosure(std::vector<algorithm::Edge>& listEdges, const cv::Mat& src, const cv::Mat& slope, int nbIterations )
{
   //Freeman directions encoding
   static const int freemanDirections[ 8 ][ 2 ] = { {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1} };

   //current start and end position
   int s_x1, s_y1, s_x2, s_y2;
   int e_x1, e_y1, e_x2, e_y2;

   //direction to follow
   int s_dir1, s_dir2, e_dir1, e_dir2;

   //edge have encounter another edge
   bool s_finish1, s_finish2, e_finish1, e_finish2;

   //vectors of new directions
   std::vector< ushort > s_vecDir1;
   std::vector< ushort > s_vecDir2;
   std::vector< ushort > e_vecDir1;
   std::vector< ushort > e_vecDir2;

   std::vector< ushort >::iterator itDir;
   std::vector<algorithm::Edge>::iterator itEdg;

   //Iterate for all edge already detected
   for(itEdg = listEdges.begin(); itEdg!=listEdges.end(); ++itEdg){

       //initiate coord
       s_x1 = itEdg->s_x; s_y1 = itEdg->s_y;
       s_x2 = itEdg->s_x; s_y2 = itEdg->s_y;
       e_x1 = itEdg->e_x; e_y1 = itEdg->e_y;
       e_x2 = itEdg->e_x; e_y2 = itEdg->e_y;

       s_finish1=false; s_finish2=false; e_finish1=false; e_finish2=false;

       //don't need to go both way for edge with only 1 point
       if(s_x1==e_x1 && s_y1==e_y1){
           //s_finish1=true;
           //s_finish2=true;
           continue;
       }

       //Iterate for nbIterations
       for(int i=0; i<nbIterations; i++){

           if(!s_finish1){
               //move to next direction
               s_dir1 = (int)(((slope.at< float >( s_x1, s_y1 )+PI)/(2*PI))*8.0);
               s_dir1 = (s_dir1+3)%8;

               //get position
               s_x1 = s_x1 + freemanDirections[s_dir1][0];
               s_y1 = s_y1 + freemanDirections[s_dir1][1];

               //if we are out of the matrice, break out
               if(s_x1==0 || s_y1==0 || s_x1>=src.rows-1 || s_y1>=src.cols-1){
                   s_x1 = s_x1 + freemanDirections[(s_dir1+4)%8][0];
                   s_y1 = s_y1 + freemanDirections[(s_dir1+4)%8][1];
                   s_finish1=true;
               }

               //test if encounter an other edge
               if(src.at< float >( s_x1, s_y1 )>0.0||src.at< float >( s_x1+ freemanDirections[(s_dir1+1)%8][0], s_y1+ freemanDirections[(s_dir1+1)%8][1] )>0.0||src.at< float >( s_x1+ freemanDirections[(s_dir1+7)%8][0], s_y1+ freemanDirections[(s_dir1+7)%8][1] )>0.0){

                   //add the news directions
                   itDir = itEdg->_directions.begin();
                   itEdg->_directions.insert(itDir, s_vecDir1.begin(), s_vecDir1.end());
                   s_finish1 = true;

                   //change start position
                   itEdg->s_x = s_x1 + freemanDirections[(s_dir1+4)%8][0];
                   itEdg->s_y = s_y1 + freemanDirections[(s_dir1+4)%8][1];
               }else{
                   //add the new direction to the temporary vector
                   s_vecDir1.push_back((s_dir1+4)%8);
               }
           }

           if(!s_finish2){
               //move to next direction
               s_dir2 = (int)(((slope.at< float >( s_x2, s_y2 )+PI)/(2*PI))*8.0);
               s_dir2 = (s_dir2+7)%8;

               //get position
               s_x2 = s_x2 + freemanDirections[s_dir2][0];
               s_y2 = s_y2 + freemanDirections[s_dir2][1];

               //if we are out of the matrice, break out
               if(s_x2==0 || s_y2==0 || s_x2>=src.rows-1 || s_y2>=src.cols-1){
                   s_x2 = s_x2 + freemanDirections[(s_dir2+4)%8][0];
                   s_y2 = s_y2 + freemanDirections[(s_dir2+4)%8][1];
                   s_finish2=true;
               }

               //test if encounter an other edge
               if(src.at< float >( s_x2, s_y2 )>0.0||src.at< float >( s_x2+ freemanDirections[(s_dir2+1)%8][0], s_y2+ freemanDirections[(s_dir2+1)%8][1] )>0.0||src.at< float >( s_x2+ freemanDirections[(s_dir2+7)%8][0], s_y2+ freemanDirections[(s_dir2+7)%8][1] )>0.0){

                   //add the news directions
                   itDir = itEdg->_directions.begin();
                   itEdg->_directions.insert(itDir, s_vecDir2.begin(), s_vecDir2.end());
                   s_finish2 = true;

                   //change start position
                   itEdg->s_x = s_x2 + freemanDirections[(s_dir2+4)%8][0];
                   itEdg->s_y = s_y2 + freemanDirections[(s_dir2+4)%8][1];
               }else{
                   //add the new direction to the temporary vector
                   s_vecDir2.push_back((s_dir2+4)%8);
               }
           }

           if(!e_finish1){
               //move to next direction
               e_dir1 = (int)(((slope.at< float >( e_x1, e_y1 )+PI)/(2*PI))*8.0);
               e_dir1 = (e_dir1+3)%8;

               //get position
               e_x1 = e_x1 + freemanDirections[e_dir1][0];
               e_y1 = e_y1 + freemanDirections[e_dir1][1];

               //if we are out of the matrice, break out
               if(e_x1==0 || e_y1==0 || e_x1>=src.rows-1 || e_y1>=src.cols-1){
                   e_x1 = e_x1 + freemanDirections[(e_dir1+4)%8][0];
                   e_y1 = e_y1 + freemanDirections[(e_dir1+4)%8][1];
                   e_finish1=true;
               }

               //test if encounter an other edge
               if(src.at< float >( e_x1, e_y1 )>0.0||src.at< float >( e_x1+ freemanDirections[(e_dir1+1)%8][0], e_y1+ freemanDirections[(e_dir1+1)%8][1] )>0.0||src.at< float >( e_x1+ freemanDirections[(e_dir1+7)%8][0], e_y1+ freemanDirections[(e_dir1+7)%8][1] )>0.0){

                   for(itDir = e_vecDir1.begin(); itDir!=e_vecDir1.end(); ++itDir){
                       //add the news directions
                       itEdg->_directions.push_back(*itDir);
                   }
                   e_finish1 = true;

                   //change end position
                   itEdg->e_x = e_x1 + freemanDirections[(e_dir1+4)%8][0];;
                   itEdg->e_y = e_y1 + freemanDirections[(e_dir1+4)%8][1];;
               }else{
                   //add the new direction to the temporary vector
                   e_vecDir1.push_back(e_dir1);
               }
           }

           if(!e_finish2){
               //move to next direction
               e_dir2 = (int)(((slope.at< float >( e_x2, e_y2 )+PI)/(2*PI))*8.0);
               e_dir2 = (e_dir2+7)%8;

               //get position
               e_x2 = e_x2 + freemanDirections[e_dir2][0];
               e_y2 = e_y2 + freemanDirections[e_dir2][1];

               //if we are out of the matrice, break out
               if(e_x2==0 || e_y2==0 || e_x2>=src.rows-1 || e_y2>=src.cols-1){
                   e_x2 = e_x2 + freemanDirections[(e_dir2+4)%8][0];
                   e_y2 = e_y2 + freemanDirections[(e_dir2+4)%8][1];
                   e_finish2=true;
               }

               //test if encounter an other edge
               if(src.at< float >( e_x2, e_y2 )>0.0||src.at< float >( e_x2+ freemanDirections[(e_dir2+1)%8][0], e_y2+ freemanDirections[(e_dir2+1)%8][1] )>0.0||src.at< float >( e_x2+ freemanDirections[(e_dir2+7)%8][0], e_y2+ freemanDirections[(e_dir2+7)%8][1] )>0.0){

                   for(itDir = e_vecDir2.begin(); itDir!=e_vecDir2.end(); ++itDir){
                       //add the news directions
                       itEdg->_directions.push_back(*itDir);
                   }
                   e_finish2 = true;

                   //change end position
                   itEdg->e_x = e_x2 + freemanDirections[(e_dir2+4)%8][0];
                   itEdg->e_y = e_y2 + freemanDirections[(e_dir2+4)%8][1];
               }else{
                   //add the new direction to the temporary vector
                   e_vecDir2.push_back(e_dir2);
               }
           }

       }

       //empty all the temporary vector of direction
       s_vecDir1.clear();
       s_vecDir2.clear();
       e_vecDir1.clear();
       e_vecDir2.clear();
   }
}

/******************************************************************************
 * Helper function to display data (i.e. image) in a window
 *
 * @param text window title
 * @param src input data
 * @param binaire a flag telling whether or not to apply a threshold
 * @param seuil the threshold to apply if required
 ******************************************************************************/
void algorithm::displayMat( const char* text, cv::Mat& src, bool binaire)
{
	// Output image
    cv::Mat res;
    if ( src.channels() == 1 ) {
		// Unsigned char with 1 component (i.e. grayscale)
        res = cv::Mat( src.rows, src.cols, 0 );
    } else {
		// Unsigned char with 3 components (i.e. color)
        res = cv::Mat( src.rows, src.cols, CV_8UC3 );
	}

	// Transform data from "float" to "unsigned char"
	// Iterate through channels
    for ( int i = 0; i < src.channels(); i++ ) {
		// Iterate through lines
        for (int x = 0; x < src.rows; x++) {
			// Iterate through columns
            for ( int y = 0; y < src.cols; y++ ) {
				// Handle grayscale image
                if ( src.channels() == 1 )
				{
                    res.at< uchar >( x, y ) = (uchar)( src.at< float >( x, y ) );
                    // LOG
					//std::cout << src.at<float>(x,y) << "  " << res.at<uchar>(x,y) << std::endl;
                }
				// Handle color image
				else
				{
                    res.at< cv::Vec3b >( x, y )[i] = (uchar)( src.at< cv::Vec3f >( x, y )[i] );
				}
            }
        }
    }
    // Check flag whether or not show in binary
    if ( binaire )
    {
        // Display image in a window
        cv::Mat temp = toBinary(res);
        cv::imshow( text, temp );
        // Delete image
        temp.~Mat();
    }else{

        // Display image in a window
        cv::imshow( text, res );
    }


	
	// Delete image
    res.~Mat();
}

/******************************************************************************
 * Trace all the egdes of a vector
 *
 * @param listEdges the list of edges
 *
 * @return matrix with edges
 ******************************************************************************/
cv::Mat algorithm::traceEdges( const std::vector<algorithm::Edge>& listEdges, int height, int width )
{
    cv::Mat res = cv::Mat( height, width, CV_8U );
    res.setTo(0);

    //coord
    int _x, _y;

    //Freeman directions encoding
    static const int freemanDirections[ 8 ][ 2 ] = { {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1} };

    std::vector< ushort >::const_iterator itDir;
    std::vector<algorithm::Edge>::const_iterator itEdg;
    for(itEdg = listEdges.begin(); itEdg!=listEdges.end(); ++itEdg){
        _x = itEdg->s_x;
        _y = itEdg->s_y;

        res.at< uchar >( _x, _y ) = 255;

        for(itDir = itEdg->_directions.begin(); itDir!=itEdg->_directions.end(); ++itDir){

            _x += freemanDirections[*itDir][0];
            _y += freemanDirections[*itDir][1];
            res.at< uchar >( _x, _y ) = 255;
        }
    }

    return res;
}

