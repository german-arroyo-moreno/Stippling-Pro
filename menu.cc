#include "menu.hh"


// --------------------------------------------------
SMenu::SMenu(char *currentPath, unsigned int w, unsigned int h)
{
   char *str;
   char *endstr;
   int i;

   this->showHelp = true;

   this->painting = false;

   if (currentPath == 0)
      this->path = new char[2];
   else
      this->path = new char[strlen(currentPath + 2)];

   this->path[0] = '\0';
   
   if (currentPath != 0)
      strcpy(this->path, currentPath);

   if (strlen(this->path) <= 0)
   {
      this->path[0] = '.';
      this->path[1] = '\0';
   }

   this->helpTexture = new S3DImage();
   str = this->fileP("../icons/help.png");
   this->helpTexture->load(str);
   this->helpTexture->setTexture2D(GL_TEXTURE0, GL_NEAREST,
				   GL_CLAMP, GL_REPLACE);
   delete [] str;

   this->width = w;
   this->height = h;

   this->zoom = 1;
   this->transX = w / 8;
   this->transY = h / 8;

   this->modKey = 0;
   
   this->moving = false;

   this->mousePrevX = 0;
   this->mousePrevY = 0;

   this->mouseCurrentX = 0;
   this->mouseCurrentY = 0;
   this->motionX = 0;
   this->motionY = 0;

   this->toolRadius = 10;

   this->initUI();

   RandomNumbers::init(); /// It is needed for random numbers initialization


   this->preComp = new S3DImage *[100];
   for (i = 0; i < 100; i ++)
      this->preComp[i] = 0;

   endstr = new char[10000];
   // Load precomputed dots
   for (i = NMIN; i <= NMAX; i ++)
   {
      endstr[0] = '\0';
      sprintf(endstr, "%d.png", i);
      str = this->fileP("../data/dots/precdots", endstr);
      this->preComp[i] = new S3DImage();
      if (!this->preComp[i]->load(str))
      {
	 delete [] str;
	 endstr[0] = '\0';
	 sprintf(endstr, "%d.bmp", i);
	 str = this->fileP("../data/dots/precdots", endstr);
	 
	 if (!this->preComp[i]->load(str))
	 {
	    this->stippF = new SStipple(NPRECOMP, NPRECOMP, i, 
					0.3, 30, 0.8, 0.9, str);
	    this->preComp[i]->load(str);
	 }
      }

      delete [] str;
      this->stippF = 0;
   }
   delete [] endstr;

   return;
}


// --------------------------------------------------
SMenu::~SMenu()
{
}


// --------------------------------------------------
void SMenu::initUI(void)
{
   char *fname;
   int i;

   fname = this->fileP("../fonts/default.ttf");
   this->fnt = TTF_OpenFont(fname, 36);
   if(!fnt) 
   {
      std::cerr << "TTF_OpenFont: " << TTF_GetError() << "\n";
   }
   delete [] fname;

   // Load all the textures for the components:
   this->imgc = new S3DImagesCache();

   glActiveTexture(GL_TEXTURE0);
   fname = this->fileP("../icons/arrow.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/hfash.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/indicator.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/lbreleased.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/lbpressed.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/lbover.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/rbreleased.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/rbpressed.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/rbover.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/upfolder.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/folder.png");
   this->imgc->addImage(fname);
   delete [] fname;


   
   fname = this->fileP("../icons/file.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/select.png");
   this->imgc->addImage(fname);
   delete [] fname;

   
   fname = this->fileP("../icons/search.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bo001.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bp001.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/br001.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bo002.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bp002.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/br002.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bo003.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bp003.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/br003.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/playo.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/playp.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/playr.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/pauso.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/pausr.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bo004.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bp004.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/br004.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bo005.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bp005.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/br005.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bo006.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/bp006.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/br006.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/o0arrow.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/p0arrow.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/r0arrow.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/o1arrow.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/r1arrow.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/o0check.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/p0check.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/r0check.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/o1check.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/r1check.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/o0mask.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/p0mask.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/r0mask.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/o1mask.png");
   this->imgc->addImage(fname);
   delete [] fname;

   fname = this->fileP("../icons/r1mask.png");
   this->imgc->addImage(fname);
   delete [] fname;


   this->imgc->setTexture2D(GL_TEXTURE0, GL_NEAREST, GL_CLAMP, GL_REPLACE);
   // ----------------------------------

   fname = this->fileP("../fonts/default.ttf");
   this->theme = new S3DTheme(fname, 20, fname, 16);
   this->theme->setColor(S3DTheme::BGMENUCOLOR, 
			 246 / 255.0, 194 / 255.0, 152 / 255.0, 0.7);
   delete [] fname;

   this->ui = new S3DInterface(theme, 0, 0, this->width, this->height);
   fname = this->fileP("../icons/arrow.png");
   this->theme->addMouseTexture(this->imgc->getTexture2D(fname));
   delete [] fname;


   // -----------------------------------

   for (i = 0; i < 3; i ++)
   {
      this->dataImg[i] = 0;
   }

   this->stippF = 0;

   // -----------------------------------

   this->initMainMenu();
   this->initBarMenu();
   this->initMSG();
}


// --------------------------------------------------
void SMenu::initTool(int i, const char *text)
{
   float w = 700;
   float h = 700;

   // -----------------------------------
   this->toolButton[i] = new S3DButton(this->theme,
				       this->width - 200, 
				       (i + 1) * 50, 
				       25, 25);
   this->toolButton[i]->setIDMouseOver(this->getIDT(44));
   this->toolButton[i]->setIDReleased(this->getIDT(45));

   this->toolButton[i]->setIDPressed(this->getIDT(42));

   this->toolButton[i]->setIDMouseOverOFF(this->getIDT(41));
   this->toolButton[i]->setIDReleasedOFF(this->getIDT(43));
   this->toolButton[i]->setTristate(true);

   // -----------------------------------
   this->labelTool[i] = new S3DStatic(this->theme,
				       this->width - 150, 
				       (i + 1) * 50 + 6.25, 
				       175, 25);
   this->labelTool[i]->setText(text);

   // -----------------------------------
   this->toolButton[i]->addListener(this);
   this->barMenu->add(this->toolButton[i]);
   this->barMenu->add(this->labelTool[i]);
}


// --------------------------------------------------
void SMenu::setSize(int w, int h)
{   
   float x, y;
   int i;

   this->ui->setSize(w, h);
   x = (float) w / (float) this->width;
   y = (float) h / (float) this->height;

   this->width = w;
   this->height = h;

   this->mainMenu->setPos(this->mainMenu->getX() * x, 
			  this->mainMenu->getY() * y);

   this->toggleToolsButton->setPos(w - 200, 0);

   for (i = 0; i < NTOOLS; i++)
   {
      if (this->toolButton[i] != 0)
      {
	 this->toolButton[i]->setPos(w - 200, (i + 1) * 50);
	 this->labelTool[i]->setPos(w - 150, (i + 1) * 50 + 6.25);
      }
   }
}


// --------------------------------------------------
GLuint SMenu::getIDT(unsigned int i)
{
   GLuint idt;
   unsigned int j;
   char **fn = new char*[1000];

   for (j = 0; j < 100; j ++)
      fn[j] = 0;

   fn[0] = this->fileP("../icons/upfolder.png");
   fn[1] = this->fileP("../icons/folder.png");
   fn[2] = this->fileP("../icons/file.png");
   fn[3] = this->fileP("../icons/select.png");
   fn[4] = this->fileP("../icons/search.png");
   fn[5] = this->fileP("../icons/indicator.png");
   fn[6] = this->fileP("../icons/hfash.png");
   fn[7] = this->fileP("../icons/lbreleased.png");
   fn[8] = this->fileP("../icons/lbover.png");
   fn[9] = this->fileP("../icons/lbpressed.png");
   fn[10] = this->fileP("../icons/rbreleased.png");
   fn[11] = this->fileP("../icons/rbover.png");
   fn[12] = this->fileP("../icons/rbpressed.png");
   fn[13] = this->fileP("../icons/bo001.png");
   fn[14] = this->fileP("../icons/bp001.png");
   fn[15] = this->fileP("../icons/br001.png");
   fn[16] = this->fileP("../icons/bo002.png");
   fn[17] = this->fileP("../icons/bp002.png");
   fn[18] = this->fileP("../icons/br002.png");
   fn[19] = this->fileP("../icons/bo003.png");
   fn[20] = this->fileP("../icons/bp003.png");
   fn[21] = this->fileP("../icons/br003.png");
   fn[22] = this->fileP("../icons/playo.png");
   fn[23] = this->fileP("../icons/playp.png");
   fn[24] = this->fileP("../icons/playr.png");
   fn[25] = this->fileP("../icons/pauso.png");
   fn[26] = this->fileP("../icons/pausr.png");
   fn[27] = this->fileP("../icons/bo004.png");
   fn[28] = this->fileP("../icons/bp004.png");
   fn[29] = this->fileP("../icons/br004.png");
   fn[30] = this->fileP("../icons/bo005.png");
   fn[31] = this->fileP("../icons/bp005.png");
   fn[32] = this->fileP("../icons/br005.png");
   fn[33] = this->fileP("../icons/bo006.png");
   fn[34] = this->fileP("../icons/bp006.png");
   fn[35] = this->fileP("../icons/br006.png");
   fn[36] = this->fileP("../icons/o0arrow.png");
   fn[37] = this->fileP("../icons/p0arrow.png");
   fn[38] = this->fileP("../icons/r0arrow.png");
   fn[39] = this->fileP("../icons/o1arrow.png");
   fn[40] = this->fileP("../icons/r1arrow.png");
   fn[41] = this->fileP("../icons/o0check.png");
   fn[42] = this->fileP("../icons/p0check.png");
   fn[43] = this->fileP("../icons/r0check.png");
   fn[44] = this->fileP("../icons/o1check.png");
   fn[45] = this->fileP("../icons/r1check.png");
   fn[46] = this->fileP("../icons/o0mask.png");
   fn[47] = this->fileP("../icons/p0mask.png");
   fn[48] = this->fileP("../icons/r0mask.png");
   fn[49] = this->fileP("../icons/o1mask.png");
   fn[50] = this->fileP("../icons/r1mask.png");

   if (i >= 100)
      idt = 0;
   else
      idt =  this->imgc->getTexture2D(fn[i]);

   for (j = 0; j < 100; j ++)
      if (fn[j] != 0)
         delete fn[j];

   delete [] fn;

   return idt;
}


// --------------------------------------------------
void SMenu::initMSG(void)
{
   float w = 700;
   float h = 700;


   this->msgD = new S3DContainer(this->theme, 0, 0, 
				 w, h);

   // -----------------------------------
   this->okMSGButton = new S3DButton(this->theme, w * .05, 0, 
                                     w * .2, h * .05);
   this->okMSGButton->setIDMouseOver(this->getIDT(27));
   this->okMSGButton->setIDPressed(this->getIDT(28));
   this->okMSGButton->setIDReleased(this->getIDT(29));
   
   // -----------------------------------
   this->labelMSG = new S3DStatic(this->theme, 0, h * .10, 
				  w * .2, h * .05);
   this->labelMSG->setText("Display some text in one line here.");

   // ------------------------------------
   this->okMSGButton->addListener(this);
   this->msgD->add(this->okMSGButton);

   // ------------------------------------
   this->msgD->add(this->labelMSG);

   // ------------------------------------
   this->msgD->setPos(w * .1, h * .5);
   this->msgD->setSize(w * .8, h * .15);
   this->ui->add(this->msgD);

   this->msgD->setProperty(S3DComponent::HIDED, 1);
}


// --------------------------------------------------
void SMenu::initMainMenu(void)
{
   float w = 700;
   float h = 700;
   S3DStatic *title1;
   char *str;
   this->mainMenu = new S3DContainer(this->theme, 0, 0, 
                                     w, h);
//   this->mainMenu->setProperty(S3DComponent::STATIC, 1);

   // -----------------------------------
   str = this->fileP("../media/photos");
   this->fcImage = new S3DFileChooser(this->theme, str, 0, 
				      h * .5, w * .25, 
				      h * .25);
   delete [] str;

   this->fcImage->setTextures(this->getIDT(0),
                              this->getIDT(1),
                              this->getIDT(2),
                              this->getIDT(3),
                              this->getIDT(4),
                              this->getIDT(5),
                              this->getIDT(6),
                              this->getIDT(7),
                              this->getIDT(8),
                              this->getIDT(9),
                              this->getIDT(10),
                              this->getIDT(11),
                              this->getIDT(12));

   // -----------------------------------
#ifndef SPANISH
   this->fcImage->setTitle("Image:");
#else
   this->fcImage->setTitle("Imagen:");
#endif

   this->fcImage->setDefaultFile("example.jpg");


   str = this->fileP("../media/papers");
   this->fcPaper = new S3DFileChooser(this->theme, str, w * .275, 
				      h * .5, w * .25, 
				      h * .25);
   delete [] str;

   this->fcPaper->setTextures(this->getIDT(0),
                              this->getIDT(1),
                              this->getIDT(2),
                              this->getIDT(3),
                              this->getIDT(4),
                              this->getIDT(5),
                              this->getIDT(6),
                              this->getIDT(7),
                              this->getIDT(8),
                              this->getIDT(9),
                              this->getIDT(10),
                              this->getIDT(11),
                              this->getIDT(12));
   // -----------------------------------
#ifndef SPANISH
   this->fcPaper->setTitle("Paper:");
#else
   this->fcPaper->setTitle("Papel:");
#endif

   this->fcPaper->setDefaultFile("default.jpg");


   str = this->fileP("../media/absorp");
   this->fcAbsorp = new S3DFileChooser(this->theme, str, w * .55, 
				      h * .5, w * .25, 
				      h * .25);
   delete [] str;

   this->fcAbsorp->setTextures(this->getIDT(0),
                              this->getIDT(1),
                              this->getIDT(2),
                              this->getIDT(3),
                              this->getIDT(4),
                              this->getIDT(5),
                              this->getIDT(6),
                              this->getIDT(7),
                              this->getIDT(8),
                              this->getIDT(9),
                              this->getIDT(10),
                              this->getIDT(11),
                              this->getIDT(12));
#ifndef SPANISH
   this->fcAbsorp->setTitle("Absorption:");
#else
   this->fcAbsorp->setTitle("Absorción:");
#endif

   this->fcAbsorp->setDefaultFile("randomhard.jpg");

   // -----------------------------------
   this->ratioImage = new S3DSlider(this->theme, 
				    0, h * .40,
				    w * .25, 
				    h * .025);

   this->ratioImage->setTitle("Ratio: 1x");
   this->ratioImage->setMinValue(1);
   this->ratioImage->setMaxValue(6);
   this->ratioImage->setIncrement(0.5);
   this->ratioImage->setValue(2);

   this->ratioImage->setTextures(this->getIDT(5),
				 this->getIDT(6),
				 this->getIDT(7),
				 this->getIDT(8),
				 this->getIDT(9),
				 this->getIDT(10),
				 this->getIDT(11),
				 this->getIDT(12));

   // -----------------------------------
   title1 = new S3DStatic(this->theme,
			  w * .1, h * .30, 
			  w * .60, h * .025);
#ifndef SPANISH
   title1->setText("Size of the dots:");
#else
   title1->setText("Tamaño de los puntos:");
#endif

   // -----------------------------------
   this->minPenSize = new S3DSlider(this->theme, 
				    0, h * .20,
				    w * .60, 
				    h * .025);
#ifndef SPANISH
   this->minPenSize->setTitle("Minimum:"); //inimum size for dots:");
#else
   this->minPenSize->setTitle("Mínimo:"); //inimum size for dots:");
#endif

   this->minPenSize->setMinValue(NMIN);
   this->minPenSize->setMaxValue(NMAX);
   this->minPenSize->setIncrement(1);
   this->minPenSize->setValue(NMIN);


   this->minPenSize->setTextures(this->getIDT(5),
				 this->getIDT(6),
				 this->getIDT(7),
				 this->getIDT(8),
				 this->getIDT(9),
				 this->getIDT(10),
				 this->getIDT(11),
				 this->getIDT(12));

   // -----------------------------------
   this->maxPenSize = new S3DSlider(this->theme, 
				    0, h * .15,
				    w * .60, 
				    h * .025);
#ifndef SPANISH
   this->maxPenSize->setTitle("Maximum:");
#else
   this->maxPenSize->setTitle("Mínimo:");
#endif

   this->maxPenSize->setMinValue(NMIN);
   this->maxPenSize->setMaxValue(NMAX);
   this->maxPenSize->setIncrement(1);
   this->maxPenSize->setValue((int) (NMAX / 4.0));

   this->maxPenSize->setTextures(this->getIDT(5),
				 this->getIDT(6),
				 this->getIDT(7),
				 this->getIDT(8),
				 this->getIDT(9),
				 this->getIDT(10),
				 this->getIDT(11),
				 this->getIDT(12));


   // -----------------------------------
   this->inkColorP = new S3DColorPicker(this->theme, 
					w * .5, 
					h * .15,
					w * .25, 
					h * .25);
   this->inkColorP->setColor(0, 0, 0, 0.8);
#ifndef SPANISH
   this->inkColorP->setTitle("Ink colour:");
#else
   this->inkColorP->setTitle("Color de la tinta:");
#endif

   // -----------------------------------
   this->startButton = new S3DButton(this->theme, w * .5, 0, 
                                     w * .2, h * .05);
   this->startButton->setIDMouseOver(this->getIDT(13));
   this->startButton->setIDPressed(this->getIDT(14));
   this->startButton->setIDReleased(this->getIDT(15));
   
   

   // -----------------------------------   
   this->mainMenu->add(this->startButton);
   this->mainMenu->add(title1);
   this->mainMenu->add(this->fcImage);
   this->mainMenu->add(this->fcPaper);
   this->mainMenu->add(this->fcAbsorp);
   this->mainMenu->add(this->ratioImage);
   this->mainMenu->add(this->minPenSize);
   this->mainMenu->add(this->maxPenSize);
   this->mainMenu->add(this->inkColorP);

   // ------------------------------------
   this->minPenSize->addListener(this);
   this->maxPenSize->addListener(this);

   // ------------------------------------
   this->startButton->addListener(this);


   // ------------------------------------
   this->mainMenu->setPos(w * .1, h * .075);
   this->mainMenu->setSize(w * .85, h * .85);
   this->ui->add(this->mainMenu);
}


// --------------------------------------------------
void SMenu::initBarMenu(void)
{
   int i;
   float w = 700;
   float h = 700;

   this->barMenu = new S3DContainer(this->theme, 0, 0, 
                                     w, h);
   this->barMenu->setProperty(S3DComponent::STATIC, 1);
   this->barMenu->setProperty(S3DComponent::NOROUNDCORNERS, 1);
   this->barMenu->setProperty(S3DComponent::HIDEDBACKGROUND, 1);
   this->barMenu->setProperty(S3DComponent::NOBORDER, 1);

   // -----------------------------------
   this->toggleToolsButton = new S3DButton(this->theme, 
					   this->width - 200, 0, 
					   90, h * .02);
   this->toggleToolsButton->setIDMouseOver(this->getIDT(36));
   this->toggleToolsButton->setIDReleased(this->getIDT(38));

   this->toggleToolsButton->setIDPressed(this->getIDT(37));
    
   this->toggleToolsButton->setIDMouseOverOFF(this->getIDT(39));
   this->toggleToolsButton->setIDReleasedOFF(this->getIDT(40));
   this->toggleToolsButton->setTristate(true);

   // -----------------------------------
   for (i = 0; i < NTOOLS; i ++)
   {
      this->toolButton[i] = 0;
      this->labelTool[i] = 0;
   }

   i = 0;

#ifndef SPANISH
   this->initTool(i++, "Remove");
   this->initTool(i++, "Redraw");
   this->initTool(i++, "Draw First");
   this->initTool(i++, "Draw Later");
   this->initTool(i++, "Enhance Borders");
   this->initTool(i++, "Shade");
   this->initTool(i++, "Invert");
   this->initTool(i++, "Stop stippling");
   this->initTool(i++, "Continue stippling");
   this->initTool(i++, "More distant dots");
   this->initTool(i++, "Closer dots");
   this->initTool(i++, "Larger dots");
   this->initTool(i++, "Smaller dots");
#else
   this->initTool(i++, "Borrar");
   this->initTool(i++, "Redibujar");
   this->initTool(i++, "Dibujar primero");
   this->initTool(i++, "Dibujar después");
   this->initTool(i++, "Realzar bordes");
   this->initTool(i++, "Sombrear");
   this->initTool(i++, "Invertir");
   this->initTool(i++, "Dejar de puntear");
   this->initTool(i++, "Continuar punteando");
   this->initTool(i++, "Puntos distantes");
   this->initTool(i++, "Puntos cercanos");
   this->initTool(i++, "Puntos menores");
   this->initTool(i++, "Puntos mayores");
#endif

   this->toolButton[0]->setState(true);
   this->toolsActive = false;
   this->numOfTool = 0;


   // -----------------------------------
   this->playButton = new S3DButton(this->theme, 0, 0, 
                                     w * .1, h * .1);
   this->playButton->setIDMouseOver(this->getIDT(25));
   this->playButton->setIDReleased(this->getIDT(26));

   this->playButton->setIDPressed(this->getIDT(23));

   this->playButton->setIDMouseOverOFF(this->getIDT(22));
   this->playButton->setIDReleasedOFF(this->getIDT(24));
   this->playButton->setTristate(true);

   // -----------------------------------
   this->newButton = new S3DButton(this->theme, w * .13, 0, 
                                     w * .1, h * .1);
   this->newButton->setIDMouseOver(this->getIDT(30));
   this->newButton->setIDPressed(this->getIDT(31));
   this->newButton->setIDReleased(this->getIDT(32));

   // -----------------------------------
   this->saveButton = new S3DButton(this->theme, w * .23, 0, 
                                     w * .1, h * .1);
   this->saveButton->setIDMouseOver(this->getIDT(16));
   this->saveButton->setIDPressed(this->getIDT(17));
   this->saveButton->setIDReleased(this->getIDT(18));

   // -----------------------------------
   this->maskButton = new S3DButton(this->theme, w * .33, 0,
                                     w * .1, h * .1);
   this->maskButton->setIDMouseOver(this->getIDT(49));
   this->maskButton->setIDReleased(this->getIDT(50));

   this->maskButton->setIDPressed(this->getIDT(47));

   this->maskButton->setIDMouseOverOFF(this->getIDT(46));
   this->maskButton->setIDReleasedOFF(this->getIDT(48));
   this->maskButton->setTristate(true);

   // -----------------------------------
   this->helpButton = new S3DButton(this->theme, w * .43, 0, 
                                     w * .1, h * .1);
   this->helpButton->setIDMouseOver(this->getIDT(19));
   this->helpButton->setIDPressed(this->getIDT(20));
   this->helpButton->setIDReleased(this->getIDT(21));

   // -----------------------------------
   this->exitButton = new S3DButton(this->theme, w * .53, 0, 
                                     w * .1, h * .1);
   this->exitButton->setIDMouseOver(this->getIDT(33));
   this->exitButton->setIDPressed(this->getIDT(34));
   this->exitButton->setIDReleased(this->getIDT(35));


   // ------------------------------------
   this->toggleToolsButton->addListener(this);
   this->barMenu->add(this->toggleToolsButton);

   this->newButton->addListener(this);
   this->barMenu->add(this->newButton);

   this->playButton->addListener(this);
   this->barMenu->add(this->playButton);

   this->saveButton->addListener(this);
   this->barMenu->add(this->saveButton);

   this->helpButton->addListener(this);
   this->barMenu->add(this->helpButton);

   this->exitButton->addListener(this);
   this->barMenu->add(this->exitButton);

   this->maskButton->addListener(this);
   this->barMenu->add(this->maskButton);

   this->barMenu->setSize(w * 1.03, h);
   this->barMenu->setProperty(S3DComponent::HIDED, 1);

   this->ui->add(this->barMenu);
}


// --------------------------------------------------
void SMenu::readEventsSDL(SDL_Event *event)
{
   char *key;
   float r;
   float x, y;

   switch(event->type)
   {
      case SDL_KEYDOWN: {
	 key = SDL_GetKeyName(event->key.keysym.sym);
	 if (key[0] == '+')
	    this->zoom *= 1.1;
	 else
	    if (key[0] == '-')
	       this->zoom *= .9;

	 if (key[0] == '2')
	    this->toolRadius *= 1.1;
	 else
	    if (key[0] == '1')
	       this->toolRadius *= .9;

	 if ((event->key.keysym.sym == SDLK_RCTRL) || 
	     (event->key.keysym.sym == SDLK_LCTRL) )
	    this->modKey = 1;

	 if ( (key[0] == SDLK_RCTRL) ||  
	      (key[0] == SDLK_LCTRL) )
	    this->modKey = 1;

	 if (key[0] == 'c')
	    this->modKey = 1;

      } break;

      case SDL_KEYUP: {
	 this->modKey = 0;
      } break;

      case SDL_MOUSEMOTION: {
	 this->motionX = event->motion.x;
	 this->motionY = event->motion.y;

	 if (this->moving)
	 {
	    this->mouseCurrentX = event->motion.x;
	    this->mouseCurrentY = event->motion.y;
	 }

	 if (this->painting)
	 {
	    x = event->motion.x;
	    y = event->motion.y;


	    if ( (this->stippF != 0) && ((abs(this->lastX - x) > 5) ||
					 (abs(this->lastY - y) > 5)) )
	    {
	       this->lastX = x; 
	       this->lastY = y; 

	       x -= this->stippF->getWidth()/2;
	       x -= this->transX;
	       x /= this->zoom;
	       x += this->stippF->getWidth()/2;

	       y -= this->height + this->stippF->getHeight(); 
	       y = event->motion.y - this->height + this->stippF->getHeight(); 
	       y -= this->stippF->getHeight()/2;
	       y += this->transY;
	       y /= this->zoom;
	       y += this->stippF->getHeight()/2;


	       r = this->toolRadius / this->zoom; // radius

	       if ( (this->toolsActive) && 
		    (this->barMenu->getProperty(S3DComponent::HIDED) == 0) )
		  this->stippF->drawProb((int) x, (int) y, r, this->numOfTool);
	    }

	 }

      } break;

      case SDL_MOUSEBUTTONDOWN:{

	 switch (event->button.button)
	 {
	    case SDL_BUTTON_WHEELDOWN:{
	       if (this->modKey != 1)
		  this->zoom *= 0.9;
	       else
		  this->toolRadius *= 0.9;

	       if (this->toolRadius < 1)
		  this->toolRadius = 1;

	    } break;
	       
	    case SDL_BUTTON_WHEELUP:{
	       if (this->modKey != 1)
		  this->zoom *= 1.1;
	       else
		  this->toolRadius *= 1.1;

	       if (this->toolRadius > 50)
		  this->toolRadius = 50;

	    } break;

	    case 1: {
	       if (this->modKey)
	       {
		  this->moving = true;
		  this->mousePrevX = event->button.x;
		  this->mousePrevY = event->button.y;
		  this->mouseCurrentX = event->button.x;
		  this->mouseCurrentY = event->button.y;
	       }
	       else
	       {
		  this->painting = true;

		  x = event->motion.x;
		  y = event->motion.y;

		  if ( (this->stippF != 0) && ((abs(this->lastX - x) > 5) ||
					       (abs(this->lastY - y) > 5)) )
		  {
		     this->lastX = x; 
		     this->lastY = y; 

		     x -= this->stippF->getWidth()/2;
		     x -= this->transX;
		     x /= this->zoom;
		     x += this->stippF->getWidth()/2;

		     y -= this->height + this->stippF->getHeight(); 
		     y = (event->motion.y - this->height + 
			  this->stippF->getHeight()); 
		     y -= this->stippF->getHeight()/2;
		     y += this->transY;
		     y /= this->zoom;
		     y += this->stippF->getHeight()/2;


		     r = this->toolRadius / this->zoom; // radius

		     if ( (this->toolsActive) && 
			  (!this->barMenu->getProperty(S3DComponent::HIDED)) )
			this->stippF->drawProb((int) x, (int) y, r,
					       this->numOfTool);
		  }


	       }
	    } break;

	    case 2: {
	       this->moving = true;
	       this->mousePrevX = event->button.x;
	       this->mousePrevY = event->button.y;
	       this->mouseCurrentX = event->button.x;
	       this->mouseCurrentY = event->button.y;
	    } break;

	    case 3: {
	       this->zoom = 1.0;
	       this->moving = false;
	       this->transX = this->width / 8;
	       this->transY = this->height / 8;
	    } break;
	 }

      } break;

      case SDL_MOUSEBUTTONUP:{
	 if (this->moving)
	 {
	    this->transX += this->mouseCurrentX - this->mousePrevX;
	    this->transY -= this->mouseCurrentY - this->mousePrevY;
	 }

	 this->painting = false;	 
	 this->moving = false;
	 
	 if (this->showHelp == true)
	 {
	    if (this->stippF != 0)
	       this->stippF->setPlay(this->playButton->getState());
	    this->showHelp = false;
	 }
      } break;
   }

   if (!this->showHelp)
      this->ui->readEventsSDL(event);
}


// --------------------------------------------------
void SMenu::displayMSG(int w, int h, const char *text)
{
   this->stippF->setPlay(false);
   this->barMenu->setProperty(S3DComponent::HIDED, 1);
   this->labelMSG->setText(text);

   this->okMSGButton->setPos(w - 0.2, 0);
   this->msgD->setPos(this->width * .1, this->height * .5);
   this->msgD->setSize(w * 2, h);

   this->msgD->setProperty(S3DComponent::HIDED, 0);
}


// --------------------------------------------------
void SMenu::draw(void)
{
   int i;

   if (this->stippF != 0)
   {


      for (i = 0; i < NTOOLS; i ++)
	 if (this->toolButton[i] != 0)
	 {
	    this->toolButton[i]->setProperty(S3DComponent::HIDED, 
					     !this->toolsActive);
	    this->labelTool[i]->setProperty(S3DComponent::HIDED,
					    !this->toolsActive);
	 }


      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
      glEnable(GL_TEXTURE_2D);
      
      glDepthMask(false); 
      glDepthFunc(GL_LEQUAL);
      
//      glClearColor(1, 1, 1, 1);
//      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      {
	 glLoadIdentity();
	 glOrtho(0, this->width, 0, this->height, -10, 10);
         
	 glMatrixMode(GL_MODELVIEW);
	 glPushMatrix();
	 {
	    glLoadIdentity();

	    // draw some points in every iteration
	    for (int k = 0; k < 10; k ++)
	       this->stippF->draw(false);

	    glBindTexture(GL_TEXTURE_2D, this->stippF->draw());

	    glTranslatef((float) this->stippF->getWidth()/2, 
			 (float) this->stippF->getHeight()/2, 0.0);


	    if (!this->moving)
	       glTranslatef(this->transX, this->transY, 0.0);
	    else
	       glTranslatef((this->transX + 
			     this->mouseCurrentX - this->mousePrevX),
			    (this->transY -
			     this->mouseCurrentY + this->mousePrevY),
			    0.0);

	    glScalef(this->zoom, this->zoom, 1.0);

	    glTranslatef(- (float) this->stippF->getWidth()/2, 
			- (float) this->stippF->getHeight()/2, 0.0);

	    S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 
				      this->stippF->getWidth(), 
				      this->stippF->getHeight(), false);
	 }
	 glPopMatrix();

	 if (this->toolsActive)
	 {
	    glPushMatrix();
	    {
	       glDisable(GL_TEXTURE_3D);
	       glDisable(GL_TEXTURE_2D);
	       glDisable(GL_TEXTURE_1D);
	       glColor4f(0.6, 0.0, 0.0, 0.6);
	       glTranslatef(this->motionX, 
			    this->height - this->motionY, 0);
	       S3DPrimitive3D::drawCircle(GL_TRIANGLE_FAN, 0, 0, 0, 
					  this->toolRadius);
	    }
	    glPopMatrix();
	 }

      }
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glMatrixMode(GL_MODELVIEW);
      glDepthFunc(GL_ALWAYS);
      glDepthMask(false); 
   }

   // ----------------------------------
   if (this->mainMenu->getProperty(S3DComponent::HIDED) == 0)
   {
      if ( (this->fcImage->getFile() == 0) ||
	   (this->fcPaper->getFile() == 0) ||
	   (this->fcAbsorp->getFile() == 0) )
      {
	 this->startButton->setProperty(S3DComponent::HIDED, 1);
      }
      else
      {
	 this->startButton->setProperty(S3DComponent::HIDED, 0);
      }
   }


   // ----------------------------------
   if (!this->showHelp)
      this->ui->draw();
   else
   {
      glViewport(0, 0, this->width, this->height);

      glPushMatrix();
      {
	 glDepthMask(false); // Disable the z buffer 

	 glEnable(GL_BLEND);
	 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	 glEnable(GL_TEXTURE_2D);

	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(0, 1, 0, 1, -10, 10);
	 glMatrixMode(GL_MODELVIEW);


	 // select modulate to mix texture with color for shading
	 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

//	 glClearColor(.58, .36, .1, 0.0);
//	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 glLoadIdentity();
	 glBindTexture(GL_TEXTURE_2D, this->helpTexture->getTexture2D());

	 S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);
      }
      glPopMatrix();

      SDL_GL_SwapBuffers();
   }
}


// --------------------------------------------------
char *SMenu::fileP(const char *fname, const char *end)
{
   char *str;

   if (end == 0)
      str = new char[strlen(this->path) + strlen(fname) + 1000];
   else
      str = new char[strlen(this->path) + strlen(fname) + strlen(end) + 1000];

   str[0]='\0'; 

   if (end == 0)
      sprintf(str, "%s/%s", this->path, fname);
   else
      sprintf(str, "%s/%s%s", this->path, fname, end);

   return str;
}


// --------------------------------------------------
void SMenu::listenEvent(S3DListener *sender, const char *msg, 
			      float x, float y, unsigned int button, 
			      unsigned int key, 
			      void *data, unsigned int n)
{
   int i, j;
   char *fname;

   // New image
   if (sender == this->newButton)
   {
      this->barMenu->setProperty(S3DComponent::HIDED, 1);
      this->mainMenu->setProperty(S3DComponent::HIDED, 0);
      delete this->stippF;
      this->stippF = 0;
      this->playButton->setState(false);
      return;
   }

   // Show the menu
   if (sender == this->toggleToolsButton)
   {
      this->toolsActive = this->toggleToolsButton->getState();
      return;
   }

   // Hide message
   if (sender == this->okMSGButton)
   {
      this->toggleToolsButton->setState(false);
      this->toolsActive = false;
      this->barMenu->setProperty(S3DComponent::HIDED, 0);
      this->msgD->setProperty(S3DComponent::HIDED, 1);
      this->stippF->setPlay(this->playButton->getState());
      return;
   }

   // Exit
   if (sender == this->maskButton)
   {
      this->stippF->showMask(this->maskButton->getState());
   }

   // Exit
   if (sender == this->exitButton)
   {
      SDL_Quit();
      exit (0);
   }

   // Show the help
   if (sender == this->helpButton)
   {
      this->stippF->setPlay(false);
      this->showHelp = true; 
      return;
   }

   // Save the image
   if ( (sender == this->saveButton) && (this->stippF != 0) )
   {
      fname = this->fileP("../output/output.bmp");
      this->stippF->save(fname);
      delete [] fname;
      this->displayMSG(280, 100,
		       "The image has been saved in the file " 
		       "output/output.bmp");
      return;
   }

   // Run / pause the algorithm
   if (sender == this->playButton)
   {
      this->stippF->setPlay(this->playButton->getState());
      return;
   }
   
   // If maxsize pressed
   if (sender == this->maxPenSize)
   {
      if (this->maxPenSize->getValue() < this->minPenSize->getValue())
	 this->minPenSize->setValue(this->maxPenSize->getValue());
   }

   // If minsize pressed
   if (sender == this->minPenSize)
   {
      if (this->maxPenSize->getValue() < this->minPenSize->getValue())
	 this->maxPenSize->setValue(this->minPenSize->getValue());
   }

   // If start pressed
   if (sender == this->startButton)
   {
      this->mainMenu->setProperty(S3DComponent::HIDED, 1);
      this->barMenu->setProperty(S3DComponent::HIDED, 0);
   
      for (i = 0; i < 3; i ++)
      {
	 if (this->dataImg[i] != 0)
	    delete this->dataImg[i];
	 this->dataImg[i] = new S3DImage();
      }

      this->dataImg[0]->load(this->fcImage->getFile());
      this->dataImg[1]->load(this->fcPaper->getFile());
      this->dataImg[2]->load(this->fcAbsorp->getFile());

      if (this->stippF != 0)
	 delete this->stippF;

      this->stippF = new SStipple(this->dataImg[0], 
				  this->dataImg[1],
				  this->dataImg[2], 
				  this->ratioImage->getValue(),
				  this->minPenSize->getValue(), 
				  this->maxPenSize->getValue());
      this->stippF->setInkColor(this->inkColorP->getRed(), 
				this->inkColorP->getGreen(),
				this->inkColorP->getBlue(),
				this->inkColorP->getAlpha());
      this->stippF->showMask(this->maskButton->getState());

      this->stippF->setPreComp (this->preComp);

      return;
   }


   // Select the tool
   for (i = 0; i < NTOOLS; i ++)
   {
      if ( (this->toolButton[i] != 0) && (this->toolButton[i] == sender) )
      {
	 for (j = 0; j < NTOOLS; j++)
	    if (this->toolButton[j] != 0)
	       this->toolButton[j]->setState(0);
	 this->toolButton[i]->setState(1);
	 this->numOfTool = i;
	 return;
      }
   }
   

}
