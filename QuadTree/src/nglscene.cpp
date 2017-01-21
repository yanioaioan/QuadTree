#include <QMouseEvent>
#include <QGuiApplication>
#include "nglscene.h"
#include <iostream>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>
#include <QTime>


//Initialize QuadTree
QuadTree tree(0,0,totalCollisionObjects,totalCollisionObjects);


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.1;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=5;
static int m_frames=0;
QTime currenttime;
double tmpTimeElapsed=0;
int fps=0;

NGLScene::NGLScene()
{
    setFocusPolicy(Qt::StrongFocus);//to make the keyevents respond
//     this->resize(_parent->size ());

    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    m_rotate=false;
    // mouse rotation values set to 0
    m_spinXFace=0;
    m_spinYFace=0;
    updateTimer=startTimer(1000);
    m_animate=true;

}

NGLScene::~NGLScene()
{
//    ngl::NGLInit *Init = ngl::NGLInit::instance();
    std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
//    Init->NGLQuit();
}

void NGLScene::initializeGL ()
{
    ngl::NGLInit::instance();
    glClearColor (0.4,0.4,0.4,1);
    std::cout<<"Initializing NGL\n";

    ngl::Vec3 from(100,200,200);ngl::Vec3 to(0,0,0);ngl::Vec3 up(0,1,0);
    m_cam = new ngl::Camera(from,to,up);
    m_cam->setShape(45,(float)720/576,0.05,350);

    m_text=new ngl::Text(QFont("Arial",14));
    m_text->setScreenSize (width (),height ());

    // now to load the shader and set the values
    // grab an instance of shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["nglDiffuseShader"]->use();

    shader->setShaderParam4f("Colour",1,0,0,1);
    shader->setShaderParam3f("lightPos",1,1,1);
    shader->setShaderParam4f("lightDiffuse",1,1,1,1);



    glEnable(GL_DEPTH_TEST);
    // enable multisampling for smoother drawing
    glEnable(GL_MULTISAMPLE);

    // as re-size is not explicitly called we need to do this.
   glViewport(0,0,width(),height());



    //Fill random 2D Values to Quatree
    for(int i=0;i<totalCollisionObjects;i++)
    {
       ngl::Random *rng=ngl::Random::instance ();
       int x = (int)rng->randomNumber(100);
       int y = (int)rng->randomPositiveNumber (100);

       //save positions
       Point t(x,y);
       treePositions.push_back (t);

       Point tempPoint(x,y);//or insert x,y instead of i,i to create some randomness
       tree.addPoint(tempPoint);
    }


    //find & get the collision neighbours of Point a(8,8), if (8,8) is in the tree
//    Point a(3,3);
    //Point a(2530,7399);    
//    getPointCollisions(a,&tree);

    currenttime.start ();
    tmpTimeElapsed = 0;
    fps= 0;
}

void NGLScene::resizeGL (QResizeEvent *_event )
{
    int w=_event->size().width();
    int h=_event->size().height();
    // set the viewport for openGL
    glViewport(0,0,w,h);
    // now set the camera size values as the screen size has changed
    m_cam->setShape(45,(float)w/h,0.05,350);
    update ();
}

ngl::Mat4 MV;
ngl::Mat4 MVP;
ngl::Mat3 normalMatrix;
ngl::Mat4 M;
///////////////////////////////////
void NGLScene::loadMatricesToShader(ngl::Transformation &_transform, const ngl::Mat4 &_globalTx, ngl::Camera *_cam, ngl::Colour &c)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
//    (*shader)["nglDiffuseShader"]->use();

    shader->setRegisteredUniformFromColour ("Colour",c);


    M=m_transform.getMatrix()*m_mouseGlobalTX;
    MV=  M*m_cam->getViewMatrix();
    MVP=  MV*m_cam->getProjectionMatrix();
    normalMatrix=MV;
    normalMatrix.inverse();
//    shader->setShaderParamFromMat4("MV",MV);
    shader->setShaderParamFromMat4("MVP",MVP);
    shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
//    shader->setShaderParamFromMat4("M",M);
}


void NGLScene::detectAndResolveCollisions(Point &a, std::vector<Point> *collisionAreaPoints , const float & width, const float & height)
{
    for(int i=0;i<collisionAreaPoints->size ();i++)
    {
       if(a.x < (*collisionAreaPoints)[i].x+width &&
          a.x+width > (*collisionAreaPoints)[i].x &&
          a.y < (*collisionAreaPoints)[i].y+height &&
          a.y+height > (*collisionAreaPoints)[i].y)
       {
//          a.x=0.0001;
//          a.y=0.0001;

           //calculate x intersection
           float xOffset=/*abs*/(a.x-(*collisionAreaPoints)[i].x);
           float yOffset=/*abs*/(a.y-(*collisionAreaPoints)[i].y);

           //this point make resovles collisions, and makes it run fast
           //(IF commented out it runs slower)

           //move point / push away
           a.x+=xOffset;
           a.y+=yOffset;

//           (*collisionAreaPoints)[i].x+=xOffset;
//           (*collisionAreaPoints)[i].y+=yOffset;
       }


    }

}


//This method used to be a member function of Quatree Structure
void NGLScene::getPointCollisions(Point &a, QuadTree *tree)
{
//    //if tree node is a leaf node
//    if (tree.dl==NULL && tree.dr==NULL && tree.ul==NULL && tree.ur==NULL)
//    {



//        std::find(tree.dl->container.begin(), tree.dl->container.end(), (a) );

//        //found element
////             if( std::find(tree.dl->container.begin(), tree.dl->container.end(), a) !=tree.dl->container.end() )
//        {
//            //print out container
////                 for(std::vector<Point>::iterator iter=tree.dl->container.begin (); iter!=tree.dl->container.end (); iter++)
////                 {
////                     std::cout<<(*iter).x<<(*iter).y<<std::endl;
////                 }
//        }

//        //if.......................

//    }
//    else//if it's a branch , find if one of the 4 sub-areas intersects with the searched area
//    {

////             dr =new QuadTree(x+halfwidth,y+halfheight, halfwidth,halfheight);



//        //ul
//        if (a.x >=tree.ul->x &&a.y >=tree.ul->y &&a.x <tree.ul->x +tree.ul->width /2 && a.y <tree.ul->y +tree.ul->height /2)
//        {
//            getPointCollisions (a,*(tree.ul));
//        }
//        //ur
//        if (a.x >=tree.ur->x+tree.ur->width /2 &&a.y >=tree.ur->y &&a.x <tree.ur->x +tree.ur->width /2 && a.y <tree.ur->y +tree.ur->height /2)
//        {
//            getPointCollisions (a,*(tree.ur));
//        }
//        //dl
//        if (a.x >=tree.dl->x &&a.y >=tree.dl->y+tree.dl->height /2 &&a.x <tree.dl->x +tree.dl->width /2 && a.y <tree.dl->y +tree.dl->height /2)
//        {
//            getPointCollisions (a,*(tree.dl));
//        }
//        //dr
//        if (a.x >=tree.dr->x+tree.dr->width /2  &&a.y >=tree.dr->y+tree.dr->height /2 &&a.x <tree.dr->x +tree.dr->width /2 && a.y <tree.dr->y +tree.dr->height /2)
//        {
//            getPointCollisions (a,*(tree.dr));
//        }

//    }


    //if tree node is a leaf node
    if ( (tree->ul==NULL && tree->ur==NULL && tree->dl==NULL && tree->dr==NULL) /*&&  std::find(tree->container.begin(), tree->container.end(), a) !=tree->container.end()*/ )
    {
        //found element in root tree
        std::vector<Point>::iterator element=std::find(tree->container.begin(), tree->container.end(), a);
        if( element !=tree->container.end ())//found element
        {
            //print out THE WHOLE container
//                  for(std::vector<Point>::iterator iter=tree->container.begin (); iter!=tree->container.end (); iter++)
//                  {
//                      std::cout<<"Point="<<(*iter).x<<","<<(*iter).y<<std::endl;
//                  }


            /////////////////////       !!!!!!!!!!!!!!!!!
            /// \Section Added - For each element  of the tree that we search, we print the whole container using a another colour.
            /// Each neighbourhood should now have a different separate colour
            ///
            ngl::Colour collisionAreaColour(ngl::Random::instance ()->randomPositiveNumber (), ngl::Random::instance ()->randomPositiveNumber (), ngl::Random::instance ()->randomPositiveNumber (), 1);

            std::vector<Point> *collisionAreaPoints = &tree->container;

//            std::vector<Point> * t=&tree->container;
//            for(int i=0;i<t->size ();i++)
//            {
//                (*t)[i].x=0;
//            }

            //Check collisions and push cube that collide with eachother further apart
//            detectAndResolveCollisions( (*element), collisionAreaPoints, tree->width, tree->height);

            for(int i=0;i<collisionAreaPoints->size ();i++)
            {
                m_transform.setPosition ( (*collisionAreaPoints)[i].x/*/totalCollisionObjects*/ ,(*collisionAreaPoints)[i].y/*/totalCollisionObjects*/, 0);
//                m_transform.setScale (0.01, 0.01, 1);

                loadMatricesToShader (m_transform,m_mouseGlobalTX, m_cam, collisionAreaColour);
                ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance ();
                prim->draw ("cube");


//                std::cout<<"collisionAreaPoints.size ()="<<collisionAreaPoints.size ()<<std::endl;
            }
            /////////////////////       !!!!!!!!!!!!!!!!!


//           std::cout<<"Search Point="<<(*element).x<<","<<(*element).y<<std::endl;
//           for(std::vector<Point>::iterator iter=tree->container.begin (); iter!=tree->container.end (); iter++)
//           {
//               if(!((*iter)==a))
//               {
//                   std::cout<<"Collision Neighbour Point="<<(*iter).x<<","<<(*iter).y<<std::endl;
//               }
//               else
//               {
//                   std::cout<<"No Collision Point Found"<<std::endl;
//               }
//           }


             return;
        }



    }
    else//if it's a branch , find if one of the 4 sub-areas intersects with the searched area / and if yes..then draw it there in that area
    {

        if (a.x >= tree->dl->x && a.x <= tree->dl->x+tree->dl->width &&
            a.y >= tree->dl->y && a.y <= tree->dl->y+tree->dl->height)
        {
            getPointCollisions (a,(tree->dl));
        }

        if (a.x >= tree->dr->x && a.x <= tree->dr->x+tree->dr->width &&
            a.y >= tree->dr->y && a.y <= tree->dr->y+tree->dr->height)
        {
            getPointCollisions (a,(tree->dr));
        }

        if (a.x >= tree->ul->x && a.x <= tree->ul->x+tree->ul->width &&
            a.y >= tree->ul->y && a.y <= tree->ul->y+tree->ul->height)
        {
            getPointCollisions (a,(tree->ul));
        }

        if (a.x >= tree->ur->x && a.x <= tree->ur->x+tree->ur->width &&
            a.y >= tree->ur->y && a.y <= tree->ur->y+tree->ur->height)
        {
            getPointCollisions (a,(tree->ur));
        }
    }

}

//Is used to visualize different levels of the QuadTree But it's not quite working at the moment!
void NGLScene::findTreeElements(QuadTree &tree)
{

    if (tree.container.size ()!=0 /*|| tree.dl!=NULL  || tree.dr!=NULL  || tree.ul!=NULL  || tree.ur!=NULL*/)
    {
        ngl::Colour collisionAreaColour(ngl::Random::instance ()->randomPositiveNumber (), ngl::Random::instance ()->randomPositiveNumber (), ngl::Random::instance ()->randomPositiveNumber (), 1);

        std::vector<Point> collisionAreaPoints(tree.container);

        for(int i=0;i<collisionAreaPoints.size ();i++)
        {
            m_transform.setPosition (collisionAreaPoints[i].x ,collisionAreaPoints[i].y, 0);
//            m_transform.setScale (0.015,0.015,0.015);

            loadMatricesToShader (m_transform,m_mouseGlobalTX, m_cam, collisionAreaColour);
            ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance ();
            prim->draw ("cube");

        }



    }
    else
    {
        return;
    }


    if (tree.ul!=NULL)
    {
        findTreeElements(*(tree.ul));
    }
    if (tree.ur!=NULL)
    {
        findTreeElements(*(tree.ur));
    }
    if (tree.dl!=NULL)
    {
        findTreeElements(*(tree.dl));
    }
    if (tree.dr!=NULL)
    {
        findTreeElements(*(tree.dr));
    }

}




void NGLScene::deleteAreaByAreaElements(QuadTree &tree)
{
    ngl::ShaderLib *shader = ngl::ShaderLib::instance ();
     (*shader)["nglDiffuseShader"]->use();

    if (tree.container.size ()!=0)
    {
        //std::vector<Point> collisionAreaPoints(tree.container);

        ngl::Colour collisionAreaColour(ngl::Random::instance ()->randomPositiveNumber (), ngl::Random::instance ()->randomPositiveNumber (), ngl::Random::instance ()->randomPositiveNumber (), 1);
//        for(int i=0;i<tree.container.size ();i++)
        {

            //clear all tree area elements
//            tree.container.clear ();

            loadMatricesToShader (m_transform,m_mouseGlobalTX, m_cam, collisionAreaColour);
            ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance ();
            prim->draw ("cube");
        }

        update ();

    }
    else
    {
        return;
    }



    //Comment out some of the following quadtree collision regions and PushButton on the Gui, to delete some of the 1st level quatree splits (1 of the 4 first quadrants of the quadtree)
    if (tree.ul!=NULL)
    {
//        tree.ul->container.clear ();
        update ();
    }
    if (tree.ur!=NULL)
    {
//        tree.ur->container.clear ();
        update ();
    }
    if (tree.dl!=NULL)
    {
//        tree.dl->container.clear ();
        update ();
    }
    if (tree.dr!=NULL)
    {
//        tree.dr->container.clear ();
        update ();
    }

}

void NGLScene::paintGL ()
{


    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["nglDiffuseShader"]->use();

    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Rotation based on the mouse position for our global
    // transform
    ngl::Mat4 rotX;
    ngl::Mat4 rotY;
    // create the rotation matrices
    rotX.rotateX(m_spinXFace);
    rotY.rotateY(m_spinYFace);
    // multiply the rotations
    m_mouseGlobalTX=rotY*rotX;
    // add the translations
    m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
    m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
    m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;


    //

//    findTreeElements(tree);

//    for(int i=0;i<totalCollisionObjects;i++)
//    {
//       Point a(i,i);
//       getPointCollisions(a,&tree);
//    }


        for(unsigned int i=0;i<treePositions.size ();i++)
        {
           getPointCollisions(treePositions[i],&tree);
        }


//        for(int i=0;i<treePositions.size ();i++)
//        {
//            m_transform.setPosition (treePositions[i].x/*/totalCollisionObjects*/ ,treePositions[i].y/*/totalCollisionObjects*/, 0);
////            m_transform.setScale (0.015,0.015,0.015);

//            ngl::Colour collisionAreaColour(1,0,0,1);
//            loadMatricesToShader (m_transform,m_mouseGlobalTX, m_cam, collisionAreaColour);
//            ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance ();
//            prim->draw ("cube");

//        }



//    glRectd(tree.x, tree.y, tree.width, tree.height);



        QString text;
        text.sprintf ("Framerate is %d", fps);
        m_text->setColour (0.2,0.6,0.5);
        m_text->renderText (10,10,text);


        m_frames++;
}


void NGLScene::testButtonClicked(bool b)
{
    emit clicked (b);
    std::cout<<"Button Clicked - manual signal-slot connection"<<std::endl;
    // m_rColor=1;

    //delete some of the 1st level nodes of the tree
    deleteAreaByAreaElements(tree);

    update ();
}


void NGLScene::timerEvent( QTimerEvent *_event )
{
    if(_event->timerId() == updateTimer)
    {
        if(currenttime.elapsed() > 1000)
        {
            fps=m_frames;
            m_frames=0;
            currenttime.restart();
        }


        update ();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    update ();
  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

    // check the diff of the wheel position (0 means no change)
    if(_event->delta() > 0)
    {
        m_modelPos.m_z+=ZOOM;
    }
    else if(_event->delta() <0 )
    {
        m_modelPos.m_z-=ZOOM;
    }
    update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
//  case Qt::Key_Up    : m_plane->tilt(1.0,1,0); break;
//  case Qt::Key_Down  : m_plane->tilt(-1.0,1,0); break;
//  case Qt::Key_Left  : m_plane->tilt(-1.0,0,1); break;
//  case Qt::Key_Right : m_plane->tilt(1.0,0,1); break;
  default : break;
  }
  // finally update the GLWindow and re-draw
//  if (isExposed())
    update();
}
