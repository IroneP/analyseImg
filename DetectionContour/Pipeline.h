/*
 * Image processing : edge detection
 *
 * Authors : Pascal Guehl, Clement Picq
 */

/** 
 * @version 1.0
 */

#ifndef PIPELINE_H
#define PIPELINE_H

/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/
 
 // System
#include <cstdio>
#include <cmath>

// OpenCV
#ifdef __linux__
    #include <cv.h>
    #include <highgui.h>

#elif _WIN32
    #include <opencv/cv.hpp>

#endif

// STL
#include <vector>
#include <string>

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

 /******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
 ******************************** CLASS USED **********************************
 ******************************************************************************/

// Project
class Image;
class Filter;
class Hough;

/******************************************************************************
 ****************************** CLASS DEFINITION ******************************
 ******************************************************************************/
 
/**
 * @class Pipeline
 */
class Pipeline
{

	/**************************************************************************
	 ***************************** PUBLIC SECTION *****************************
	 **************************************************************************/
	 
public:

	/****************************** INNER TYPES *******************************/

    /**
     * Threshold types
     */
    enum ThresholdType
    {
        eGlobalThreshold = 0,
        eLocalThreshold,
        eHysteresisThreshold,
        eUserDefinedThreshold,
        eNbThresholdTypes
    };

    /**
     * Filter kernel types
     */
    enum FilterKernelType
    {
        ePrewittKernel = 0,
        eSobelKernel,
        eKirshKernel,
        eCustomKernel,
        eNbFilterKernelTypes
    };

    /**
     * Norm types
     */
    enum NormType
    {
        eLInfinity = 0,
        eL1,
        eL2,
        eNbNormTypes
    };

    /**
     * Directional filtering types
     */
    enum DirectionalFilteringType
    {
        eBidirectional = 0,
        eMultidirectional,
        eNbDirectionalFilteringTypes
    };

    /******************************* ATTRIBUTES *******************************/

	/******************************** METHODS *********************************/

	/**
	 * Constructor
	 */
    Pipeline();

    /**
     * Destructor
     */
    virtual ~Pipeline();

    /**
     * Set application path
     *
     * @param pPath application path
     */
    static void setApplicationPath( const char* pPath );

    /**
     * Print info
     */
    void print();

    /**
     * Execute pipeline
     */
    bool execute();

    /**
    const Image* getImage() const;
    Image* editImage();
    void setImage( Image* pImage );
    **/
    const char* getImageFilename() const;
    void setImageFilename( const char* pFilename );

    /**
     * Initialize filters
     *
     * @return a flag telling whether or not it succeeds
     */
    bool initializeFilters();

    /**
     * Finalize filters
     *
     * @return a flag telling whether or not it succeeds
     */
    bool finalizeFilters();

    /**
     * Get the filter kernel type
     *
     * @return the filter kernel type
     */
    FilterKernelType getFilterKernelType() const;

    /**
     * Set the filter kernel type
     *
     * @param pValue the filter kernel type
     */
    void setFilterKernelType( FilterKernelType pValue );

    /**
     * Get the directional filter kernel type
     *
     * @return the directional filter kernel type
     */
    DirectionalFilteringType getDirectionalFilteringType() const;

    /**
     * Set the directional filter kernel type
     *
     * @param pValue the directional filter kernel type
     */
    void setDirectionalFilteringType( DirectionalFilteringType pValue );

    /**
     * Get the threshold type
     *
     * @return the threshold type
     */
    ThresholdType getThresholdType() const;

    /**
     * Set the threshold type
     *
     * @param pValue the threshold type
     */
    void setThresholdType( ThresholdType pValue );

    /**
     * Get the norm type
     *
     * @return the norm type
     */
    NormType getNormType() const;

    /**
     * Set the norm type
     *
     * @param pValue the norm type
     */
    void setNormType( NormType pValue );

    /**
     * Get the gradient module
     *
     * @return the gradient module
     */
    const cv::Mat& getGradientModule() const;

    /**
     * Get the gradient module
     *
     * @return the gradient module
     */
    cv::Mat& editGradientModule();

    /**
     * Get the gradient slope
     *
     * @return the gradient slope
     */
    const cv::Mat& getGradientSlope() const;

    /**
     * Get the gradient slope
     *
     * @return the gradient slope
     */
    cv::Mat& editGradientSlope();

    // Visualization

    /**
     * Set the flag telling whether or not to visualise the input image
     *
     * @param pFlag the flag telling whether or not to visualise the input image
     */
    void setImageVisible( bool pFlag );

    /**
     * Get the flag telling whether or not to visualise the input image
     *
     * @return the flag telling whether or not to visualise the input image
     */
    bool isImageVisible() const;

    /**
     * Set the flag telling whether or not to visualise the components of the gradient (horizontal, vertical, etc...)
     *
     * @param pFlag the flag telling whether or not to visualise the components of the gradient
     */
    void setGradientVisible( bool pFlag );

    /**
     * Get the flag telling whether or not to visualise the components of the gradient (horizontal, vertical, etc...)
     *
     * @return the flag telling whether or not to visualise the components of the gradient
     */
    bool isGradientVisible() const;

    /**
     * Set the flag telling whether or not to use gradient
     *
     * @return a flag telling whether or not to use gradient
     */
    void setUseGradient( bool pFlag );

    /**
     * Set the flag telling whether or not to use threshold
     *
     * @return a flag telling whether or not to use threshold
     */
    void setUseThreshold( bool pFlag );

    /**
     * Set the flag telling whether or not to use local extrema
     *
     * @return a flag telling whether or not to use local extrema
     */
    void setUseLocalExtrema( bool pFlag );

    /**
     * Set the flag telling whether or not to use edge extraction
     *
     * @return a flag telling whether or not to use edge extraction
     */
    void setUseEdgeExtraction( bool pFlag );

    /**
     * Set the flag telling whether or not to use edge closure
     *
     * @return a flag telling whether or not to use edge closure
     */
    void setUseEdgeClosure( bool pFlag );

    /**
     * Get the percentage of valid pixels for the Global Thresholding method
     *
     * @return the percentage of valid pixels for the Global Thresholding method
     */
    int getGlobalThresholdValidPixelPercentage() const;

    /**
     * Set the percentage of valid pixels for the Global Thresholding method
     *
     * @param pValue the percentage of valid pixels for the Global Thresholding method
     */
    void setGlobalThresholdValidPixelPercentage( int pValue );

    /**
     * Get the threshold compuped by Global Thresholding method
     *
     * @return the threshold compuped by Global Thresholding method
     */
    int getGlobalThresholdValue() const;

    /**
     * Get the window size used by Local Thresholding method
     *
     * @return the window size used by Local Thresholding method
     */
    int getLocalThresholdWindowSize() const;

    /**
     * Set the window size used by Local Thresholding method
     *
     * @param pValue the window size used by Local Thresholding method
     */
    void setLocalThresholdWindowSize( int pValue );

    /**
     * Get the HIGH percentage of valid pixels used by the Hysteresis Thresholding method
     *
     * @return the HIGH percentage of valid pixels used by the Hysteresis Thresholding method
     */
    int getHysteresisThresholdHighValidPixelPercentage() const;

    /**
     * Set the HIGH percentage of valid pixels used by the Hysteresis Thresholding method
     *
     * @param pValue the HIGH percentage of valid pixels used by the Hysteresis Thresholding method
     */
    void setHysteresisThresholdHighValidPixelPercentage( int pValue );

    /**
     * Get the HIGH threshold value computed by the Hysteresis Thresholding method
     *
     * @return the HIGH threshold value computed by the Hysteresis Thresholding method
     */
    int getHysteresisThresholdHighValue() const;

    /**
     * Get the LOW percentage of valid pixels used by the Hysteresis Thresholding method
     *
     * @return the LOW percentage of valid pixels used by the Hysteresis Thresholding method
     */
    int getHysteresisThresholdLowValidPixelPercentage() const;

    /**
     * Set the LOW percentage of valid pixels used by the Hysteresis Thresholding method
     *
     * @param pValue the LOW percentage of valid pixels used by the Hysteresis Thresholding method
     */
    void setHysteresisThresholdLowValidPixelPercentage( int pValue );

    /**
     * Get the LOW threshold value computed by the Hysteresis Thresholding method
     *
     * @return the LOW threshold value computed by the Hysteresis Thresholding method
     */
    int getHysteresisThresholdLowValue() const;

    /**
     * Get the flag to tell whether or not to display images in binary mode
     *
     * @return the flag to tell whether or not to display images in binary mode
     */
    bool useBinaryDisplay() const;

    /**
     * Set the flag to tell whether or not to display images in binary mode
     *
     * @param pFlag the flag to tell whether or not to display images in binary mode
     */
    void setUseBinaryDisplay( bool pFlag );

    /**
     * Get the number of iterations used during the Edge Closure algorithm
     *
     * @return the number of iterations used during the Edge Closure algorithm
     */
    unsigned int getEdgeClosureNbIterations() const;

    /**
     * Set the number of iterations used during the Edge Closure algorithm
     *
     * @param pValue the number of iterations used during the Edge Closure algorithm
     */
    void setEdgeClosureNbIterations( unsigned int pValue );

    /**
     * Set the flag telling whether or not the Hough Transform for segment detection is activated
     *
     * @param pFlag the flag telling whether or not the Hough Transform for segment detection is activated
     */
    void setHoughSegmentDetection( bool pFlag );

    /**
     * Set the Hough Transform criteria for segment detection, i.e. min number of points to determine a segment
     *
     * @param pValue the Hough Transform criteria for segment detection, i.e. min number of points to determine a segment
     */
    void setHoughSegmentCriteria( unsigned int pValue );

    /**
     * Set the flag telling whether or not the Hough Transform for circle detection is activated
     *
     * @param pFlag the flag telling whether or not the Hough Transform for circle detection is activated
     */
    void setHoughCircleDetection( bool pFlag );

    /**
     * Set the Hough Transform criteria for circle detection, i.e. min number of points to determine a circle
     *
     * @param pValue the Hough Transform criteria for circle detection, i.e. min number of points to determine a circle
     */
    void setHoughCircleCriteria( unsigned int pValue );

    void setHoughSegmentThreshold( bool pFlag );
    void setHoughFollowGradientDirection( bool pFlag );

    void setHoughCircleThresholdVotes( bool pFlag );
    void setHoughCircleThresholdVotesValue( unsigned int pValue );

    void setHoughCircleUseFixedRadius( bool pFlag );

    /**************************************************************************
	 **************************** PROTECTED SECTION ***************************
	 **************************************************************************/

protected:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

    /**
     * Application path
     */
    static std::string _applicationPath;

    /**
     * Image filename
     */
    std::string _imageFilename;

    /**
     * Image
     */
    Image* _image;

    /**
     * List of filter kernels
     */
    std::vector< Filter* > _filters;

    /**
     * Gradient module
     */
    cv::Mat _gradientModule;

    /**
     * Gradient slope
     */
    cv::Mat _gradientSlope;

    /**
     * percentage of the high Threshold
     */
    float _highValueHysteresis;

    /**
     * percentage of the low Threshold
     */
    float _lowValueHysteresis;

    /**
     * Flag telling whether or not pipeline use bidirectional or multidirectional filtering
     */
    bool _isMultidirectional;
    /**
     * Directional filtering types
     */
    DirectionalFilteringType _directionalFilteringType;

    /**
     * Flag telling whether or not to use Local Extrema to reduce contour size
     */
    bool _useLocalExtrema;

    /**
     * Flag telling whether or not to use Threhold during the process
     */
    bool _useThreshold;

    /**
     * Threshold type
     */
    ThresholdType _thresholdType;

    /**
     * Filter type
     */
    FilterKernelType _filterKernelType;

    /**
     * TODO CLEAN
     *
     * OpenCV matrices used to stored data during the process.
     * - we store them to be able to display all possible result from the pipeline process.
     */
    cv::Mat _matKernel;
    cv::Mat _gradient[ 4 ];
    cv::Mat _kernelDirection[ 4 ];
    cv::Mat _module;
    cv::Mat _moduleThreshold;
    cv::Mat _pente;
    cv::Mat _penteColor;
    cv::Mat _localExtrema;
    cv::Mat _edges;

    /**
     * Flag telling whether or not to visualize the input image
     */
    bool _visualizeImage;

    /**
     * Flag telling whether or not to visualize the different components of the gradient
     */
    bool _visualizeGradient;

    /**
     * The norm type
     */
    NormType _normType;

    /**
     * Flag telling whether or not to visualize the threshold results
     */
    bool _visualizeThreshold;

    /**
     * Flag telling whether or not to use edge extraction during the pipeline process
     */
    bool _useEdgeExtraction;

    /**
     * Flag telling whether or not to use edge closure during the pipeline process
     */
    bool _useEdgeClosure;

    /**
     * Flag telling whether or not to visualize the edges generated during the pipeline process
     */
    bool _visualizeEdges;

    /**
     * Flag telling whether or not to use gradient computation during the pipeline process
     */
    bool _useGradient;

    // Hysteresis
    // Threshold percentage/parameters
    int _globalThresholdValidPixelPercentage;
    int _localThresholdWindowSize;
    int _hysteresisThresholdHighValidPixelPercentage;
    int _hysteresisThresholdLowValidPixelPercentage;
    // Threshold computed values
    int _globalThresholdValue;
    int _hysteresisThresholdHighValue;
    int _hysteresisThresholdLowValue;

    /**
     * Flag to tell whether or not to display images in binary mode
     */
    bool _useBinaryDisplay;

    /**
     * Number of iterations used during the Edge Closure algorithm
     */
    unsigned int _edgeClosureNbIterations;

    /**
     * Flag telling whether or not the Hough Transform for segment detection is activated
     */
    bool _useHoughSegmentDetection;

    /**
     * Flag telling whether or not the Hough Transform for circle detection is activated
     */
    bool _useHoughCircleDetection;

    /**
     * Hough Transform criteria for segment detection, i.e. min number of points to determine a segment
     */
    unsigned int _houghSegmentCriteria;

    /**
     * Hough Transform criteria for circle detection, i.e. min number of points to determine a circle
     */
    unsigned int _houghCircleCriteria;

    bool _houghSegmentThreshold;
    bool _houghFollowGradientDirection;

    bool _houghCircleThresholdVotes;
    unsigned int _houghCircleThresholdVotesValue;

    bool _useHoughCircleFixedRadius;

    /******************************** METHODS *********************************/
	
    /**************************************************************************
	 ***************************** PRIVATE SECTION ****************************
	 **************************************************************************/

private:

	/****************************** INNER TYPES *******************************/

	/******************************* ATTRIBUTES *******************************/

    Hough* hough;
	
    /******************************** METHODS *********************************/

    // TODO
    // - use dedicated smaller functions
    //bool initializeFilters();
    void computeGradient();
    void threshold();
    void extractLocalExtrema();
    void extractEdges();
    void closeEdges();
	
};

/**************************************************************************
 ***************************** INLINE SECTION *****************************
 **************************************************************************/

#endif // PIPELINE_H
