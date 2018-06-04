#include "stipple.hh"


// --------------------------------------------------
SStipple::SStipple(S3DImage *img, S3DImage *paper, S3DImage *absorp, 
		   float sizeRatio, float minRd, float maxRd)
{
   int i, j;
   long double ldv;
   unsigned char *c;
   float rw, rh;
   float maxh, maxv, minh, minv;
   int cf;
   float color, orgcolor;
   RandomNumbers rnd;
   S3DMatrix *mv, *mh, *mg;
   S3DImage *gauss;
   long double p0, p1, p2, p3;
   float r, g, b, a;
   float rdint;

   this->preComp = new S3DImage *[100];
   for (i = 0; i < 100; i ++)
      this->preComp[i] = 0;


   this->copyAlphaToColor = false;

   this->updateFBO = true;

   this->lastUpdate = SDL_GetTicks();

   mv = new S3DMatrix(S3DMatrix::UNDEFINED, 3);

   mv->set(0, 0, 1);
   mv->set(1, 0, 2);
   mv->set(2, 0, 1);
   mv->set(0, 1, 0);
   mv->set(1, 1, 0);
   mv->set(2, 1, 0);
   mv->set(0, 2, -1);
   mv->set(1, 2, -2);
   mv->set(2, 2, -1);
/*
   mv->set(0, 0, 0);
   mv->set(1, 0, 1);
   mv->set(2, 0, 0);
   mv->set(0, 1, 1);
   mv->set(1, 1, -4);
   mv->set(2, 1, 1);
   mv->set(0, 2, 0);
   mv->set(1, 2, 1);
   mv->set(2, 2, 0);
*/

   mh = new S3DMatrix(S3DMatrix::UNDEFINED, 3);

   mh->set(0, 0, -1);
   mh->set(1, 0, 0);
   mh->set(2, 0, 1);
   mh->set(0, 1, -2);
   mh->set(1, 1, 0);
   mh->set(2, 1, 2);
   mh->set(0, 2, -1);
   mh->set(1, 2, 0);
   mh->set(2, 2, 1);
/*
   mh->set(0, 0, 0);
   mh->set(1, 0, 1);
   mh->set(2, 0, 0);
   mh->set(0, 1, 1);
   mh->set(1, 1, -4);
   mh->set(2, 1, 1);
   mh->set(0, 2, 0);
   mh->set(1, 2, 1);
   mh->set(2, 2, 0);
*/


/*
 // laplace
   mv = new S3DMatrix(S3DMatrix::UNDEFINED, 5);
   for (j = 0; j < 5; j ++)
      for (i = 0; i < 5; i ++)
	 mv->set(i, j, -1);
   mv->set(2, 2, 24);


   mh = new S3DMatrix(S3DMatrix::UNDEFINED, 5);
   for (j = 0; j < 5; j ++)
      for (i = 0; i < 5; i ++)
	 mh->set(i, j, -1);
   mh->set(2, 2, 24);

*/

   mg = new S3DMatrix(S3DMatrix::UNDEFINED, 3);
   mg->set(0, 0, .11);
   mg->set(1, 0, .11);
   mg->set(2, 0, .11);
   mg->set(0, 1, .11);
   mg->set(1, 1, .11);
   mg->set(2, 1, .11);
   mg->set(0, 2, .11);
   mg->set(1, 2, .11);
   mg->set(2, 2, .11);

   this->lastProb = -1;
   this->maxProb = 90;

   this->play = false;

   this->photoTexture = img;
   this->paperTexture = paper;
   this->absorpTexture = absorp;

   this->photoTexture->convert(3);

   this->maxSpace = 4;
   if (minRd <= 0)
      this->minRadius = 0;
   else
      this->minRadius = minRd;

   if (maxRd <= this->minRadius)
      this->maxRadius = 0;
   else
      this->maxRadius = maxRd - this->minRadius;


   this->inkDefColor[0] = this->inkDefColor[1] = this->inkDefColor[2] = 0;
   this->inkDefColor[3] = 0.40;

   this->contrastTexture = new S3DImage(this->photoTexture);

   this->bordersTBTexture = new S3DImage();
   this->bordersLRTexture = new S3DImage();

   gauss = new S3DImage();
   gauss->convolute(this->photoTexture, mg);
//   gauss->scale(this->photoTexture->getWidth() / 4,
//		this->photoTexture->getHeight() / 4, S3DImage::LINEAR);
//   gauss->scale(this->photoTexture->getWidth(),
//		this->photoTexture->getHeight(), S3DImage::LINEAR);

   this->bordersTBTexture->convolute(gauss, mv);
//   this->bordersTBTexture->save("testv.bmp");

   this->bordersLRTexture->convolute(gauss, mh);
//   this->bordersLRTexture->save("testh.bmp");

   delete mh;
   delete mv;


   this->swidth = (float) this->photoTexture->getWidth() * sizeRatio;
   this->sheight = (float) this->photoTexture->getHeight() * sizeRatio;
   this->fbo = new S3DFBO(this->swidth,
			  this->sheight,
			  3, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, false);

   this->pBufferTexture = new S3DImage(this->swidth, this->sheight, 4);

   this->photoTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
				    GL_CLAMP, GL_REPLACE);

   this->finalImg = new S3DImage();

   if (this->paperTexture != 0)
      this->paperTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
				       GL_CLAMP, GL_REPLACE);

   if (this->absorpTexture != 0)
   {
      this->absorpTexture->convert(3);
      this->absorpTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
					GL_CLAMP, GL_REPLACE);
   }
   this->step = 0; 
   this->id = 0;
   this->radProb = new long double[this->swidth * this->sheight];
   this->spaceProb = new long double[this->swidth * this->sheight];
   this->nLprob = new long double[this->swidth * this->sheight];
   this->orgGprob = new long double[this->swidth * this->sheight];
   this->nGprob = new long double[this->swidth * this->sheight];
   this->stressB = new long double[this->swidth * this->sheight];

   this->dotImg = new S3DImage(this->swidth, this->sheight, 4);

   this->inkViscosity = 30; // % in [0, 100]
   this->absorpFactor = 1.0; // % in [0,1]

   if (this->absorpTexture != 0)
   {
      this->xRat = ((float) this->absorpTexture->getWidth() / 
		    (float) this->swidth);
      this->yRat = ((float) this->absorpTexture->getHeight() / 
		    (float) this->sheight);
   }
   else
   {
      this->xRat = 0;
      this->yRat = 0;
   }

   rw = ((float) this->photoTexture->getWidth() / 
	 (float) this->swidth);
   rh = ((float) this->photoTexture->getHeight() / 
	 (float) this->sheight);

   this->xIRat = rw;
   this->yIRat = rh;


   for (i = 0; i < 256; i ++)
      this->histogram[i] = 0;

   this->maxIntensity = 0;
   this->minIntensity = 255;
   for (j = 0; j < this->sheight; j ++)
      for (i = 0; i < this->swidth; i ++)
      {
	 c = this->photoTexture->getPixelRGB((int) (i * rw), (int) (j * rh));
	 if (c != 0)
	 {
	    cf = (c[0] + c[1] + c[2]) / 3;
//	    if (cf <= 200)
	    this->histogram[cf] ++;
	    if (cf > this->maxIntensity)
	       this->maxIntensity = cf;
	    if (cf < this->minIntensity)
	       this->minIntensity = cf;

	    delete [] c;
	 }
      }

   maxh = -1;
   minh = -1;
   minv = 0;
   maxv = 0;
   for (i = 0; i <= 255; i ++)
   {
      if ( (this->histogram[i] > maxh) || (maxh < 0) )
      {
	 maxv = i;
	 maxh = this->histogram[i];
      }

      if ( (this->histogram[i] < minv) || (maxv < 0) )
      {
	 minv = i;
	 minh = this->histogram[i];
      }
   }

   this->maxProbability = -1;
   this->maxContrast = 0;
   this->maxBorder = 0;



   this->maxIntensity /= 255.0;
   this->minIntensity /= 255.0;
//   this->diffIntensity = this->maxIntensity - this->minIntensity;
   this->diffIntensity = fabs(this->maxIntensity - this->minIntensity);

   // improve the variety of the size in dots
   //   this->diffIntensity = this->diffIntensity / 4.0;

   if (this->diffIntensity < 0.001) // avoid division by 0
      this->diffIntensity = 0.001;



   /// Changing probabilities
   for (j = 0; j < this->sheight; j ++)
      for (i = 0; i < this->swidth; i ++)
      {
	 this->dotImg->setPixelRGBA(i, j, 1, 1, 1, 0);
	 ldv = 0;
	 c = this->photoTexture->getPixelRGB((int) (i * rw), (int) (j * rh));

	 // Contrast image:
	 orgcolor = (c[0] + c[1] + c[2]) / (255.0 * 3.0);
	 color = pow(orgcolor, 2);
	 this->contrastTexture->setPixelRGB((int) (i * rw), (int) (j * rh),
					    color, color, color);

	 if (color * 255.0 > this->maxContrast)
	    this->maxContrast = color * 255.0;

	 // Add the candidate
	 if (color > 0)
	 {
	    this->candX.push_back(i);
	    this->candY.push_back(j);
	 }


	 // Set the maximum value of the histogram
	 this->maxHistogram = maxh;

	 // Change value according histogram
	 // Similar values to the maximal value in histogram has probability 0
	 if ((c != 0) && (abs(orgcolor * 255 - maxv) > 30))
	 {
	    p0 = (double) this->histogram[(int) color] / (double) maxh;
	    delete [] c;

	    c = this->bordersLRTexture->getPixelRGB((int) (i * rw), 
						    (int) (j * rh));
	    p1 = (c[0] / 255.0);
	    delete [] c;

	    c = this->bordersTBTexture->getPixelRGB((int) (i * rw), 
						    (int) (j * rh));
	    p2 = (c[0] / 255.0);
	    delete [] c;


	    if ((p1 + p2) * 0.5 * 255.0 > this->maxBorder)
	       this->maxBorder = (p1 + p2) * 0.5 * 255.0;

	    p3 = (1 - color);
	    

	    ldv = p0 * 0.1 + p1 * 0.3 + p2 * 0.3 + p3 * 0.2;
	 }
	 else
	    ldv = 0;


	 this->nGprob[i + j * this->swidth] = ldv * 100;
	 this->orgGprob[i + j * this->swidth] = ldv * 100;

	 a = ldv;


	 // Improve radius
	 rdint = (1 - orgcolor) / this->diffIntensity;
	 rdint *= 2;
	 rdint -= 1;
	 if (rdint < 0)
	    rdint = 0;

	 ldv = (rdint * this->maxRadius) + this->minRadius;
	 this->radProb[i + j * this->swidth] = ldv;
	 g = rdint;


	 ldv += (color * 0.5 + (p1 + p2) * 0.25) * this->maxSpace;
	 this->spaceProb[i + j * this->swidth] = ldv;
	 b = 1.0 - ldv;

	 ldv = 0.0;
	 this->stressB[i + j * this->swidth] = ldv;
	 r = ldv;

	 this->pBufferTexture->setPixelRGBA(i, j, r, g, b, a);
      }

   this->pBufferTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
				      GL_CLAMP, GL_REPLACE);

   this->showBufferMask = false;
   this->placedDots = 0; 
   
   return;
}


// --------------------------------------------------
SStipple::SStipple(unsigned int colP, unsigned int rowP, float sizeP, 
		   float inkC, float vis, float minAbsorp, float maxAbsorp,
		   char *fname)
{
   int i, j;
   RandomNumbers rndN(minAbsorp, maxAbsorp);
   float c;
   S3DMatrix *mg;
   S3DImage *gauss;
   float max = 25;
   unsigned char *clr;



   this->preComp = new S3DImage *[100];
   for (i = 0; i < 100; i ++)
      this->preComp = 0;

   this->xRat = this->yRat = 1;

   mg = new S3DMatrix(S3DMatrix::UNDEFINED, 3);

   mg->set(0, 0, .34);
   mg->set(0, 1, .34);
   mg->set(0, 2, .34);
   mg->set(1, 0, .34);
   mg->set(1, 1, .34);
   mg->set(1, 2, .34);
   mg->set(2, 0, .34);
   mg->set(2, 1, .34);
   mg->set(2, 2, .34);
/*
   mg->set(0, 0, .11);
   mg->set(0, 1, .11);
   mg->set(0, 2, .11);
   mg->set(1, 0, .11);
   mg->set(1, 1, .11);
   mg->set(1, 2, .11);
   mg->set(2, 0, .11);
   mg->set(2, 1, .11);
   mg->set(2, 2, .11);
*/


   this->inkViscosity = vis; // % in [0, 100]
   this->absorpFactor = 1.0; // % in [0,1]

   this->inkDefColor[0] = this->inkDefColor[1] = this->inkDefColor[2] = 0.9;
   this->inkDefColor[3] = inkC;


   this->swidth = max * (colP + 1);
   this->sheight = max * (rowP + 1);

   this->radProb = new long double[this->swidth * this->sheight];
   this->spaceProb = new long double[this->swidth * this->sheight];
   this->nLprob = new long double[this->swidth * this->sheight];
   this->orgGprob = new long double[this->swidth * this->sheight];
   this->nGprob = new long double[this->swidth * this->sheight];
   this->stressB = new long double[this->swidth * this->sheight];

   this->photoTexture = new S3DImage();
   this->contrastTexture = new S3DImage();

   this->bordersTBTexture = new S3DImage();
   this->bordersLRTexture = new S3DImage();
   this->pBufferTexture = new S3DImage();


   this->fbo = new S3DFBO(this->swidth,
			  this->sheight,
			  3, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, false);

   this->dotImg = new S3DImage(this->swidth, this->sheight, 4);

   this->absorpTexture = new S3DImage(this->swidth, this->swidth, 4);

   for (j = 0; j < this->sheight; j ++)
      for (i = 0; i < this->swidth; i ++)
      {
	 c = rndN.getUniformf();
	 this->absorpTexture->setPixelRGBA(i, j, c, c, c, 1);
	 this->dotImg->setPixelRGBA(i, j, 1, 1, 1, 0);
      }

   this->copyAlphaToColor = true;

   for (j = 0; j < rowP; j ++)
      for (i = 0; i < colP; i ++)
	 this->drawDotBuffer(i * max - max / 2 - 1, j * max - max / 2 - 1, 
			     1000000, max / 2, 0, 0, 0, 1);

//   gauss = new S3DImage();

   gauss = new S3DImage(this->dotImg);

   if (sizeP >= 5)
      gauss->convolute(this->dotImg, mg);   

   for (j = 0; j < this->sheight; j ++)
      for (i = 0; i < this->swidth; i ++)
      {
	 clr = gauss->getPixelRGBA(i, j);
	 gauss->setPixelRGBA(i, j, clr[0] / 255.0, 
			     clr[1] / 255.0, 
			     clr[2] / 255.0, 
			     1);
	 delete [] clr;
      }


   gauss->scale((colP + 1) * sizeP, (rowP + 1) * sizeP, S3DImage::NEAREST);

   gauss->save(fname);

   delete gauss;
   delete mg;
   
   return;
}


// --------------------------------------------------
unsigned int SStipple::getWidth(void)
{
   return this->swidth;
}


// --------------------------------------------------
void SStipple::setPreComp(S3DImage **array)
{
   int i;

   if (array == 0)
      return;

   for (i = 0; i < 100; i ++)
      this->preComp[i] = array[i];

   return;
}


// --------------------------------------------------
unsigned int SStipple::getHeight(void)
{
   return this->sheight;
}


// --------------------------------------------------
void SStipple::showMask(bool show)
{
   this->showBufferMask = show;
}


// --------------------------------------------------
void SStipple::drawCircleBuffer(long int x, long int y, float r) 
{
   unsigned char *c;
   int i, j;
   float cf;
   
   r = r / 2.0;
   
   for (i = -r;  i <= r; i ++)
      for (j = -r; j <= r; j ++)
	 if ( (i * i + j * j <= r * r) &&
	      (x + i >= 0) && (y + j >= 0) &&
	      (x + i < this->swidth) && (y + j < this->sheight) )
	 {
	    this->dotImg->setPixelRGBA(x + i, y + j, 
				       this->inkDefColor[0],
				       this->inkDefColor[1],
				       this->inkDefColor[2],
				       this->inkDefColor[3]);
	 }
}


// --------------------------------------------------
bool SStipple::drawPrecompBuffer(long int x, long int y, unsigned int r) 
{
   unsigned char *c, *c2, *c3;
   int i, j, k;
   float rc0, gc0, bc0, ac0;
   float rc1, gc1, bc1, ac1;
   float rc2, gc2, bc2, ac2;
   float cf;
   float rm;
   RandomNumbers rndN(0, NPRECOMP - 1);
   bool drawn;
   float a;
 
   if (this->preComp == 0)
      return false;

   if (this->preComp[r] == 0)
      return false;
    
   rm = r / 2;

   k = rndN.getUniformi();

   drawn = false;
   for (i = 0; i < r; i ++)
      for (j = 0; j < r; j ++)
      {
	 c = this->preComp[r]->getPixelRGBA(i + r * k, 
					    j + r * k);
	 c2 = this->dotImg->getPixelRGBA(x + i - rm, y + j - rm);
	 c3 = this->absorpTexture->getPixelRGBA((x + i - rm) * xRat,
						(y + j - rm) * yRat);
	 if (c != 0)
	 {
	    if (c2 != 0)
	    {
	       rc0 = (c[0] / 255.0);
	       gc0 = (c[1] / 255.0);
	       bc0 = (c[2] / 255.0);
	       ac0 = 1 - (rc0 + gc0 + bc0) / 3.0;
	       
	       if (ac0 < 0)
		  ac0 = 0;

	       rc1 = c2[0] / 255.0;
	       gc1 = c2[1] / 255.0;
	       bc1 = c2[2] / 255.0;
	       ac1 = c2[3] / 255.0;
	       
	       rc0 *= pow(this->inkDefColor[0], 4);
	       gc0 *= pow(this->inkDefColor[1], 4);
	       bc0 *= pow(this->inkDefColor[2], 4);

	       rc2 = (1 - ac0) * rc1 + (ac0) * rc0;
	       gc2 = (1 - ac0) * gc1 + (ac0) * gc0;
	       bc2 = (1 - ac0) * bc1 + (ac0) * bc0;

	       if (ac0 > 0.2)
	       {
		  a = (2.0 - c3[0] / 255.0);

		  if (a < 0)
		     a = 0;
		  else
		     if (a > 1) 
			a = 1;

		  ac2 = ac1 + a * (this->inkDefColor[3]);
	       }
	       else
		  ac2 = ac1;

	       if (ac2 > 1)
		  ac2 = 1;

	       this->dotImg->setPixelRGBA(x + i - rm, y + j - rm, 
					  rc2, gc2, bc2, ac2);
	       drawn = true;
	    }
	    
	    delete [] c;
	 }

	 if (c2 != 0)
	    delete [] c2;

	 if (c3 != 0)
	    delete [] c3;
      }

   return drawn;
}


// --------------------------------------------------
void SStipple::drawDotBuffer(long int x, long int y, 
			     unsigned int ndots, float size, float seedSize,
			     int stressX, int stressY, unsigned int nseeds)
{
   const long double prX = .5 / (12.0 * 8.0);
   const long double prY = .5 / (6.0 * 8.0);
   const long double prZ = .5; 
   GLuint pointID;
   unsigned int i, j;
   long long int ii, jj;
   long double v, p, r;
   unsigned long long int nprob;
   RandomNumbers rndN;
   RandomNumbers rndP;
   RandomNumbers rndV(0, 100);
   unsigned char *color;
   unsigned char *c, *ab;
   float  cf;
   float caf;
   Uint32 pixel;
   bool end = false;
   long double prob;
   std::vector<unsigned int> xList;
   std::vector<unsigned int> yList;
   std::vector<unsigned int> xNb;
   std::vector<unsigned int> yNb;
   long double distSrc, distStress;
   bool addNewElement;
   float srcX, srcY;
   float strX, strY;
   long long int minPos, maxPos;
   double mindist, maxdist;
   double d;
   int maxAbs, maxP;
   int nf, rdindex;
   RandomNumbers rndOut(0, 1);

   if (this->dotImg == 0)
      return;

   if (this->absorpTexture == 0)
      return;

   if (y < 0)
      return;
   if (x < 0)
      return;
   if (x >= this->dotImg->getWidth())
      return;
   if (y >= this->dotImg->getHeight())
      return;


   seedSize *= size;
   
   if (seedSize < 1)
      seedSize = 1;



   mindist = maxdist = -1;

   srcX = x;
   srcY = y;

   strX = x + stressX;
   strY = y + stressY;

   distSrc = 0;
   distStress = 0;
   

   // Clear the local probability:
   memset (this->nLprob, 0, (sizeof (long double) * 
			     this->swidth * this->sheight));
//   memset (this->orgGprob, 0, sizeof (long double));

   // Define the range of probabilities:
   rndN.range(0, 100);


   for (i = 0; i < nseeds; i ++)
   {
      if ( (i == 1) && ( (stressX != 0) || (stressY != 0) ) )
      {
	 x = strX;
	 y = strY;
      }
      else 
	 if (i > 0)
	 {
	    rndP.range(x - size, x + size);
	    x = rndP.getUniformi();
	    rndP.range(y - size, y + size);
	    y = rndP.getUniformi();
	 }

      if (x < 0)
	 x = 0;
      else
	 if (x >= this->dotImg->getWidth())
	    x =  this->dotImg->getWidth() - 1;

      if (y < 0)
	 y = 0;
      else
	 if (y >= this->dotImg->getHeight())
	    y =  this->dotImg->getHeight() - 1;

      // Place the seed:
      if ( (x >= 0) && (x < this->dotImg->getWidth()) || 
	   (y >= 0) && (y < this->dotImg->getHeight()) )
      {
	 c = this->dotImg->getPixelRGBA(x, y);

	 cf = 1.0 - c[3] / 255.0 - this->inkDefColor[3];
	 if (cf < 0)
	    cf = 0;
	 else
	    if (cf > 1)
	       cf = 1;

	 delete [] c;
      }
      else
	 cf = 0;

      rdindex = xList.size();
      if (seedSize == 1)
      {
	 if ( (x >= 0) && (x < this->dotImg->getWidth()) || 
	      (y >= 0) && (y < this->dotImg->getHeight()) )
	 {
	    xList.push_back(x);
	    yList.push_back(y);
	 }
      }
      else
      {
	 // draw circle
	 for (jj = -seedSize + 1;  jj <= seedSize - 1; jj ++)
	    for (ii = -seedSize + 1;  ii <= seedSize - 1; ii ++)
	    {
	       r = (double) (seedSize - 1);
	       r *= r;
	       p = (double) (ii * ii) + (double) (jj * jj);

//	       std::cerr << "Circle = " << ii << ", " << jj << " ... " 
//			 << r << " =?= " << p << std::endl;

	       if (p < r)
	       {
		  if ( (x + ii >= 0) && (x + ii < this->dotImg->getWidth()) || 
		       (y + jj >= 0) && (y + jj < this->dotImg->getHeight()) )
		  {
		     xList.push_back(x + ii);
		     yList.push_back(y + jj);
//		     std::cerr << "Pushing" << std::endl;
		  }
		  
	       }
	    }
      }


      nf = xList.size() - rdindex;
      if (nf < 0)
	 nf = 0;

      for (ii = rdindex; ii < xList.size(); ii ++)
      {
	 x = xList[ii];
	 y = yList[ii];


	 if ((x >= 0) && (y >= 0) && (x < this->swidth) && (y < this->sheight))
	 {
	    if (this->copyAlphaToColor)
	       this->dotImg->setPixelRGBA(x, y, 
					  cf, cf, cf,
					  1 - cf);
	    else
	       this->dotImg->setPixelRGBA(x, y, 
					  this->inkDefColor[0],
					  this->inkDefColor[1],
					  this->inkDefColor[2],
					  1 - cf);


	    // Decrease the global probability
	    if (this->nGprob[x + y * this->swidth] >= 1)
	    {
	       this->nGprob[x + y * this->swidth] -= 200;
	       if (this->nGprob[x + y * this->swidth] < 0)
		  this->nGprob[x + y * this->swidth] = 0;
	    }

	    this->nLprob[x + y * this->swidth] += (100.0 / 
						   (double) (nseeds * nf));
	 }
      }

   }

   /// ----------- Start algorithm ---------

   x = srcX;
   y = srcY; 


   // Iterations:
   i = 0;
   while (!end)
   {

      // Get a random position in the set
      j = 0;
      v = p = 0;

      v = rndN.getUniformld();
      do {
	 if (xList.size() == 0)
	    return; 

	 v -= p;
	 p = this->nLprob[x + y * this->swidth];


	 x = xList[j];
	 y = yList[j];
//	 std::cerr << "Prob[" << j << "] = " 
//		   << v << " < (?) " << p << std::endl;

	 j ++;
      } while ( (v > p) && (j < xList.size()) );

      // Restore the las element
      j --;


      // Place the dot:
      c = this->dotImg->getPixelRGBA(x, y);
//   std::cerr << "Pixel: "
//	     << (int) c[0] << ", " 
//	     << (int) c[1] << ", " 
//	     << (int) c[2] << "\n";
      if (c != 0)
	 cf = 1.0 - c[3] / 255.0 - this->inkDefColor[3]; 
      else 
	 return;

      if (cf < 0)
	 cf = 0;
      else
	 if (cf > 1)
	    cf = 1;

//      std::cerr << "Gray: "
//		<< cf << "\n";

      ab = this->absorpTexture->getPixelRGB(x * xRat, y * yRat);



      if (this->copyAlphaToColor)
	 this->dotImg->setPixelRGBA(x, y, 
				    cf, cf, cf,
				    1 - cf);
      else
	 this->dotImg->setPixelRGBA(x, y, 
				    this->inkDefColor[0],
				    this->inkDefColor[1],
				    this->inkDefColor[2],
				    1 - cf);

      // Decrease the global probability
      if (this->nGprob[x + y * this->swidth] >= 1)
      {
	 this->nGprob[x + y * this->swidth] -= 200;
	 if (this->nGprob[x + y * this->swidth] < 0)
	    this->nGprob[x + y * this->swidth] = 0;
      }



      caf = cf - (ab[0] / 255.0) * this->absorpFactor;
      if (caf > 0)
      {
	 prob = 0;

	 p = rndV.getUniformld();
	 if (p > this->inkViscosity)
	 {
	    // probability for viscosity
	    prob = this->nLprob[x + y * this->swidth] * .5;
	    // / ((1.0 - p) / 100.0);
	 }
      }
      else
      {
	 // probability for absorption
	 prob = this->nLprob[x + y * this->swidth];
      }

      delete [] ab;
      delete [] c;

      // Adjust probability
      if (prob > 0)
      {
	 xNb.clear();
	 yNb.clear();

	 // Search the neighbours
	 for (jj = -1; jj <= 1; jj++)
	    for (ii = -1; ii <= 1; ii++)
	    {
	       if ( ((long int) x + ii >= 0) && 
		    ((long int) x + ii < this->swidth) && 
		    ((long int) y + jj >= 0) && 
		    ((long int) y + jj < this->sheight) )
	       {

		  maxAbs = -1;
		  maxP = 0;
		  if ((ii != 0) || (jj != 0) )
		  {
		     ab = this->absorpTexture->getPixelRGB(x * xRat, y * yRat);
		     if ( (ab != 0) && ((int) ab[0] > maxAbs) )
		     {
			maxAbs = ab[0];
			maxP = xNb.size();
		     }
		     
		     if (ab != 0)
			delete [] ab;

		     xNb.push_back(x + ii);
		     yNb.push_back(y + jj);
		  }
	       }
	    }

	 for (ii = 0; ii < xNb.size(); ii++)
	 {
	    // Change probabilities with absorption

	    if (ii == maxP)
	    {
	       p = prob * prZ;
	    }
	    else
	    {
	       if ( (x == xNb[ii]) || (y == yNb[ii]) )
		  p = prob * prY * xNb.size();
	       else
		  p = prob * prX * xNb.size();
	    }


	    this->nLprob[xNb[ii] + yNb[ii] * this->swidth] += p;

	    // update the list
	    addNewElement = true;
	    if ( (stressX == 0) && (stressY == 0) )
	    { 
	       for (jj = 0; (jj < xList.size()) && (addNewElement); jj++)
		  if ( (xList[jj] == xNb[ii]) && (yList[jj] == yNb[ii]) )
		     addNewElement = false;
	    }
	    else // if there is a stress point
	    {
	       for (jj = 0; jj < xList.size(); jj++)
	       {
		  distSrc = sqrt((srcX - xList[jj]) * (srcX - xList[jj]) +
				 (srcY - yList[jj]) * (srcY - yList[jj])); 
		  distStress = sqrt((strX - xList[jj]) * (strX - xList[jj]) +
				    (strY - yList[jj]) * (strY - yList[jj])); 
		  d = distSrc + distStress;

		  if ( (d < mindist) || (mindist < 0) )
		  {
		     mindist = d;
		     minPos = jj;
		  }

		  if ( (d > maxdist) || (maxdist < 0) )
		  {
		     maxdist = d;
		     maxPos = jj;
		  }

		  if ( (xList[jj] == xNb[ii]) && (yList[jj] == yNb[ii]) )
		     addNewElement = false;
	       }

	       // switch probability
	       d = this->nLprob[xList[maxPos] + yList[maxPos] * this->swidth];
	       this->nLprob[xList[minPos] + yList[minPos] * this->swidth] += d;
	       this->nLprob[xList[maxPos] + yList[maxPos] * this->swidth] = 0;
	    }
	    
	    if (addNewElement)
	    {
	       xList.push_back(xNb[ii]);
	       yList.push_back(yNb[ii]);
	    }
	 }

	 this->nLprob[x + y * this->swidth] -= prob;

	 if (this->nLprob[x + y * this->swidth] <= 0.0)
	 {
	    xList.erase(xList.begin() + j);
	    yList.erase(yList.begin() + j);
	    this->nLprob[x + y * this->swidth] = 0;
	 }
      }

      if ( (size > 0) && (!end) )
      {
	 distSrc = sqrt((x - srcX) * (x - srcX) + (y - srcY) * (y - srcY)); 
	 if (rndOut.getUniformf() < 1.0)
	    if (distSrc > size) 
	       end = true;
      }

      if (i > ndots)
	 end = true;
      else
	 i++;
   }

}


// --------------------------------------------------
void SStipple::setInkColor(float r, float g, float b, float a)
{
   this->inkDefColor[0] = r;
   this->inkDefColor[1] = g;
   this->inkDefColor[2] = b;
   this->inkDefColor[3] = a;
}

// --------------------------------------------------
void SStipple::setDotSize(float min, float max)
{
   this->minRadius = min;
   if (min <= max)
      this->maxRadius = 0;

   this->maxRadius = max - min;
}


// --------------------------------------------------
void SStipple::renderRay(int srcx, int srcy)
{
   int x, y;
   unsigned char *c;
   float r;
   long int i, j;
   float rborder, rcontrast;
   RandomNumbers rndN;
   long double stress;
   long double sim;
   int nkern;
   float rs;
   float reg = 0.6;
   double space;


   x = srcx;
   y = srcy;

 
   c = this->contrastTexture->getPixelRGB(x * this->xIRat,
					  y * this->yIRat);
   if ( (c != 0)/* && (c[0] < 250)*/ )
   {
      r = this->radProb[x + this->swidth * y];
      if (r < 1)
	 r = 1;

//	    rndN.range(-r, r);
//	    this->drawDotBuffer(srcx, srcy, 1000000, r, 1,
//				rndN.getUniformf(), rndN.getUniformf());

      stress = this->stressB[x + this->swidth * y];

      sim = reg;
      if (sim < 0)
	 sim = 0;

      nkern = 1;
      rs = 0;


      if (stress > 0)
      {
	 sim = reg - (stress) * reg;
      }

      if (!drawPrecompBuffer(x, y, r)) 
	 this->drawDotBuffer(srcx, srcy, 1000000, r / 2.0, sim, rs, rs, nkern);
//    this->drawCircleBuffer(srcx, srcy, r);
      
      this->placedDots ++;

      if ( (this->placedDots % 100 == 0) && (this->placedDots > 0) )  
      {
	 this->dotImg->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
				    GL_CLAMP, GL_REPLACE);
	 if (this->updateFBO)
	 {
	    this->pBufferTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
					       GL_CLAMP, GL_REPLACE);
	    this->updateFBO = false;
	    this->lastUpdate = SDL_GetTicks();
	 }
      }

      // Decrease probability when a dot is placed
      r = this->spaceProb[x + y * this->swidth] / 3.0;
      for (j = -r; j <= r; j ++)
	 for (i = -r; i <= r; i ++)
	 {
	    x = srcx + i;
	    y = srcy + j;
	    
	    if ((x >= 0) && (x < this->swidth) &&
		(y >= 0) && (y < this->sheight) )
	    {
	       this->nGprob[x + y * this->swidth] -= 5;
	    }
	 }      
   }

   if (c != 0)
      delete [] c;
}


// --------------------------------------------------
void SStipple::playPause(void)
{
   this->play = !(this->play); 
}


// --------------------------------------------------
void SStipple::setPlay(bool p)
{
   this->play = p;
}


// --------------------------------------------------
GLuint SStipple::draw(bool showTex)
{
   RandomNumbers rndN;   
   int x, y, srcx, srcy;
   unsigned int maxX, maxY;
   long double maxP, p, p0, n;
   float r;
   long int i, j;
   unsigned char *c;
   S3DMatrix m(S3DMatrix::UNDEFINED, 6);

   m.set(0, 0, .11);
   m.set(0, 1, .11);
   m.set(0, 2, .11);
   m.set(1, 0, .11);
   m.set(1, 1, .11);
   m.set(1, 2, .11);
   m.set(2, 0, .11);
   m.set(2, 1, .11);
   m.set(2, 2, .11);

   this->step = 1;

   if (this->play)
   {

      switch (this->step)
      {
	 case 0: { // slow option

	    if (candX.size() > 0)
	    {
	       rndN.range(0, this->candX.size() - 1);

	       maxP = 0;
	       i = 0;
	       do {
		  do {
		     j = rndN.getUniformi();
		     x = this->candX[j];
		     y = this->candY[j];
		     
		     i++;
		     
		     p0 = this->nGprob[x + y * this->swidth];
		     if (p0 > maxP)
		     {
			maxP = p0;
			srcx = x;
			srcy = y;
		     }

//                   very slow
//		     if (p0 <= 0)
//		     {
//			this->candX.erase(this->candX.begin() + j);
//			this->candY.erase(this->candY.begin() + j);
//		     }
		     
		  } while ( (p0 < this->maxProb) && (i < 5) );
	       } while ( (p0 < this->maxProb) && (i < 10) );

	       if (i >= 10)
	       {
		  x = srcx;
		  y = srcy;
		  p0 = maxP;
	       }

	       // pretty slow
	       this->candX.erase(this->candX.begin() + j);
	       this->candY.erase(this->candY.begin() + j);

	       if (p0 > 0)
		  this->renderRay(x, y);

	       this->lastProb = p0;
	       if (p0 > maxP)
		  this->maxProb = p0;
	       else
		  this->maxProb = (0.9 * maxP + p0 * 0.1);

	    }

	 } break;


	 case 1: {

	    maxP = 0;
	    i = 0;
	    do {
	       do {
		  rndN.range(0, this->swidth - 1);
		  x = rndN.getUniformi();
		  rndN.range(0, this->sheight - 1);
		  y = rndN.getUniformi();
		     
		  i++;
		     
		  p0 = this->nGprob[x + y * this->swidth];
		  if (p0 > maxP)
		  {
		     maxP = p0;
		     srcx = x;
		     srcy = y;
		  }

	       } while ( (p0 < this->maxProb) && (i < 1000) );
	    } while ( (p0 < this->maxProb) && (i < 2000) );

//	    std::cerr << i << std::endl;

	    if (i >= 2000)
	    {
	       x = srcx;
	       y = srcy;
	    }
	    
	    if (p0 > 0)
	       this->renderRay(x, y);
	    
	    this->lastProb = p0;
	    if (p0 > maxP)
	       this->maxProb = p0;
	    else
	       this->maxProb = (0.9 * maxP + p0 * 0.1);
	 } break;

      }
   }
   else
   {
      if (this->updateFBO)
      {
	 this->dotImg->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
				    GL_CLAMP, GL_REPLACE);   
	 this->pBufferTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST, 
					    GL_CLAMP, GL_REPLACE);   
	 this->updateFBO = false;
	 this->lastUpdate = SDL_GetTicks();
      }
   }


   if (showTex == false)
      return this->fbo->getTexture();

   this->fbo->renderFBO();
   {
      glDepthMask(false); 
      glDepthFunc(GL_LEQUAL);
      
      glViewport(0, 0, this->swidth, this->sheight);
      glClearColor(1, 1, 1, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
      glEnable(GL_TEXTURE_2D);
      

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      {
	 glLoadIdentity();
	 glOrtho(0, 1, 0, 1, -10, 10);
	 
	 glMatrixMode(GL_MODELVIEW);
	 glPushMatrix();
	 {
	    glLoadIdentity();
	    glColor4f(1, 1, 1, 1);
	    if (this->paperTexture != 0)
	    {
	       glBindTexture(GL_TEXTURE_2D, 
			     this->paperTexture->getTexture2D());
	       S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);
	    }

	    glColor4f(1, 1, 1, 1);
	    if (this->dotImg != 0)
	    {
	       glBindTexture(GL_TEXTURE_2D, 
			     this->dotImg->getTexture2D());
	       S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);
	    }

	    if (this->showBufferMask)
	    {
	       glColor4f(1, 1, 1, 1);
	       if ( (this->pBufferTexture != 0) && 
		    (this->pBufferTexture->getTexture2D() != 0) )
	       {
		  glBindTexture(GL_TEXTURE_2D, 
				this->pBufferTexture->getTexture2D());
		  S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);
	       }
	    }
/*
	    glColor4f(1, 1, 1, 1);
	    if (this->finalImg != 0)
	    {
	       glBindTexture(GL_TEXTURE_2D, 
			     this->finalImg->getTexture2D());
	       S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);
	    }
*/
	    glColor4f(1, 1, 1, 1);
	 }
	 glPopMatrix();
	 
	 glMatrixMode(GL_PROJECTION);
      }
      glPopMatrix();
      
      glMatrixMode(GL_MODELVIEW);
   }
   this->fbo->renderFramebuffer();

   return this->fbo->getTexture();   
}


// --------------------------------------------------
void SStipple::drawProb(int x, int y, float r, int type)
{
   int i, j;
   long double ldv, p, p0, p1, p2, p3, color;
   long int pos;
   unsigned char *c;
   float rw, rh;
   int px, py;
   float cr, cg, cb, ca;
   float orgcolor;
   float rdint;

   rw = this->xIRat;
   rh = this->yIRat;

   for (j = -r; j <= r; j ++)  
      for (i = -r; i <= r; i ++)  
      {
	 if ( (i * i + j * j < r * r) &&
	      (i + x >= 0) && (i + x < this->swidth) &&
	      (j + y >= 0) && (j + y < this->sheight) )
	 {
	    pos = i + x + (j + y) * this->swidth;

	    p = this->orgGprob[pos];

	    px = (i + x) * rw;
	    py = (j + y) * rh;

	    c = this->contrastTexture->getPixelRGB(px, py);
	    if (c != 0)
	    {
	       p3 = (c[0] / 255.0);
	       delete [] c;
	    }
	    else
	       p3 = 0;

	    c = this->photoTexture->getPixelRGB(px, py);
	    if (c != 0)
	    {
	       orgcolor = (c[0] + c[1] + c[2]) / (255.0 * 3.0);
	       delete [] c;
	    }
	    else
	       orgcolor = 0;

	    color = p3;

	    p0 = (double) this->histogram[(int) color] / this->maxHistogram;


	    c = this->bordersLRTexture->getPixelRGB(px, py);
	    if (c != 0)
	    {
	       p1 = (c[0] / 255.0);
	       delete [] c;
	    }
	    else
	       p1 = 0;
	    
	    c = this->bordersTBTexture->getPixelRGB(px, py);
	    if (c != 0)
	    {
	       p2 = (c[0] / 255.0);
	       delete [] c;
	    } 
	    else 
	       p2 = 0;


	    p3 = 1 - p3;

	    switch(type)
	    {
	       case 0: { // delete 
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  this->nGprob[pos] = -1;
	       } break;

	       case 1: { // redo
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  this->nGprob[pos] = this->orgGprob[pos];

		  // Improve radius
		  rdint = (1 - orgcolor) / this->diffIntensity;
		  rdint *= 2;
		  rdint -= 1;
		  if (rdint < 0)
		     rdint = 0;
		  ldv = (rdint * this->maxRadius) + this->minRadius;
		  this->radProb[pos] = ldv;

		  ldv += ((1 - p3) * 0.5 + (p1 + p2) * 0.25) * this->maxSpace;
		  this->spaceProb[pos] = ldv;
	       } break; 

	       case 2: { // draw first
		  if (this->nGprob[pos] < this->orgGprob[pos])
		     this->nGprob[pos] = this->orgGprob[pos];
//		  if (this->nGprob[pos] > 20)
		     this->nGprob[pos] *= 1.11;
	       } break;

	       case 3: { // draw later
		  this->nGprob[pos] *= 0.89;
	       } break;

	       case 4: { // borders (remove shade)

		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  if ((p1 < 0.01) && (p2 < 0.01))
		  {
		     p1 = 0;
		     p2 = 0;
		     ldv = 0;
		  }
		  else
		  {
		     if ( (p1 > 0.0) && (p1 < 0.04) )
			p1 = 0.04;
		     if ( (p2 > 0.0) && (p2 < 0.04) )
			p2 = 0.04;
		     
		     ldv = (p1 + p2); //p1 * 0.45 + p2 * 0.45 + p3 * 0.1;
		  }

		  this->nGprob[pos] = ldv * 200;


		  // Improve radius
		  rdint = (1 - orgcolor) / this->diffIntensity;
		  rdint *= 2;
		  rdint -= 1;
		  if (rdint < 0)
		     rdint = 0;
		  
		  ldv = (rdint * this->maxRadius) + this->minRadius;
		  this->radProb[pos] = ldv;

		  ldv *= 0.9;

		  ldv += ((1 - orgcolor) * 0.25 + 
			  (1 - p1 + 1 - p2) * 0.75 * 0.25) * this->maxSpace;
		  this->spaceProb[pos] = ldv;
	       } break; 

	       case 5: { // shade (add shade, remove borders)
		  ldv = p3;
		  this->nGprob[pos] = ldv * 100;



		  rdint = (1 - orgcolor) / this->diffIntensity;
		  rdint *= 2;
		  rdint -= 1;
		  if (rdint < 0)
		     rdint = 0;
		  
		  ldv = (rdint * this->maxRadius) + this->minRadius;
		  this->radProb[pos] = ldv;

		  ldv += 1 + p3 * 0.9 * this->maxSpace;
		  this->spaceProb[pos] = ldv;

	       } break; 

	       case 6: { // invert stippling
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);
		  ldv = p3;
		  if (ldv <= 0.1)
		     this->nGprob[pos] = 0;
		  else
		     this->nGprob[pos] = 100 - ldv * 100;



		  rdint = (orgcolor) / this->diffIntensity;
		  rdint *= 2;
		  rdint -= 1;
		  if (rdint < 0)
		     rdint = 0;
		  
		  ldv = (rdint * this->maxRadius) + this->minRadius;
		  this->radProb[pos] = ldv * 0.25;

		  ldv += 1 + (1 - p3) * 0.9 * this->maxSpace;
		  this->spaceProb[pos] = ldv * 0.25;

/*
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  if (p1 > 0.04)
		     p1 = 0;
		  else
		     p1 = .1 - p1;
		  if (p2 > 0.04)
		     p2 = 0;
		  else
		     p2 = .1 - p2;

		  if (p1 < 0)
		     p1 = 0;
		  if (p2 < 0)
		     p2 = 0;
		  
		  ldv = (p1 + p2); //p1 * 0.45 + p2 * 0.45 + p3 * 0.1;
		  this->nGprob[pos] = ldv * 100;

		  // divide by 2 the radius to avoid very thick dots
		  rdint = (1 - orgcolor) / this->diffIntensity;
		  rdint *= 2;
		  rdint -= 1;
		  if (rdint < 0)
		     rdint = 0;

		  ldv = this->maxRadius - (rdint * 0.25 + (p2 + p1) * 0.75 *
			 this->maxRadius) + this->minRadius;
		  this->radProb[pos] = ldv;

//		  ldv *= 2; // compensate the radius 

		  ldv += ((1 - orgcolor) * 0.25 + 
			  (1 - p1 + 1 - p2) * 0.75) * this->maxSpace;
		  if (ldv < this->maxRadius * 4.0)
		     ldv = this->minRadius * 2;
		  this->spaceProb[pos] = ldv;
*/
	       } break; 

	       case 7: { // stop stippling
		  this->nGprob[pos] = -1;
	       } break;

	       case 8: { // continue stippling
		  this->nGprob[pos] = this->orgGprob[pos];

		  // Improve radius
		  rdint = (1 - orgcolor) / this->diffIntensity;
		  rdint *= 2;
		  rdint -= 1;
		  if (rdint < 0)
		     rdint = 0;
		  ldv = (rdint * this->maxRadius) + this->minRadius;
		  this->radProb[pos] = ldv;

		  ldv += ((1 - p3) * 0.5 + (p1 + p2) * 0.25) * this->maxSpace;
		  this->spaceProb[pos] = ldv;
	       } break;


	       case 9: { // add space
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  if (this->nGprob[pos] < this->orgGprob[pos])
		     this->nGprob[pos] = this->orgGprob[pos];

		  ldv = this->spaceProb[pos];
		  this->spaceProb[pos] = ldv * 1.01;
	       } break; 

	       case 10: { // remove space
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  if (this->nGprob[pos] < this->orgGprob[pos])
		     this->nGprob[pos] = this->orgGprob[pos];

		  ldv = this->spaceProb[pos];
		  this->spaceProb[pos] = ldv * 0.99;
	       } break; 

	       case 11: { // bigger dots
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  if (this->nGprob[pos] < this->orgGprob[pos])
		     this->nGprob[pos] = this->orgGprob[pos];

		  ldv = this->radProb[pos];
		  this->radProb[pos] = ldv * 1.01;
	       } break; 

	       case 12: { // smaller dots
		  this->dotImg->setPixelRGBA(x + i, j + y, 255, 255, 255, 0);

		  if (this->nGprob[pos] < this->orgGprob[pos])
		     this->nGprob[pos] = this->orgGprob[pos];

		  ldv = this->radProb[pos];
		  this->radProb[pos] = ldv * 0.99;
	       } break; 


/*
	       case 8: { // no stress
		  this->stressB[pos] = 0;
	       } break;

	       case 9: { // increase stress
		  this->stressB[pos] += 0.1;
		  if (this->stressB[pos] > 1)
		     stressB[pos] = 1;
	       } break;

	       case 10: { // decrease stress
		  this->stressB[pos] -= 0.1;
		  if (this->stressB[pos] < 0)
		     stressB[pos] = 0;
	       } break;
*/
	    }

//	    if (this->nGprob[pos] > 0)
//	    {
//	       this->candX.push_back(x + i);
//	       this->candY.push_back(y + j);
//	    }

	    cr = this->stressB[pos];
	    cg = ( (float) (this->radProb[pos] - (float) this->minRadius) / 
		   (float) this->maxRadius);
	    cb = 1.0 - this->spaceProb[pos];
	    ca = this->nGprob[pos] / 100;

	    this->pBufferTexture->setPixelRGBA(x + i, y + j, cr, cg, cb, ca);

	    if (this->lastUpdate - SDL_GetTicks() > 1000)
	       this->updateFBO = true;
	 }
      }
}


// --------------------------------------------------
void SStipple::save(char *filename)
{
   int i, j;
   unsigned char *buffer;
   S3DImage *img;
   unsigned long pos;

   buffer = new unsigned char[4 * this->swidth * this->sheight];

   this->fbo->renderFBO();
   {
      glDepthMask(false); 
      glDepthFunc(GL_LEQUAL);
      
      glViewport(0, 0, this->swidth, this->sheight);
      glClearColor(1, 1, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
      glEnable(GL_TEXTURE_2D);
      

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      {
	 glLoadIdentity();
	 glOrtho(0, 1, 0, 1, -10, 10);
	 
	 glMatrixMode(GL_MODELVIEW);
	 glPushMatrix();
	 {
	    glLoadIdentity();
	    glColor4f(1, 1, 1, 1);
	    if (this->paperTexture != 0)
	    {
	       glBindTexture(GL_TEXTURE_2D, 
			     this->paperTexture->getTexture2D());
	       S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, false);
	    }

	    glColor4f(1, 1, 1, 1);
	    if (this->dotImg != 0)
	    {
	       glBindTexture(GL_TEXTURE_2D, 
			     this->dotImg->getTexture2D());
	       S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, false);
	    }
/*
	    glColor4f(1, 1, 1, 1);
	    if (this->finalImg != 0)
	    {
	       glBindTexture(GL_TEXTURE_2D, 
			     this->finalImg->getTexture2D());
	       S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);
	    }
*/
	    glColor4f(1, 1, 1, 1);

	    glReadPixels(0, 0, this->swidth, this->sheight, GL_RGBA,
			 GL_UNSIGNED_BYTE, buffer);
	 }
	 glPopMatrix();
	 
	 glMatrixMode(GL_PROJECTION);
      }
      glPopMatrix();
      
      glMatrixMode(GL_MODELVIEW);
   }
   this->fbo->renderFramebuffer();   

   img = new S3DImage(this->swidth, this->sheight, 4, buffer);
/*
   for (j = 0; j < this->sheight; j ++)
      for (i = 0; i < this->swidth; i ++)
      {
	 pos = 4 * (i + this->swidth * j);
	 img->setPixelRGB(i, j, buffer[pos] / 255.0, 
			  buffer[pos + 1] / 255.0, 
			  buffer[pos + 2] / 255.0);
      }
*/
   img->save(filename);
   delete img;
   delete [] buffer;
}


// --------------------------------------------------
SStipple::~SStipple()
{
   delete [] this->nGprob;
   delete [] this->stressB;
   delete [] this->orgGprob;
   delete [] this->radProb;
   delete [] this->spaceProb;

   delete this->contrastTexture;
   delete this->bordersTBTexture;
   delete this->bordersLRTexture;

   delete this->finalImg;
   delete this->dotImg;
   delete this->pBufferTexture;

   delete [] this->preComp;

   delete this->fbo;
}

