#ifndef __STIPPLE__
#define __STIPPLE__

#include <GL/glew.h>
#include <fstream>
#include <vector>
#include <SDL/SDL_ttf.h>
#include <framebuffer.hh>
#include <image.hh>
#include <primitive3d.hh>
#include "random.hh"


/** @class   SStipple stipple.hh
 *  @author  Germán Arroyo
 *  @date    2009
 *  @brief   This class does the stippling process
 *
 *  @bug     No bugs detected yet
 */

#define NPRECOMP 30
#define NMIN 5
#define NMAX 50

class SStipple {
   public:

      /** 
       * @pre img must be distinct to 0 and a valid pointer
       * @param[in] img The image to stipple
       * @param[in] paper The paper texture
       * @param[in] absorp The absorption of the paper as texture
       * @param[in] sizeRatio A value greater or equal to 1 that is multiplied 
       *                      by the size of the image
       * @param[in] minRd The minimun radius of the dots
       * @param[in] maxRd The maximun radius of the dots
       * @note The images might not have the same resolution
       * @post Constructor. Inizialite the empty component.
       */
      SStipple (S3DImage *img, S3DImage *paper, S3DImage *absorp, 
		float sizeRatio, float minRd, float maxRd);

      /** 
       * @post Constructor. Use only for pre-computing the dots
       */
      SStipple (unsigned int colP, unsigned int rowP, float sizeP, 
		float inkC, float vis, float minAbsorp, float maxAbsorp,
		char *fname);

      /*
       * @param[in] x The position of the dot
       * @param[in] y The position of the dot
       * @param[in] ndots The maximal level of recursion
       * @param[in] size The maximal size of the point (if it is 0, 
       *                 the size will depend on the ndots parameter)
       * @param[in] streessX The relative position of the stress dot
       * @param[in] streessY The relative position of the stress dot
       * @param[in] nseeds Number of seeds
       * @post Draw a dot in the buffer
       */
      void drawDotBuffer(long int x, long int y, 
			 unsigned int ndots, float size,
			 float seedSize = 0,
			 int stressX = 0, int stressY = 0, 
			 unsigned int nseeds = 1);

      /*
       * @param[in] x The position of the dot
       * @param[in] y The position of the dot
       * @param[in] r The size of the point
       * @post Draw a circle in the buffer
       */
      void drawCircleBuffer(long int x, long int y, float r);


      /*
       * @param[in] x The position of the dot
       * @param[in] y The position of the dot
       * @param[in] r The size of the point
       * @post Draw a precomputed dot in the buffer or return false
       */
      bool drawPrecompBuffer(long int x, long int y, unsigned int r);


      /*
       * @post The id of the texture when drawing the result in the FBO
       */
      GLuint draw(bool showTex=true);

      /*
       * @param[in] array The array of dots
       * @post Set the precomputed dots
       * @warning Do not free the array, the pointer is not a copy
       */
      void setPreComp(S3DImage **array);

      /*
       * @param[in] x The x position of the ray
       * @param[in] y The y position of the ray
       * @post Render a ray
       */
      void renderRay(int x, int y);

      /*
       * @post The size of the FBO
       */
      unsigned int getWidth(void);

      /*
       * @param[in] min The minimum size
       * @param[in] max The maximum size
       * @post Change the size of the dots
       */
      void setDotSize (float min, float max);

      /*
       * @param[in] r The red component
       * @param[in] g The green component
       * @param[in] b The blue component
       * @param[in] a The alpha channel
       * @post Change the color of the dots
       */
      void setInkColor (float r, float g, float b, float a);

      /*
       * @param[in] show If the mask is shown or not
       * @post Change the state of visibility of the mask
       */
      void showMask(bool show);

      /*
       * @post Play/Pause the algorithm
       */
      void playPause(void);

      /*
       * @param[in] x The position
       * @param[in] y The position
       * @param[in] r The radius
       * @param[in] type The type of operation
       * @post Draw in the global probability matrix 
       */
      void drawProb(int x, int y, float r, int type);

      /*
       * @param[in] p If it is true, run the algorithm, in other case pause it
       * @post Change the mode of play / pause
       */
      void setPlay(bool p);

      /*
       * @param[in] filename The name of the file
       * @post Save a capture of the FBO
       */
      void save(char *filename);


      /*
       * @post The size of the FBO
       */
      unsigned int getHeight(void);

      /*
       * @post Destructor. Clean the memory
       */
      ~SStipple(void);


   private:
      S3DImage *finalImg; /// The final image
      S3DImage *photoTexture; /// The original image
      S3DImage *contrastTexture; /// The contrasted original image
      S3DImage *paperTexture; /// The texture of the paper
      S3DImage *absorpTexture; /// The absorption of the paper
      S3DImage *bordersTBTexture; /// Top-bottom borders 
      S3DImage *bordersLRTexture; /// Left-right borders 
      S3DImage *pBufferTexture; /// Texture of the probability buffer
      S3DFBO *fbo; /// The FBO to render the final image
      unsigned int swidth, sheight; /// Size of the FBO
      int step; /// Step in the algorithm
      GLuint id; /// Id of the FBO
      float histogram[256]; /// The histogram of the original image
      double maxHistogram; /// Maximum value of the histogram
      float inkViscosity; /// Viscosity of the ink
      long double *nLprob, *nGprob; /// Local and global probability
      long double *radProb, *spaceProb; /** Probability that affects to size
					    of the dots and the space between
					    dots
					**/
      long double *orgGprob; /// Original global probability
      long double *stressB; /// Original global probability
      long double maxProbability; /// Maximal probability
      unsigned char maxContrast, maxBorder; /// Maximal intensity 
      S3DImage *dotImg; /// The image with the points
      float xRat, yRat; /// The ratio for coordinates in absorption images 
      float xIRat, yIRat; /// The ratio for coordinates in original image
      float absorpFactor; /// A factor to increase the absorption of paper
      long long int placedDots; /// The placed dots by the alogrithm
      long double lastProb; /// Last probability
      long double maxProb; /// Maximum probability
      std::vector <int> candX, candY; /// Candidates
      bool play;
      float maxIntensity; /// Max intensity
      float minIntensity; /// Min intensity
      float diffIntensity; /// Max difference between intensity
      double maxSpace; /// Max space between points+
      double minRadius, maxRadius; /// Compute the radius of the points
      bool updateFBO; /// The FBO needs to be updated
      bool showBufferMask; /// If the mask is shown or not
      float inkDefColor[4]; /// The color of the ink
      int lastUpdate; /// The time of the last update
      bool copyAlphaToColor; /// Copy the alpha channel to color for dots
      S3DImage **preComp; /// Precomputed dots
};
#endif
