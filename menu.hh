#ifndef __MENU__
#define __MENU__

#include <GL/glew.h>
#include <fstream>
#include <SDL/SDL_ttf.h>
#include <color.hh>
#include <component.hh>
#include <filemgr.hh>
#include <slider.hh>
#include <theme.hh>
#include <interface.hh>
#include <imgcache.hh>
#include <framebuffer.hh>
#include <container.hh>
#include <filechooser.hh>
#include <button.hh>
#include <colorpicker.hh>
#include <static.hh>
#include "stipple.hh"
#include "random.hh"

#define NTOOLS    100


/** @class   SMenu menu.hh
 *  @author  Germán Arroyo
 *  @date    2009
 *  @brief   This class is a menu for the application
 *
 *  @bug     No bugs detected yet
 */

class SMenu : public S3DListener {
   public:

      /** 
       * @pre currentPath must be a valid path
       * @param[in] currentPath It is the current path of the application
       * @param[in] width It is the width of the application 
       * @param[in] height It is the width of the application 
       * @post Constructor. Inizialite the empty component.
       */
      SMenu(char *currentPath, unsigned int width, unsigned int height);

      /*
       * @post Destructor. Clean the memory
       */
      ~SMenu(void);

      /*
       * @param[in] w The width
       * @param[in] h The height
       * @post Resize the component
       */
      void setSize(int w, int h);

      /*
       * @param[in] event The event
       * @post Read the events for the components
       */
      void readEventsSDL(SDL_Event *event);

      /*
       * @post Draw the UI
       */
      void draw(void);

      /**
       * @param[in] sender The listener which send the message
       * @param[in] msg The message, it must be exact to understand something
       * @param[in] x The x position of the mouse
       * @param[in] y The y position of the mouse
       * @param[in] button The button of the mouse which is pressed
       * @param[in] key The pressed key if any (in other case 0)
       * @param[in] data Additional data if any, according to the message
       * @param[in] n The size of the array, only if data is an array
       * @post Read an event and do something
       * @warning msg must be a valid well defined message and sender must be 
       *          a valid listener in order everything works fine
       */
      virtual void listenEvent(S3DListener *sender, const char *msg, 
			       float x, float y, unsigned int button, 
			       unsigned int key, 
			       void *data, unsigned int n);

   protected:

      /*
       * @param[in] fname The name of the file
       * @param[in] end The end string
       * @post The concatenated file with the path
       * @note The returned string can be freed (it is a copy)
       */
      char *fileP(const char *fname, const char *end=0);

      /*
       * @param[in] i The position of the texture in the cache
       * @post Get the id of the texture in cache
       */
      GLuint getIDT(unsigned int i);

      /*
       * @post Initialize the UI
       */
      void initUI(void);

      /*
       * @param[in] i The index of the tool
       * @param[in] text The text of the tool
       * @post Initialize a new tool
       */
      void initTool(int i, const char *text);

      /*
       * @post Initialize the MSG
       */
      void initMSG(void);

      /*
       * @pre text must be a valid text (!= 0)
       * @param[in] w The width of the window
       * @param[in] h The height of the window
       * @param[in] msg The text
       * @post Initialize the MSG
       */
      void displayMSG(int w, int h, const char *text);

      /*
       * @post Initialize the Main Menu
       */
      void initMainMenu(void);

      /*
       * @post Initialize the menu for the bar
       */
      void initBarMenu(void);


   private:
      char *path; /// Path to the current location
      TTF_Font *fnt; /// The font for the UI
      unsigned int width, height; /// Size of the viewport
      S3DImagesCache *imgc; /// The texture cache
      S3DImage *helpTexture; /// The texture of the help menu
      S3DInterface *ui; /// The interface
      S3DTheme *theme; /// The theme
      S3DContainer *mainMenu; /// The main menu of the application
      S3DFileChooser *fcImage; /// The image loader     
      S3DFileChooser *fcPaper; /// The paper loader
      S3DFileChooser *fcAbsorp; /// The absorption image loader
      S3DButton *startButton; /// The start button
      S3DSlider *ratioImage; /// The ratio of the output image 
      S3DSlider *minPenSize; /// The minimum size of the pen
      S3DSlider *maxPenSize; /// The maximum size of the pen
      S3DContainer *barMenu; /// The bar menu of the application
      S3DButton *saveButton; /// The save button
      S3DButton *newButton; /// The new button
      S3DButton *exitButton; /// The exit button
      S3DButton *helpButton; /// The help button
      S3DButton *playButton; /// The play button
      S3DButton *maskButton; /// The mask button
      S3DButton *toggleToolsButton; /// The button to toggle the tools menu
      S3DButton *toolButton[NTOOLS]; /// The tool button
      S3DStatic *labelTool[NTOOLS]; /// The label of the tool button
      S3DColorPicker *inkColorP; /// The color picker for the ink
      S3DContainer *msgD; /// The message window
      S3DButton *okMSGButton; /// The ok button for the msg window
      S3DStatic *labelMSG; /// The ok button for the msg window
      bool showHelp; /// Show the help 
      SStipple *stippF; /// Object for stippling
      S3DImage *dataImg[3]; /// Data for stippling
      double transX, transY; /// New position of the image
      double zoom; /// The zoom of the image
      double mousePrevX, mousePrevY; /// Previous coordinates of the mouse
      double mouseCurrentX, mouseCurrentY; /// Current coordinates of the mouse
      bool moving; /// If the image is moving or not
      int modKey; /// Modifiers of the keyboard
      bool painting; /// If it is painting or not
      bool toolsActive; /// If the tools are active or not
      int numOfTool; /// The number of the active tool
      S3DImage **preComp; /// Precomputed dots
      int lastX, lastY; /// Last x and y in events
      int motionX, motionY; /// x and y position of the mouse
      float toolRadius; /// The radius of the tool (the pen)
};


#endif
