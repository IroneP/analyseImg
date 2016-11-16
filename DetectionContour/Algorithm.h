/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/** 
 * @version 1.0
 */

#ifndef ALGORITHM_H
#define ALGORITHM_H

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
  * @class algorithm
  */
class algorithm
{

	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/
	 
public:

	/****************************** INNER TYPES *******************************/

    /**
     * @brief The Edge struct
     */
    struct Edge
    {
        Edge() : _directions() {}

        /**
         * Pixel start position
         */
        int s_x,s_y;

        /**
         * Pixel end position
         */
        int e_x,e_y;

        /**
         *  Freeman code : encode the change of directions
         */
        std::vector< unsigned short > _directions;
    };

	/******************************* ATTRIBUTES *******************************/

    /**
     * percentage of show pixels
     */
    //static float percent;

    /**
     * percentage of the high Threshold
     */
    static float _highThresholdPercent;

    /**
     * percentage of the low Threshold
     */
    static float _lowThresholdPercent;

	/******************************** METHODS *********************************/

	/**
	 * Constructor
	 */
    algorithm();
	
	/**
	 * Image filtering - Convolution
	 * - use a kernel to filter an image (in place)
	 *
	 * @param src input image that will be filtered by a kernel
	 * @param filter kernel (Mean, Prewitt, Sobel, etc...)
	 *
	 * @return output filtered data
	 */
    static cv::Mat filter( const cv::Mat& src, const cv::Mat& filter );
	
	/**
	 * Normalize a matrice (i.e. an image)
	 *
	 * @param input matrice
	 *
	 * @return the normalized matrice
	 */
    static void normalize( cv::Mat& src );

    /**
     * Normalize a matrice (i.e. an image)
     *
     * @param input matrice
     *
     * @return the normalized matrice
     */
    static cv::Mat normalizeData( const cv::Mat& src );

    /**
     * Set every pixels of a matrice to 0.0 or 255.0
     *
     * @param src input matrice
     * @param limit limit for the validity of the pixel
     *
     * @return the binary matrice
     */
    static cv::Mat toBinary(const cv::Mat& src);

	/**
	 * Generate the module with norm "L infinite" given a set of components
	 * - i.e. for each pixel, find the max of all components 
	 *
	 * @param modules list of module components
	 * @param NbDirection nulber of directions
	 *
	 * @return the module
	 */
    static cv::Mat moduleLinf( cv::Mat* modules, int NbDirection );
    
	/**
	 * Generate the module with norm "L1" given a set of components
	 * - i.e. for each pixel, compute the sum of absolute of components
	 *
	 * NOTE : for multidirectional filtering, first find the 2 max components, then compute classical L1 norm
	 *
	 * @param modules list of module components
	 * @param NbDirection nulber of directions
	 *
	 * @return the module
	 */
	static cv::Mat moduleL1( cv::Mat* modules, int NbDirection );
    
	/**
	 * Generate the slope given a module
	 *
	 * @param resulting slope image
	 * @param number of directions
	 * @param module input module
     * @param matColor result of the colorisation of the slope
	 *
	 * @return the slope
	 */
    static cv::Mat pente(cv::Mat* pentes, int NbDirection, const cv::Mat& module );

    /**
     * Generate the slope given a module
     *
     * @param slope input slope
     * @param module input module
     *
     * @return the matrice color
     */
    static cv::Mat colorMat(const cv::Mat& slope, const cv::Mat& module );

    /**
     * Apply a threshold given an input dataset with a "local" criteria
     * NOTE : based on an histogram of input data
     * - users have to specify a size to the window of locality
     *
     * @param src input data
     * @param window size of the window
     *
     * @return the Matrix with applied threshold
     */
    static cv::Mat localThreshold( cv::Mat& src, int window );
    static cv::Mat localThresholdEnhanced( cv::Mat& src, int window );

	/**
	 * Determine a threshold given an input dataset with a "global" criteria
	 * NOTE : based on an histogram of input data
	 * - users have to specify a percentage of valid pixels
	 *
	 * @param input data
	 *
	 * @return the computed threshold 
	 */
    static int globalThreshold(const cv::Mat& src , float percentFilter);

	/**
	 * Apply a threshold given an input dataset
	 *
	 * @param input data to threshold
	 * @param threshold
	 */
    static void applyThreshold( cv::Mat* src, int seuil );

    /**
     * Apply a threshold on a given input dataset
     *
     * @param pData input data
     * @param threshold
     *
     * @return the thresholded data
     */
     static cv::Mat applyThreshold( const cv::Mat& pData, int pThreshold );

    /**
     *  Hysteresis Threshold filtering : Return a Matrice made with a high filter and a low filter
     *
     * @param src The matrice on wich we apply the Threshold
     *
     * @return res The matrice filtered
     */
    static cv::Mat hysteresis( const cv::Mat& src, int& pHysteresisHighThreshold, int& pHysteresisLowThreshold );

    /**
     * Suppression of pixels whitout others in n-ring neighborhood
     *
     * @param src The matrice on wich we apply the reduction
     */
    static void supprIsoletedPoints( cv::Mat& src, int n );

    /**
     * extract all the local extremum
     *
     * @param slope matrix of the slope value
     * @param module input module
     */
    static cv::Mat localExtremum( const cv::Mat& slope, const cv::Mat& module );

    /**
     * Detect all edges in the Matrix
     *
     * @param src input matrix
     *
     * @result list of edges
     */
    static std::vector<Edge> freemanEncoding( cv::Mat& src );

    /**
     * Connect every edge close to each other
     *
     * @param listEdges list of all edges already exist
     * @param src binary matrice of edges
     * @param slope matrice of gradient directions
     * @param nbIterations number of iterations for the algorithm
     */
    static void edgesClosure(std::vector<algorithm::Edge>& listEdges, const cv::Mat& src, const cv::Mat& slope, int nbIterations );

    /**
     * Helper function to display data (i.e. image) in a window
     *
     * @param text window title
     * @param src input data
     * @param binaire a flag telling whether or not to apply a threshold
     * @param seuil the threshold to apply if required
     */
    static void displayMat(const char* text, cv::Mat& src, bool binaire);

    /**
     * Trace all the egdes of a vector
     *
     * @param listEdges the list of edges
     *
     * @return matrix with edges
     */
    static cv::Mat traceEdges(const std::vector<Edge>& listEdges , int height, int width);

	/**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

    /**
     * Follow an edge to an end
     *
     * @param src input matrix
     * @param dejaVue matrix of pixels already seen
     * @param edg Edge to folow
     */
    static void freemanEdges(cv::Mat& src, cv::Mat& dejaVue, Edge& edg);

protected:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/
	
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

#endif // ALGORITHM_H
